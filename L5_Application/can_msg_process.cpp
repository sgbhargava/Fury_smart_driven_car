#include <cstdio>
#include "FreeRTOS.h"
#include "queue.h"
#include "can_msg_process.hpp"

#include "CAN.h"
#include "shared_handles.h"


#ifdef CAN_MSG_CLASS
CANMsg::CANMsg(void){
    if (CAN_init(can_t::can1, 100, 512, 512, NULL, NULL)){
        printf("CAN initialization is done\n");
    }
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can_t::can1);

    m_CANRxQueueHandler = xQueueCreate(1, sizeof(can_msg_t));
    m_CANTxQueueHandler = xQueueCreate(1, sizeof(can_msg_t));

    m_pSpeed = SpeedCtrl::getInstance();
    m_pDir = DirectionCtrl::getInstance();
}

CANMsg* CANMsg::getInstance(void){
    if (m_pInstance == NULL){
        m_pInstance = new CANMsg();
    }
    return m_pInstance;
}

void CANMsg::analysisCanMsg(void)
{
    can_msg_t mMsgRecv;
    if (xQueueReceive(m_CANRxQueueHandler, &mMsgRecv, 0 )){

        printf("Enter Semaphore\n");
        if (mMsgRecv.msg_id == CAN_MSG_ID_STEER)
        {
            dir_can_msg_t *pDirCanMsg = (dir_can_msg_t *)&mMsgRecv.data.bytes[0];
            m_pDir->setDirection(pDirCanMsg->turn);
        }
        else if (mMsgRecv.msg_id == CAN_MSG_ID_THROTTLE)
        {
            throttle_can_msg_t *pThrottleCanMsg = (throttle_can_msg_t *)&mMsgRecv.data.bytes[0];
            if (pThrottleCanMsg->forward == 0xf)// Hack
                m_pSpeed->setSpeedPWM(FORWARD_SPEED);
            if (pThrottleCanMsg->backward == 0xf) // Hack
                m_pSpeed->setSpeedPWM(BACKWARD_SPEED);
        }
    }

}
void CANMsg::receiveCanMsg(){

    can_msg_t mMsgRecv;
    if (CAN_rx(can1, &mMsgRecv, 0)){
        xQueueSend( m_CANRxQueueHandler, &mMsgRecv, 0 );    //m_CANRxQueueHandler;
    }

}

#else

QueueHandle_t m_CANRxQueueHandler =  xQueueCreate(1, sizeof(can_msg_t));
QueueHandle_t m_CanTxQueueHandler = xQueueCreate(1, sizeof(int));

void receiveCanMsg(){

    can_msg_t mMsgRecv;
    if (CAN_rx(can1, &mMsgRecv, 0)){
        xQueueSend( m_CANRxQueueHandler, &mMsgRecv, 0 );    //m_CANRxQueueHandler;
    }

}

void analysisCanMsg(void)
{
    can_msg_t mMsgRecv;
    if (xQueueReceive( m_CANRxQueueHandler, &mMsgRecv, 0 ))
    {

        if (mMsgRecv.msg_id == CAN_MSG_ID_STEER)
        {
            dir_can_msg_t *pDirCanMsg = (dir_can_msg_t *)&mMsgRecv.data.bytes[0];
            DirectionCtrl::getInstance()->setDirection(pDirCanMsg->turn);
            printf("Dir REV MSG %x %x\n", mMsgRecv.msg_id , mMsgRecv.data.bytes[0]);
        }
        else if (mMsgRecv.msg_id == CAN_MSG_ID_THROTTLE)
        {
            SpeedCtrl * m_pSpeed = SpeedCtrl::getInstance();
            throttle_can_msg_t *pThrottleCanMsg = (throttle_can_msg_t *)&mMsgRecv.data.bytes[0];
            if (pThrottleCanMsg->forward == 0xf)// Hack
            {
                printf("Go Forward\n");
                m_pSpeed->setSpeedPWM(FORWARD_SPEED);
            }
            if (pThrottleCanMsg->backward == 0xf) // Hack
            {
                printf("Go Backward\n");
                m_pSpeed->setSpeedPWM(BACKWARD_SPEED);
            }
            printf("Speed REV MSG %x %x\n", mMsgRecv.msg_id , mMsgRecv.data.bytes[0]);
        }
    }
}

void sendSpeed(void)
{
    float rpm, speed;
    SpeedMonitor* speedMonitor = SpeedMonitor::getInstance();
    speedMonitor->getSpeed(&rpm, &speed);
    printf("Speed RPM: %f, Speed:%f \n", rpm, speed);

    can_msg_t msg;
    msg.msg_id = CAN_MSG_ID_SPEED;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 1;
    speed_can_msg_t* pSpeedCanMsg = (speed_can_msg_t*) &msg.data.bytes[0];
    pSpeedCanMsg->rpm = (int)rpm;
    pSpeedCanMsg->speed = (int)speed;
    //CAN_tx(can_t::can1, &msg, 0);
}
#endif
