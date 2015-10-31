/*
 * can_gpsCompass.cpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhishek Gurudutt, Tejeshwar
 *
 *  CAN communication code
 */

#include "can_gpsCompass.hpp"

//static checkPointData_t *nextChkPnt = NULL;
static checkPointData_t *prevChkPnt = NULL;
static checkPointData_t *firstChkPnt = NULL;
static uint8_t numberOfChkPnts = 0;


void addChkPnts(float_t lat, float_t lon, uint8_t num)
{
    checkPointData_t *newChkPnt = new checkPointData_t;
    if (NULL != newChkPnt)
    {
        newChkPnt->chkPntLat = lat;
        newChkPnt->chkPntLong = lon;
        newChkPnt->chkPntNo = num;
        newChkPnt->prev = prevChkPnt;
        newChkPnt->next = NULL;
        ++numberOfChkPnts;

            if(NULL == firstChkPnt)
            {
                firstChkPnt = newChkPnt;
            }

            if(NULL == prevChkPnt)
            {
                prevChkPnt = newChkPnt;
            }
            else
            {
                prevChkPnt->next = newChkPnt;
                prevChkPnt = newChkPnt;
            }
    }
}

uint8_t getNumOfChkPnts()
{
    return numberOfChkPnts;
}
