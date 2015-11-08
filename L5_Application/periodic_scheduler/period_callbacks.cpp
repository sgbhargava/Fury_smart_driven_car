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
#include "SensorDirection.hpp"

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
can_msg_t can_tx_data;

QueueHandle_t my_queue = xQueueCreate(1, sizeof(int));

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{

    CAN_init(can1, 100, 10, 10, NULL, NULL);
    /* Zero out the filtering registers */
     LPC_CANAF->SFF_sa     = 0;
     LPC_CANAF->SFF_GRP_sa = 0;
     LPC_CANAF->EFF_sa     = 0;
     LPC_CANAF->EFF_GRP_sa = 0;
     LPC_CANAF->ENDofTable = 0;
     //const can_ext_id_t elist[] = { CAN_gen_eid(can1, 0x18DAF111), CAN_gen_eid(can1, 0x18DAF11D)};
     CAN_setup_filter(NULL, 0, NULL, 0, NULL, 0, NULL, 0);
     //CAN_bypass_filter_accept_all_msgs();
     CAN_reset_bus(can1);
     can_tx_data.frame_fields.is_rtr = 0;
     can_tx_data.frame_fields.is_29bit = 0; // Using 11-bit Format
     can_tx_data.frame_fields.data_len = 1; // Sending 1 byte of data

    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}


// CAN communication
void period_1Hz(void)
{
    //Send heart beat
    can_tx_data.msg_id = 0x140 ;
    can_tx_data.frame_fields.data_len = 0; // Sending 0 byte of data
    CAN_tx(can1, &can_tx_data, 10);

#if 0
     uint8_t direction;
     direction = RCdirection(sensor_data, LidarData);
     can_tx_data.msg_id = 0x021 ;
     can_tx_data.frame_fields.data_len = 1;
     can_tx_data.data.bytes[0] = direction;
     CAN_tx(can1, &can_tx_data, 10);
     if(CAN_rx(can1, &can_rx_data, 10)){
         u0_dbg_printf("Can received = %x\n", can_rx_data.msg_id);
     }
#endif

}


void period_10Hz(void)
{
    // LIDAR LASER SENSOR
    #if 1
         static QueueHandle_t sensor_data_q = scheduler_task::getSharedObject("sonic_queue");
         SonicSensors_t sensor_data;
         if(xQueueReceive(sensor_data_q, &sensor_data, 0))
          {
              //u0_dbg_printf("Sensors data: \n1)%x\n2)%x\n3)%x\n", sensor_data.SonicSensor1, sensor_data.SonicSensor2, sensor_data.SonicSensor3);
             //u0_dbg_printf("direction %d\n", RCdirection(sensor_data));
              can_tx_data.msg_id = 0x142 ;
              can_tx_data.frame_fields.data_len = 8; // Sending 6 byte of data
              can_tx_data.data.bytes[0] = ((sensor_data.LIDAR >> 8) & 0xff);
              can_tx_data.data.bytes[1] = ((sensor_data.LIDAR >> 0) & 0xff);
              can_tx_data.data.bytes[2] = ((sensor_data.SonicSensor1 >> 8) & 0xff);
              can_tx_data.data.bytes[3] = ((sensor_data.SonicSensor1 >> 0) & 0xff);
              can_tx_data.data.bytes[4] = ((sensor_data.SonicSensor2 >> 8) & 0xff);
              can_tx_data.data.bytes[5] = ((sensor_data.SonicSensor2 >> 0) & 0xff);
              can_tx_data.data.bytes[6] = ((sensor_data.SonicSensor3 >> 8) & 0xff);
              can_tx_data.data.bytes[7] = ((sensor_data.SonicSensor3 >> 0) & 0xff);
              CAN_tx(can1, &can_tx_data, 10);
              LE.toggle(1);
          }
    #endif
}

void period_100Hz(void)
{

}

void period_1000Hz(void)
{

}
