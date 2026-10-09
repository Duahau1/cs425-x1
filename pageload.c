#define _POSIX_C_SOURCE 200809L

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define READ_BUFFER_SIZE 8192
#define MAX_HEADER_SIZE (1024U * 1024U)
#define NUMERIC_HOST_LENGTH INET6_ADDRSTRLEN
#define NUMERIC_SERVICE_LENGTH 6

struct url {
    char *host;
    char *port;
    char *path;
    bool host_is_ipv6;
    bool port_was_specified;
};

struct trial_result {
    double connect_ms;
};

static void usage(const char *program)
{
    fprintf(stderr, "Usage: %s [-4 | -6] [-n trials] [-o file] http://host[:port][/path]\n",
            program);
}

static double elapsed_ms(const struct timespec *start, const struct timespec *end)
{
    return (double)(end->tv_sec - start->tv_sec) * 1000.0 +
           (double)(end->tv_nsec - start->tv_nsec) / 1000000.0;
}

static int now(struct timespec *time)
{
    if (clock_gettime(CLOCK_MONOTONIC, time) == -1) {
        perror("clock_gettime");
        return -1;
    }
    return 0;
}

static void free_url(struct url *url)
{
    free(url->host);
    free(url->port);
    free(url->path);
    memset(url, 0, sizeof(*url));
}

static char *copy_range(const char *start, size_t length)
{
    char *copy = malloc(length + 1);

    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, start, length);
    copy[length] = '\0';
    return copy;
}

static bool invalid_url_character(const char *text)
{
    const unsigned char *character = (const unsigned char *)text;

    while (*character != '\0') {
        if (*character <= 0x20U || *character == 0x7fU) {
            return true;
        }
        character++;
    }
    return false;
}

static int parse_port(const char *text)
{
    char *end;
    long value;

    if (*text == '\0') {
        return -1;
    }
    errno = 0;
    value = strtol(text, &end, 10);
    if (errno != 0 || *end != '\0' || value < 1 || value > 65535) {
        return -1;
    }
    return (int)value;
}

static int parse_url(const char *input, struct url *url)
{
    const char *authority;
    const char *path;
    const char *host_start;
    const char *host_end;
    const char *port_start = NULL;
    size_t host_length;

    memset(url, 0, sizeof(*url));
    if (strncmp(input, "http://", 7) != 0) {
        fprintf(stderr, "Invalid URL: only http:// URLs are supported.\n");
        return -1;
    }
    authority = input + 7;
    if (*authority == '\0' || invalid_url_character(authority) ||
        strchr(authority, '#') != NULL || strchr(authority, '@') != NULL) {
        fprintf(stderr, "Invalid URL: missing host or invalid URL character.\n");
        return -1;
    }

    path = strpbrk(authority, "/?");
    if (path == NULL) {
        path = authority + strlen(authority);
    }
    if (authority[0] == '[') {
        host_start = authority + 1;
        host_end = memchr(host_start, ']', (size_t)(path - host_start));
        if (host_end == NULL || host_end == host_start) {
            fprintf(stderr, "Invalid URL: malformed bracketed IPv6 address.\n");
            return -1;
        }
        url->host_is_ipv6 = true;
        if (host_end + 1 < path) {
            if (host_end[1] != ':') {
                fprintf(stderr, "Invalid URL: unexpected data after IPv6 address.\n");
                return -1;
            }
            port_start = host_end + 2;
        }
    } else {
        host_start = authority;
        host_end = memchr(authority, ':', (size_t)(path - authority));
        if (host_end == NULL) {
            host_end = path;
        } else {
            port_start = host_end + 1;
        }
        if (memchr(host_start, ':', (size_t)(host_end - host_start)) != NULL) {
            fprintf(stderr, "Invalid URL: IPv6 literals must be enclosed in brackets.\n");
            return -1;
        }
    }

    host_length = (size_t)(host_end - host_start);
    if (host_length == 0 || memchr(host_start, '[', host_length) != NULL ||
        memchr(host_start, ']', host_length) != NULL) {
        fprintf(stderr, "Invalid URL: host is missing or malformed.\n");
        return -1;
    }
    url->host = copy_range(host_start, host_length);
    url->port = copy_range(port_start == NULL ? "80" : port_start,
                           port_start == NULL ? 2 : (size_t)(path - port_start));
    if (url->host == NULL || url->port == NULL || parse_port(url->port) == -1) {
        fprintf(stderr, "Invalid URL: port must be a number from 1 to 65535.\n");
        free_url(url);
        return -1;
    }
    url->port_was_specified = port_start != NULL;
    if (*path == '\0') {
        url->path = copy_range("/", 1);
    } else if (*path == '?') {
        size_t query_length = strlen(path);

        url->path = malloc(query_length + 2);
        if (url->path != NULL) {
            url->path[0] = '/';
            memcpy(url->path + 1, path, query_length + 1);
        }
    } else {
        url->path = copy_range(path, strlen(path));
    }
    if (url->path == NULL) {
        perror("malloc");
        free_url(url);
        return -1;
    }
    return 0;
}

