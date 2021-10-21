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
#include "tasks/settings/settings.h"
#include "tasks/pairing/device_pairing.h"
#include "tasks/wireless_control/wireless_control.h"
#include "tasks/bluetooth_server/ble_server.h"
#include "tasks/led_strip/led_strip.h"
#include "tasks/time_keeper/time_keeper.h"
#include "tasks/deep_sleep/deep_sleep.h"
#include "tasks/wifi_services/wifi_services.h"
#include "WiFi.h"
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

//*****************************************************       DATA TYPES        *****************************************************/
#define uS_TO_mS_FACTOR 1000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP_mS 5000 /* Time ESP32 will go to sleep (in seconds) */

//*****************************************************         OBJECTS         *****************************************************/
RTC_DATA_ATTR DeviceSettings device_settings;

Terminal terminal;
SystemOnChip esp;
BluetoothLowEnergyServer bleServer;
SPIFFS_Memory spiffsMemory;
RealTimeClock rtc;
Adafruit_NeoPixel led_strip(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_LIS3DH accelerometer = Adafruit_LIS3DH();

//*********************************************************       APP       *********************************************************/
void monitorBattery();
void monitorBottlePosition();

void BottleBirdApp::begin()
{
    //* 1. Set CPU frequency to 10 Mhz
    setCpuFrequencyMhz(80);

    //* 2. Begin UART port for debug output
    esp.uart0.begin(UART0_BAUD_RATE);
    esp.uart0.println();
    terminal.begin(&esp.uart0);

    //* 3. Measure if battery voltage is above threshold
    monitorBattery();

    //* 5. If battery above threshold, get wake up source
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    //* 6. Start different modes according to wake up reason
    switch (wakeup_reason)
    {
    //*** USER BUTTON WAKE UP
    //***
    //*** The device will wake up when the user presses the button
    case ESP_SLEEP_WAKEUP_EXT0:
    {
        if (device_settings.device_is_setup)
        {
            //***** If device is setup, go into control mode
            wirelessControlMode();
        }

        else
        {
            //***** If device is not setup, go into pairing mode
            wirelessPairingMode();
        }

        //***** Both methods above have timeouts, error handling, and they set the device to sleep. App never gets
        abort();

        break;
    }

    //*** TIMER WAKE UP
    //***
    //*** The device sets a periodic timer to wake up & check bottle position
    case ESP_SLEEP_WAKEUP_TIMER:
    {
        //***** 6.1 Monitor bottle position
        //*
        //* Description: During this mode the unit will monitor the Z-axis accelerometer reading.
        //*
        //*              If a change in position is detected (bottle is opened or bottle is closed) the device
        //*              will:
        //*
        //*              1. Connect to the network provided by the user
        //*              2. Delete the document in Firestore containing the device settings & information
        //*              3. Create the same document in Firestore with the new settings & information
        //*
        //*              If the WiFi connection or the database queries fail, the device will display the error
        //*              to the terminal & save such error in memory. These can then be seen through the mobile
        //*              application when the user connects to the device using Bluetooth.
        //*

        monitorBottlePosition();

        //***** 6.2 Handle user button press while monitoring bottle
        // wirelessControlMode();

        //***** 6.3 End terminal
        terminal.end();

        //***** 6.4 Start deep sleep
        startDeepSleep();

        break;
    }

    //*** OTHER WAKE UP SOURCES
    //***
    //*** The device will go to sleep again. These shouldn't happen unless an error happened
    default:
        terminal.printMessage(TerminalMessage("First boot or reset detected",
                                              "SLE", WARNING, micros()));

        // TODO: Handle error if unit reset.

        terminal.end();

        esp.uart0.println("\n\n");
        esp.uart0.print("Power off");

        esp_sleep_enable_ext0_wakeup((gpio_num_t)USER_BUTTON_PIN, 1);
        esp_deep_sleep_start();
        break;
    }
}

//*************************************************       FUNCTION DEFINITIONS       *********************************************************/
void monitorBattery()
{
    uint16_t raw_battery_adc_value = analogRead(VBAT_3V3_PIN);
    double battery_voltage = 2 * ((raw_battery_adc_value * 3.3) / 4095); // 1/2 voltage divier

    terminal.printMessage(TerminalMessage(
        "Battery voltage -> " + String(battery_voltage) + " V. Threshold is -> " + String((double)MINIMUM_BAT_VOLTAGE_FOR_BOOT_mV / 1000),
        "SLE", INFO, micros()));

    //* 4. Go back to sleep if voltage too low. Device will only wake up again from user button
    if ((battery_voltage * 1000) <= MINIMUM_BAT_VOLTAGE_FOR_BOOT_mV)
    {
        //* 4.1 Enable LED strip
        pinMode(LED_GND_ENABLE, OUTPUT);
        digitalWrite(LED_GND_ENABLE, HIGH);
        vTaskDelay(5 / portTICK_PERIOD_MS); // Allow some time to turn on

        //* 4.2 Begin & clear pixels
        led_strip.begin();
        led_strip.clear();
        led_strip.show();

        //* 4.3 Display red LEDs to indicate low battery
        for (int i = 0; i < NUMPIXELS; i++)
        {
            led_strip.setPixelColor(i, led_strip.Color(50, 0, 0)); // RGB/
            led_strip.show();
        }

        terminal.printMessage(TerminalMessage("Battery voltage is too low, starting deep sleep",
                                              "SLE", INFO, micros()));
        terminal.end();

        vTaskDelay(100 / portTICK_PERIOD_MS);

        //* 4.4 Turn off LED strip
        digitalWrite(LED_GND_ENABLE, LOW);

        esp.uart0.println("\n\n");
        esp.uart0.print("Power off");

        esp_sleep_enable_ext0_wakeup((gpio_num_t)USER_BUTTON_PIN, 1);
        esp_deep_sleep_start();
    }
}

void monitorBottlePosition()
{
    /* Timer wake up means device is simply checking for bottle position*/
    terminal.printMessage(TerminalMessage("Wakeup caused by sleep timer",
                                          "SLE", INFO, micros()));

    //* 1. Begin I2C port for accelerometer
    esp.i2c0.begin(21, 22);

    //* 2. Begin accelerometer
    if (!accelerometer.begin(0x18))
    {
        esp.uart0.println("\nCouldnt initialize accelerometer");
        abort();
    }

    //* 3. Get acceleration values
    sensors_event_t event;
    accelerometer.getEvent(&event);

    //* 4. Monitor bottle state
    bool state_update = false;

    // 4.1 If bottle is positioned up (LED's facing up), then bottle is OFF
    if (event.acceleration.z > 0)
    {
        if (device_settings.bottle_is_open)
        {
            state_update = true;
            device_settings.bottle_is_open = false;

            terminal.printMessage(TerminalMessage("Bottle is OFF, logging finish time",
                                                  "SLE", INFO, micros()));
        }
    }

    // 4.2 If bottle is positioned down (LED's facing down), then bottle is ON
    else
    {
        if (!device_settings.bottle_is_open)
        {
            state_update = true;
            device_settings.bottle_is_open = true;

            terminal.printMessage(TerminalMessage("Bottle is ON, logging start time",
                                                  "SLE", INFO, micros()));
        }
    }

    //* 5. Write to database if state changes
    if (state_update)
    {
        ESP_ERROR update_firebase = updateFirebase();

        if (update_firebase.on_error)
        {
            terminal.printMessage(TerminalMessage(update_firebase.debug_message,
                                                  "FIR", ERROR, micros()));
        }

        state_update = false;
    }
    else
    {
        terminal.printMessage(TerminalMessage("No position update",
                                              "FIR", ERROR, micros()));
    }
}

// End.