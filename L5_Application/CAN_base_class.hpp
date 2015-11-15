/*
 * CAN_base_class.hpp
 *
 *  Created on: Nov 7, 2015
 *      Author: Bhargava
 */

#ifndef L5_APPLICATION_CAN_BASE_CLASS_HPP_
#define L5_APPLICATION_CAN_BASE_CLASS_HPP_
#include "can.h"
#include "stdio.h"

class CAN_base_class
{
public:
	CAN_base_class();
	bool get_data(uint16_t id, uint64_t *data);
	uint16_t getword(uint64_t data, uint8_t n_word);
	uint8_t getbyte(uint64_t data, uint8_t n_byte);
	uint32_t getdword(uint64_t data, uint8_t n_dword);
	bool add_can_id(uint16_t id1,uint16_t id2);

};
#endif /* L5_APPLICATION_CAN_BASE_CLASS_HPP_ */
