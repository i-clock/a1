# ESP32 LED Clock

> **Supported Target:** ESP32  
> **Note:** This code was generated using ChatGPT for iOS version 1.2024.347 on an iPhone 8.

This project implements an ESP32-based clock that connects to Wi-Fi, obtains the current time via NTP, and blinks an LED based on the time of day. It also supports deep sleep to conserve power between operations.

---

## 🕹️ Functionality

- In its default configuration:
  - Blinks the LED once every 15 minutes (at 00, 15, 30, and 45 minutes past the hour).
  - At the top of the hour, blinks a number of times corresponding to the current hour (e.g., 12 blinks at noon).
  - The noon chime blinks more slowly than regular hourly blinks.
- The device enters deep sleep and wakes:
  - Every 840 seconds after a quarter-hour chime.
  - Then 60 seconds for normal checks.

---

## 💻 Software Environment

- **IDE:** Visual Studio Code with the ESP-IDF extension  
- **Framework:** ESP-IDF v5.3.3  
- **Operating System:** Debian GNU/Linux 12 (Bookworm)

```bash
pi@raspberrypi:~/esp $ cat /etc/os-release
PRETTY_NAME="Debian GNU/Linux 12 (bookworm)"
...

pi@raspberrypi:~/esp $ cat /proc/version
Linux version 6.12.25+rpt-rpi-v8 ...
```

> ⚙️ **Before compiling and flashing:**  
> Make sure to update your Wi-Fi SSID and password:  
> **VSCode > ESP-IDF Explorer > SDK Configuration Editor**

---

## 📋 Project Overview

### 🔧 Features

- Connects to Wi-Fi and syncs time via NTP.
- Blinks an LED based on the current time:
  - Every 15 minutes
  - Hourly chimes, with blink count matching the hour
- Supports three chime modes:
  - All chimes (default)
  - Hourly-only
  - Noon-only
- Saves power by using deep sleep.
- Displays 2 blinks on startup, and 10 fast blinks on Wi-Fi failure.
- Resynchronizes NTP time approximately every 24 wake cycles (~120 minutes).

### 😴 Deep Sleep Behavior

- **First Boot:** Connects to Wi-Fi and synchronizes the time via SNTP.
- **Subsequent Wakeups:** Uses RTC to maintain time without resyncing.
- **Time Zone:** Singapore (UTC+8)
- **Wake Count:** Stored in RTC memory
- **Sleep Duration:**
  - 780 seconds after a chime
  - 60 seconds during regular operation

---

## 🕒 Example Blink Durations

| Event        | Blink Count | Delay per Blink | Total Duration |
|--------------|-------------|------------------|----------------|
| 3 AM Hourly  | 3           | 100 ms           | ~0.6 sec       |
| 12 PM (Noon) | 12          | 200 ms           | ~6 sec         |

> 🔁 A brief pause is inserted after every three blinks to improve clarity.

---

## 🔌 Hardware

The board includes an onboard LED connected to **GPIO2** by default.

### Optional External LED Setup

| LED Pin | ESP32 Pin | Note                         |
|---------|-----------|------------------------------|
| Anode   | GPIO2     | Configurable via `menuconfig`|
| Cathode | GND       | Through a 220Ω resistor      |

![ESP32 LED Clock](./img/a2025esp32LedClock3.jpeg)

---

**Author:** Alvin T W Ng © 2025
