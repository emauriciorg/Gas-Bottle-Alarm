/*
 * File Name: app.cpp
 * Project: Gas Bottle Alarm
 * Version: 1.0
 * Compartible Hardware: REV1.0
 */

//*****************************************************        LIBRARIES        *****************************************************/
#include "gas_bottle_alarm/app/app.h"
#include "tasks/rtos/rtos.h"
#include "tasks/terminal/terminal.h"
#include "tasks/wake_up_manager/deep_sleep.h"
#include "tasks/settings/settings.h"
#include "tasks/pairing/device_pairing.h"
#include "tasks/bluetooth_server/ble_server.h"
#include "tasks/led_strip/led_strip.h"
#include "tasks/main_app/main_app.h"
#include "tasks/time_keeper/time_keeper.h"

#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

//*****************************************************       DATA TYPES        *****************************************************/

//*****************************************************         OBJECTS         *****************************************************/
DeviceSettings device_settings;

Terminal terminal;
SystemOnChip esp;
BluetoothLowEnergyServer bleServer;
SPIFFS_Memory spiffsMemory;
RealTimeClock rtc;

Adafruit_NeoPixel led_strip(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_LIS3DH accelerometer = Adafruit_LIS3DH();

#define uS_TO_mS_FACTOR 1000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP_mS 1000 /* Time ESP32 will go to sleep (in seconds) */

void beginMainApp();

void start_deep_sleep()
{
    //* 1. Setup wake up timer of 500 ms
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_mS * uS_TO_mS_FACTOR);

    //* 2. Start deep sleep
    esp_deep_sleep_start();
}

