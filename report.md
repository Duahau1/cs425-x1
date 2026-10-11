# Project Report

**Name:** Van Nguyen

**Class:** CS452

**Project Name:** X1 - Anatomy of a Page Load

## Task 3 - Predictions

### 1. Distance vs. Delay (Theoretical Minimum Connect Time)

_Laptop location:_ [Boise, Idaho, US] (about 43.6135, -116.2035). The network is T-Mobile (AS21928).

_Assumed propagation speed in fiber:_ $2 \times 10^8\text{ m/s}$ ($200\text{ km/ms}$).

| Target Mirror  | URL                                             | Location           | One-Way Great-Circle Distance (km) | Predicted Min Connect Time (ms) |
| :------------- | :---------------------------------------------- | :----------------- | :--------------------------------- | :------------------------------ |
| **CDN**        | <http://example.com/>                           | Nearest POP        | Denver - around 1000               | 15                              |
| **Utah**       | <http://mirrors.xmission.com/debian/README>     | Salt Lake City, UT | 476                                | 7.14                            |
| **California** | <http://mirrors.ocf.berkeley.edu/debian/README> | Berkeley, CA       | 818                                | 12.54                           |
| **New York**   | <http://mirrors.rit.edu/debian/README>          | Rochester, NY      | 3091                               | 46.365                          |
| **Germany**    | <http://ftp.fau.de/debian/README>               | Erlangen, Germany  | 8454                               | 126.62                          |
| **Japan**      | <http://ftp.jaist.ac.jp/debian/README>          | Nomi, Japan        | 8485                               | 127.85                          |

**Calculation Method / Work:**

