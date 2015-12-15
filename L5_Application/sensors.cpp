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
	if(get_data(id_sensors, &temp))
	{
		lidar = getword(temp,0);
		if(lidar == 0)
			lidar = 200;
		left = getword(temp, 1);
		right = getword(temp, 2);
		back = getword(temp, 3);
		return true;
	}
	return false;
}

bool sensor_class::sensor_class_init()
{

	add_can_id(id_heart_beat,id_sensors);
	add_can_id(id_battery, 0x150);
	return true;
}
sensor_class* sensor_class::single = NULL;
sensor_class* sensor_class::getInstance()
{
	if(single == NULL)
	{
		single = new sensor_class();
	}

	return single;
}

bool sensor_class::get_heartbeat()
{
	uint64_t temp;
	return get_data(id_heart_beat, &temp);

}

bool sensor_class::reset()
{
	can_msg_t sensor_can_mess;
	sensor_can_mess.msg_id = id_reset;
	sensor_can_mess.data.bytes[0] = 0x00;
	sensor_can_mess.frame_fields.data_len = 1;
	sensor_can_mess.frame_fields.is_29bit = 0;
	if(!CAN_tx(can1, &sensor_can_mess,0))
		return false;
	return true;
}

bool sensor_class::get_battery()
{
	uint64_t temp;
	if(get_data(id_battery, &temp))
	{
		battery = getbyte(temp,1);
		return true;
	}
}
