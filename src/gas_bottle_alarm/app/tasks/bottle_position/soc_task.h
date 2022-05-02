#ifndef __SOC_TASK_H__
#define __SOC_TASK_H__

#include <stdint.h>
#include <stdbool.h>
#include <esp32_utilities.h>
#include <HardwareSerial.h>


class Soc_task{
	
	private:
	//drivers 
		SystemOnChip esp;
	public:
		HardwareSerial &get_serial_driver(void);

	//args
		void *task_args;
		
	//events
		int  *events_out;
		int  *events_input;

	
	public :
		Soc_task(void);

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

#endif /*__Soc_task_H__*/