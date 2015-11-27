/*
 * geo_controller.hpp
 *
 *  Created on: Nov 13, 2015
 *      Author: tbalachandran
 */

#ifndef L5_APPLICATION_SOURCE_GEO_CONTROLLER_HPP_
#define L5_APPLICATION_SOURCE_GEO_CONTROLLER_HPP_
#include "CAN_base_class.hpp"

	typedef struct long_lat{
	        uint32_t lattitude_dec :8;
	        uint32_t lattitude_float :20;
	        uint32_t longitude_dec :8;
	        uint32_t longitude_float :20;
	        uint32_t checkpoint:7;
	        uint32_t bIsFinal:1;
	}long_lat __attribute__((packed));


	typedef struct compass{
		  int8_t turnDecision     : 8;
		  uint8_t checkpoint      : 8;
		  uint32_t dist_finalDest : 16;
		  uint32_t dist_nxtPnt    : 16;
	}compass __attribute__((packed));

class geo_controller_class: public CAN_base_class
{
public:
	static geo_controller_class* getInstance();
	uint16_t id_heart_beat = 0x160;
	uint16_t id_compass_heading_data =0x162;
	uint16_t id_gps_coordinates = 0x164;
	uint16_t id_gps_distance =0x165;
	uint16_t id_gps_checkpoint_req = 0x168;
	uint16_t id_reset = 0x320;
	uint16_t distance_destination =0;
	uint16_t distance_checkpoint =0;
	uint8_t  checkpoint_request =0;
	compass *compass_data = new compass;
	long_lat *lat_long_data = new long_lat;

	bool get_coordinates();
	bool get_compass_data();
	bool get_checkpoint_req();
	bool geo_controller_class_init();
	bool geo_controller_send_coordinates();
	bool reset();
private:
	static  geo_controller_class *single;
};
#endif /* L5_APPLICATION_SOURCE_GEO_CONTROLLER_HPP_ */

