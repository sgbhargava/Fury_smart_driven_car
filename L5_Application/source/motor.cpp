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
	motor_speed = getword(temp, 0);
	motor_rpm = getword(temp,1);
	return true;
}

bool motor_class::send_motor_throttle()
{
	can_msg_t motor_throttle_can_mess;
	motor_throttle_can_mess.msg_id = id_motor_throttle;
	motor_throttle_can_mess.data.bytes[0] = throttle;
	motor_throttle_can_mess.frame_fields.data_len = 1;
	motor_throttle_can_mess.frame_fields.is_29bit = 0;
	if(!CAN_tx(can1, &motor_throttle_can_mess,0))
		return false;
	return true;
}
bool motor_class::send_motor_steering()
{
	can_msg_t motor_steering_can_mess;
	motor_steering_can_mess.msg_id = id_motor_steering;
	motor_steering_can_mess.data.bytes[0] = motor_steering;
	motor_steering_can_mess.frame_fields.data_len = 0x01;
	motor_steering_can_mess.frame_fields.is_29bit = 0;

	if(!CAN_tx(can1, &motor_steering_can_mess,0))
		return false;
	return true;
}
bool motor_class::stop()
{
	throttle = 0x01;
	return true;
}

bool motor_class::reverse()
{
	throttle = 0x0C;
	return true;
}
bool motor_class::custom_1()
{
	throttle = 0x0E;
	return true;
}
bool motor_class::custom_2()
{
	throttle = 0x16;
	return true;
}
bool motor_class::custom_3()
{
	throttle = 0x1E;
	return true;
}
motor_class* motor_class::single = NULL;
motor_class* motor_class::getInstance()
{
	if(single == NULL)
		single = new motor_class();
	return single;
}


bool motor_class::get_heartbeat()
{
	uint64_t temp;
	return get_data(id_heart_beat, &temp);

}

bool motor_class::reset()
{
	can_msg_t motor_can_mess;
	motor_can_mess.msg_id = id_reset;
	motor_can_mess.data.bytes[0] = 0x00;
	motor_can_mess.frame_fields.data_len = 1;
	motor_can_mess.frame_fields.is_29bit = 0;
	if(!CAN_tx(can1, &motor_can_mess,0))
		return false;
	return true;
}
