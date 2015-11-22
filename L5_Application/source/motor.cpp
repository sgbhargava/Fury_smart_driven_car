/*
 * motor.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: Bhargav
 */
#include "motor.hpp"
motor_class::motor_class()
{

}

bool motor_class::motor_class_init()
{
	printf("init motor\n");
	motor_throttle_can_mess.msg_id = id_motor_throttle;
	motor_steering_can_mess.msg_id = id_motor_steering;
	if(!add_can_id(id_heart_beat,id_motor_status))
	{
	printf("false\n");
	return false;
	}
//printf("true\n");
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
	if(!CAN_tx(can1, &motor_throttle_can_mess,0))
		return false;
	return true;
}
bool motor_class::send_motor_steering()
{
	motor_steering_can_mess.data.bytes[0] = motor_steering;
	motor_steering_can_mess.frame_fields.data_len = 1;
	motor_steering_can_mess.frame_fields.is_29bit = 0;
	printf("id is %x\n",motor_steering_can_mess.msg_id);
	printf("data is %d\n", motor_steering_can_mess.data.bytes[0]);
	if(!CAN_tx(can1, &motor_steering_can_mess,0))
		return false;
	return true;
}

motor_class* motor_class::single = NULL;
motor_class* motor_class::getInstance()
{
	if(single == NULL)
		single = new motor_class();
	return single;
}

