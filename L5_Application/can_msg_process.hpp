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

#define CAN_MSG_ID_STEER 0x021
#define CAN_MSG_ID_THROTTLE 0x22
#define CAN_MSG_ID_SPEED  0x102

#define FORWARD_SPEED 8.68
#define BACKWARD_SPEED 8.0

//#define CAN_MSG_CLASS
typedef struct{
    uint64_t turn:3;
}dir_can_msg_t;

typedef struct{
    uint64_t backward:4;
    uint64_t forward:4;
}throttle_can_msg_t;

typedef struct{
    uint64_t speed:8;
    uint64_t rpm:8;
}speed_can_msg_t;

#ifdef CAN_MSG_CLASS
class CANMsg
{
    public:
        static CANMsg * getInstance();
        void analysisCanMsg(void);
        void receiveCanMsg(void);

    private:
        CANMsg();
        static CANMsg * m_pInstance;
        SpeedCtrl *m_pSpeed;
        DirectionCtrl *m_pDir;
        QueueHandle_t m_CANRxQueueHandler;
        QueueHandle_t m_CANTxQueueHandler;
};
#else
void receiveCanMsg(void);
void analysisCanMsg(void);
void sendSpeed(void);
#endif
#endif /* L5_APPLICATION_CAN_MSG_PROCESS_HPP_ */
