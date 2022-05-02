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


#include "tasks/bottle_position/bottle_position.h"
#include "tasks/bottle_position/dummy_task.h"
#include "tasks/bottle_position/app_events.h"

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
    setCpuFrequencyMhz(80);

   
    Bottle_position bottle;
    Dummy_task_app dummy_task;
    App_events app_events;

    //TODO: move esp object to a class
    esp.uart0.begin(UART0_BAUD_RATE);
    esp.uart0.println();
    terminal.begin(esp.uart0);
    esp.i2c0.begin(21, 22);

    
    printf("Program started\r\n");

    app_events.begin();
    dummy_task.start_task(0);
    bottle.start_task(0);
    while(1);
   
}

void app_run(void)
{
    
    


}