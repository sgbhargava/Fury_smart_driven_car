/*
 * sensors.cpp
 *
 *  Created on: Nov 7, 2015
 *      Author: Bhargava
 */
#include "sensors.hpp"

bool sensor_class::get_sensor_reading()
{
	uint64_t temp;
	if(get_data(sensors, &temp))
	{
		lidar = getword(temp,0);
		left = getword(temp, 1);
		right = getword(temp, 2);
		back = getword(temp, 3);
		return true;
	}
	return false;
}



