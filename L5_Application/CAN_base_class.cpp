/*
 * CAN_base_class.cpp
 *
 *  Created on: Nov 7, 2015
 *      Author: Bhargava
 */
#include "CAN_base_class.hpp"
#define first 0
#define second 16
#define third 32
#define fourth 48

bool CAN_base_class::get_data(uint16_t id, uint64_t *data)
{
	bool status = false;
	can_fullcan_msg_t *fc1 = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, id));
	can_fullcan_msg_t fc_temp;
	status = CAN_fullcan_read_msg_copy(fc1, &fc_temp);
	if(status)
		*data = fc_temp.data.qword;
	return status;
}
uint16_t CAN_base_class::getword(uint64_t data, uint8_t n_word)
{
	uint16_t temp1_16 = 0;
	uint16_t temp2_16 = 0;
	uint16_t temp3_16 = 0;
	uint16_t returned_data = 0;

	temp1_16 = (data >> (n_word *16)) & 0xFFFF;
	temp2_16 = (temp1_16 >> 8);
	temp3_16 = (temp1_16 << 8);
	returned_data = temp2_16 | temp3_16;
	return returned_data;
}

uint8_t CAN_base_class::getbyte(uint64_t data, uint8_t n_byte)
{
		uint8_t returned_data = 0;

		returned_data = ((data >> (n_byte *8)) & 0xFF);
		return returned_data;

}

uint32_t CAN_base_class::getdword(uint64_t data, uint8_t n_dword)
{
		uint32_t returned_data = 0;

		returned_data = ((data >> (n_dword *32)) & 0xFFFFFFFF);
		return returned_data;

}


void bus_off_cb(uint32_t arg)
{

	printf("bus off\n");
	CAN_reset_bus(can1);

}

void data_ovr_cb(uint32_t arg)
{
	printf("data_ovr_cb\n");
	CAN_reset_bus(can1);

}

CAN_base_class::CAN_base_class()
{

	printf("can constructor\n");

	if(CAN_init(can1, 100, 5, 5, bus_off_cb,data_ovr_cb))
		printf("can intialised\n");
	CAN_reset_bus(can1);
}

bool CAN_base_class::add_can_id(uint16_t id1, uint16_t id2){

	CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, id1), CAN_gen_sid(can1, id2));


}
