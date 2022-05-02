#include "led_task.h"

/**
 * @file Leds_task.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <esp32_utilities.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include "app_events.h"

#define APP_TASK_NAME "Leds_task"
static const char* LOG_TAG = APP_TASK_NAME;



Leds_task::Leds_task(void) {

}


void Leds_task::start_task(void *task_data){
	 ::xTaskCreatePinnedToCore(soft_thread,
							APP_TASK_NAME,
							10000,
							this,
							22,
							NULL,
						   xPortGetCoreID());
}

void Leds_task::begin(void) {

	//* 1. Enabled LED's by driving LED GND pin
//	pinMode(LED_GND_ENABLE, OUTPUT);
//	digitalWrite(LED_GND_ENABLE, HIGH);
  //  vTaskDelay(10 / portTICK_PERIOD_MS); // Allow some time to turn on

	//* 2. Begin & clear pixels
	//led_strip.begin();
//	led_strip.clear();
   

} 


void Leds_task::get_driver_event() {

}

void Leds_task::soft_thread(void *ptask_instance) {

	Leds_task* task_instance = (Leds_task*) ptask_instance;  
	task_instance->begin();
	
	//led_strip.show();
	

	uint8_t red;
	uint8_t green;
	uint8_t blue;
	bool breath_in = true;
	uint8_t brightness;
	uint8_t minimum_brightness = 40;
	uint8_t maximum_brightness = 120;


	led_events_t events;

	while (1){

		xQueueReceive(led_msg, (void *)&events, 2);


	
		red    = events.red  * brightness;
		green  = events.green* brightness;
		blue  =  events.blue * brightness;
	
		for (int i = 0; i < NUMPIXELS; i++)
		{
			//if (task_instance->led_table[i].red == false) continue;
		
	//		led_strip.setPixelColor(i, led_strip.Color(red, green, blue));
//			led_strip.show();
		 
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

	//	vTaskDelay(LED_BLINKING_PERIOD_mS / portTICK_PERIOD_MS);
	}	

}

