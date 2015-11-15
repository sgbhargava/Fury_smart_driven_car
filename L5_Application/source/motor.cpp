/*
 * motor.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: Bhargav
 */
#include "motor.hpp"

bool motor_class::motor_class_init()
{
	motor_throttle_can_mess.msg_id = id_motor_throttle;
	motor_steering_can_mess.msg_id = id_motor_steering;
	if(!add_can_id(id_heart_beat,id_motor_status))
	return false;
return true;
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

bool motor_class::send_motor_throttle()
{
	if(!CAN_rx(can1, &motor_throttle_can_mess,0))
		return false;
	return true;
}
bool motor_class::send_motor_steering()
{
	if(!CAN_rx(can1, &motor_steering_can_mess,0))
		return false;
	return true;
}
