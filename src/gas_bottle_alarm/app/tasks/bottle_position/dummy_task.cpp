#include "dummy_task.h"

/**
 * @file Dummy_task_app.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "dummy_task.h"
#include <esp32_utilities.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include "app_events.h"

#define APP_TASK_NAME "Dummy_task_app"


static const char* LOG_TAG = APP_TASK_NAME;

//uint8_t bottle_state_msg[][] = {"ON","OFF"}

Dummy_task_app::Dummy_task_app(void) {

}
Dummy_task_app::Dummy_task_app(char axis, int angle, int tolerance) {
	
};


void Dummy_task_app::start_task(void *task_data){
     ::xTaskCreatePinnedToCore(soft_thread,
                            APP_TASK_NAME,
                            10000,
                            this,
                            22,
                            NULL,
                           (0));
}

void Dummy_task_app::begin(void) {

	// esp.i2c0.begin(21, 22);
   
} 


void Dummy_task_app::get_driver_event() {

}

void Dummy_task_app::soft_thread(void *ptask_instance) {

    Dummy_task_app* task_instance = (Dummy_task_app*) ptask_instance;  
    task_instance->begin();
	app_events_t events;
	

	while (1){

        // if (xQueueReceive(msg_queue, (void *)&rcv_msg, 0) == pdTRUE) {
        //     Serial.print(rcv_msg.body);
        //     Serial.println(rcv_msg.count);
        // }
        
         
      //  printf("Accelerometer Z : %f ,X : %f \r\n",event.acceleration.z,event.acceleration.x);
        vTaskDelay(500/portTICK_PERIOD_MS);
		events.soft_delay++;
		if (xQueueSend(msg_queue, (void *)&events, 10) != pdTRUE) {
            printf("ERROR: Could not put item on delay queue.\r\n");
          }
            //	get_driver_event() blocking queue;


    }	

}

