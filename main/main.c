/*
    LED Chime Clock
    This code implements a simple LED chime clock that connects to Wi-Fi, obtains the current
     time from an NTP server, and blinks an LED at specified intervals based on the time.
    It supports a test mode for simulating various scenarios and uses FreeRTOS for task
     management and delays.

    @author Alvin T W Ng
*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "esp_attr.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_netif.h"
#include "sdkconfig.h"

// Configuration for Wi-Fi credentials and blink delays at menuConfig

#define TAG "MAIN"

// Set up test mode flag from menuconfig. Set to 1 for test mode
#ifdef CONFIG_TEST_MODE
    #define TEST_MODE 1
#else
    #define TEST_MODE 0
#endif

RTC_DATA_ATTR static int wake_count = 0;
static int sleep_duration; // after quarter chimes, seconds

static void blink_led(int times, int delay_ms); // phototype

static bool connect_wifi() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));

    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Connecting to Wi-Fi...");
    ESP_ERROR_CHECK(esp_wifi_connect());
    vTaskDelay(pdMS_TO_TICKS(5000));

    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        ESP_LOGI(TAG, "Connected to WiFi");
        return true;
    } else {
        ESP_LOGE(TAG, "WiFi connection failed. Blinking error pattern.");
        blink_led(10, 100);  // rapid blinks indicate WiFi failure
        return false;
    }
}

static void obtain_time() {
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();

    time_t now = 0;
    struct tm timeinfo = { 0 };

    while (timeinfo.tm_year < (2020 - 1900)) {
        ESP_LOGI(TAG, "Waiting for time...");
        blink_led(1, 100); // blink while waiting 
        time(&now);
        localtime_r(&now, &timeinfo);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Time obtained.");
}

static void getWifiTime() {
    bool wifi_ok = connect_wifi();
    if (wifi_ok) {
        obtain_time();
        esp_wifi_stop();
    }
    ESP_LOGI(TAG, "Current time resync: %s", asctime(localtime(&(time_t){time(NULL)})));
}

static void blink_led(int times, int delay_ms) {
    gpio_set_direction(CONFIG_LED_GPIO, GPIO_MODE_OUTPUT);
    for (int i = 1; i <= times; ++i) {
        gpio_set_level(CONFIG_LED_GPIO, 1);
        vTaskDelay(delay_ms / portTICK_PERIOD_MS);
        gpio_set_level(CONFIG_LED_GPIO, 0);
        vTaskDelay(delay_ms / portTICK_PERIOD_MS);
        if (i % 3 == 0) {
            vTaskDelay(2 * delay_ms / portTICK_PERIOD_MS); // blank blink after every 3 blinks
        }
    }
    ESP_LOGI(TAG, "                  LED blinked %d times with %d ms delay", times, delay_ms);
}

void between_blinks_delay() {
    // This function is used to add a delay between blinks
    vTaskDelay(CONFIG_BETWEEN_BLINKS_DELAY / portTICK_PERIOD_MS);
}

void chimesTimes(int hour, int minute) {
    // This function handles the chime logic based on the current time
    if (minute % 15 == 0) {
        #if CONFIG_CHIME_MODE_ALL
            blink_led(1, CONFIG_QUARTER_HOUR_BLINK_DELAY);  // Quarter-hour chime
        #endif
        sleep_duration = 60 * 14;    // Set sleep duration to 14 minutes after chime
        // ESP_LOGI(TAG, "Chime at %02d:%02d", hour, minute);
    }
    between_blinks_delay();
    #if CONFIG_CHIME_MODE_ALL || CONFIG_CHIME_MODE_HOURLY
        if (minute == 0) {
            // Noon chime - slower, more distine. Hourly blink - faster
            int count = (hour == 12) ? 12 : ((hour % 24 == 0) ? 24 : hour % 24);
            int delay = (hour == 12) ? CONFIG_NOON_BLINK_DELAY : CONFIG_HOURLY_BLINK_DELAY;
            blink_led(count, delay);
        }
    #endif
    #if CONFIG_CHIME_MODE_NOON_ONLY
        if (hour == 12 && minute == 0) {
            blink_led(12, CONFIG_NOON_BLINK_DELAY); // Noon chime
        }
    #endif
}

// This function is only for testing purposes
void test_mode() {
    ESP_LOGI(TAG, "TEST MODE ENABLED");
    ESP_LOGI(TAG, "Simulating first boot LED test...");
    blink_led(2, 200);
    ESP_LOGI(TAG, "Simulating Wi-Fi failure blink...");
    blink_led(10, 100);
    between_blinks_delay();

    int testHour = CONFIG_TEST_MODE_HOUR;
    int testMinute = CONFIG_TEST_MODE_MINUTE;
    ESP_LOGI(TAG, "Simulating chime at %02d:%02d", testHour, testMinute);
    chimesTimes(testHour, testMinute);
    between_blinks_delay();
    
    #if CONFIG_TEST_NOON_BLINK
        ESP_LOGI(TAG, "Simulating time obtain at 12:00 noon.");
        struct tm test_time = {
            .tm_year = 2025 - 1900,
            .tm_mon  = 5 - 1,
            .tm_mday = 29,
            .tm_hour = 12,
            .tm_min  = 0,
            .tm_sec  = 0
        };
        time_t fake_time = mktime(&test_time);
        struct timeval now_val = { .tv_sec = fake_time };
        settimeofday(&now_val, NULL);
    #endif

    ESP_LOGI(TAG, "TEST MODE COMPLETED");
}


void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());

    // Set Singapore timezone (UTC+8)
    setenv("TZ", "SGT-8", 1);
    tzset();

    #if TEST_MODE
        test_mode();
    #endif

    wake_count++;
    sleep_duration = 60; // Reset sleep duration for next wake

    esp_sleep_wakeup_cause_t wake_cause = esp_sleep_get_wakeup_cause();
    // Only run LED test and Wi-Fi setup on first boot (not from deep sleep)
    if (wake_cause == ESP_SLEEP_WAKEUP_UNDEFINED) {
        ESP_LOGI(TAG, "First boot or reset detected. Running initial setup.");
        blink_led(2, 200);  // two short blinks to confirm startup
        getWifiTime();
	ESP_LOGI(TAG, "https://github.com/43525/esp32LedClock");
    } else {
        ESP_LOGI(TAG, "Woken up from deep sleep. Wake cause: %d, Wake count: %d",
             wake_cause, wake_count);
    }

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    //ESP_LOGI(TAG, "Current time: %s", asctime(&timeinfo));

    chimesTimes(timeinfo.tm_hour, timeinfo.tm_min);

    if (wake_count % CONFIG_WAKE_COUNT_TO_RESYNC == 0) {
        getWifiTime(); // This resync costs at least 8sec
    }

    #if !TEST_MODE
        ESP_LOGI(TAG, "Sleeping... Will wake up after %d seconds", sleep_duration);
        esp_sleep_enable_timer_wakeup(sleep_duration * 1000000ULL);
        esp_deep_sleep_start();
    #endif
}
