#ifndef __DUMMY_TASK_H__
#define __DUMMY_TASK_H__

#include <stdint.h>
#include <stdbool.h>



class Dummy_task_app {
	
	private:
	//drivers 
			
	//args
		void *task_args;
	//events
		int  *events_out;
		int  *events_input;

	
	public :
		Dummy_task_app(void);

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

#endif /*__DUMMY_TASK_H__*/