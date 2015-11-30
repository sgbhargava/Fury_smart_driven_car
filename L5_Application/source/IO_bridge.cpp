/*
 * IO_bridge.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: bhargav
 */
#include "IO_bridge.hpp"

IO_base_class* IO_base_class::single = NULL;
IO_base_class* IO_base_class::get_Instance()
{
	if(single == NULL)
		single = new IO_base_class();
	return single;
}

bool IO_base_class::get_heartbeat()
{
	uint64_t temp;
	if(get_data(id_heart_beat, &temp))
	{
		return true;
	}
	return false;
}
bool IO_base_class::IO_base_class_init()
{
	add_can_id(id_heart_beat, id_drive_mode);
	add_can_id(id_location, 0xFFFF);
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

bool IO_base_class::reset()
{
	can_msg_t IO_controller_can_mess;
	IO_controller_can_mess.msg_id = id_reset;
	IO_controller_can_mess.data.bytes[0] = 0x00;
	IO_controller_can_mess.frame_fields.data_len = 1;
	IO_controller_can_mess.frame_fields.is_29bit = 0;
	if(!CAN_tx(can1, &IO_controller_can_mess,0))
		return false;
	return true;
}
