/*
 * motor.hpp
 *
 *  Created on: Nov 14, 2015
 *      Author: SCS_USER
 */

#ifndef L5_APPLICATION_SOURCE_MOTOR_HPP_
#define L5_APPLICATION_SOURCE_MOTOR_HPP_
#include "CAN_base_class.hpp"

class motor_class:public CAN_base_class
{
	uint16_t id_heart_beat = 0x100;
	uint16_t id_motor_status = 0x102;

	uint32_t motor_speed = 0;
	uint32_t motor_rpm = 0;
	bool motor_class_init();
	bool get_motor_status();

};




#endif /* L5_APPLICATION_SOURCE_MOTOR_HPP_ */
