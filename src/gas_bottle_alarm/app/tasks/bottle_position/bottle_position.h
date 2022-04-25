#include <stdint.h>

typedef struct axis_st {
	char axis;
	int angle;
}axis_t;



class Bottle_position {
	//args
		char axis_to_monitor;
		int  angle_to_trigger;
    	int  angle_tolerance; 

	//events
		int  *events_out;
		int  *events_input;
	public :
		Bottle_position(char axis, int angle, int tolerance);
	
	//getters
		uint8_t get_angle(axis_t *axis);
		uint8_t get_bottle_state(uint8_t *state)
	//setters
		uint8_t set_angle(axis_t *axis);

    //thread : friend class from esp32
		void thread(void);

	//drivers : friend, inheretance? class from driver
		void driver(void);

};



/* init example

  Bottle_position bottle ('x',90,0.5);
*/
