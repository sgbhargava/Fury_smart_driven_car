/*
 * custom_can.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: Bhargava
 */
#include <stdio.h>
#include "can_custom_header.hpp"
#include <LPC17xx.h>
#define CAN_BAUD 100
#define CAN_RX_SIZE 10
#define CAN_TX_SIZE 10



const can_ext_id_t *elist = NULL;
//const can_ext_grp_id_t eglist[] = NULL;can


void bus_off_cb(uint32_t arg)
{
	printf("can bus entered bus error state\n");
	CAN_reset_bus(can1);
}
void data_ovr_cb(uint32_t arg)
{
	printf("can bus entered data-overrun state\n");
	CAN_reset_bus(can1);
}

//can_std_grp_id_t
int can_custom_init()
{
	/*can_std_id_t can_test1;
	can_std_id_t can_test2;
	can_test1.can_num = 0;
	can_test1.disable = 0;
	can_test1.fc_intr = 0;
	can_test1.id = 0x142;

	can_test2.can_num = 0;
	can_test2.disable = 0;
	can_test2.fc_intr = 0;
	can_test2.id = 0x144;*/


	const can_std_id_t slist[]  = { CAN_gen_sid(can1, 0x142), CAN_gen_sid(can1, 0x144)
								  };

	printf("can id is %x\t", slist[0].id);
	const can_std_grp_id_t sglist[] { {CAN_gen_sid(can1, 0x140), CAN_gen_sid(can1, 0x14f)},
									 // {CAN_gen_sid(can1, 0x140), CAN_gen_sid(can1, 0x14F)}
	};

	if(!CAN_init(can1, CAN_BAUD, CAN_RX_SIZE, CAN_TX_SIZE, bus_off_cb, data_ovr_cb))
		printf("CAN INITIALIZATION ERROR\n");
	  // Zero out the filtering registers
	        /* LPC_CANAF->SFF_sa     = 0;
	         LPC_CANAF->SFF_GRP_sa = 0;
	         LPC_CANAF->EFF_sa     = 0;
	         LPC_CANAF->EFF_GRP_sa = 0;
	         LPC_CANAF->ENDofTable = 0;
	         LPC_CANAF->AFMR &= ~(3<<0);*/

	if(CAN_fullcan_add_entry(can1, slist[0], slist[1]) == false)
		printf("can_add_entry failure \n");



	if(CAN_setup_filter(NULL, 0, sglist, 1, elist, 0, NULL, 0) == false)
	{
		printf("can filter set up failure \n");
	}
printf("can initialized in init\n");
		CAN_reset_bus(can1);
	//CAN_bypass_filter_accept_all_msgs();


		//
return 0;
}
