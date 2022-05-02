#ifndef __DUMMY_TASK_H__
#define __DUMMY_TASK_H__

#include <stdint.h>
#include <stdbool.h>



class Dummy_task_app {
	
	//drivers 
	
	//args
	
	void *task_args;
	//events
		int  *events_out;
		int  *events_input;

	
	public :
		Dummy_task_app(char axis, int angle, int tolerance);//, event_group_t
		Dummy_task_app(void);

	//getters
		uint8_t get_angle(char axis);
		uint8_t get_bottle_state(uint8_t *state);
		void get_driver_event(void);

	//setters
		uint8_t set_angle(char axis);

    //thread : friend class from esp32

		void start_task(void *task_data); //passes arguments into task
	
	//drivers : 
		void begin(void); // calls initial drivers 

	private: 
		 static void soft_thread(void *ptask_instance);
	
};

#endif /*__DUMMY_TASK_H__*/