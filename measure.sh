#!/usr/bin/env bash

set -u

LATENCY_TRIALS=5
DOWNLOAD_TRIALS=3

# name|URL|response body filename relative to measurements-<vantage>-bodies/
TARGETS=(
    "public-address|http://checkip.amazonaws.com/|public-ip.txt"
    "latency-cdn|http://example.com/|cdn.body"
    "latency-utah|http://mirrors.xmission.com/debian/README|utah.body"
    "latency-california|http://mirrors.ocf.berkeley.edu/debian/README|california.body"
    "latency-new-york|http://mirrors.rit.edu/debian/README|new-york.body"
    "latency-germany|http://ftp.fau.de/debian/README|germany.body"
    "latency-japan|http://ftp.jaist.ac.jp/debian/README|japan.body"
    "download-utah|http://mirrors.xmission.com/debian/dists/trixie/main/binary-amd64/Packages.gz|utah-Packages.gz"
    "download-japan|http://ftp.jaist.ac.jp/debian/dists/trixie/main/binary-amd64/Packages.gz|japan-Packages.gz"
)

LATENCY_TARGETS=(
    "latency-cdn"
    "latency-utah"
    "latency-california"
    "latency-new-york"
    "latency-germany"
    "latency-japan"
)

usage() {
    printf 'Usage: %s <vantage-name>\n' "$0" >&2
}

print_network_addresses() {
    if command -v ip >/dev/null 2>&1; then
        ip -4 addr
    elif command -v ifconfig >/dev/null 2>&1; then
        ifconfig | awk '
            /^[[:alnum:]][[:alnum:]._-]*:/ { interface = $1 }
            $1 == "inet" { print interface, $0 }
        '
    else
        printf 'No supported command is available to show IPv4 addresses.\n'
    fi
}

section() {
    printf '\n=== %s ===\n' "$1"
    printf 'started  %s\n' "$(date -u)"
}

lookup_target() {
    local requested_name=$1
    local entry
    local name

    for entry in "${TARGETS[@]}"; do
        IFS='|' read -r name TARGET_URL TARGET_OUTPUT <<<"$entry"
        if [[ $name == "$requested_name" ]]; then
            return 0
        fi
    done
    return 1
}

run_pageload() {
    local label=$1
    local trials=$2
    local target_name=$3
    local status

    if ! lookup_target "$target_name"; then
        printf 'Target configuration for %s was not found.\n' "$label"
        failures=$((failures + 1))
        return 1
    fi
    ./pageload -4 -n "$trials" -o "$body_directory/$TARGET_OUTPUT" "$TARGET_URL"
    status=$?
    if ((status != 0)); then
        failures=$((failures + 1))
        printf '%s failed with exit status %d; continuing.\n' "$label" "$status"
        return "$status"
    fi
    return 0
}

if [[ $# -ne 1 || -z $1 ]]; then
    usage
    exit 1
fi

vantage=$1
log_file="measurements-${vantage}.txt"
body_directory="measurements-${vantage}-bodies"

if ! : >"$log_file"; then
    printf 'Unable to create %s\n' "$log_file" >&2
    exit 1
fi
exec > >(tee -a "$log_file") 2>&1

SECONDS=0
printf '=== x1 measure.sh vantage=%s ===\n' "$vantage"
printf 'date      %s\n' "$(date -u)"
printf 'hostname  %s\n' "$(hostname)"
printf 'uname     %s\n' "$(uname -a)"
printf 'commit    %s\n' "$(git rev-parse --short HEAD)"
print_network_addresses

make
status=$?
if ((status != 0)); then
    printf 'Build failed with exit status %d; no measurements were run.\n' "$status"
    exit "$status"
fi

mkdir -p "$body_directory"
status=$?
if ((status != 0)); then
    printf 'Unable to create %s (exit status %d).\n' "$body_directory" "$status"
    exit "$status"
fi

failures=0

for target_name in "${LATENCY_TARGETS[@]}"; do
    location=${target_name#latency-}
    section "latency $location"
    run_pageload "latency $location" "$LATENCY_TRIALS" "$target_name"
done

for target_name in download-utah download-japan; do
    location=${target_name#download-}
    section "download $location"
    run_pageload "download $location" "$DOWNLOAD_TRIALS" "$target_name"
done

section "two at once utah"
if lookup_target "latency-utah"; then
    first_log=''
    second_log=''
    if first_log=$(mktemp) && second_log=$(mktemp); then
        ./pageload -4 -n "$LATENCY_TRIALS" -o "$body_directory/two-at-once-utah-1.body" \
            "$TARGET_URL" >"$first_log" 2>&1 &
        first_pid=$!
        ./pageload -4 -n "$LATENCY_TRIALS" -o "$body_directory/two-at-once-utah-2.body" \
            "$TARGET_URL" >"$second_log" 2>&1 &
        second_pid=$!

        wait "$first_pid"
        first_status=$?
        wait "$second_pid"
        second_status=$?
        printf '%s\n' "--- first Utah run ---"
        cat "$first_log"
        printf '%s\n' "--- second Utah run ---"
        cat "$second_log"
        rm -f "$first_log" "$second_log"

        if ((first_status != 0)); then
            failures=$((failures + 1))
            printf 'first concurrent Utah run failed with exit status %d; continuing.\n' "$first_status"
        fi
        if ((second_status != 0)); then
            failures=$((failures + 1))
            printf 'second concurrent Utah run failed with exit status %d; continuing.\n' "$second_status"
        fi
    else
        failures=$((failures + 1))
        [[ -n $first_log ]] && rm -f "$first_log"
        printf 'Unable to create temporary files for concurrent Utah runs; continuing.\n'
    fi
else
    failures=$((failures + 1))
    printf 'Concurrent Utah target configuration was not found; continuing.\n'
fi

section "DNS trace"
if command -v dig >/dev/null 2>&1; then
    dig +trace ftp.jaist.ac.jp
    status=$?
    if ((status != 0)); then
        printf 'NOTE: DNS trace failed with exit status %d; this is expected on Onyx because it blocks non-campus DNS servers.\n' "$status"
    fi
else
    printf 'NOTE: DNS trace was not run because dig is not installed; continuing.\n'
fi

section "public address"
if lookup_target "public-address"; then
    ./pageload -4 -o "$TARGET_OUTPUT" "$TARGET_URL"
    status=$?
    if ((status == 0)); then
        cat "$TARGET_OUTPUT"
    else
        failures=$((failures + 1))
        printf 'public address failed with exit status %d; continuing.\n' "$status"
    fi
else
    failures=$((failures + 1))
    printf 'Public address target configuration was not found; continuing.\n'
fi

printf '\n=== measure.sh summary ===\n'
printf 'total runtime: %d seconds\n' "$SECONDS"
printf 'targets failed: %d\n' "$failures"

if ((failures > 0)); then
    exit 1
fi
