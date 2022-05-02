/**
 * @file bottle_position.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "bottle_position.h"
#include <esp32_utilities.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include "app_events.h"

#define APP_TASK_NAME "bottle_position"


static const char* LOG_TAG = APP_TASK_NAME;

//uint8_t bottle_state_msg[][] = {"ON","OFF"}

Bottle_position::Bottle_position(void) {

}
Bottle_position::Bottle_position(char axis, int angle, int tolerance) {
		args.axis_to_monitor  = axis;
		args.angle_to_trigger = angle;
		args.angle_tolerance  = tolerance; 	
};


void Bottle_position::start_task(void *task_data){
	 ::xTaskCreatePinnedToCore(soft_thread,
							APP_TASK_NAME,
							10000,
							this,
							22,
							NULL,
						   (0));
}

void Bottle_position::begin(void) {
	//TODO : call the system class esp
	// esp.i2c0.begin(21, 22);
	accelerometer = Adafruit_LIS3DH();
   if (!accelerometer.begin(0x18))
	{
		printf("[Error ]Accelerometer init \r\n");
		abort();
	}
} 


void Bottle_position::get_driver_event() {

}

void Bottle_position::soft_thread(void *ptask_instance) {

	Bottle_position* bottle = (Bottle_position*) ptask_instance;  
	sensors_event_t sensor_event;
	app_events_t events;
	

	bottle->begin();

	while (1){
		if (xQueueReceive(msg_queue, (void *)&events, 0) == pdTRUE) {
			printf("led delay %d\r\n",events.soft_delay);
		}


		bottle->accelerometer.getEvent(&sensor_event);
		printf("Accelerometer Z : %f ,X : %f \r\n",
			sensor_event.acceleration.z,
			sensor_event.acceleration.x);
		vTaskDelay(200/portTICK_PERIOD_MS);

		if (sensor_event.acceleration.z > bottle->args.angle_to_trigger) {
			printf("Angle got triggered \r\n");
		}
	}	

}



#if 0
//getters
uint8_t Bottle_position:: get_angle(axis_t *axis);
uint8_t Bottle_position:: get_bottle_state(uint8_t *state)

//setters
uint8_t Bottle_position::set_angle(axis_t *axis);

//thread 
void Bottle_position::thread(void);

//drivers : class friend
void Bottle_position::driver(void);
#endif