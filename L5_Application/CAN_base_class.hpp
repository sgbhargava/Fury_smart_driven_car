/*
 * CAN_base_class.hpp
 *
 *  Created on: Nov 7, 2015
 *      Author: Bhargava
 */

#ifndef L5_APPLICATION_CAN_BASE_CLASS_HPP_
#define L5_APPLICATION_CAN_BASE_CLASS_HPP_
#include "can_custom_header.hpp"
class CAN_base_class
{
public:
	bool get_data(uint16_t id, uint64_t *data);
	uint16_t getword(uint64_t data, uint8_t n_word);

};
#endif /* L5_APPLICATION_CAN_BASE_CLASS_HPP_ */
