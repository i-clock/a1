ESP32 LED Clock - Setup Guide
---

## Setup Using Command-Line Tools
To build, configure, and flash the ESP32 firmware without using VSCode or any IDE, you can rely solely on the ESP-IDF command-line tools. Below is a complete step-by-step guide using only the terminal.

### ✅ 1. Install ESP-IDF
Follow the official instructions based on your operating system:
- **Linux/macOS (Terminal):**
    ```bash
    mkdir esp
    git clone --recursive https://github.com/espressif/esp-idf.git
    cd esp-idf
    ./install.sh
    ```
- **Windows (PowerShell):**
    ```powershell
    git clone --recursive https://github.com/espressif/esp-idf.git
    cd esp-idf
    .\install.ps1
    ```

### ✅ 2. Load Your Project
Download and unzip your project:
```bash
unzip esp32LedClock.zip
cd esp32LedClock
```

### ✅ 3. Set Up the Environment
Before running any `idf.py` commands, source the environment script:
- **Linux/macOS:**
    ```bash
    . $HOME/esp/esp-idf/export.sh
    ```
- **Windows (PowerShell):**
    ```powershell
    . $env:USERPROFILE\esp\esp-idf\export.ps1
    ```

### ✅ 4. Configure with MenuConfig
Use `idf.py menuconfig` to update settings:
```bash
idf.py menuconfig
```

You will see a terminal-based configuration menu. Navigate using the arrow keys:
- Go to **Component config** → **ESP32 LED CLOCK Configuration**
- Update the following:
  - Wi-Fi SSID
  - Wi-Fi Password
  - Chime Mode (ALL, HOURLY_ONLY, NOON_ONLY)
  - Blink Durations
- Press `Esc` twice to exit and save changes.

### ✅ 5. Build the Project
```bash
idf.py build
```

### ✅ 6. Flash to ESP32
- Connect your ESP32 and run:
	```bash
	idf.py -p /dev/ttyUSB0 flash monitor
	```
- Replace `/dev/ttyUSB0` with your actual port (use `ls /dev/ttyUSB*` on Linux or check Device Manager on Windows).
- Or use auto-detect:
	```bash
	idf.py flash monitor
	```

### ✅ 7. Done!
You're now running the firmware with your custom settings.  
The monitor will display logs. Press `Ctrl+]` to exit.

---
---
## Setup Using Visual Studio Code (VSCode)

### Extensions Required
- ESP-IDF Extension `v5.3.3` (latest at the time of writing)
- Ensure **ESP-IDF: Explorer** appears in the sidebar

### Test the Connection
Use the built-in `scan` example to test communication with your ESP32.

Steps:
1. Open **ESP-IDF: Explorer**
2. Select the correct port (e.g., `/dev/ttyUSB0`)
3. Build the project
4. Flash the device using `UART`
5. Monitor output from the device

---
### Update Configuration with MenuConfig
To update Wi-Fi SSID and password:
- Open **ESP-IDF: Explorer**
- Go to **SDK Configuration Editor (menuconfig)**

Make changes under **ESP32 LED CLOCK Configuration**.

---
### Sample Monitoring, `ESP_LOGI` `MAIN`
``` nginx
pi@raspberrypi:~/esp32LedClock $ . $HOME/esp/v5.3.3/esp-idf/export.sh
...
pi@raspberrypi:~/esp32LedClock $ idf.py build flash
...
pi@raspberrypi:~/esp32LedClock $ idf.py monitor
...
I (538) main_task: Started on CPU0
I (548) main_task: Calling app_main()
I (568) MAIN: First boot or reset detected. Running initial setup.
I (1368) MAIN:                   LED blinked 2 times with 200 ms delay
...
I (1538) wifi:mode : sta (ec:e3:34:bf:c4:d8)
I (1548) wifi:enable tsf
I (1548) MAIN: Connecting to Wi-Fi...
...
I (6548) MAIN: Connected to WiFi
I (6548) MAIN: Waiting for time...
I (6748) MAIN:                   LED blinked 1 times with 100 ms delay
I (8748) MAIN: Waiting for time...
I (8948) MAIN:                   LED blinked 1 times with 100 ms delay
I (10248) wifi:<ba-add>idx:1 (ifx:0, 5e:ef:68:30:2c:b9), tid:0, ssn:0, winSize:64
I (10948) MAIN: Waiting for time...
I (11148) MAIN:                   LED blinked 1 times with 100 ms delay
I (13148) MAIN: Time obtained.
...
I (13198) MAIN: Current time resync: Sat Jun 21 13:54:24 2025

I (13198) MAIN: https://github.com/43525/esp32LedClock
I (13698) MAIN: Sleeping... Will wake up after 60 seconds
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
...
I (537) main_task: Started on CPU0
I (547) main_task: Calling app_main()
I (567) MAIN: Woken up from deep sleep. Wake cause: 4, Wake count: 2
I (1067) MAIN: Sleeping... Will wake up after 60 seconds
...
...
I (537) main_task: Started on CPU0
I (547) main_task: Calling app_main()
I (567) MAIN: Woken up from deep sleep. Wake cause: 4, Wake count: 7
I (1167) MAIN:                   LED blinked 1 times with 300 ms delay
I (5267) MAIN:                   LED blinked 14 times with 100 ms delay
I (5267) MAIN: Sleeping... Will wake up after 840 seconds
...
```

---
