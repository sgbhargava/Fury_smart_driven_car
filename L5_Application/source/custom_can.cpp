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
#include <printf_lib.h>

#define CAN_ASSERT(x)   if (!(x)) { u0_dbg_printf("Failed at %i, BUS: %s MOD: 0x%08x, GSR: 0x%08x\n"\
                                           "IER/ICR: 0x%08X/0x%08x BTR: 0x%08x"\
                                           "\nLine %i: %s\n", __LINE__, \
                                           CAN_is_bus_off(can1) ? "OFF" : "ON", \
                                           (int)LPC_CAN1->MOD, (int)LPC_CAN1->GSR, \
                                           (int)LPC_CAN1->IER, (int)LPC_CAN1->ICR, \
                                           (int)LPC_CAN1->BTR, \
                                           __LINE__, #x); return false; }

uint32_t id = 0x100;

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
	CAN_init(can1, 100, 5, 5, bus_off_cb,data_ovr_cb);
	CAN_reset_bus(can1);
	id = 0x140;
	//printf("can entries initial = %d/n", CAN_fullcan_get_num_entries());
	 CAN_ASSERT(CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, id), CAN_gen_sid(can1, id+2)));
	    CAN_ASSERT(2 == CAN_fullcan_get_num_entries());
	    CAN_ASSERT(LPC_CANAF->SFF_sa     == 4);
	    CAN_ASSERT(LPC_CANAF->SFF_GRP_sa == 4);
	    CAN_ASSERT(LPC_CANAF->EFF_sa     == 4);
	    CAN_ASSERT(LPC_CANAF->EFF_GRP_sa == 4);
	    CAN_ASSERT(LPC_CANAF->ENDofTable == 4);

	    CAN_ASSERT(CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, id+2), CAN_gen_sid(can1, id+3)));
	    CAN_ASSERT(4 == CAN_fullcan_get_num_entries());
	    CAN_ASSERT(LPC_CANAF->SFF_sa     == 8);


return 1;
}
