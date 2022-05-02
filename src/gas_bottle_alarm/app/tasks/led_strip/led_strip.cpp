#include "led_strip.h"

// TODO: How to start custom blinking sessions after
void breath();
void refreshLED(void *parameters);

uint8_t brightness;
uint8_t minimum_brightness = 40;
uint8_t maximum_brightness = 120;

String color;

void setColor(String color_requested)
{
    color = color_requested;
    led_strip.clear();
    led_strip.show();
}

void setupLED_Strip(void *parameters)
{
    //* 1. Enabled LED's by driving LED GND pin
    pinMode(LED_GND_ENABLE, OUTPUT);
    digitalWrite(LED_GND_ENABLE, HIGH);
    vTaskDelay(10 / portTICK_PERIOD_MS); // Allow some time to turn on

    //* 2. Begin & clear pixels
    led_strip.begin();
    led_strip.clear();
    led_strip.show();

    color = "white";

    xTaskCreatePinnedToCore(refreshLED, "Refresh LED", 1000, NULL, 25, NULL, xPortGetCoreID());

    vTaskDelete(NULL);
}




void refreshLED(void *parameters)
{
    bool breath_in = true;
    while (1)
    {

        for (int i = 0; i < 5; i++)
        {
            if (color == "white")
            {
                led_strip.setPixelColor(i, led_strip.Color(brightness, brightness, brightness));
                led_strip.show();
            }

            if (color == "red")
            {
                led_strip.setPixelColor(i, led_strip.Color(brightness, 0, 0));
                led_strip.show();
            }

            if (color == "green")
            {
                led_strip.setPixelColor(i, led_strip.Color(0, brightness, 0));
                led_strip.show();
            }

            if (color == "blue")
            {
                led_strip.setPixelColor(i, led_strip.Color(0, 0, brightness));
                led_strip.show();
            }
        }

        if (breath_in)
        {
            brightness = brightness + 4;

            if (brightness == maximum_brightness)
                breath_in = false;
        }

        if (!breath_in)
        {
            brightness = brightness - 4;

            if (brightness == minimum_brightness)
                breath_in = true;
        }

        vTaskDelay(LED_BLINKING_PERIOD_mS / portTICK_PERIOD_MS);
    }
}
