#ifndef __APP_EVENTS_H__
#define __APP_EVENTS_H__

#include <stdint.h>
#include <esp32_utilities.h>


extern const uint8_t buf_len ;
extern const char command[8] ;
extern const int msg_queue_len ;
extern QueueHandle_t msg_queue;
extern QueueHandle_t led_msg;


typedef struct app_events_st {
	int soft_delay;
} app_events_t;


typedef struct led_events_st {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} led_events_t;

 class App_events{
	 public: 
	 	void begin(void);

 };


#endif /*__APP_EVENTS_H__*/