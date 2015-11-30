/*
 * master_class.hpp
 *
 *  Created on: Nov 27, 2015
 *      Author: Bhargav
 */

#ifndef MASTER_CLASS_HPP_
#define MASTER_CLASS_HPP_

#include "CAN_base_class.hpp"
#include "IO_bridge.hpp"
#include "geo_controller.hpp"
#include "motor.hpp"
#include "sensors.hpp"

class master_class:public CAN_base_class
{
public:

	static master_class* getInstance();
	bool master_class_init();
	bool heartbeat();

private:
	static master_class *single;
	sensor_class *sensor;
	motor_class *motor;
	geo_controller_class *geo_controller;
	IO_base_class *IO_controller;
	uint16_t id_reset_motor = 0x020;
	uint16_t id_reset_sensor = 0x220;
	uint16_t id_reset_communication_bridge = 0x420;
	uint16_t id_dest_geo_controller = 0x321;
	uint16_t id_disp_communication_bridge = 0x421;
};



#endif /* MASTER_CLASS_HPP_ */
