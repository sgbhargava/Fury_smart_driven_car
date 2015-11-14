/*
 * IO_bridge.hpp
 *
 *  Created on: Nov 14, 2015
 *      Author: bhargav
 */

#ifndef L5_APPLICATION_IO_BRIDGE_HPP_
#define L5_APPLICATION_IO_BRIDGE_HPP_

#include "CAN_base_class.hpp"

typedef struct long_lat{
	        uint32_t lattitude_dec :8;
	        uint32_t lattitude_float :20;
	        uint32_t longitude_dec :8;
	        uint32_t longitude_float :20;
	        uint32_t checkpoint:8;
}long_lat __attribute__((packed));

class IO_base_class:public CAN_base_class
{
		uint16_t id_heart_beat = 0x180;
		uint16_t id_drive_mode = 0x181;
		uint16_t id_location = 0x182;
		long_lat *lat_long_from_IO = new long_lat;
		uint8_t drive_auth =0;

		bool IO_base_class_init();
		bool get_drive_authotization();
		bool get_location_details();

};
	#endif /* L5_APPLICATION_IO_BRIDGE_HPP_ */
