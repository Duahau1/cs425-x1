# Project Report

**Name:** Van Nguyen
**Class:** CS452
**Project Name:** X1 - Anatomy of a Page Load

## Task 3 - Predictions

### 1. Distance vs. Delay (Theoretical Minimum Connect Time)

_Assumed propagation speed in fiber:_ $2 \times 10^8\text{ m/s}$ ($200\text{ km/ms}$).

| Target Mirror | URL | Location | Estimated Distance (RTT km) | Predicted Min Connect Time (ms) |
| :------------ | :-- | :------- | :-------------------------- | :------------------------------ |
| **CDN** | <http://example.com/> | Nearest POP | [Insert km] | [Insert ms] |
| **Utah** | <http://mirrors.xmission.com/debian/README> | Salt Lake City, UT | [Insert km] | [Insert ms] |
| **California** | <http://mirrors.ocf.berkeley.edu/debian/README> | Berkeley, CA | [Insert km] | [Insert ms] |
| **New York** | <http://mirrors.rit.edu/debian/README> | Rochester, NY | [Insert km] | [Insert ms] |
| **Germany** | <http://ftp.fau.de/debian/README> | Erlangen, Germany | [Insert km] | [Insert ms] |
| **Japan** | <http://ftp.jaist.ac.jp/debian/README> | Nomi, Japan | [Insert km] | [Insert ms] |

**Calculation Method / Work:**

- _Example formula:_ $\text{Min Connect Time (ms)} = \frac{2 \times \text{One-Way Distance (km)}}{200\text{ km/ms}}$
- [Add notes on how you calculated or measured distances from your location]

---

### 2. Longest Connect Time from Codespace

- **Predicted Mirror:** [Insert Mirror Name]
- **Reasoning:** [Explain why based on geographical distance, fiber routing, and transit paths relative to the Codespace data center region].

---

### 3. DNS Lookup Duration (Trial 1 vs. Trials 2–5)

- **Prediction:** [Will DNS time increase, decrease, or stay the same?]
- **Reasoning:** [Explain in terms of local DNS resolver behavior, caching mechanisms, TTL, and cache hits vs. cache misses].

---

### 4. NAT Identification

- **Laptop:** [Behind NAT / Public IP]
- **Onyx:** [Behind NAT / Public IP]
- **Codespace:** [Behind NAT / Public IP]
- **Reasoning:** [Briefly justify based on expected local IP address ranges (RFC 1918) vs. public IP structures].

---

### 5. Transfer Time: Utah vs. Japan (13 MB File)

- **Prediction:** [Will downloading the file take about the same time or significantly different time?]
- **Reasoning:** [Explain considering round-trip time (RTT), TCP congestion window dynamics/slow start, packet loss, bandwidth-delay product (BDP), and TCP throughput limits].

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