static int send_all(int socket_fd, const char *data, size_t length)
{
    size_t sent = 0;

    while (sent < length) {
        ssize_t result = send(socket_fd, data + sent, length - sent, 0);

        if (result > 0) {
            sent += (size_t)result;
        } else if (result == -1 && errno == EINTR) {
            continue;
        } else {
            perror("send");
            return -1;
        }
    }
    return 0;
}

static int numeric_endpoint(const struct sockaddr *address, socklen_t length,
                            char *host, size_t host_length,
                            char *port, size_t port_length)
{
    int result = getnameinfo(address, length, host, host_length, port, port_length,
                             NI_NUMERICHOST | NI_NUMERICSERV);

    if (result != 0) {
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(result));
        return -1;
    }
    return 0;
}

static void print_endpoint(const char *host, const char *port)
{
    if (strchr(host, ':') != NULL) {
        printf("[%s]:%s", host, port);
    } else {
        printf("%s:%s", host, port);
    }
}

static ssize_t header_end(const char *data, size_t length)
{
    size_t index;

    for (index = 3; index < length; index++) {
        if (data[index - 3] == '\r' && data[index - 2] == '\n' &&
            data[index - 1] == '\r' && data[index] == '\n') {
            return (ssize_t)(index + 1);
        }
    }
    return -1;
}

static int save_body(FILE *output, const char *body, size_t body_length)
{
    if (output != NULL && body_length > 0 &&
        fwrite(body, 1, body_length, output) != body_length) {
        perror("write output file");
        return -1;
    }
    return 0;
}

