/*
 * custom_can.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: Bhargava
 */
#include "can_custom_header.hpp"
#define CAN_BAUD 100
#define CAN_RX_SIZE 10
#define CAN_TX_SIZE 10

can_std_id_t can_test1;
can_std_id_t can_test2;
can_fullcan_msg_t *can_ptr;
can_fullcan_msg_t *can_test1_ptr;

can_void_func_t bus_off_cb()
{
	printf("can bus entered bus error state\n");
}
can_void_func_t data_ovr_cb()
{
	printf("can bus entered data-overrun state\n");
}

//can_std_grp_id_t
bool can_custom_init()
{
	can_test1.can_num = 0;
	can_test1.disable = 0;
	can_test1.fc_intr = 1;
	can_test1.id = 0x001;

	can_test2.can_num = 0;
	can_test2.disable = 1;
	can_test1.id = 0xFF;

if(!CAN_init(can1, CAN_BAUD, CAN_RX_SIZE, CAN_TX_SIZE, bus_off_cb(), data_ovr_cb()))
		printf("CAN INITIALIZATION ERROR\n");


	if(!CAN_fullcan_add_entry(can1, can_test1,can_test2))
		printf("can_add_entry failure \n");
	CAN_bypass_filter_accept_all_msgs(void)
		CAN_reset_bus(can1);

}
