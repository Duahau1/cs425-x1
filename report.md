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
- **Reasoning:** My Ip address on my machine is 192.168.12.235 after I ran `ipconfig getifaddr en0` so we are definitely behind a NAT. And for Onyx, we are only able to connect to it if we set up the VPN so it needs to be behind some controlled NAT. And then for Codespace, it is running in an isolated Docker container on a virtual machine in Microsoft Azure so it definitely needs a NAT.

---

### 5. Transfer Time: Utah vs. Japan (13 MB File)

- **Prediction:** No they will take different time, Japan has greater download time compared to Utah
- **Reasoning:** So the total time formula is Total Time = 2 x RTT + transmission time of HTML file at the server. 13MB is not large and based on the calculation that we did for 2 X RTT in the prediction #1, so if we assume no package loss and transmission time is the same as bandwidth does not depend on physical distance, the RTT will dominate and the RTT of Japan is a lot greater than Utah

---

## Task 5 - Analysis Questions

### Chapter 1 - Delay and Throughput

#### Question 1

...

#### Question 2

...

#### Question 3

...

#### Question 4

...

#### Question 5

...

### Chapter 2 - The Application Layer

#### Question 6

...

#### Question 7

...

#### Question 8

...

#### Question 9

...

### Chapter 3 - The Transport Layer

#### Question 10

...

#### Question 11

...

#### Question 12

...

### Chapter 4 - The Network Layer

#### Question 13

...

#### Question 14

...

#### Question 15

...

---

## Task 6 - Catch the Agent

- **Quote from Agent Session:**
  > "[Paste exact statement here]"
- **Verification Method:** [Man page / Textbook section / Specific measurement]
- **Explanation & Correct Answer:** [Explain why the agent was incorrect/incomplete and state the correct behavior/concept]
