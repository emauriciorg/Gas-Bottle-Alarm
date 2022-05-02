#include "soc_task.h"

/**
 * @file Soc_task.cpp
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
#include "../../../device/soc_settings.h"
#define APP_TASK_NAME "Soc_task"
static const char* LOG_TAG = APP_TASK_NAME;


Soc_task::Soc_task(void) {

}


void Soc_task::start_task(void *task_data){
     ::xTaskCreatePinnedToCore(soft_thread,
                            APP_TASK_NAME,
                            10000,
                            this,
                            22,
                            NULL,
                           xPortGetCoreID());
}

void Soc_task::begin(void) {

    setCpuFrequencyMhz(80);
    esp.uart0.begin(UART0_BAUD_RATE);
    esp.uart0.println();
	esp.i2c0.begin(I2C0_SDA_PIN_APP, I2C0_SCL_PIN_APP);  
} 


void Soc_task::get_driver_event() {

}

HardwareSerial& Soc_task:: get_serial_driver(void){
    return  (esp.uart0);
}  

void Soc_task::soft_thread(void *ptask_instance) {

    Soc_task* task_instance = (Soc_task*) ptask_instance;  
    task_instance->begin();
	app_events_t events;
	

	while (1){

		vTaskDelay(500/portTICK_PERIOD_MS);
		events.soft_delay++;
		if (xQueueSend(msg_queue, (void *)&events, 10) != pdTRUE) {
			printf("ERROR: Could not put item on delay queue.\r\n");
		}
    }	

}

