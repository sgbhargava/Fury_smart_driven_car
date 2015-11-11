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

#define BEARINGMODE     0
#define CALIBRATIONMODE 1
#define HEADINGMODE     2

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);


void period_1Hz(void)
{

}

void period_10Hz(void)
{
    char LEDdisplay[4] = {'F', 'U', 'R', 'Y'};
    static QueueHandle_t gpsCurrData_q = scheduler_task::getSharedObject("gps_queue");
    gpsData_t gpsCurrentData;
    float_t distToDest, distToChkPnt;
    double_t presentLat, presentLon, chkPntLat, chkPntLon, currentheading;
    uint8_t presentChkPnt, selectChar = 0;
    static bool finalChkPnt_b = false;
    bool chkPntRchd_b = false;
    static uint8_t mode = 0;

    if(NULL == gpsCurrData_q)
    {
        LE.toggle(2);
    }
    else if(xQueueReceive(gpsCurrData_q, &gpsCurrentData, 0))
    {
        presentChkPnt = getPresentChkPnt();

        // present car latitude and longitude
        presentLat = gpsCurrentData.latitude;
        presentLon = gpsCurrentData.longitude;

        // latitude and longitude of checkpoint
        chkPntLat = getLongitude(presentChkPnt);
        chkPntLon = getLatitude(presentChkPnt);
        //chkPntReached = checkPntReached(gpsCurrentData.latitude, gpsCurrentData.longitude, chkPntLat, chkPntLon);

        // heading degree of car
        currentheading = headingdir(presentLat, presentLon, chkPntLat, chkPntLon);
        compass_actualHeadingDir(currentheading);

        // Distance of checkpoint and final distance
 /*       distToChkPnt = calcDistToNxtChkPnt(presentLat, presentLon, chkPntLat, chkPntLon);
        distToDest = calcDistToFinalDest(distToChkPnt);

        // check if the car has reached the checkpoint
        finalChkPnt_b = checkPntReached(distToChkPnt);

        if(finalChkPnt_b)
        {
            // if final checkpoint reached then display 'FURY'
            LD.setLeftDigit(LEDdisplay[selectChar]);
            LD.setRightDigit(LEDdisplay[selectChar+1]);
            selectChar = (selectChar + 1) % 3;
        }*/

    }
    else
    {

    }

    if(BEARINGMODE == mode)
    {
        //compassBearing_fullCircle();  //bearing mode
        compassBearing_inDeg();
    }
    else if(CALIBRATIONMODE == mode)
        mode = compass_calibrationMode(mode); //calibration mode

    else if(HEADINGMODE == mode)
        mode = compass_headingMode();   //To get back to bearing mode

    else
    {
        if(SW.getSwitch(2))
            mode = 2;//0

        if(SW.getSwitch(1))
            mode = 1;
    }
}

void period_100Hz(void)
{
#if CAN_USAGE
    if(CAN_is_bus_off(can1))
        CAN_reset_bus(can1);
#endif
}

void period_1000Hz(void)
{

}
