/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */


#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "stdio.h"
#include "can_custom_header.hpp"

#define rx

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
/*
can_std_id_t can_test1;
 can_std_id_t can_test2;
 can_fullcan_msg_t *can_ptr;
 can_fullcan_msg_t *can_test1_ptr;
*/

void period_1Hz(void)
{
#ifdef rx
	can_test1_ptr = CAN_fullcan_get_entry_ptr(can_test1);
	if(CAN_fullcan_read_msg_copy(can_ptr,can_test1_ptr))
	{printf("new message : %d \n", can_test1_ptr->data.bytes);

	}
	else
		printf("no message at this time");
#endif

#ifdef tx
	static can_msg_t mess;

	mess.msg_id = 0x100;
	mess.frame_fields.is_29bit = 1;
	mess.frame_fields.data_len = 8;
	mess.data.qword = led0on;
	CAN_tx(can1, &mess, portMAX_DELAY);
#endifcan_msg_t mess;

	mess.msg_id = 0x123;
	mess.frame_fields.is_29bit = 1;
	mess.frame_fields.data_len = 8;
#endif
}

void period_10Hz(void)
{
    LE.toggle(2);
}

void period_100Hz(void)
{
    LE.toggle(3);
}

void period_1000Hz(void)
{
    LE.toggle(4);
}
