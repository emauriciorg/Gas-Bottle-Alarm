#ifndef __LEDS_TASK_H__
#define __LEDS_TASK_H__

#include <stdint.h>
#include <stdbool.h>

#define NUMPIXELS 12


typedef struct led_color_st{
        uint8_t red;
        uint8_t green;
        uint8_t blue;
}led_color_t;


class Leds_task{
	
	private:
	//drivers 
		led_color_t led_table[NUMPIXELS];
	//args
		void *task_args;
	//events
		int  *events_out;
		int  *events_input;

	
	public :
		Leds_task(void);

	//getters
		uint8_t get_args(void *args);
		void get_driver_event(void);

	//setters
		uint8_t set_args(void *args);

    //thread : 
		void start_task(void *task_data); //passes arguments into task
	
	//drivers : 
		void begin(void); // calls initial drivers 

	private: 
		 static void soft_thread(void *ptask_instance);
	
};

#endif /*__Leds_TASK_H__*/