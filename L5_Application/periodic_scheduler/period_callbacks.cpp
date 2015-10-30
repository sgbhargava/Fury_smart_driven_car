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
#include "SensorDataType.h"
#include "file_logger.h"
#include "tasks.hpp"
#include "stdio.h"
#include "printf_lib.h"
#include "queue.h"
#include "can.h"
#include "i2c2_device.hpp"


/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

QueueHandle_t my_queue = xQueueCreate(10, sizeof(int));

// CAN communication
void period_1Hz(void)
{
// LIDAR LASER SENSOR
#if 1
            // P0.0 = CAN1.Rx
            // P0.1 = CAN1.Tx

        uint8_t reading[2] = { 0 };
        can_msg_t can_tx_data;
        CAN_init(can1, 100, 10, 10, NULL, NULL);
             /* Zero out the filtering registers */
                 LPC_CANAF->SFF_sa     = 0;
                 LPC_CANAF->SFF_GRP_sa = 0;
                 LPC_CANAF->EFF_sa     = 0;
                 LPC_CANAF->EFF_GRP_sa = 0;
                 LPC_CANAF->ENDofTable = 0;
                 CAN_bypass_filter_accept_all_msgs();
                 CAN_reset_bus(can1);
                 vTaskDelay(10);

                 can_tx_data.frame_fields.is_rtr = 0;
                 can_tx_data.frame_fields.is_29bit = 0; // Using 11-bit Format
                 can_tx_data.frame_fields.data_len = 2; // Sending 1 byte of data

                 // CAN TX
                  if (xQueueReceive(my_queue, &reading, 1))
                         {

                             can_tx_data.msg_id = 0x142 ;
                             can_tx_data.frame_fields.data_len = 2; // Sending 2 byte of data
                             can_tx_data.data.bytes[0]= reading[0]; // Distance
                             can_tx_data.data.bytes[1]= reading[1]; // Distance
                             CAN_tx(can1, &can_tx_data, 10);
                         }


    						 vTaskDelay(50);
    						 LE.toggle(1);
                             // Send heart beat
                             can_tx_data.msg_id = 0x140 ;
                             can_tx_data.frame_fields.data_len = 0; // Sending 0 byte of data
                             CAN_tx(can1, &can_tx_data, 10);


#endif
#if 1
     static QueueHandle_t sensor_data_q = scheduler_task::getSharedObject("sonic_queue");
     SonicSensors_t sensor_data;
     if(xQueueReceive(sensor_data_q, &sensor_data, 0))
     {
         u0_dbg_printf("Sensors data: \n1)%u\n2)%u\n3)%u\n", sensor_data.SonicSensor1, sensor_data.SonicSensor2, sensor_data.SonicSensor3);
         can_tx_data.msg_id = 0x141 ;
         can_tx_data.frame_fields.data_len = 6; // Sending 2 byte of data
         can_tx_data.data.bytes[0] = ((sensor_data.SonicSensor1 >> 8) & 0xff);
         can_tx_data.data.bytes[1] = ((sensor_data.SonicSensor1 >> 0) & 0xff);
         can_tx_data.data.bytes[2] = ((sensor_data.SonicSensor2 >> 8) & 0xff);
         can_tx_data.data.bytes[3] = ((sensor_data.SonicSensor2 >> 0) & 0xff);
         can_tx_data.data.bytes[4] = ((sensor_data.SonicSensor3 >> 8) & 0xff);
         can_tx_data.data.bytes[5] = ((sensor_data.SonicSensor3 >> 0) & 0xff);
         CAN_tx(can1, &can_tx_data, 10);
     }
}
#endif
void period_10Hz(void)
{

#if 1
    uint8_t buffer[2] = { 0 };

    if(I2C2::getInstance().writeReg(0xc4, 0x0, 0x4))					// Set Sensor Reading
    {
    vTaskDelay(50);
    if (I2C2::getInstance().readRegisters(0xc5, 0x8f, &buffer[0], 2))  // Get reading
        xQueueSend(my_queue, &buffer, 10);
    }
//                      int distance = (buffer[0] << 8) + buffer[1];
#endif
}

void period_100Hz(void)
{

}

void period_1000Hz(void)
{

}
