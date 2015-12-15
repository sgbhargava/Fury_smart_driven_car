#include <cstdio>
#include <cstring>
#include "FreeRTOS.h"
#include "queue.h"
#include "_can_dbc/auto_gen.inc"
#include "can_msg_process.hpp"
#include <string>

#include "CAN.h"
#include "shared_handles.h"

#include "uart2.hpp"
#include "i2c2.hpp"

#include "io.hpp"

static int cheartbeatCnt = 0;
void can_msg_process_init(void)
{
    if (CAN_init(can_t::can1, 100, 10, 10, NULL, NULL))
    {
        printf("CAN initialization is done\n");
    }
    CAN_reset_bus(can_t::can1);

    // Add CAN ID
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, CAN_MSG_ID_RESET),
            CAN_gen_sid(can1, CAN_MSG_ID_STEER));
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, CAN_MSG_ID_THROTTLE),
            CAN_gen_sid(can1, CAN_MSG_ID_SENSOR));
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, CAN_MSG_ID_SENSOR_HEARTBEAT),
            CAN_gen_sid(can1, CAN_MSG_ID_GPS_COMPASS));
    CAN_fullcan_add_entry(can1, CAN_gen_sid(can1, CAN_MSG_ID_GPS_GPS),
            CAN_gen_sid(can1, CAN_MSG_ID_GPS_HEARTBEAT));

    Uart2::getInstance().init(38400);
}
void recvAndAnalysisCanMsg(void)
{
    can_fullcan_msg_t fc_temp;
    can_fullcan_msg_t *reset_fc_ptr = CAN_fullcan_get_entry_ptr(
            CAN_gen_sid(can1, CAN_MSG_ID_RESET));

    if (CAN_fullcan_read_msg_copy(reset_fc_ptr, &fc_temp))
    {
        sys_reboot();
    }

    can_fullcan_msg_t *steer_fc_ptr = CAN_fullcan_get_entry_ptr(
            CAN_gen_sid(can1, CAN_MSG_ID_STEER));
    if (CAN_fullcan_read_msg_copy(steer_fc_ptr, &fc_temp))
    {
        DRIVER_TX_STEER_t steer_msg;
        DRIVER_TX_STEER_decode(&steer_msg, (uint64_t*) &fc_temp.data.bytes,
                &DRIVER_TX_STEER_HDR);
        DirectionCtrl::getInstance()->setDirection(steer_msg.DRIVER_STEER_dir);
    }

    can_fullcan_msg_t *throttle_fc_ptr = CAN_fullcan_get_entry_ptr(
            CAN_gen_sid(can1, CAN_MSG_ID_THROTTLE));
    if (CAN_fullcan_read_msg_copy(throttle_fc_ptr, &fc_temp))
    {
        SpeedCtrl * m_pSpeed = SpeedCtrl::getInstance();

        DRIVER_TX_THROTTLE_t throttle_msg;
        DRIVER_TX_THROTTLE_decode(&throttle_msg,
                (uint64_t*) &fc_temp.data.bytes, &DRIVER_TX_THROTTLE_HDR);

        if (throttle_msg.DRIVER_THROTTLE_stop)
        {   m_pSpeed->setStop();
        }
        else
        {
            bool isForward =
                    throttle_msg.DRIVER_THROTTLE_forward ? true : false;
            bool isIncr = throttle_msg.DRIVER_THROTTLE_incr ? true : false;

            if (throttle_msg.DRIVER_THROTTLE_usecustom)
            {
                m_pSpeed->setSpeedCustom(isForward,
                        throttle_msg.DRIVER_THROTTLE_custom);
            }
            else
            {
                if (isForward)
                {
                    if (isIncr)
                        m_pSpeed->incrSpeedPWM();
                    else
                        m_pSpeed->descrSpeedPWM();
                }
                else
                {
                    if (isIncr)
                        m_pSpeed->descrSpeedPWM();
                    else
                        m_pSpeed->incrSpeedPWM();
                }
            }
        }
    }

}
//#define PRINT_ALL_CAN_MSG
void readCANMsgs(void)
{
    std::string printStr ("");
    char buffer[100];
    can_fullcan_msg_t fc_temp;

    float rpm = 0;
    float speed = 0;
    SpeedMonitor::getInstance()->getSpeed(&rpm, &speed);

    int desiredDir = SpeedCtrl::getInstance()->getGoDesiredDirection();
    bool stop = desiredDir == SpeedCtrl::goStop;
    bool isForward = desiredDir == SpeedCtrl::goForward;

    sprintf(buffer, "RPM:%d, Act:%s\n", (int)rpm, stop? "Stop": (isForward? "Fwd": "Back"));
    printStr.append(buffer);

    can_fullcan_msg_t *sensor_fc_ptr = CAN_fullcan_get_entry_ptr(
            CAN_gen_sid(can1, CAN_MSG_ID_SENSOR));

    if (CAN_fullcan_read_msg_copy(sensor_fc_ptr, &fc_temp))
    {

        const int centerSensor = 1;
        const int leftSensor = 3;
        const int rightSensor = 5;
        const int backSensor = 7;
        if (fc_temp.data_len == 8)
        {
            sprintf(buffer, "L%d|C%d|R%d|B%d",
                    fc_temp.data.bytes[leftSensor],
                    fc_temp.data.bytes[centerSensor],
                    fc_temp.data.bytes[rightSensor],
                    fc_temp.data.bytes[backSensor] );
            printStr.append(buffer);
        }
    }

    can_fullcan_msg_t *compass_fc_ptr = CAN_fullcan_get_entry_ptr(
            CAN_gen_sid(can1, CAN_MSG_ID_GPS_COMPASS));
    if (CAN_fullcan_read_msg_copy(compass_fc_ptr, &fc_temp))
    {


        sprintf(buffer, "T:%d|Chk:%d\nFD:%d|CD:%d\n",
                fc_temp.data.bytes[0],
                fc_temp.data.bytes[1],
                fc_temp.data.words[1],
                fc_temp.data.words[2]);
        printStr.append(buffer);
    }

    can_fullcan_msg_t *gps_fc_ptr = CAN_fullcan_get_entry_ptr(
            CAN_gen_sid(can1, CAN_MSG_ID_GPS_GPS));
    if (CAN_fullcan_read_msg_copy(gps_fc_ptr, &fc_temp))
    {

#ifdef PRINT_ALL_CAN_MSG
        for (int i = 0; i < fc_temp.data_len; i ++)
        {
            printf("GPS::GPS(%d) %x\n", i, fc_temp.data.bytes[i]);
        }
#endif
    }

    Uart2::getInstance().putline("$CLR_SCR");
    Uart2::getInstance().printf(printStr.c_str());

}
void sendSpeed(void)
{
    float rpm = 0;
    float speed = 0;
    SpeedMonitor::getInstance()->periodGetSpeed(&rpm, &speed);
    printf("RPM %f\n", rpm);

    can_msg_t msg;

    msg.data.bytes[0] = (((int) speed ) >> 8) & 0xff;
    msg.data.bytes[1] = ((int) speed ) & 0xff;
    msg.data.bytes[2] = (((int) rpm ) >> 8) & 0xff;
    msg.data.bytes[3] = ((int) rpm ) & 0xff;
    msg.msg_id = CAN_MSG_ID_SPEED;
    msg.frame_fields.data_len = 4;//hdr.dlc;
    CAN_tx(can_t::can1, &msg, 0);
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