static int fetch_trial(const struct url *url, int family, unsigned int number,
                       FILE *output, struct trial_result *result)
{
    struct addrinfo hints;
    struct addrinfo *addresses = NULL;
    struct addrinfo *address;
    struct timespec start;
    struct timespec end;
    struct timespec first_byte;
    struct sockaddr_storage local_address;
    struct sockaddr_storage remote_address;
    socklen_t local_length = sizeof(local_address);
    socklen_t remote_length = sizeof(remote_address);
    char resolved_host[NUMERIC_HOST_LENGTH];
    char local_host[NUMERIC_HOST_LENGTH];
    char remote_host[NUMERIC_HOST_LENGTH];
    char local_port[NUMERIC_SERVICE_LENGTH];
    char remote_port[NUMERIC_SERVICE_LENGTH];
    char request[4096];
    char host_header[4096];
    char read_buffer[READ_BUFFER_SIZE];
    char *headers = NULL;
    size_t headers_length = 0;
    bool headers_complete = false;
    bool received_first_byte = false;
    char status_line[1024] = "(no complete HTTP status line)";
    uint64_t body_bytes = 0;
    int socket_fd = -1;
    int status = -1;
    double dns_ms;
    double ttfb_ms;
    double transfer_ms;
    int request_length;
    int host_header_length;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;
    if (now(&start) == -1) {
        goto done;
    }
    status = getaddrinfo(url->host, url->port, &hints, &addresses);
    if (now(&end) == -1) {
        goto done;
    }
    if (status != 0) {
        fprintf(stderr, "DNS resolution for %s: %s\n", url->host, gai_strerror(status));
        status = -1;
        goto done;
    }
    dns_ms = elapsed_ms(&start, &end);
    address = addresses;
    if (numeric_endpoint(address->ai_addr, address->ai_addrlen, resolved_host,
                         sizeof(resolved_host), remote_port, sizeof(remote_port)) == -1) {
        goto done;
    }

    socket_fd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (socket_fd == -1) {
        perror("socket");
        goto done;
    }
    if (now(&start) == -1) {
        goto done;
    }
    if (connect(socket_fd, address->ai_addr, address->ai_addrlen) == -1) {
        perror("connect");
        goto done;
    }
    if (now(&end) == -1) {
        goto done;
    }
    result->connect_ms = elapsed_ms(&start, &end);
    if (getsockname(socket_fd, (struct sockaddr *)&local_address, &local_length) == -1 ||
        getpeername(socket_fd, (struct sockaddr *)&remote_address, &remote_length) == -1) {
        perror("getsockname/getpeername");
        goto done;
    }
    if (numeric_endpoint((struct sockaddr *)&local_address, local_length, local_host,
                         sizeof(local_host), local_port, sizeof(local_port)) == -1 ||
        numeric_endpoint((struct sockaddr *)&remote_address, remote_length, remote_host,
                         sizeof(remote_host), remote_port, sizeof(remote_port)) == -1) {
        goto done;
    }

    host_header_length = snprintf(host_header, sizeof(host_header),
                                  url->port_was_specified
                                      ? (url->host_is_ipv6 ? "[%s]:%s" : "%s:%s")
                                      : (url->host_is_ipv6 ? "[%s]" : "%s"),
                                  url->host, url->port);
    if (host_header_length < 0 || (size_t)host_header_length >= sizeof(host_header)) {
        fprintf(stderr, "Host header is too large.\n");
        goto done;
    }
    request_length = snprintf(request, sizeof(request),
                              "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
                              url->path, host_header);
    if (request_length < 0 || (size_t)request_length >= sizeof(request)) {
        fprintf(stderr, "HTTP request is too large.\n");
        goto done;
    }
    if (send_all(socket_fd, request, (size_t)request_length) == -1 || now(&start) == -1) {
        goto done;
    }

    for (;;) {
        ssize_t received = recv(socket_fd, read_buffer, sizeof(read_buffer), 0);

        if (received == -1 && errno == EINTR) {
            continue;
        }
        if (received == -1) {
            perror("recv");
            goto done;
        }
        if (received == 0) {
            if (!headers_complete) {
                fprintf(stderr, "Invalid HTTP response: connection closed before headers completed.\n");
                goto done;
            }
            if (now(&end) == -1) {
                goto done;
            }
            transfer_ms = elapsed_ms(&first_byte, &end);
            break;
        }
        if (!received_first_byte) {
            if (now(&first_byte) == -1) {
                goto done;
            }
            received_first_byte = true;
        }
        if (!headers_complete) {
            char *expanded;
            ssize_t end_offset;

            if (headers_length + (size_t)received > MAX_HEADER_SIZE) {
                fprintf(stderr, "HTTP response headers exceed %u bytes.\n", MAX_HEADER_SIZE);
                goto done;
            }
            expanded = realloc(headers, headers_length + (size_t)received + 1);
            if (expanded == NULL) {
                perror("realloc");
                goto done;
            }
            headers = expanded;
            memcpy(headers + headers_length, read_buffer, (size_t)received);
            headers_length += (size_t)received;
            headers[headers_length] = '\0';
            end_offset = header_end(headers, headers_length);
            if (end_offset < 0) {
                continue;
            }
            headers_complete = true;
            {
                char *line_end = strstr(headers, "\r\n");
                size_t line_length = line_end == NULL ? headers_length : (size_t)(line_end - headers);

                if (line_length >= sizeof(status_line)) {
                    line_length = sizeof(status_line) - 1;
                }
                memcpy(status_line, headers, line_length);
                status_line[line_length] = '\0';
            }
            body_bytes += headers_length - (size_t)end_offset;
            if (save_body(output, headers + end_offset,
                          headers_length - (size_t)end_offset) == -1) {
                goto done;
            }
            free(headers);
            headers = NULL;
        } else {
            body_bytes += (size_t)received;
            if (save_body(output, read_buffer, (size_t)received) == -1) {
                goto done;
            }
        }
    }

    ttfb_ms = elapsed_ms(&start, &first_byte);
    printf("Trial %u\n", number);
    printf("  DNS:      %.3f ms, %s (%s)\n", dns_ms, resolved_host,
           address->ai_family == AF_INET ? "IPv4" : "IPv6");
    printf("  Connect:  %.3f ms, ", result->connect_ms);
    print_endpoint(local_host, local_port);
    printf(" -> ");
    print_endpoint(remote_host, remote_port);
    printf("\n");
    printf("  Status:   %s\n", status_line);
    printf("  First byte: %.3f ms\n", ttfb_ms);
    printf("  Transfer: %.3f ms, %llu body bytes, %.3f Mbit/s\n", transfer_ms,
           (unsigned long long)body_bytes,
           transfer_ms > 0.0 ? ((double)body_bytes * 8.0) / (transfer_ms * 1000.0) : 0.0);
    status = 0;

done:
    free(headers);
    if (addresses != NULL) {
        freeaddrinfo(addresses);
    }
    if (socket_fd != -1) {
        close(socket_fd);
    }
    return status;
}

