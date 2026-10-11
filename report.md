# Project Report

**Name:** Van Nguyen

**Class:** CS452

**Project Name:** X1 - Anatomy of a Page Load

## Task 3 - Predictions

_Write these before running any measurement, then paste them into the agent session so the log timestamps them._

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

_Use your own measurements for every answer, show your work, and cite the textbook section. Delete the italic hints when you finish._

### Chapter 1 - Delay and Throughput

#### Question 1 - Distance versus delay

_Propagation RTT = 2 × distance ÷ 200 km/ms = distance ÷ 100, using the propagation-delay model from J. F. Kurose and K. W. Ross, Computer Networking: A Top-Down Approach, Pearson, 2026, sec. 1.4.1. Min connect is the minimum of the 5 trials in `measurements-<vantage>.txt`; ratio = min connect ÷ propagation RTT. Distances are great-circle distances in km. Interpreting it:
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

_Work shown: one worked example of the great-circle (haversine) calculation goes here._

#### Question 2 - Where the extra time goes

_Four components of nodal delay (section 1.4): processing, queuing, transmission, propagation. Plus the path is not a straight line. Which dominates for Japan, and which for Utah?_

According to the textbook section 1.4, there are 4 types of nodal delay where the total nodal delay is equal $$d_{\text{nodal}} = d_{\text{proc}} + d_{\text{queue}} + d_{\text{trans}} + d_{\text{prop}}$$
Calculation in theory did not account for any of this and also it is hard to know what these number are since they are not dynamic and fluctuated all the time. This total delay node added to the connect time which made the actual connection bigger that the one we calculated.

I have also included the factor where path is not a straight line (or crow flies) in my prediction in Task 3. In reality, it would be impossible just have a crow flies access(which is the shortest possible path). In chapter 1.4, the textbook introduce `traceroute` example which was a very niche demonstration that path is not a straight line. I ran `traceroute ftp.fau.de` on my computer and by observing IPs of all the hops the way data packets travel are zigzag where each hop is a router chosen by real-world forwarding decisions, not by a straight-line geographic route.

Based on the above table, we can see the pattern where for Japan, the propagation delay is the dominant factor because the distance is about 8,500 km, giving a propagation RTT of about 85 ms. The measured minimum connect time is about 188 ms, so the remaining delay is mostly queueing and processing along the path. For Utah, if you take the average of the three vantage points difference between min connect time and propagation RTT, it is around 40ms which accounts for 86% of the average min connect time of the 3 vantages point. This matches the nodal-delay model:
d_nodal = d_proc + d_queue + d_trans + d_prop
with propagation dominating for long-haul Japan and queueing/processing dominating for short-haul Utah.

#### Question 3 - Minimum, not mean

_Mirror used: New York (`mirrors.rit.edu`), because it shows both the noisiest and the quietest vantage points. Connect times are in ms, from the five trials of each `latency new-york` section. Spread = max - min._
Sample standard deviation: $s = \sqrt{\frac{1}{n-1}\sum_{i=1}^{n}(x_i - \bar{x})^2}$, where $x_i$ are the 5 connect times, $\bar{x}$ is their mean, and $n = 5$.

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

_Use the 3 trials of each Packages.gz download. Table: vantage, mirror, Mbit/s per trial. Is the access link the bottleneck in both cases? Evidence: does Utah throughput differ from Japan on the same link?_

_Throughput in Mbit/s from the `download` sections (13,332,733 bytes each). RTT is the minimum connect time from the same section._

| Vantage   | Mirror | RTT (ms) | Trial 1 (Mbit/s) | Trial 2 (Mbit/s) | Trial 3 (Mbit/s) | Max (Mbit/s) | Mean (Mbit/s) |
| :-------- | :----- | :------- | :--------------- | :--------------- | :--------------- | :----------- | :------------ |
| Laptop    | Utah   | 58.482   | 94.965           | 94.830           | 129.469          | 129.469      | 106.42        |
| Onyx      | Utah   | 49.773   | 85.324           | 78.522           | 87.112           | 87.112       | 83.65         |
| Codespace | Utah   | 31.384   | 319.607          | 318.705          | 311.124          | 319.607      | 316.48        |

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