- $\text{Theoretical Minimum Connect Time (ms)} = \frac{2 \times 1.5 \times \text{One-Way Distance (km)}}{200\text{ km/ms}}$
- A TCP connection typically takes about 1 RTT to establish. I estimate the one-way propagation delay as $\frac{\text{One-Way Distance (km)}}{200\text{ km/ms}}$ and multiply the great-circle distance by 1.5 to account for real-world fiber routes being longer than the straight-line distance. I used [this article](https://www.scaleway.com/en/blog/understanding-network-latency) as a reference because the textbook does not specify whether to use straight-line distance.

---

### 2. Longest Connect Time from Codespace

- **Predicted Mirror:** Germany: <http://ftp.fau.de/debian/README>
- **Reasoning:** Since I purposely set my region in the Github's Codespace as `us-west`, I used San Francisco as an approximate starting point(in real life I don't know where it is I picked San Francisco as I thought Bay Area is famous and furthest west that I know of). According to [MapTools](https://www.maptools.co/tools/distance-between/), Germany is about 9,276 km away and Japan is about 8,450 km away. Using the formula above, the estimate is 139.14 ms for Germany and 126.75 ms for Japan. Since Germany's estimated time is longer, I predict that the Germany mirror will have the longest connect time.

---

### 3. DNS Lookup Duration (Trial 1 vs. Trials 2–5)

- **Prediction:** I think the dns time will decrease after the first trial and then for trial 2-5 it will be the same
- **Reasoning:** The first time when you query for that DNS record, there is a really high chance you won't be able to retrieve the domain from you local dns cache as your dns resolver have never resolved that record before. The dns resolver then have to go up to a higher dns resolver namely root servers, TLD, authoritative name server to do look up where takes a few trips before it returned to you and stored in the local cache. After the first time, your next 2-5 consecutive lookups (this is so that we know that cache will not invalidate this item due to ttl) will be hit from cache so you may see the dns lookup time dropped significantly.

---

### 4. NAT Identification

- **Laptop:** Behind a NAT
- **Onyx:** Behind a NAT
- **Codespace:** Behind a NAT
- **Reasoning:** My Ip address on my machine is `192.168.12.235` after I ran `ipconfig getifaddr en0` so we are definitely behind a NAT. And for Onyx, we are only able to connect to it if we set up the VPN so it needs to be behind some controlled NAT. And then for Codespace, it is running in an isolated Docker container on a virtual machine in Microsoft Azure so it definitely needs a NAT.

---

### 5. Transfer Time: Utah vs. Japan (13 MB File)

- **Prediction:** No they will take different time, Japan has greater download time compared to Utah
- **Reasoning:** So the total time formula is `Total Time = 2 x Theoretical Minimum Connect Time + transmission time of HTML file at the server`. `13MB` is not large and based on the calculation that we did for `2 X RTT` in the prediction #1, so if we assume no package loss and transmission time is the same as bandwidth does not depend on physical distance, the RTT will dominate and the RTT of Japan is a lot greater than Utah

---

## Task 5 - Analysis Questions

### Chapter 1 - Delay and Throughput

#### Question 1 - Distance versus delay

_Propagation RTT = 2 × distance ÷ 200 km/ms = distance ÷ 100, using the propagation-delay model from section 1.4.1. Min connect is the minimum of the 5 trials in `measurements-<vantage>.txt`; ratio = min connect ÷ propagation RTT. Distances are great-circle distances in km. Interpreting it:
A ratio close to 1 means almost all delay is just physical propagation.
A larger ratio means additional delay from queuing, transmission, and processing._

**Laptop** (Boise, ID: 43.6135, -116.2035)

| Mirror           | Distance (km) | Propagation RTT (ms) | Min connect (ms) | Ratio |
| :--------------- | :------------ | :------------------- | :--------------- | :---- |
| CDN (Denver POP) | 1,025         | 10.25                | 34.814           | 3.40  |
| Utah             | 476           | 4.76                 | 60.236           | 12.65 |
| California       | 818           | 8.18                 | 53.783           | 6.57  |
| New York         | 3,091         | 30.91                | 82.763           | 2.68  |
| Germany          | 8,454         | 84.54                | 174.865          | 2.07  |
| Japan            | 8,485         | 84.85                | 187.719          | 2.21  |

**Onyx** (Boise State campus, Boise, ID: same coordinates as the laptop)

| Mirror           | Distance (km) | Propagation RTT (ms) | Min connect (ms) | Ratio |
| :--------------- | :------------ | :------------------- | :--------------- | :---- |
| CDN (Denver POP) | 1,025         | 10.25                | 11.381           | 1.11  |
| Utah             | 476           | 4.76                 | 49.612           | 10.42 |
| California       | 818           | 8.18                 | 24.988           | 3.05  |
| New York         | 3,091         | 30.91                | 52.355           | 1.69  |
| Germany          | 8,454         | 84.54                | 145.237          | 1.72  |
| Japan            | 8,485         | 84.85                | 131.973          | 1.56  |

**Codespace** (Quincy/Moses Lake, WA: about 47.234, -119.852; the public IP 51.143.4.81 geolocates to Moses Lake, WA and belongs to Microsoft, so the `us-west` region is in Washington, not San Francisco)

| Mirror                    | Distance (km) | Propagation RTT (ms) | Min connect (ms) | Ratio |
| :------------------------ | :------------ | :------------------- | :--------------- | :---- |
| CDN (assumed Seattle POP) | 191           | 1.91                 | 5.150            | 2.69  |
| Utah                      | 960           | 9.60                 | 31.301           | 3.26  |
| California                | 1,060         | 10.60                | 24.345           | 2.30  |
| New York                  | 3,300         | 33.00                | 75.809           | 2.30  |
| Germany                   | 8,259         | 82.59                | 152.897          | 1.85  |
| Japan                     | 8,018         | 80.18                | 107.391          | 1.34  |

#### Question 2 - Where the extra time goes

According to the textbook section 1.4, there are 4 types of nodal delay where the total nodal delay is equal $$d_{\text{nodal}} = d_{\text{proc}} + d_{\text{queue}} + d_{\text{trans}} + d_{\text{prop}}$$
Calculation in theory did not account for any of this and also it is hard to know what these number are since they are not constant and fluctuated all the time. This total delay node added to the connect time which made the actual connection bigger that the one we calculated.

I have also included the factor where path is not a straight line (or crow flies) in my prediction in Task 3. In reality, it would be impossible just have a crow flies access(which is the shortest possible path). In chapter 1.4, the textbook introduce `traceroute` example which was a very niche demonstration that path is not a straight line. I ran `traceroute ftp.fau.de` on my computer and by observing IPs of all the hops the way data packets travel are zigzag where each hop is a router chosen by real-world forwarding decisions, not by a straight-line geographic route.

Based on the above table, we can see the pattern where for Japan, the propagation delay is the dominant factor because the distance is about 8,500 km, giving a propagation RTT of about 85 ms. The measured minimum connect time is about 188 ms, so the remaining delay is mostly queueing and processing along the path. For Utah, if you take the average of the three vantage points difference between min connect time and propagation RTT, it is around 40ms which accounts for 86% of the average min connect time of the 3 vantages point. This matches the nodal-delay model:
d_nodal = d_proc + d_queue + d_trans + d_prop
with propagation dominating for long-haul Japan and queueing/processing dominating for short-haul Utah.

#### Question 3 - Minimum, not mean

We will use New York (`mirrors.rit.edu`), because it shows both the noisiest and the quietest vantage points and also the Japan mirrors as well.

Sample standard deviation: $s = \sqrt{\frac{1}{n-1}\sum_{i=1}^{n}(x_i - \bar{x})^2}$, where $x_i$ are the 5 connect times, $\bar{x}$ is their mean, and $n = 5$. Spread = max - min

| Trial          | Laptop  | Onyx   | Codespace |
| :------------- | :------ | :----- | :-------- |
| 1              | 82.763  | 52.355 | 76.041    |
| 2              | 119.521 | 52.542 | 76.181    |
| 3              | 96.448  | 52.494 | 75.967    |
| 4              | 140.964 | 52.412 | 75.809    |
| 5              | 257.615 | 52.529 | 76.009    |
| **Min**        | 82.763  | 52.355 | 75.809    |
| **Mean**       | 139.462 | 52.466 | 76.001    |
| **Median**     | 119.521 | 52.494 | 76.009    |
| **Max**        | 257.615 | 52.542 | 76.181    |
| **Spread**     | 174.852 | 0.187  | 0.372     |
| **Std dev**    | 69.687  | 0.080  | 0.134     |
| **Mean / min** | 1.69    | 1.00   | 1.00      |

_Second mirror: Japan (`ftp.jaist.ac.jp`), from the `latency japan` sections._

| Trial          | Laptop  | Onyx    | Codespace |
| :------------- | :------ | :------ | :-------- |
| 1              | 187.719 | 132.146 | 109.071   |
| 2              | 213.822 | 132.067 | 107.703   |
| 3              | 188.974 | 131.973 | 107.391   |
| 4              | 195.768 | 132.160 | 107.548   |
| 5              | 188.404 | 132.253 | 107.760   |
| **Min**        | 187.719 | 131.973 | 107.391   |
| **Mean**       | 194.937 | 132.120 | 107.895   |
| **Median**     | 188.974 | 132.146 | 107.703   |
| **Max**        | 213.822 | 132.253 | 109.071   |
| **Spread**     | 26.103  | 0.280   | 1.680     |
| **Std dev**    | 11.042  | 0.105   | 0.673     |
| **Mean / min** | 1.04    | 1.00    | 1.00      |

Propagation delay depends on distance and the speed of light in fiber, so for a fixed server it should be nearly the same in every trial. The delay that changes from trial to trial is mainly one of the 4 nodal delay type, which only ever adds to the other components of nodal delay (section 1.4). The mean mixes this queuing into the estimate, while the minimum is the trial with the least of it. The minimum is still not pure propagation: it also contains the fixed processing and transmission delays and the extra distance of a real cable route, so it is the best estimate of the propagation floor, and an upper bound on it.

My own trials support this. For New York, the laptop spread is 174.852 ms with a mean/min ratio of 1.69, while Onyx and Codespace spread only 0.187 ms and 0.372 ms. For Japan, the laptop spread is 26.103 ms with a ratio of 1.04, while Onyx and Codespace spread 0.280 ms and 1.680 ms, both small. The queuing varies by path and by trial: the same laptop was far noisier to New York than to Japan, so a single mean would be unreliable. The distance from one vantage point to a mirror is the same in all five trials, so the trial-to-trial variation is not caused by distance. Because propagation is fixed for a given distance, the minimum is the best estimate of the floor, and the mean would inflate the Question 1 ratios.

#### Question 4 - Transmission delay

In section 1.4 of the textbook, the transmission-delay formula is $d_{trans} = L / R$, where $L$ is the packet size in bits and $R$ is the link rate in bits per second.

For a 64-byte packet, $L = 64 \times 8 = 512$ bits. At $R = 10$ Mbit/s,

$$
d_{trans} = \frac{512}{10 \times 10^6} = 5.12 \times 10^{-5} \text{ s} = 0.0512 \text{ ms}.
$$

At $R = 1$ Gbit/s,

$$
d_{trans} = \frac{512}{10^9} = 5.12 \times 10^{-7} \text{ s} = 0.000512 \text{ ms}.
$$

These values are tiny compared with the Utah propagation RTT of about 4.76 ms, so for this small packet the transmission delay is negligible relative to propagation delay.

#### Question 5 - Throughput

_Throughput in Mbit/s from the `download` sections (13,332,733 bytes each). RTT is the minimum connect time from the same section._

| Vantage   | Mirror | RTT (ms) | Trial 1 (Mbit/s) | Trial 2 (Mbit/s) | Trial 3 (Mbit/s) | Max (Mbit/s) | Mean (Mbit/s) |
| :-------- | :----- | :------- | :--------------- | :--------------- | :--------------- | :----------- | :------------ |
| Laptop    | Utah   | 58.482   | 94.965           | 94.830           | 129.469          | 129.469      | 106.42        |
| Onyx      | Utah   | 49.773   | 85.324           | 78.522           | 87.112           | 87.112       | 83.65         |
| Codespace | Utah   | 31.384   | 319.607          | 318.705          | 311.124          | 319.607      | 316.48        |

_Japan (`ftp.jaist.ac.jp`):_

| Vantage   | Mirror | RTT (ms) | Trial 1 (Mbit/s) | Trial 2 (Mbit/s) | Trial 3 (Mbit/s) | Max (Mbit/s) | Mean (Mbit/s) |
| :-------- | :----- | :------- | :--------------- | :--------------- | :--------------- | :----------- | :------------ |
| Laptop    | Japan  | 181.134  | 5.724            | 6.110            | 7.241            | 7.241        | 6.36          |
| Onyx      | Japan  | 132.129  | 2.626            | 2.093            | 1.061            | 2.626        | 1.93          |
| Codespace | Japan  | 107.348  | 11.761           | 23.086           | 23.034           | 23.086       | 19.29         |

_Utah ÷ Japan, by vantage point:_

| Vantage   | Mean Utah ÷ mean Japan | Max Utah ÷ max Japan |
| :-------- | :--------------------- | :------------------- |
| Laptop    | 16.7                   | 17.9                 |
| Onyx      | 43.4                   | 33.2                 |
| Codespace | 16.4                   | 13.8                 |

Section 1.4 in the book defines a connection's bottleneck link as the link along its path with the smallest transmission rate, which limits the end-to-end throughput.

The access link cannot be the bottleneck in both cases. Each vantage point uses one access link for both downloads, so if that link limited both, Utah and Japan would give about the same throughput. The same pattern appears at all three vantage points, even though the links differ in speed by about 4× between Onyx and Codespace. Utah is 16.7 times faster than Japan on the laptop, 43.4 times on Onyx and 16.4 times on Codespace (by mean).That is strong evidence the limit is not the access link, because Japan stays far below what the same link delivered from Utah.

### Chapter 2 - The Application Layer

#### Question 6 - DNS caching

The mirror is used is the one from Utah

| Vantage   | Mirror | Trial 1 | Trial 2 | Trial 3 | Trial 4 | Trial 5 | Mean 2-5 |
| :-------- | :----- | :------ | :------ | :------ | :------ | :------ | :------- |
| Laptop    | Utah   | 125.745 | 1.408   | 1.454   | 1.879   | 3.278   | 2.005    |
| Onyx      | Utah   | 29.182  | 28.597  | 31.931  | 28.563  | 28.369  | 29.365   |
| Codespace | Utah   | 35.278  | 0.485   | 0.497   | 0.516   | 0.491   | 0.497    |

When a program looks up a name, it asks a local DNS server, which answers from its cache or else resolves the name through the root, TLD and authoritative servers (section 2.4). The first lookup is slow because the cache is empty. The answer is then stored for its TTL, and later lookups are served from the cache without leaving the local network. The ttl is long enough so that we can safely assume that trial 2-5's result are from the local cache.

The laptop and Codespace follow this pattern. Trial 1 is slow (125.745 ms and 35.278 ms), and trials 2 to 5 are fast (about 2 ms and 0.5 ms), so a nearby cache answered them. Onyx does not: all five trials are about 28 to 32 ms.
Local caches make repeat lookups almost free, while a remote shared server charges one round trip every time.I went into the `measurements-onyx.txt` and looked at dns lookup time of other locations and I realized that trial 2-5 lookup time sometimes even higher than the first look up time. Looking at the above table, there are 2 possible explanations for this, one is the dns record did not get cached on locally properly on Onyx or the dns record has already get cached so what I got was hit record retrieved from the cache.

#### Question 7 - The DNS hierarchy

The laptop's trace (`dig +trace ftp.jaist.ac.jp` in `measurements-laptop.txt`). We can use either Codespace or laptop vantage point as the one on Onyx get blocked for access. Based on the textbook (J. F. Kurose and K. W. Ross, Computer Networking: A Top-Down Approach, Pearson, 2026, sec. 2.4), the 4 hierarchy in order : Local DNS server, Root DNS server, TLD DNS server, authoritative DNS server.

| Step | Server asked                              | Answer (referral or address)                                                       | Level in hierarchy                                    |
| :--- | :---------------------------------------- | :--------------------------------------------------------------------------------- | :---------------------------------------------------- |
| 1    | Local resolver, 10.50.23.249              | Referral: the 13 root servers (`a` to `m.root-servers.net`)                        | Local DNS server, which returned the root server list |
| 2    | `i.root-servers.net`, 192.36.148.17       | Referral: the 8 `.jp` name servers (`a`, `b`, `c`, `d`, `e`, `f`, `g`, `h.dns.jp`) | Root                                                  |
| 3    | `g.dns.jp`, 203.119.40.1                  | Referral: `jaist.ac.jp` name servers `dns-x.sinet.ad.jp` and `ns0.jaist.ac.jp`     | TLD                                                   |
| 4    | `ns0.jaist.ac.jp`, 2001:df0:2ed:8250::130 | Address: `ftp.jaist.ac.jp` A 150.65.7.130                                          | Authoritative                                         |

#### Question 8 - The CDN

| Vantage   | Address (number of trials)            | Min connect (ms) |
| :-------- | :------------------------------------ | :--------------- |
| Laptop    | 172.66.147.243 (5)                    | 34.814           |
| Onyx      | 172.66.147.243 (4), 104.20.23.154 (1) | 11.381           |
| Codespace | 172.66.147.243 (2), 104.20.23.154 (3) | 5.150            |

All three vantage points received the same IP address 172.66.147.243, but there are address changed between trials at Onyx and Codespace.
In question 1, I have a table that had all the connect time, and the connect time of `example.com` is the lowest compare to all the mirrors.

Based on chapter 2.5 of the textbook, I think the strategy that it is using is cluster selection strategy where cdn maps your local dns server's ip to a geographic location using geo-location databases and then pick the nearest/best-performing one to you. The fact that the connect time is very low with Onyx and Codespace it is only 5ms-12ms means that the CDN has a cluster near each vantage point. The addresses differed between trials at Onyx and Codespace meaning the CDN does not always send us to the same server, it is select the most optimal one to send us to (this is base on delay, bandwidth, not just closest edge).

#### Question 9 - Counting round trips

First byte ≈ 1 RTT + server time, so server time ≈ first byte − min connect

| Mirror | Vantage | Trial | Connect / RTT (ms) | First byte (ms) | Estimated server time (ms) |
| :----- | :------ | :---- | :----------------- | :-------------- | :------------------------- |
| Utah   | Onyx    | 1     | 49.863             | 50.405          | 0.542                      |
| Utah   | Onyx    | 2     | 49.820             | 50.356          | 0.536                      |
| Utah   | Onyx    | 3     | 49.612             | 50.100          | 0.488                      |
| Utah   | Onyx    | 4     | 49.944             | 50.536          | 0.592                      |
| Utah   | Onyx    | 5     | 50.053             | 50.474          | 0.421                      |
| Japan  | Onyx    | 1     | 132.146            | 199.182         | 67.036                     |
| Japan  | Onyx    | 2     | 132.067            | 133.281         | 1.214                      |
| Japan  | Onyx    | 3     | 131.973            | 133.297         | 1.324                      |
| Japan  | Onyx    | 4     | 132.160            | 141.132         | 8.972                      |
| Japan  | Onyx    | 5     | 132.253            | 133.451         | 1.198                      |

In the below table we will calculate DNS start to last byte (ms)
= DNS (ms) + Connect / RTT (ms) + First byte (ms) + Transfer (ms)
According to chapter 2.2 in the textbook, for a non-persistent HTTP connection, the estimation for retrieving a single base HTML file is: Total Response Time = $2 \times RTT$ + File Transmission Time. We can further break it down to Total RTTs = 1 RTT (TCP Handshake) + 1 RTT (HTTP Request/Response) = 2 RTTs total + File Transmission Time. The DNS look up time happens before the RTT as it needs to make the IP address known.

| Mirror | Trial | DNS time (ms) | Min connect time / RTT (ms) | Transfer after first byte (ms) | DNS start to last byte (ms) | 2 x RTT + transmission time (ms) | Ratio: (Connect + First byte + Transfer) / (2 x RTT + transmission time) |
| :----- | :---- | :------------ | :-------------------------- | :----------------------------- | :-------------------------- | :------------------------------- | :----------------------------------------------------------------------- |
| Utah   | 1     | 29.182        | 49.612                      | 0.018                          | 129.468                     | 99.242                           | 1.011                                                                    |
| Utah   | 2     | 28.597        | 49.612                      | 0.011                          | 128.784                     | 99.235                           | 1.010                                                                    |
| Utah   | 3     | 31.931        | 49.612                      | 0.010                          | 131.653                     | 99.234                           | 1.005                                                                    |
| Utah   | 4     | 28.563        | 49.612                      | 0.010                          | 129.053                     | 99.234                           | 1.013                                                                    |
| Utah   | 5     | 28.369        | 49.612                      | 0.037                          | 128.933                     | 99.261                           | 1.013                                                                    |
| Japan  | 1     | 344.676       | 131.973                     | 0.018                          | 676.022                     | 263.964                          | 1.255                                                                    |
| Japan  | 2     | 31.828        | 131.973                     | 0.010                          | 297.186                     | 263.956                          | 1.005                                                                    |
| Japan  | 3     | 203.394       | 131.973                     | 0.010                          | 468.674                     | 263.956                          | 1.005                                                                    |
| Japan  | 4     | 207.562       | 131.973                     | 0.010                          | 480.864                     | 263.956                          | 1.035                                                                    |
| Japan  | 5     | 201.697       | 131.973                     | 0.043                          | 467.444                     | 263.989                          | 1.007                                                                    |

From the above table, we can see based on the last ratio column, the ratio is around 1 -1.2 which means it matched well with the formula form section 2.2. If we are taking the minimum of connect time, first byte, and dns lookup time, and to compute the DNS start to last byte (ms)/ min RTT (s) we get 2.6 and 2.25 for Utah and Japan respectively. These data are close to 2 RTTs which matched well with what section 2.2 was showing.

### Chapter 3 - The Transport Layer

#### Question 10 - The handshake

When the client-side `connect()` call returns, the following `TCP` segments have been sent and received:

- Sent by Client: The initial `SYN` segment.

- Received & Sent by Server: The server received the `SYN` and replied with the `SYN-ACK` segment.

- Sent by Client: The client received the `SYN-ACK` and transmitted the final `ACK` segment back.

As outline in section 3.5.6, the client does not need to wait for the `ACK` reply from the server because the client transitions from `SYN_SENT` to `ESTABLISHED` as soon as it receives the server's `SYN-ACK` and fires off its own final `ACK`. As soon as the client dispatches that last acknowledgment packet, the local connection state is marked open, allowing `connect()` to return, both sides are ready to send data.

#### Question 11 - Demultiplexing

Using the "two at once" section of `measurements-onyx.txt`, against Utah's mirror, trial 1 for each run.

| Run | Local address:port   | Remote address:port |
| :-- | :------------------- | :------------------ |
| 1   | 132.178.227.11:56554 | 198.60.22.13:80     |
| 2   | 132.178.227.11:56568 | 198.60.22.13:80     |

The only source port's difference is port 56554 versus 56568 in the local address. This is because the OS picked an ephemeral port when `connect` was called as the program did not assign a fixed port in the code.

In section 3.2, when multiple TCP connections arrive at the web server’s port 80, they are not distinguished by the destination port. The transport layer used the 4 tuple to demultiplex: each segment goes to the socket that match all 4 values, so the 2 connections are uniquely identified even though they ran in parallel.

#### Question 12 - Slow start

Using the onyx vantage point, with the package size 13MB
Segments = ceil(13332733/ 1460)= 9132.0089 so it is 9133 segments
Then using 10 × (2^n − 1) ≥ segments we get n = 10 as n=9 is less than what we need.
The shortest time is n x RTT and measured is the mean transfer time of the 3 downloads

| Mirror | Segments | RTTs needed | RTT (ms) | Shortest time (ms) | Measured (ms) |
| :----- | :------- | :---------- | :------- | :----------------- | :------------ |
| Utah   | 9133     | 10          | 49.612   | 496.1              | 1277.6        |
| Japan  | 9133     | 10          | 131.976  | 1319.7             | 64028.1       |

The Utah path comes much closer to its theoretical estimate, scaling far better than the trans-continental Japan link.
I think slow start did set the minimum as we can see that the measured time for Utah is about 2.6 this minium and the rest is link rate, nodal delay, or congestion. The measured Japan is about 48 times the shortest time, however, we are measuring this reality where all sort of things could have happened so just the slow start time cannot explain this in this case. Higher propagation delay over long distances increases the penalty of any packet drops or retransmissions, forcing the connection out of slow-start and into congestion avoidance or timeout recovery.

### Chapter 4 - The Network Layer

#### Question 13 - Private and public addresses

The local address is the source address in the 4 tuple from `measurements-<vantage>.txt`. The public address is coming from `checkip`. And according to section 4.3, where it said that for private RFC 1918 address blocks(RFC 1918 ranges: 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16),they must pass through a NAT-enabled router, which translates between private and public addresses.

| Vantage   | Local address  | Public address (checkip) | Private (RFC 1918)? | Behind NAT? |
| :-------- | :------------- | :----------------------- | :------------------ | :---------- |
| Laptop    | 192.168.12.235 | 172.56.148.24            | Yes                 | Yes         |
| Onyx      | 132.178.227.11 | 132.178.227.11           | No                  | No          |
| Codespace | 10.0.0.97      | 51.143.4.81              | Yes                 | Yes         |

From the above table, only 132.178.227.11 does not belong to RFC 1918 address blocks so it is not behind a NAT

#### Question 14 - The NAT table

I picked laptop vantage point and the first Utah connection and below is the
| WAN side | LAN side |
| :------- | :------- |
| 172.56.148.24:`UNKNOWN-PORT` | 192.168.12.235:49417 |

From section 4.3 and using figure 4.25, the reason that we cannot see the WAN's side NAT's public port is because all incoming packets simply see destination 172.56.148.24 where the detailed mappings and choices of public ports are: maintained only inside the NAT translation table (not exposed to clients or routers). The NAT rewrites the packet after it leaves the laptop.
The internal private address structure is completely invisible and stripped from the packet headers.

#### Question 15 - Subnetting

For laptop since I am on MacOS, I have to use `ifconfig`, and from the following `en0: 	inet 192.168.12.235 netmask 0xffffff00` we get `192.168.12.235/24`. For Onyx and Codespace (both on Linux) it showed you the cdir block directly for `eno12399np01` and `eth0` respectively.

From the book section 4.3, where an address written as a.b.c.d/x, the x most significant bits form the network portion (the prefix), and the remaining 32 − x bits distinguish the individual devices within that network. It also gives an example where an address like a.b.c.d/24 can refer to a specific subnet.

The usable range and usable hosts are calculate using the following formula: (2^(32−prefix) − 2) where the .0 is the network-address and the last ip of the range is used for broadcast so they are not part of the usable range.

| Vantage   | Address/prefix    | Network       | Broadcast       | Usable range                  | Usable hosts |
| :-------- | :---------------- | :------------ | :-------------- | :---------------------------- | :----------- |
| Laptop    | 192.168.12.235/24 | 192.168.12.0  | 192.168.12.255  | 192.168.12.1–192.168.12.254   | 254          |
| Onyx      | 132.178.227.11/25 | 132.178.227.0 | 132.178.227.127 | 132.178.227.1–132.178.227.126 | 126          |
| Codespace | 10.0.0.97/16      | 10.0.0.0      | 10.0.255.255    | 10.0.0.1–10.0.255.254         | 65,534       |

The binary of Onyx 132.178.227.11/25. The first three octets are fully in the network part so I will show the last octet.

| Value                                  | Last octet in binary | Last octet in decimal |
| :------------------------------------- | :------------------- | :-------------------- |
| Address                                | `00001011`           | 11                    |
| Subnet mask                            | `10000000`           | 128                   |
| Network address (address AND mask)     | `00000000`           | 0                     |
| Broadcast address (host bits set to 1) | `01111111`           | 127                   |
| First usable host                      | `00000001`           | 1                     |
| Last usable host                       | `01111110`           | 126                   |

---

## Task 6 - Catch the Agent

- **Quote from Agent Session:** The agent wrote `measure.sh` with these targets (`copilot-session.md`, around line 3273):

  > `"latency-utah|http://mirrors.xmission.com/|utah.body"`
  > `"download-utah|http://mirrors.xmission.com/ubuntu/dists/noble/main/binary-amd64/Packages.gz|utah-Packages.gz"`
  > `"download-japan|http://ftp.jaist.ac.jp/pub/Linux/debian/dists/stable/main/binary-amd64/Packages.gz|japan-Packages.gz"`

  Its own reasoning noted "I should verify the download URLs for accuracy", but it did not. It then reported the run as complete (around line 3930).

- **Verification Method:** I compared the URLs in `measure.sh` with the mirror list in the assignment. The assignment gives `http://mirrors.xmission.com/debian/README` for Utah and `http://ftp.jaist.ac.jp/debian/README` for Japan, and later gives `/debian/dists/trixie/main/binary-amd64/Packages.gz` for the downloads.
- **Explanation & Correct Answer:** The agent invented URLs that the assignment did not give. The latency targets were the bare mirror roots, not the `/debian/README` paths. The Utah download used an Ubuntu path (`/ubuntu/dists/noble/`) on a Debian mirror. The Japan download used `/pub/Linux/debian/dists/stable/`, which is also not the specified path. A script that runs without errors does not prove its targets are right. The agent should have used only the URLs from the assignment, or asked. The correct targets are `/debian/README` for latency and `/debian/dists/trixie/main/binary-amd64/Packages.gz` for downloads, which the agent applied only after I corrected it, you can read about this in `session-1.md`.
