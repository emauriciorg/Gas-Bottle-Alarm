#include "app_events.h"
#include <stdint.h>

 const uint8_t buf_len = 255;     // Size of buffer to look for command
 const char command[] = "delay "; // Note the space!
 const int delay_queue_len = 5;   // Size of delay_queue
 const int msg_queue_len = 5;     // Size of msg_queue
 const uint8_t blink_max = 100;   // Num times to blink before message

 QueueHandle_t msg_queue;




void  App_events::begin(void){
	msg_queue = xQueueCreate(msg_queue_len, sizeof(app_events_t));
}