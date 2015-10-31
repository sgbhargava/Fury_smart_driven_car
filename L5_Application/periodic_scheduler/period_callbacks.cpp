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
#include "utilities.h"
#include "file_logger.h"
#include "queue.h"
#define rx
typedef struct{
	 uint16_t SonicSensor1;
	    uint16_t SonicSensor2;
	    uint16_t SonicSensor3;
}SonicSensor_t;

SonicSensor_t SonicData;
uint16_t lidar = 0;
/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
/*
 *
 *
 *
can_std_id_t can_test1;
 can_std_id_t can_test2;
 can_fullcan_msg_t *can_ptr;
 can_fullcan_msg_t *can_test1_ptr;
*/
can_fullcan_msg_t can_ptr;

QueueHandle_t can_queue = xQueueCreate(100,sizeof(can_msg_t));
void period_1Hz(void)
{

	/*
#ifdef rx
	can_test1_ptr = CAN_fullcan_get_entry_ptr(can_test1);
	if(CAN_fullcan_read_msg_copy(&can_ptr,can_test1_ptr) == true)
	{
		printf("new message : %d \n", can_ptr.data.bytes);

	}
	else
		printf("no message at this time \n");
#endif

#ifdef tx
	static can_msg_t mess;

	mess.msg_id = 0x100;
	mess.frame_fields.is_29bit = 1;
	mess.frame_fields.data_len = 8;
	mess.data.qword = 10;
	if(CAN_tx(can1, &mess, portMAX_DELAY) == true)
	printf("sent \n");

	else
		printf("not sent\n");
#endif
*/


}

void period_10Hz(void)
{
   // LE.toggle(2);
	can_msg_t temp;


	if(!xQueueReceive(can_queue, &temp, 0))
	{

	}
	//printf("address %ld \t %d\n", temp.msg_id, temp.data);

	switch (temp.msg_id)
		{
//	sonic sensor data
		case 0x141:
uint16_t  temp3, temp2;

		temp2 = (temp.data.words[0]>> 8);
		temp3 = (temp.data.words[0] << 8);
		SonicData.SonicSensor1 = 0;
		SonicData.SonicSensor1 = temp2 | temp3;

		temp2 = (temp.data.words[1]>> 8);
		temp3 = (temp.data.words[1] << 8);
		SonicData.SonicSensor2 = 0;
		SonicData.SonicSensor2 = temp2 | temp3;

		temp2 = (temp.data.words[2]>> 8);
		temp3 = (temp.data.words[2] << 8);
		SonicData.SonicSensor3 = 0;
		SonicData.SonicSensor3 = temp2 | temp3;


		//printf("%x\t %x\t %x\n",SonicData.SonicSensor1, SonicData.SonicSensor2, SonicData.SonicSensor3);



		break;

		case 0x142:


					temp2 = (temp.data.words[0]>> 8);
					temp3 = (temp.data.words[0] << 8);
					lidar = 0;
					lidar = temp2 | temp3;
		//printf("lidar is %x\n", lidar);
		break;
		//default:
			//printf("default\n");
	}


	enum direction {farRight, right, straight, left, FarLeft};
	    int correctDirection = straight;
	 if(lidar < 75)
	    {
	        if(SonicData.SonicSensor1 < SonicData.SonicSensor2)
	        {
	            correctDirection = right;
	            LE.off(1);
	            LE.off(2);
	            LE.on(3);
	        }
	        else
	        {
	            correctDirection = left;
	            LE.on(1);
	            LE.off(2);
	            LE.off(3);
	        }
	    }
	    else if(SonicData.SonicSensor1 < 75)
	    {
	        if(lidar < SonicData.SonicSensor2)
	        {
	            correctDirection = right;
	            LE.on(3);
	            LE.off(1);
	            LE.off(2);
	        }
	        else
	        {
	            correctDirection = straight;
	            LE.on(2);
	            LE.off(1);
	            LE.off(3);
	        }
	    }
	    else if(SonicData.SonicSensor2 < 75)
	    {
	        if(lidar < SonicData.SonicSensor1)
	        {
	            correctDirection = left;
	            LE.on(1);
	            LE.off(2);
	            LE.off(3);
	        }
	        else
	        {
	            correctDirection = straight;
	            LE.on(2);
	            LE.off(1);
	            LE.off(3);
	        }
	    }
	    else
	    {
	        correctDirection = straight;
	        LE.on(2);
	        LE.off(1);
	        LE.off(3);
	    }
	    printf("head =%d \n",correctDirection);

}

void period_100Hz(void)
{
	can_msg_t msg;
	if(CAN_rx(can1, &msg,0))
	{
		if(!xQueueSend(can_queue, &msg, 0))
		{
			//couldnt log
		}
	}

}

void period_1000Hz(void)
{
    //LE.toggle(4);
}
