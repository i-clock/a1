ESP32 Battery Life Eatimation
---

To calculate how long the ESP32-DevKitC will last on 4 AA batteries, we need to consider the current consumption in different modes and the total capacity of the batteries.

## Assumptions and Estimates:

### 1. **AA Battery Capacity**
Let's assume standard alkaline AA batteries. A typical capacity for an alkaline AA battery is around **2500 mAh** (milliampere-hours). Since there are 4 batteries, we'll consider them in series for voltage (6V total, likely regulated down to 3.3V for the ESP32) but the **total mAh capacity remains that of a single battery** if they are in series, assuming the regulator efficiency is handled separately. For simplicity, we'll assume the system draws current from a theoretical 2500mAh reservoir. More accurately, we should consider the energy in Watt-hours (Wh) for the batteries.
- Energy per AA battery: 1.5V * 2500mAh = 3.75 Wh
- Total energy for 4 AA batteries: 4 * 3.75 Wh = **15 Wh**
- The ESP32-DevKitC typically operates at 3.3V. The 4 AA batteries in series provide 6V, so a voltage regulator will be used. We need to account for its efficiency. A typical linear regulator might have 50-70% efficiency when stepping down from 6V to 3.3V, especially at low currents. For power calculation, let's assume an average efficiency of **70%** for the regulator.
- Effective battery capacity at 3.3V: (15 Wh * 0.70) / 3.3V = **3.18 Ah or 3180 mAh** (at 3.3V). We will use this effective mAh value for easier calculation with current draws.

### 2. ESP32 Current Consumption:
- **Deep Sleep**: The ESP32 itself can go into very low deep sleep (around 5-10 µA). However, the DevKitC board includes components like the USB-to-serial converter (CP2102 or CH340C) and a voltage regulator, which significantly increase deep sleep current.
    * A typical ESP32-DevKitC in deep sleep can draw **3-11 mA** (due to the onboard components). Let's use an average of **5 mA** for deep sleep on a standard DevKitC. Some optimized boards or removal of these components can get it much lower, but for a stock DevKitC, this is a reasonable estimate.
- **Active (Chime/Blink)**: When the ESP32 wakes up to blink the LED and perform calculations, it will be in active mode.
    * ESP32 active current (CPU only, Wi-Fi off): Around **20-50 mA**. Let's use **30 mA**.
    * LED current: A standard LED typically draws **5-20 mA**. Let's assume **10 mA** for a visible blink.
    * Total active current (ESP32 + LED): 30 mA + 10 mA = **40 mA**.
- **NTP Sync (Wi-Fi On)**: Connecting to Wi-Fi and performing an NTP sync is the most power-intensive operation.
    * ESP32 with Wi-Fi connected can draw **80-150 mA** (with spikes up to 300-500 mA during transmission). Let's average this at **120 mA** for the duration of the sync.

## Cycle Breakdown and Energy Consumption:

Let's break down a 1-hour cycle:

### Quarter Chimes (4 times per hour):
- Each chime involves the LED blinking once for 200ms.
- Time active per chime: ~200ms (LED on) + short wake-up and processing time. Let's estimate 1 second of active time per chime (to include wake-up, processing, and LED).
- Current consumption during chime: 40 mA
- Energy per chime: (40 mA * 1s) = 40 mAs = 0.011 mAh (40 mA * 1/3600 h)

### Hourly Chime (1 time per hour):
- Blinks for the number of hours (e.g., 1 blink at 1 AM, 12 blinks at 12 PM/AM).
- Each blink is 100ms, except noon (200ms).
- Average blinks per hour: Let's assume an average of 6.5 blinks per hour (sum of 1 to 12 blinks for 12 hours, divided by 12, then doubled for 24-hour cycle: ((1+12)*12/2)/12 = 6.5).
- Time per average hourly chime (active): 6.5 blinks * 100ms/blink = 650ms. Noon is 12 blinks * 200ms/blink = 2400ms. Let's average this to roughly 1 second of active time for hourly chime.
- Current consumption during hourly chime: 40 mA
- Energy per hourly chime: (40 mA * 1s) = 40 mAs = 0.011 mAh