static int compare_doubles(const void *left, const void *right)
{
    const double a = *(const double *)left;
    const double b = *(const double *)right;

    return (a > b) - (a < b);
}

int main(int argc, char *argv[])
{
    struct url url;
    struct trial_result *results = NULL;
    const char *output_path = NULL;
    unsigned long parsed_trials;
    unsigned int trials = 1;
    int family = AF_UNSPEC;
    bool force_ipv4 = false;
    bool force_ipv6 = false;
    int option;
    int exit_status = EXIT_FAILURE;
    unsigned int index;

    while ((option = getopt(argc, argv, "46n:o:")) != -1) {
        switch (option) {
        case '4':
            family = AF_INET;
            force_ipv4 = true;
            break;
        case '6':
            family = AF_INET6;
            force_ipv6 = true;
            break;
        case 'n': {
            char *end;

            errno = 0;
            parsed_trials = strtoul(optarg, &end, 10);
            if (errno != 0 || *optarg == '\0' || *end != '\0' ||
                parsed_trials == 0 || parsed_trials > UINT32_MAX) {
                fprintf(stderr, "Invalid trial count: %s\n", optarg);
                return EXIT_FAILURE;
            }
            trials = (unsigned int)parsed_trials;
            break;
        }
        case 'o':
            output_path = optarg;
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (argc - optind != 1 || (force_ipv4 && force_ipv6)) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (parse_url(argv[optind], &url) == -1) {
        return EXIT_FAILURE;
    }
    results = calloc(trials, sizeof(*results));
    if (results == NULL) {
        perror("calloc");
        goto done;
    }
    signal(SIGPIPE, SIG_IGN);
    for (index = 0; index < trials; index++) {
        FILE *output = NULL;

        if (output_path != NULL && index == trials - 1) {
            output = fopen(output_path, "wb");
            if (output == NULL) {
                perror(output_path);
                goto done;
            }
        }
        if (fetch_trial(&url, family, index + 1, output, &results[index]) == -1) {
            if (output != NULL) {
                fclose(output);
            }
            goto done;
        }
        if (output != NULL && fclose(output) == EOF) {
            perror(output_path);
            goto done;
        }
    }
    {
        double *connect_times = malloc((size_t)trials * sizeof(*connect_times));
        double median;

        if (connect_times == NULL) {
            perror("malloc");
            goto done;
        }
        for (index = 0; index < trials; index++) {
            connect_times[index] = results[index].connect_ms;
        }
        qsort(connect_times, trials, sizeof(*connect_times), compare_doubles);
        if (trials % 2U == 0U) {
            median = (connect_times[trials / 2U - 1U] + connect_times[trials / 2U]) / 2.0;
        } else {
            median = connect_times[trials / 2U];
        }
        printf("Summary\n");
        printf("  Trials:               %u\n", trials);
        printf("  Minimum connect time: %.3f ms\n", connect_times[0]);
        printf("  Median connect time:  %.3f ms\n", median);
        free(connect_times);
    }
    exit_status = EXIT_SUCCESS;

done:
    free(results);
    free_url(&url);
    return exit_status;
}
