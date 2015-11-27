/*
 * sensors.hpp
 *
 *  Created on: Nov 7, 2015
 *      Author: Bhargava
 */

#ifndef L5_APPLICATION_SENSORS_HPP_
#define L5_APPLICATION_SENSORS_HPP_
#include "CAN_base_class.hpp"

class sensor_class:public CAN_base_class
{
public:
	uint16_t left = 0;
	uint16_t right = 0;
	uint16_t lidar = 0;
	uint16_t back = 0;
	uint16_t lidar_threshold = 75;
	uint16_t sensor_threshold = 50;
	static sensor_class* getInstance();
	bool get_sensor_reading();
	bool sensor_class_init();
	bool get_heartbeat();
private:
	static sensor_class *single;
	uint16_t id_heart_beat = 0x140;
	uint16_t id_sensors = 0x142;
	uint16_t id_battery = 0x144;

};




#endif /* L5_APPLICATION_SENSORS_HPP_ */