### Deep Sleep:
- After a quarter chime, there's a 14-minute deep sleep, followed by 1 minute deep sleep. This is one 15-minute period.
- Total deep sleep time per hour: (14 min + 1 min) * 4 quarters = 60 minutes.
- However, the question states "14mins after quarter chime, follow by 1min deep sleep". This sounds like a total of 15 minutes of deep sleep for each 15-minute segment (quarter). This means the device is almost always in deep sleep, except for the very brief active periods.
- Let's refine this:
    * Quarter chime activity: ~1 second (as calculated above)
    * Hourly chime activity: ~1 second (as calculated above, averaged). This occurs in addition to one of the quarter chimes.
    * NTP sync activity: Occurs every 3 hours.
- So, in a 15-minute quarter period, the device is active for ~1 second for the chime, and then deep sleeps for the remaining ~14 minutes and 59 seconds.
- Total active time per hour (assuming 4 quarter chimes and 1 hourly chime):
    * 4 * 1s (quarter chimes) + 1 * 1s (hourly chime) = 5 seconds active time.

### NTP Sync:
- Every 3 hours. Let's assume it takes 15 seconds to connect to Wi-Fi and sync time.
- Current consumption during NTP sync: 120 mA
- Energy per NTP sync: (120 mA * 15s) = 1800 mAs = 0.5 mAh

## Total Daily Energy Consumption Calculation (24 hours):

### 1. Deep Sleep Energy:
- Total time in 24 hours: 24 * 60 * 60 = 86400 seconds.
- Total active time for chimes per day: 5 seconds/hour * 24 hours = 120 seconds.
- Total active time for NTP syncs per day: 8 NTP syncs * 15 seconds/sync = 120 seconds.
- Total active time: 120 + 120 = 240 seconds.
- Total deep sleep time: 86400 - 240 = 86160 seconds.
- Deep sleep current: 5 mA
- Deep sleep energy: (5 mA * 86160 s) = 430800 mAs = **119.67 mAh**
### 2. Chime (Blink) Energy:
- Daily quarter chimes: 4 chimes/hour * 24 hours = 96 chimes.
- Energy for quarter chimes: 96 * 0.011 mAh = **1.056 mAh**
- Daily hourly chimes: 24 hourly chimes.
- Energy for hourly chimes: 24 * 0.011 mAh = **0.264 mAh** (This is an approximation based on average blinks. The actual energy for hourly chimes will vary depending on the specific hour, but this is a good average).
- Total chime energy: 1.056 mAh + 0.264 mAh = **1.32 mAh**
### 3. NTP Sync Energy:
- Number of NTP syncs per day: 24 hours / 3 hours = 8 syncs.
- Energy for NTP syncs: 8 * 0.5 mAh = **4 mAh**

## Total Daily Consumption:

Total Daily Current Consumption = Deep Sleep Energy + Chime Energy + NTP Sync Energy

Total Daily Current Consumption = 119.67 mAh + 1.32 mAh + 4 mAh = 124.99 mAh

## Battery Life Calculation:

Battery Capacity (effective at 3.3V) = 3180 mAh

Daily Consumption = 124.99 mAh

Battery Life (Days) = Total Battery Capacity / Daily Consumption

Battery Life (Days) = 3180 mAh / 124.99 mAh/day = 25.44 days

## Conclusion:

Under these assumptions, the ESP32-DevKitC board powered by 4 standard AA alkaline batteries would last approximately **25 to 26 days**.

## Important Considerations and Factors that can affect battery life:
- **Actual Deep Sleep Current**: This is the most critical factor. If the DevKitC has a particularly inefficient regulator or other components drawing power in deep sleep, the life will be significantly shorter. Conversely, highly optimized designs can achieve much lower deep sleep currents (tens of microamps), leading to months or even years of battery life.
- **AA Battery Chemistry**: Lithium AA batteries offer higher capacity (e.g., 3000-3400 mAh) and better performance at low temperatures and high discharge rates, which would extend the life. NiMH rechargeable batteries (typically 2000-2700 mAh) have lower nominal voltage (1.2V), which might be an issue for the regulator, but are rechargeable.
- **Voltage Regulator Efficiency**: A more efficient buck converter (switching regulator) would significantly improve battery life compared to a linear regulator, especially with the 6V input from 4 AA batteries.
- **Actual Active Time**: The estimated 1 second active time for chimes and 15 seconds for NTP sync are approximations. Longer active times will reduce battery life.
- **LED Brightness**: A brighter LED draws more current.
- **Temperature**: Battery capacity decreases at lower temperatures.
- **Self-Discharge of Batteries**: Batteries lose charge over time even when not in use. Alkaline batteries have a relatively low self-discharge rate, but it's still a factor over longer periods.

---
Extracted from Google Gemini
