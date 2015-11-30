/*
 * geo_controller.cpp
 *
 *  Created on: Nov 13, 2015
 *      Author: tbalachandran
 */
#include "geo_controller.hpp"


geo_controller_class* geo_controller_class::single = NULL;
geo_controller_class* geo_controller_class::getInstance()
{
	if(single == NULL)
		single = new geo_controller_class();
	return single;
}

bool geo_controller_class::get_heartbeat()
{
	uint64_t temp;
	if(get_data(id_heartbeat, &temp))
	{
		return true;
	}
	return false;
}

bool geo_controller_class::geo_controller_class_init()
{
	if(!add_can_id(id_heartbeat,id_compass_heading_data ))
		printf("unable to add HB id\n");
	add_can_id(id_gps_coordinates,id_gps_distance );
	add_can_id(id_gps_checkpoint_req, 0x170);

	printf("geo add added\n");
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
	return true;
}

bool geo_controller_class::geo_controller_send_coordinates()
{
	can_msg_t geo_controller_can_mess;
	geo_controller_can_mess.msg_id = id_gps_coordinates;
	geo_controller_can_mess.data.qword = *(uint64_t *)lat_long_data;
	geo_controller_can_mess.frame_fields.data_len = 8;
	geo_controller_can_mess.frame_fields.is_29bit = 0;
	if(!CAN_tx(can1, &geo_controller_can_mess,0))
		return false;
	return true;
}

bool geo_controller_class:: reset()
{
	can_msg_t geo_controller_can_mess;
		geo_controller_can_mess.msg_id = id_reset;
		geo_controller_can_mess.data.bytes[0] = 0x00;
		geo_controller_can_mess.frame_fields.data_len = 1;
		geo_controller_can_mess.frame_fields.is_29bit = 0;
		if(!CAN_tx(can1, &geo_controller_can_mess,0))
			return false;
		return true;

}
