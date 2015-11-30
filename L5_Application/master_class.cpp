/*
 * master_class.cpp
 *
 *  Created on: Nov 27, 2015
 *      Author: Bhargav
 */

#include "master_class.hpp"

master_class* master_class::single = NULL;
master_class* master_class::getInstance()

{
	if(single == NULL)
		single = new master_class;
	return single;
}

bool master_class::master_class_init()
{
	printf("init master\n");

	if(!add_can_id(id_reset_motor,id_reset_sensor))
	{
	printf("false\n");
	return false;
	}
	if(!add_can_id(id_reset_communication_bridge, 0xFF))
	{
		printf("false\n");
			return false;
	}
	if(!add_can_id(id_dest_geo_controller, id_disp_communication_bridge))
	{
		printf("false\n");
			return false;
	}
//printf("true\n");
	return true;
}

