#include "deep_sleep.h"

#define uS_TO_mS_FACTOR 1000
#define TIME_TO_SLEEP_mS 3000

void startDeepSleep()
{
    //* 1. Setup user button wake up source
    esp_sleep_enable_ext0_wakeup((gpio_num_t)USER_BUTTON_PIN, 1);

    //* 2. Periodic timer to check for bottle state
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_mS * uS_TO_mS_FACTOR);

    //* TODO: 3. Real time clock wake up

    //* TODO: 4. Setup light sensor wake up

    //* TODO: 5. Setup hardware for deep sleep

    esp.uart0.println("\n\n");
    esp.uart0.print("Power off");
    esp_deep_sleep_start();
}