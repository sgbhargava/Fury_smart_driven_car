/*
 * can_msg_process.hpp
 *
 *  Created on: Oct 31, 2015
 *      Author: Cynthia
 */

#ifndef L5_APPLICATION_CAN_MSG_PROCESS_HPP_
#define L5_APPLICATION_CAN_MSG_PROCESS_HPP_

#include "CAN.h"
#include "shared_handles.h"
#include "tasks.hpp"
#include "motor.hpp"

#define CAN_MSG_ID_RESET 0x20
#define CAN_MSG_ID_STEER 0x21
#define CAN_MSG_ID_THROTTLE 0x22

#define CAN_MSG_ID_HEARTBEAT 0x100
#define CAN_MSG_ID_SPEED  0x102

//CAN MSG for debug
#define CAN_MSG_ID_SENSOR_HEARTBEAT 0x140
#define CAN_MSG_ID_SENSOR 0x142
#define CAN_MSG_ID_GPS_HEARTBEAT 0x160
#define CAN_MSG_ID_GPS_COMPASS 0x162
#define CAN_MSG_ID_GPS_GPS 0x164

#define FORWARD_SPEED 8.66
#define BACKWARD_SPEED 8.0

typedef struct{
    uint64_t speed:8;
    uint64_t rpm:8;
}speed_can_msg_t;

void can_msg_process_init(void);
void sendSpeed(void);
void sendHeartBeat(void);
void recvAndAnalysisCanMsg(void);
void readCANMsgs(void);
#endif /* L5_APPLICATION_CAN_MSG_PROCESS_HPP_ */
