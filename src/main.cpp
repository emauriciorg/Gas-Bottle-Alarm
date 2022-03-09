#include <Arduino.h>
#include <gas_bottle_alarm/app/app.h>

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/ulp_macros.html
// https://github.com/wardjm/esp32-ulp-i2c

BottleBirdApp app;

void setup()
{
    app.begin();
}

void loop() {}
