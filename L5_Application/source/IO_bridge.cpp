/*
 * IO_bridge.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: bhargav
 */
#include "IO_bridge.hpp"

bool IO_base_class::IO_base_class_init()
{
	add_can_id(id_heart_beat, id_drive_mode);
	add_can_id(id_location, 0xFF);
	return false;
}

bool IO_base_class::get_drive_authotization()
{
	uint64_t temp;
	if(!get_data(id_drive_mode, &temp))
		return false;
	drive_auth = getbyte(temp, 0);
	return true;
}

bool IO_base_class::get_location_details()
{
	uint64_t temp;
	if(!get_data(id_location, &temp))
		return false;
	lat_long_from_IO = (long_lat *)temp;
	return true;
}


