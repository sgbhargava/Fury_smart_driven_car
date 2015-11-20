#include <cstdio>
#include "FreeRTOS.h"
#include "queue.h"
#include "_can_dbc/auto_gen.inc"
#include "can_msg_process.hpp"

#include "CAN.h"
#include "shared_handles.h"

#include "uart2.hpp"
#include "i2c2.hpp"

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
void can_msg_process_init(void)
{
    if (CAN_init(can_t::can1, 100, 10, 10, NULL, NULL)){
        printf("CAN initialization is done\n");
    }
    CAN_reset_bus(can_t::can1);

    // Add CAN ID
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, CAN_MSG_ID_RESET), CAN_gen_sid(can1, CAN_MSG_ID_STEER));
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, CAN_MSG_ID_THROTTLE), CAN_gen_sid(can1, CAN_MSG_ID_SENSOR));
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, CAN_MSG_ID_SENSOR_HEARTBEAT), CAN_gen_sid(can1, CAN_MSG_ID_GPS_COMPASS));
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, CAN_MSG_ID_GPS_GPS), CAN_gen_sid(can1, CAN_MSG_ID_GPS_HEARTBEAT));

    Uart2::getInstance().init(38400);
}
void recvAndAnalysisCanMsg(void)
{
    can_fullcan_msg_t fc_temp;
    can_fullcan_msg_t *reset_fc_ptr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, CAN_MSG_ID_RESET));

    if (CAN_fullcan_read_msg_copy(reset_fc_ptr, &fc_temp)){
        sys_reboot();
    }

    can_fullcan_msg_t *steer_fc_ptr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, CAN_MSG_ID_STEER));
    if (CAN_fullcan_read_msg_copy(steer_fc_ptr, &fc_temp)){

#ifdef DBC_FILE
        DRIVER_TX_STEER_t steer_msg;
        DRIVER_TX_STEER_decode(&steer_msg, (uint64_t*) &fc_temp.data.bytes, &DRIVER_TX_STEER_HDR);
        DirectionCtrl::getInstance()->setDirection(steer_msg.DRIVER_STEER_dir);

#else
        dir_can_msg_t *pDirCanMsg = (dir_can_msg_t *)&fc_temp.data.bytes[0];
        DirectionCtrl::getInstance()->setDirection(pDirCanMsg->turn);
        printf("Dir REV MSG %x %x\n", (unsigned int)fc_temp.msg_id , (unsigned int)fc_temp.data.bytes[0]);
#endif
    }

    can_fullcan_msg_t *throttle_fc_ptr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, CAN_MSG_ID_THROTTLE));
    if (CAN_fullcan_read_msg_copy(throttle_fc_ptr, &fc_temp)){
        SpeedCtrl * m_pSpeed = SpeedCtrl::getInstance();
#ifdef DBC_FILE
        DRIVER_TX_THROTTLE_t throttle_msg;
        DRIVER_TX_THROTTLE_decode(&throttle_msg, (uint64_t*) &fc_temp.data.bytes, &DRIVER_TX_THROTTLE_HDR);
        if (throttle_msg.DRIVER_THROTTLE_stop)
            m_pSpeed->setStop();
        else
        {
            bool isForward = throttle_msg.DRIVER_THROTTLE_forward ? true: false;
            bool isIncr = throttle_msg.DRIVER_THROTTLE_incr ? true : false;

            if ( throttle_msg.DRIVER_THROTTLE_usecustom )
            {
                m_pSpeed->setSpeedCustom(isForward, throttle_msg.DRIVER_THROTTLE_custom);
            }
            else
            {
                if (isForward)
                {
                    if (isIncr) m_pSpeed->incrSpeedPWM();
                    else m_pSpeed->descrSpeedPWM();
                }
                else
                {
                    if (isIncr) m_pSpeed->descrSpeedPWM();
                    else m_pSpeed->incrSpeedPWM();
                }
            }
        }
#else
        throttle_can_msg_t *pThrottleCanMsg = (throttle_can_msg_t *)&fc_temp.data.bytes[0];
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
#endif
        printf("Speed REV MSG %x %x\n", (unsigned int)fc_temp.msg_id , (unsigned int)fc_temp.data.bytes[0]);
    }

}
#define PRINT_ALL_CAN_MSG
void readCANMsgs(void)
{
    can_fullcan_msg_t fc_temp;
    can_fullcan_msg_t *sensor_fc_ptr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, CAN_MSG_ID_SENSOR));

    if (CAN_fullcan_read_msg_copy(sensor_fc_ptr, &fc_temp)){

#ifdef PRINT_ALL_CAN_MSG
        const int centerSensor = 1;
        const int leftSensor = 3;
        const int rightSensor = 5;
        const int backSensor = 7;
        if (fc_temp.data_len == 8)
        {
            printf("Sensor:: Center %x\n", fc_temp.data.bytes[centerSensor]);
            printf("Sensor:: Left %x\n", fc_temp.data.bytes[leftSensor]);
            printf("Sensor:: Right %x\n", fc_temp.data.bytes[rightSensor]);
            printf("Sensor:: Back %x\n", fc_temp.data.bytes[backSensor]);
        }
#endif
    }

    can_fullcan_msg_t *compass_fc_ptr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, CAN_MSG_ID_GPS_COMPASS));
    if (CAN_fullcan_read_msg_copy(compass_fc_ptr, &fc_temp)){

#ifdef PRINT_ALL_CAN_MSG
        for (int i = 0; i < fc_temp.data_len; i ++)
        {
            printf("GPS::Compass(%d) %x\n", i, fc_temp.data.bytes[i]);
        }
#endif
    }

    can_fullcan_msg_t *gps_fc_ptr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, CAN_MSG_ID_GPS_GPS));
    if (CAN_fullcan_read_msg_copy(gps_fc_ptr, &fc_temp)){

#ifdef PRINT_ALL_CAN_MSG
        for (int i = 0; i < fc_temp.data_len; i ++)
        {
            printf("GPS::GPS(%d) %x\n", i, fc_temp.data.bytes[i]);
        }
#endif
    }

}
void sendSpeed(void)
{
    float rpm= 0 , speed= 0;
    SpeedMonitor* speedMonitor = SpeedMonitor::getInstance();
    speedMonitor->getSpeed(&rpm, &speed);
    //printf("Speed RPM: %f, Speed:%f \n", rpm, speed);

    can_msg_t msg;
#ifdef DBC_FILE
    MOTOR_TX_SPEED_t speedMsg;
    speedMsg.MOTOR_SPEED_rpm = (int) rpm;
    speedMsg.MOTOR_SPEED_speed = (int) speed;
    msg_hdr_t hdr = MOTOR_TX_SPEED_encode((uint64_t *)&msg.data.bytes, &speedMsg);

    msg.msg_id = hdr.mid;
    msg.frame_fields.data_len = hdr.dlc;
#else
    msg.msg_id = CAN_MSG_ID_SPEED;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 1;
    speed_can_msg_t* pSpeedCanMsg = (speed_can_msg_t*) &msg.data.bytes[0];
    pSpeedCanMsg->rpm = (int)rpm;
    pSpeedCanMsg->speed = (int)speed;
#endif
    CAN_tx(can_t::can1, &msg, 0);

    Uart2::getInstance().printf("RPM: %d Speed: %d", (int)rpm, (int)speed);

}

void sendHeartBeat(void)
{
    can_msg_t msg;
    msg.msg_id = CAN_MSG_ID_HEARTBEAT;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 1;
    msg.data.bytes[0] = cheartbeatCnt++;
    CAN_tx(can_t::can1, &msg, 0);
}
#endif
