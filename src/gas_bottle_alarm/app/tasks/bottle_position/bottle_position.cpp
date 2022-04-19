/**
 * @file bottle_position.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "bottle_position.h"


Bottle_position::Bottle_position(char axis, int angle, int tolerance) {
		axis_to_monitor  = axis;
		angle_to_trigger = angle;
    	angle_tolerance  = tolerance; 	
};


Bottle_position::thread(void) {};

//getters
uint8_t Bottle_position:: get_angle(axis_t *axis);
uint8_t Bottle_position:: get_bottle_state(uint8_t *state)

//setters
uint8_t Bottle_position::set_angle(axis_t *axis);

//thread 
void Bottle_position::thread(void);

//drivers : class friend
void Bottle_position::driver(void);
