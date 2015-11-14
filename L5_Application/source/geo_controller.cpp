/*
 * geo_controller.cpp
 *
 *  Created on: Nov 13, 2015
 *      Author: tbalachandran
 */
#include "geo_controller.hpp"

bool geo_controller_class::geo_controller_class_init()
{
	add_can_id(id_heart_beat,id_gps_distance);
	add_can_id(id_gps_coordinates, id_gps_checkpoint_req);
	add_can_id(id_compass_heading_data, 0xFF);
	return true;
}

bool geo_controller_class::get_checkpoint_req()
{
	uint64_t temp;
	if(get_data(id_gps_checkpoint_req,&temp))
	{
		checkpoint_request = getbyte(temp,0);
		return true;
	}
return false;
}

bool geo_controller_class::get_compass_data()
{
	uint64_t temp;
	if(!get_data(id_compass_heading_data, &temp))
		return false;
	compass_data =(compass*)temp;
	return true;
}

bool geo_controller_class::get_coordinates()
{
	uint64_t temp;
	if(!get_data(id_gps_coordinates, &temp))
		return false;
	lat_long_data = (long_lat*) temp;
}