_One mirror per vantage point. DNS time of trial 1 versus trials 2 to 5. Local DNS server and cache, TTL (section 2.4)._

_DNS times in ms from the `latency` sections (5 trials each). Mirror: Utah (`mirrors.xmission.com`). Mean 2-5 is the mean of trials 2 to 5._

| Vantage   | Mirror | Trial 1 | Trial 2 | Trial 3 | Trial 4 | Trial 5 | Mean 2-5 |
| :-------- | :----- | :------ | :------ | :------ | :------ | :------ | :------- |
| Laptop    | Utah   | 125.745 | 1.408   | 1.454   | 1.879   | 3.278   | 2.005    |
| Onyx      | Utah   | 29.182  | 28.597  | 31.931  | 28.563  | 28.369  | 29.365   |
| Codespace | Utah   | 35.278  | 0.485   | 0.497   | 0.516   | 0.491   | 0.497    |

When a program looks up a name, it asks a local DNS server, which answers from its cache or else resolves the name through the root, TLD and authoritative servers (section 2.4). The first lookup is slow because the cache is empty. The answer is then stored for its TTL, and later lookups are served from the cache without leaving the local network. The ttl is long enough so that we can safely assume that trial 2-5's result are from the local cache.

The laptop and Codespace follow this pattern. Trial 1 is slow (125.745 ms and 35.278 ms), and trials 2 to 5 are fast (about 2 ms and 0.5 ms), so a nearby cache answered them. Onyx does not: all five trials are about 28 to 32 ms.
Local caches make repeat lookups almost free, while a remote shared server charges one round trip every time.I went into the `measurements-onyx.txt` and looked at dns lookup time of other locations and I realized that trial 2-5 lookup time sometimes even higher than the first look up time. This is the indication that the dns record did not get cached on locally properly on Onyx.

#### Question 7 - The DNS hierarchy

The laptop's trace (`dig +trace ftp.jaist.ac.jp` in `measurements-laptop.txt`). We can use either Codespace or laptop vantage point as the one on Onyx get blocked for access. Based on the textbook (J. F. Kurose and K. W. Ross, Computer Networking: A Top-Down Approach, Pearson, 2026, sec. 2.4), the 4 hierarchy in order : Local DNS server, Root DNS server, TLD DNS server, authoritative DNS server.

| Step | Server asked                              | Answer (referral or address)                                                       | Level in hierarchy                                    |
| :--- | :---------------------------------------- | :--------------------------------------------------------------------------------- | :---------------------------------------------------- |
| 1    | Local resolver, 10.50.23.249              | Referral: the 13 root servers (`a` to `m.root-servers.net`)                        | Local DNS server, which returned the root server list |
| 2    | `i.root-servers.net`, 192.36.148.17       | Referral: the 8 `.jp` name servers (`a`, `b`, `c`, `d`, `e`, `f`, `g`, `h.dns.jp`) | Root                                                  |
| 3    | `g.dns.jp`, 203.119.40.1                  | Referral: `jaist.ac.jp` name servers `dns-x.sinet.ad.jp` and `ns0.jaist.ac.jp`     | TLD                                                   |
| 4    | `ns0.jaist.ac.jp`, 2001:df0:2ed:8250::130 | Address: `ftp.jaist.ac.jp` A 150.65.7.130                                          | Authoritative                                         |

#### Question 8 - The CDN

_Did `example.com` get the same address at all three vantage points? Compare its connect time with the mirrors. Section 2.6: DNS redirection versus anycast. Mention the CF-RAY colo code if you have it._

_Addresses `example.com` resolved to in the five `latency cdn` trials (`-4`) at each vantage point._

