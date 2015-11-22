/*
 * IO_bridge.hpp
 *
 *  Created on: Nov 14, 2015
 *      Author: bhargav
 */

#ifndef L5_APPLICATION_IO_BRIDGE_HPP_
#define L5_APPLICATION_IO_BRIDGE_HPP_

#include "CAN_base_class.hpp"
#include "geo_controller.hpp"

class IO_base_class:public CAN_base_class
{
public:
		static IO_base_class* get_Instance();
		uint16_t id_heart_beat = 0x180;
		uint16_t id_drive_mode = 0x181;
		uint16_t id_location = 0x182;
		long_lat *lat_long_from_IO = new long_lat;
		uint8_t drive_auth =0;

		bool IO_base_class_init();
		bool get_drive_authotization();
		bool get_location_details();

private:
		static IO_base_class *single;

};
	#endif /* L5_APPLICATION_IO_BRIDGE_HPP_ */
