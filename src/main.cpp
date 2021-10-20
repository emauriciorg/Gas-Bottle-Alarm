#include <Arduino.h>
#include <gas_bottle_alarm/app/app.h>

#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/soc_ulp.h"
#include "soc/sens_reg.h"
#include "soc/rtc_i2c_reg.h"

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/ulp_macros.html
// https://github.com/wardjm/esp32-ulp-i2c

BottleBirdApp app;

void setup()
{
    app.begin();
}

void loop() {}

// #include <Wire.h>
// #include <SPI.h>
// #include <Adafruit_LIS3DH.h>
// #include <Adafruit_Sensor.h>

// Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// void setup(void)
// {
//     //* 1. read i2c sensor or go to sleep
//     Serial.begin(115200);

//     if (!lis.begin(0x18))
//     { // change this to 0x19 for alternative i2c address
//         Serial.println("Couldnt start");
//         while (1)
//             yield();
//     }
//     Serial.println("LIS3DH found!");

//     lis.setRange(LIS3DH_RANGE_2_G); // 2, 4, 8 or 16 G!

//     Serial.print("Range = ");
//     Serial.print(2 << lis.getRange());
//     Serial.println("G");

//     lis.setDataRate(LIS3DH_DATARATE_10_HZ);
//     Serial.print("Data rate set to: ");
//     switch (lis.getDataRate())
//     {
//     case LIS3DH_DATARATE_1_HZ:
//         Serial.println("1 Hz");
//         break;
//     case LIS3DH_DATARATE_10_HZ:
//         Serial.println("10 Hz");
//         break;
//     case LIS3DH_DATARATE_25_HZ:
//         Serial.println("25 Hz");
//         break;
//     case LIS3DH_DATARATE_50_HZ:
//         Serial.println("50 Hz");
//         break;
//     case LIS3DH_DATARATE_100_HZ:
//         Serial.println("100 Hz");
//         break;
//     case LIS3DH_DATARATE_200_HZ:
//         Serial.println("200 Hz");
//         break;
//     case LIS3DH_DATARATE_400_HZ:
//         Serial.println("400 Hz");
//         break;

//     case LIS3DH_DATARATE_POWERDOWN:
//         Serial.println("Powered Down");
//         break;
//     case LIS3DH_DATARATE_LOWPOWER_5KHZ:
//         Serial.println("5 Khz Low Power");
//         break;
//     case LIS3DH_DATARATE_LOWPOWER_1K6HZ:
//         Serial.println("16 Khz Low Power");
//         break;
//     }

//     pinMode(34, INPUT);
//     pinMode(39, INPUT);
//     pinMode(36, INPUT);
// }

// void loop()
// {
//     sensors_event_t event;
//     lis.getEvent(&event);

//     /* Display the results (acceleration is measured in m/s^2) */
//     Serial.print("X: ");
//     Serial.print(event.acceleration.x);
//     Serial.print("adc_reading: ");
//     Serial.print(analogRead(39));
//     Serial.print(" \tY: ");
//     Serial.print(event.acceleration.y);
//     Serial.print("adc_reading: ");
//     Serial.print(analogRead(36));
//     Serial.print(" \tZ: ");
//     Serial.print(event.acceleration.z);
//     Serial.print("adc_reading: ");
//     Serial.print(analogRead(34));
//     Serial.println(" m/s^2 ");

//     delay(200);
// }
