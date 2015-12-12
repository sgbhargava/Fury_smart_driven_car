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

#include "compass.hpp"
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "can.h"
#include "gps.hpp"
#include "CompassGPS_calculation.hpp"
#include "can_gpsCompass.hpp"
#include "hashDefine.hpp"
#include "tlm/c_tlm_comp.h"
#include "tlm/c_tlm_var.h"
#include "receive_Canmsg.hpp"
#include "lpc_sys.h"
#include "sevenSeg_display.hpp"

const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

gpsData_t  gpsCurrentData;
float_t    distanceToDest = 0, distanceToChkPnt = 0, currentHeading = 0;
double_t   chkPntLat = 0, chkPntLon = 0, desiredHeading = 0;
uint8_t    presentChkPnt = 0, compassMode = 0;
bool       finalChkPnt_b = false;
int8_t     turn;

#if TESTCODE
double_t test_chkPntLat[5], test_chkPntLong[5];
#endif

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    can_communicationInit();
    can_addMsgIDs(MASTER_RESET_ID, COMM_GPSDATA_ID);
    //can_addMsgIDs(, 0xFFFF);

    CAN_reset_bus(can1);

    heartbeat();

    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    tlm_component *gpsCompass_cmp = tlm_component_add("GPS_Compass");

    TLM_REG_VAR(gpsCompass_cmp, presentChkPnt, tlm_uint);
    TLM_REG_VAR(gpsCompass_cmp, gpsCurrentData.latitude, tlm_float);
    TLM_REG_VAR(gpsCompass_cmp, gpsCurrentData.longitude, tlm_float);
    TLM_REG_VAR(gpsCompass_cmp, chkPntLat, tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, chkPntLon, tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, distanceToChkPnt, tlm_float);
    TLM_REG_VAR(gpsCompass_cmp, distanceToDest, tlm_float);
    TLM_REG_VAR(gpsCompass_cmp, compassMode, tlm_uint);
    TLM_REG_VAR(gpsCompass_cmp, desiredHeading, tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, currentHeading, tlm_float);
    TLM_REG_VAR(gpsCompass_cmp, finalChkPnt_b, tlm_bit_or_bool);
    TLM_REG_VAR(gpsCompass_cmp, turn, tlm_int);

#if TESTCODE
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLat[0], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLong[0], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLat[1], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLong[1], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLat[2], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLong[2], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLat[3], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLong[3], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLat[4], tlm_double);
    TLM_REG_VAR(gpsCompass_cmp, test_chkPntLong[4], tlm_double);
#endif

    return true; // Must return true upon success
}

void period_1Hz(void)
{
#if TESTCODE
    uint8_t i = 0;

    //printf("chkpnt: %d, distToChkPnt: %d, distToFinalDest: %d\n", presentChkPnt, distToChkPnt, distanceToDest);
    for(i = 0; (i < getNumOfChkPnts() && i < 5); i++)
    {
        test_chkPntLat[i] = getLatitude(i+1);
        test_chkPntLong[i] = getLongitude(i+1);
    }
#endif
    heartbeat();
}

void period_10Hz(void)
{
    static QueueHandle_t gpsCurrData_q = scheduler_task::getSharedObject("gps_queue");
    double_t presentLat, presentLon;

// compass reading and calibration
    if(BEARINGMODE == compassMode)
        currentHeading = compassBearing_inDeg();
    printf("currentHeading:%f\n",currentHeading);

    if (CALIBRATIONMODE == compassMode)
        compassMode = compass_calibrationMode(compassMode); //calibration mode
    else if (HEADINGMODE == compassMode)
        compassMode = compass_headingMode(); //To get back to bearing compassMode
    else
        LD.setNumber(13);

    if (SW.getSwitch(2))
    {
        compassMode = HEADINGMODE; //0
        LD.setNumber(10);
    }
    if (SW.getSwitch(1))
        compassMode = CALIBRATIONMODE;

    if(SW.getSwitch(3))
        compass_factoryReset();

// read gps and compute everything
    if(NULL == gpsCurrData_q)
    {
        LE.on(1);
        LE.on(2);
    }
    else if(xQueueReceive(gpsCurrData_q, &gpsCurrentData, 0))
    {
        LE.off(1);
        LE.off(2);

        presentChkPnt = getPresentChkPnt();

        // present car latitude and longitude
        presentLat = gpsCurrentData.latitude;
        presentLon = gpsCurrentData.longitude;

        // latitude and longitude of checkpoint
        chkPntLat = getLatitude(presentChkPnt);
        chkPntLon = getLongitude(presentChkPnt);

        // heading degree of car
        desiredHeading = headingdir(presentLat, presentLon, chkPntLat, chkPntLon);


        // turn to left if negative and right if positive
        turn = turnDecision(desiredHeading, currentHeading);

        // Distance of checkpoint and final distance
        distanceToChkPnt = calcDistToNxtChkPnt(presentLat, presentLon, chkPntLat, chkPntLon);
        distanceToDest = calcDistToFinalDest(distanceToChkPnt);

        // check if the car has reached the checkpoint
        finalChkPnt_b = checkPntReached(distanceToChkPnt);

        //Sending GPS data and compass data to master.
        sendGPS_data(&presentChkPnt,&presentLat,&presentLon, finalChkPnt_b);
        sendCompass_data(turn, presentChkPnt, distanceToChkPnt, distanceToDest, finalChkPnt_b);
        sendDegrees_data(desiredHeading, currentHeading);

        if(finalChkPnt_b)
            destReached();

    }
    else
    {
        finalChkPnt_b = false;
        distanceToDest = 0;
        distanceToChkPnt = 0;
        desiredHeading = 0;
        gpsCurrentData.latitude = 0;
        gpsCurrentData.longitude = 0;
        presentChkPnt = 0;

        LE.toggle(1);
        LE.toggle(2);
    }

    /*presentChkPnt = 2;
    presentLat = 37.33345;
    presentLon = 121.33345;
    finalChkPnt_b = 1;
    distanceToChkPnt = 34;
    distanceToDest = 189;
    //sendGPS_data(&presentChkPnt,&presentLat,&presentLon, finalChkPnt_b);
    sendCompass_data(turn, presentChkPnt, distanceToChkPnt, distanceToDest, finalChkPnt_b);
*/

}


void period_100Hz(void)
{
    uint64_t data = 0;

    if(can_receive(COMM_GPSDATA_ID, &data))
    {
        if(!can_addGPSData(&data))
            sys_reboot();
    }

    if(can_receive(MASTER_RESET_ID, &data))
        sys_reboot();
}

void period_1000Hz(void)
{

}
