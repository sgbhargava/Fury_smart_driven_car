#include <cstdio>
#include "FreeRTOS.h"
#include "queue.h"
#include "can_msg_process.hpp"

#include "CAN.h"
#include "shared_handles.h"


#ifdef CAN_MSG_CLASS

CANMsg * CANMsg::m_pInstance = NULL;

const int can_ids[] = {0x021, 0x321, 0x421};
CANMsg::CANMsg(void)
{
    if (CAN_init(can_t::can1, 100, 10, 10, NULL, NULL)){
        printf("CAN initialization is done\n");
    }
#ifdef FULLCAN

    can_id_len = sizeof(can_ids)/sizeof(int);
    for (int i = 0; i < can_id_len; i++){
        if (i < can_id_len)
            CAN_fullcan_add_entry(can1, CAN_gen_sid(can1,can_ids[i]), CAN_gen_sid(can1,can_ids[i+1]));
        else
            CAN_fullcan_add_entry(can1, CAN_gen_sid(can1,can_ids[i]), CAN_gen_sid(can1, 0xffff));
    }
#else
    CAN_bypass_filter_accept_all_msgs();
#endif
    CAN_reset_bus(can_t::can1);
    vTaskDelay(10);

    m_pSpeed = SpeedCtrl::getInstance();
    m_pDir = DirectionCtrl::getInstance();
    m_pSpeedMonitor = SpeedMonitor::getInstance();

    m_heartbeatCnt = 0;
}

CANMsg* CANMsg::getInstance(void){
    if (m_pInstance == NULL){
        m_pInstance = new CANMsg();
    }
    return m_pInstance;
}

void CANMsg::recvAndAnalysisCanMsg(void)
{
    can_msg_t mMsgRecv;

    while (CAN_rx(can1, &mMsgRecv, 0))
    {
        if (mMsgRecv.msg_id == CAN_MSG_ID_STEER)
        {
            dir_can_msg_t *pDirCanMsg = (dir_can_msg_t *)&mMsgRecv.data.bytes[0];
            m_pDir->setDirection(pDirCanMsg->turn);
            printf("Dir REV MSG %x %x\n", mMsgRecv.msg_id , mMsgRecv.data.bytes[0]);
        }
        else if (mMsgRecv.msg_id == CAN_MSG_ID_THROTTLE)
        {
            m_pSpeed = SpeedCtrl::getInstance();
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

void CANMsg::sendSpeed(void)
{
    int rpm =0 , speed = 0;
    //m_pSpeedMonitor->getSpeed((float *)&rpm, (float *)&speed);
    printf("Speed RPM: %d, Speed:%d \n", rpm, speed);

    can_msg_t msg;
    msg.msg_id = CAN_MSG_ID_SPEED;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 1;
    speed_can_msg_t* pSpeedCanMsg = (speed_can_msg_t*) &msg.data.bytes[0];
    pSpeedCanMsg->rpm = (int)rpm;
    pSpeedCanMsg->speed = (int)speed;
    CAN_tx(can1, &msg, 1);
}

void CANMsg::sendHeartBeat(void)
{
    can_msg_t msg;
    msg.msg_id = CAN_MSG_ID_HEARTBEAT;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 1;
    msg.data.bytes[0] = m_heartbeatCnt ++;
    CAN_tx(can1, &msg, 0);
}

void CANMsg::sendTxCanMsg(void)
{
    int rpm, speed;
    m_pSpeedMonitor->getSpeed((float *)&rpm, (float *)&speed);
    printf("Speed RPM: %d, Speed:%d \n", rpm, speed);

    can_msg_t msg;
    msg.msg_id = CAN_MSG_ID_SPEED;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 1;
    speed_can_msg_t* pSpeedCanMsg = (speed_can_msg_t*) &msg.data.bytes[0];
    pSpeedCanMsg->rpm = (int)rpm;
    pSpeedCanMsg->speed = (int)speed;
    CAN_tx(can1, &msg, 0);

    msg.msg_id = CAN_MSG_ID_HEARTBEAT;
    msg.data.bytes[0] = m_heartbeatCnt ++;
    CAN_tx(can1, &msg, 0);
}


#else
static int cheartbeatCnt = 0;
void recvAndAnalysisCanMsg(void)
{
    can_msg_t mMsgRecv;

    while (CAN_rx(can1, &mMsgRecv, 0))
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
            bool isForward = false;
            if (pThrottleCanMsg->forward)
            {
                printf("Go Forward\n");
                isForward = true;
                if (pThrottleCanMsg->customForward)
                    m_pSpeed->setSpeedCustom(isForward, pThrottleCanMsg->customForward);
            }
            else if (pThrottleCanMsg->incrForward)
                m_pSpeed->incrSpeedPWM();
            else if (pThrottleCanMsg->customForward & 0x3)
                m_pSpeed->descrSpeedPWM();
            else if (pThrottleCanMsg->backward)
            {
                printf("Go Backward\n");
                if (pThrottleCanMsg->customBackward)
                    m_pSpeed->setSpeedCustom(isForward, pThrottleCanMsg->customBackward);
            }
            else if (pThrottleCanMsg->incrBackward)
                m_pSpeed->descrSpeedPWM();
            else if (pThrottleCanMsg->customBackward & 0x3)
                m_pSpeed->incrSpeedPWM();
            else
                m_pSpeed->setStop();

            printf("Speed REV MSG %x %x\n", mMsgRecv.msg_id , mMsgRecv.data.bytes[0]);
        }
    }
}

void sendSpeed(void)
{
    float rpm= 0 , speed= 0;
    SpeedMonitor* speedMonitor = SpeedMonitor::getInstance();
    speedMonitor->getSpeed(&rpm, &speed);
    //printf("Speed RPM: %f, Speed:%f \n", rpm, speed);

    can_msg_t msg;
    msg.msg_id = CAN_MSG_ID_SPEED;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 1;
    speed_can_msg_t* pSpeedCanMsg = (speed_can_msg_t*) &msg.data.bytes[0];
    pSpeedCanMsg->rpm = (int)rpm;
    pSpeedCanMsg->speed = (int)speed;
    CAN_tx(can_t::can1, &msg, 0);
}

void sendHeartBeat(void)
{
    can_msg_t msg;
    msg.msg_id = CAN_MSG_ID_HEARTBEAT;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 1;
    msg.data.bytes[0] = 1; //cheartbeatCnt++;
    CAN_tx(can_t::can1, &msg, 0);
}
#endif