//*********************************************************       APP       *********************************************************/
void BottleBirdApp::begin()
{
    //* 1. Set CPU frequency to 10 Mhz
    setCpuFrequencyMhz(80);

    //* 2. Begin UART port for debug output
    esp.uart0.begin(UART0_BAUD_RATE);
    esp.uart0.println();
    terminal.begin(&esp.uart0);

    //* 3. Measure if battery voltage is above threshold
    uint16_t raw_battery_adc_value = analogRead(VBAT_3V3_PIN);
    double battery_voltage = 2 * ((raw_battery_adc_value * 3.3) / 4095); // 1/2 voltage divier

    terminal.printMessage(TerminalMessage(
        "Battery voltage -> " + String(battery_voltage) + " V. Threshold is -> " + String((double)MINIMUM_BAT_VOLTAGE_FOR_BOOT_mV / 1000),
        "SLE", INFO, micros()));

    //* 4. Go back to sleep if voltage too low. Device will only wake up again from user button
    // if ((battery_voltage * 1000) <= MINIMUM_BAT_VOLTAGE_FOR_BOOT_mV)
    // {
    //     //* 4.1 Enable LED strip
    //     pinMode(LED_GND_ENABLE, OUTPUT);
    //     digitalWrite(LED_GND_ENABLE, HIGH);
    //     vTaskDelay(5 / portTICK_PERIOD_MS); // Allow some time to turn on

    //     //* 4.2 Begin & clear pixels
    //     led_strip.begin();
    //     led_strip.clear();
    //     led_strip.show();

    //     //* 4.3 Display red LEDs to indicate low battery
    //     for (int i = 0; i < NUMPIXELS; i++)
    //     {
    //         led_strip.setPixelColor(i, led_strip.Color(50, 0, 0)); // RGB/
    //         led_strip.show();
    //     }

    //     terminal.printMessage(TerminalMessage("Battery voltage is too low, starting deep sleep",
    //                                           "SLE", INFO, micros()));
    //     terminal.end();

    //     vTaskDelay(100 / portTICK_PERIOD_MS);

    //     //* 4.4 Turn off LED strip
    //     digitalWrite(LED_GND_ENABLE, LOW);

    //     esp.uart0.println("\n\n");
    //     esp.uart0.print("Power off");

    //     esp_sleep_enable_ext0_wakeup((gpio_num_t)USER_BUTTON_PIN, 1);
    //     esp_deep_sleep_start();
    // }

    //* 5. If battery above threshold, get wake up source
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:

        //* 5.0 Set CPU to full clock speed
        setCpuFrequencyMhz(240);

        //* 5.1 Interrupt wake up means user pressed the button
        terminal.printMessage(TerminalMessage("Wakeup caused by user button",
                                              "SLE", INFO, micros()));

        //* 5.2 Check if device has to read settings from SPIFFS memory
        if (!app.device_settings.settings_read)
        {
            long initial_read_time = micros();

            ESP_ERROR get_settings_from_memory = getSettingsFromMemory();

            if (get_settings_from_memory.on_error)
            {
                terminal.printMessage(TerminalMessage(get_settings_from_memory.debug_message,
                                                      "SLE", ERROR, micros()));

                abort();
            }

            terminal.printMessage(TerminalMessage("Settings file read from memory",
                                                  "SLE", INFO, micros(), micros() - initial_read_time));

            app.device_settings.settings_read = true;
        }

        //* 5.3 Start pairing or start main app
        if (!app.device_settings.device_is_setup)
        {
            terminal.printMessage(TerminalMessage("Start pairing",
                                                  "SLE", INFO, micros()));
        }
        else
        {
            terminal.printMessage(TerminalMessage("Starting main app",
                                                  "SLE", INFO, micros()));

            beginMainApp();
        }

        break;
    case ESP_SLEEP_WAKEUP_TIMER:

        //* Timer wake up means device is simply checking for bottle position
        terminal.printMessage(TerminalMessage("Wakeup caused by timer",
                                              "SLE", INFO, micros()));

        esp.i2c0.begin(21, 22);

        if (!accelerometer.begin(0x18))
        {
            esp.uart0.println("\nCouldnt initialize accelerometer");
            abort();
        }

        terminal.printMessage(TerminalMessage("Setting up wake up interrupt",
                                              "SLE", INFO, micros()));

        break;
    default:
        //* Any other wake up reason is unknown.
        terminal.printMessage(TerminalMessage("First boot or reset detected",
                                              "SLE", WARNING, micros()));

        terminal.end();

        esp.uart0.println("\n\n");
        esp.uart0.print("Power off");

        // TODO: Setup deep sleep from wake up button
        esp_sleep_enable_ext0_wakeup((gpio_num_t)USER_BUTTON_PIN, 1);
        esp_deep_sleep_start();
        break;
    }

    // xTaskCreatePinnedToCore(setupDeepSleepManager,
    //                         "Deep Sleep Setup",
    //                         10000,
    //                         NULL,
    //                         24,
    //                         NULL,
    //                         1);

    // xTaskCreatePinnedToCore(setupSPIFFS,
    //                         "Deep Sleep Setup",
    //                         30000,
    //                         NULL,
    //                         23,
    //                         NULL,
    //                         0);

    // xTaskCreatePinnedToCore(pairingTask,
    //                         "Pairing Task",
    //                         10000,
    //                         NULL,
    //                         22,
    //                         NULL,
    //                         0);

    // xTaskCreatePinnedToCore(startMainApp,
    //                         "Main app",
    //                         10000,
    //                         NULL,
    //                         21,
    //                         NULL,
    //                         1);

    // xTaskCreatePinnedToCore(bleServerTask,
    //                         "BLE server Task",
    //                         10000,
    //                         NULL,
    //                         21,
    //                         NULL,
    //                         0);

    // vTaskDelete(NULL); // Delete setup & loop RTOS tasks.
}

void beginMainApp()
{
    //* 1. Print App title
    esp.uart0.println("\n\n");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                          ANZE - Gas Bottle Alarm                                         *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    //* 2. Setup RTOS first. Always.
    ESP_ERROR rtos_initialized = app.setupRTOS();

    if (rtos_initialized.on_error)
    {
        esp.uart0.println(rtos_initialized.debug_message);
        esp.uart0.println("\n\n");
        esp.uart0.println("Stopping boot.");
        while (1)
        {
        }
    }

    // //* 3. Create tasks if RTOS was setup correctly
    xTaskCreatePinnedToCore(setupTerminal,
                            "Terminal Setup",
                            10000,
                            NULL,
                            25,
                            NULL,
                            0);

    xTaskCreatePinnedToCore(setupLED_Strip,
                            "LED Setup",
                            1000,
                            NULL,
                            25,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(setupTimeKeeper,
                            "Time Keeper",
                            10000,
                            NULL,
                            22,
                            NULL,
                            1);

    vTaskDelete(NULL);
}

// End.