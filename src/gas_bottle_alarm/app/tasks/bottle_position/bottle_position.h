#ifndef __BOTTLE_POSITION_H__
#define __BOTTLE_POSITION_H__


#include <stdint.h>
#include <Adafruit_LIS3DH.h>
#include <stdbool.h>




typedef struct task_bottle_args_st {
	char axis;
	int angle;
	char axis_to_monitor;
	int  angle_to_trigger;
    int  angle_tolerance; 
}bottle_arg_t;



class Bottle_position: public Adafruit_LIS3DH {
	
	//drivers 
	Adafruit_LIS3DH accelerometer;

	//args
	
	void *task_args;
	bottle_arg_t args;
	//events
		int  *events_out;
		int  *events_input;

	
	public :
		Bottle_position(char axis, int angle, int tolerance);//, event_group_t
		Bottle_position(void);

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


#endif /*__BOTTLE_POSITION_H__*/