| Vantage   | Address (number of trials)            | Min connect (ms) |
| :-------- | :------------------------------------ | :--------------- |
| Laptop    | 172.66.147.243 (5)                    | 34.814           |
| Onyx      | 172.66.147.243 (4), 104.20.23.154 (1) | 11.381           |
| Codespace | 172.66.147.243 (2), 104.20.23.154 (3) | 5.150            |

In question 1, I have a table that had all the connect time, and the connect time of `example.com` is the lowest compare to all the mirrors.

CDNs place servers close to users using DNS-based redirection with geolocation-driven cluster selection (as covered in Section 2.5). When you request content, the authoritative DNS server responds with a CDN-specific domain name instead of a direct IP. Then the CDN's DNS system evaluates your Local DNS IP address using geolocation databases or delay probes to pinpoint your location and return the IP of the closest optimal edge server cluster.

#### Question 9 - Counting round trips

_First byte ≈ 1 RTT + server time, so server time ≈ first byte − min connect. Do two mirrors. Then count RTTs from DNS start to last byte (DNS, TCP handshake, HTTP request) and compare with the non-persistent HTTP estimate in section 2.2 and A2 step 4._

### Chapter 3 - The Transport Layer

#### Question 10 - The handshake

_When `connect` returns: SYN sent, SYN-ACK received. The third segment (ACK) is sent but not waited for. Why does the client not need to wait? Section 3.5._

#### Question 11 - Demultiplexing

_Take the "two at once" section of one measurement file. Write down both 4-tuples. What differs, who picked it (the OS picks the ephemeral source port), and how the server tells them apart (4-tuple demultiplexing, section 3.2)._

| Run | Local address:port | Remote address:port |
| :-- | :----------------- | :------------------ |
| 1   |                    |                     |
| 2   |                    |                     |

#### Question 12 - Slow start

_MSS = 1,460 B, initial cwnd = 10 segments, doubling each RTT, no loss. File = 13,332,733 B, so segments = ceil(size / 1460). Find the number of RTTs n where 10 × (2^n − 1) ≥ segments. Shortest time = n × RTT using your min connect time (add the handshake RTT if you want to be strict, and say so). Compare with the measured transfer time. Sections 3.5 and 3.7._

| Mirror | Segments | RTTs needed | RTT (ms) | Shortest time (ms) | Measured (ms) |
| :----- | :------- | :---------- | :------- | :----------------- | :------------ |

### Chapter 4 - The Network Layer

#### Question 13 - Private and public addresses

_Local address comes from the 4-tuple, public from `checkip`. RFC 1918 ranges: 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16. Behind a NAT if the local and public addresses differ._

| Vantage   | Local address | Public address (checkip) | Private (RFC 1918)? | Behind NAT? |
| :-------- | :------------ | :----------------------- | :------------------ | :---------- |
| Laptop    |               |                          |                     |             |
| Onyx      |               |                          |                     |             |
| Codespace |               |                          |                     |             |

#### Question 14 - The NAT table

_Pick a vantage behind a NAT and one connection. Draw the section 4.3 table: WAN side (public address and port) and LAN side (local address and port). The NAT's public port is not visible from the client, so say why._

| WAN side | LAN side |
| :------- | :------- |
|          |          |

#### Question 15 - Subnetting

_Use the `ip -4 addr` output (or the laptop's equivalent) in each header. For each vantage point give the interface address and prefix length, network address, broadcast address, usable host range and number of usable hosts (2^(32−p) − 2). Show the binary for one of them._

| Vantage | Address/prefix | Network | Broadcast | Usable range | Usable hosts |
| :------ | :------------- | :------ | :-------- | :----------- | :----------- |

---

## Task 6 - Catch the Agent

- **Quote from Agent Session:**
  > "[Paste exact statement here]"
- **Verification Method:** [Man page / Textbook section / Specific measurement]
- **Explanation & Correct Answer:** [Explain why the agent was incorrect/incomplete and state the correct behavior/concept]
