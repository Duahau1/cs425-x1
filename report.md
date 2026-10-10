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

_Explain: why the minimum, not the mean, estimates propagation. Processing, queuing and transmission delay can only add to the propagation delay, never subtract (section 1.4), so the smallest sample has the least added delay and is closest to the true floor. The laptop's mean is 1.69 times its minimum because of queuing, from Wi-Fi and the T-Mobile link, while Onyx and Codespace, on wired data-center links, vary by under 0.4 ms. If I had used the laptop mean, the ratio in Question 1 would have been inflated by queuing that has nothing to do with distance._

#### Question 4 - Transmission delay

_L = 64 × 8 = 512 bits. t = L / R. Do R = 10 Mbit/s and 1 Gbit/s. Compare with your Utah propagation RTT._

#### Question 5 - Throughput

_Use the 3 trials of each Packages.gz download. Table: vantage, mirror, Mbit/s per trial. Is the access link the bottleneck in both cases? Evidence: does Utah throughput differ from Japan on the same link?_

| Vantage | Mirror | Trial 1 (Mbit/s) | Trial 2 | Trial 3 |
| :------ | :----- | :--------------- | :------ | :------ |

### Chapter 2 - The Application Layer

#### Question 6 - DNS caching

_One mirror per vantage point. DNS time of trial 1 versus trials 2 to 5. Local DNS server and cache, TTL (section 2.4)._

| Vantage | Mirror | Trial 1 (ms) | Trial 2 | Trial 3 | Trial 4 | Trial 5 |
| :------ | :----- | :----------- | :------ | :------ | :------ | :------ |

#### Question 7 - The DNS hierarchy

_Use the `dig +trace` from a vantage where it worked (not Onyx). List the servers in order and what each answered (referral or address). Map to root, TLD (`.jp`), authoritative (section 2.4)._

| Step | Server asked | Answer (referral or address) | Level in hierarchy |
| :--- | :----------- | :--------------------------- | :----------------- |

#### Question 8 - The CDN

_Did `example.com` get the same address at all three vantage points? Compare its connect time with the mirrors. Section 2.6: DNS redirection versus anycast. Mention the CF-RAY colo code if you have it._

| Vantage | Address | Min connect (ms) |
| :------ | :------ | :--------------- |

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
