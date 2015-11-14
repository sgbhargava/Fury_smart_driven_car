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
#include "utilities.h"
#include "file_logger.h"
#include "queue.h"
#include "can.h"
#include "printf_lib.h"
#include "sensors.hpp"
#define rx
#define lidar_threshold 100
#define sonic_threshold 50

#define forward 0xF0
#define reverse 0x00
sensor_class sensor;

enum direction {
	straight, far_right, right, left, far_left
};



can_msg_t motor_throttle;
can_msg_t motor_steer;
can_msg_t heart_beat;

int correctDirection = straight;
int previousDirection = straight;

int correctSpeed = forward;
int previousSpeed = reverse;


uint16_t lidar = 0;
/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/*
void Obstruction_avoidance_algorithm(void)
{
	if (lidar < lidar_threshold)
		{
			 if (SonicData.SonicSensor1 < sonic_threshold)
			 {
				correctDirection = right;
			 }
			 else
			 {
				correctDirection = left;
			 }

		}
		else
		{
			if (SonicData.SonicSensor1 < sonic_threshold)
			{
				correctDirection = right;
			}
			else if (SonicData.SonicSensor2 < sonic_threshold)
			{
				correctDirection = left;
			}
			else
			{
				correctDirection = straight;
			}
		}

	}
	//  printf("head =%d \n",correctDirection);
	motor_steer.data.bytes[0] = correctDirection;
	//previousDirection=correctDirection;
	CAN_tx(can1, &motor_steer, 0);
	//printf("correctDirection =%d \n",correctDirection);
	//}

	 if(previousSpeed!=correctSpeed)
	 {
	 motor_throttle.data.bytes[0]=correctSpeed;
	 previousSpeed=correctSpeed;
	 CAN_tx(can1, &motor_throttle, 0);
	 // printf("correctSpeed =%d \n",correctSpeed);

	 }



		motor_steer.data.bytes[0] = correctDirection;
		CAN_tx(can1, &motor_steer, 0);
}
*/




/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
	CAN_base_class my_can;
	/*motor_throttle.msg_id = 0x022;
	motor_throttle.frame_fields.is_29bit = 0;
	motor_throttle.frame_fields.data_len = 1;       // Send 8 bytes

	motor_steer.msg_id = 0x021;
	motor_steer.frame_fields.is_29bit = 0;
	motor_steer.frame_fields.data_len = 1;       // Send 8 bytes

	heart_beat.msg_id = 0x221;
	heart_beat.frame_fields.is_29bit = 0;
	heart_beat.frame_fields.data_len = 1;       // Send 8 bytes*/

	sensor.sensor_class_init();

    return true; // Must return true upon success
}

//Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}


void period_1Hz(void) {

	if(sensor.get_sensor_reading())
			u0_dbg_printf("left is %x \t right is \t %x lidar is %x\n",sensor.left, sensor.right, sensor.lidar);
		else
			u0_dbg_printf("no sensor\n");


}


void period_10Hz(void) {


	// LE.toggle(2);


}


void period_100Hz(void) {
	/*can_msg_t msg;
	 if(CAN_rx(can1, &msg,0))
	 {
	 if(!xQueueSend(can_queue, &msg, 0))
	 {
	 printf("couldnt send to queue\n");
	 }
	 }
	 */
}

void period_1000Hz(void) {
	//LE.toggle(4);
}
