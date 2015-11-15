/*
 * motor.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: SCS_USER
 */
#include "motor.hpp"

bool motor_class::motor_class_init()
{
	add_can_id(id_heart_beat,id_motor_status);

}

bool motor_class::get_motor_status()
{
	uint64_t temp;
	if(! get_data(id_motor_status, &temp))
		return false;
	motor_speed = getdword(temp, 0);
	motor_rpm = getdword(temp, 1);
	return true;
}

