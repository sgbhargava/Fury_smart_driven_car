/*
 * can_gpsCompass.cpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhishek Gurudutt, Tejeshwar
 *
 *  CAN communication code
 */

#include "can_gpsCompass.hpp"

#define MASKUPPER_TWO       0xFF000000      // mask upper byte in a 32-bit number
#define MASKUPPER_THREE     0xFFF00000      // mask upper 12bits in a 32-bit numbber
#define SHIFTBY_16BIT       16              // to shift a number by 16 bits
#define SHIFTBY_12BIT       12              // shift a number by 12 bits
#define CONVERT_TOMIN       60              // convert deg to minute
#define TEN_6               1000000         // 10^6
#define TEN_5               100000          // 10^5

//static checkPointData_t *nextChkPnt = NULL;
static checkPointData_t *prevChkPnt = NULL;
static checkPointData_t *firstChkPnt = NULL;
static uint8_t numberOfChkPnts = 0;
static checkPointData_t *giveCheckPoint = NULL;

void addChkPnts(uint32_t lat, uint32_t lon, uint8_t num)
{
    float_t calcLat, calcLong;
    checkPointData_t *newChkPnt = new checkPointData_t;
    if (NULL != newChkPnt)
    {
        calcLat = (lat & MASKUPPER_TWO) >> SHIFTBY_16BIT ;
        calcLat = calcLat + (float_t)((lat & ~(MASKUPPER_TWO)) * CONVERT_TOMIN / TEN_6);

        calcLong = (lon & MASKUPPER_THREE) >> SHIFTBY_12BIT;
        calcLong = calcLong + (float_t)((lon & ~(MASKUPPER_THREE)) * CONVERT_TOMIN / TEN_5);

        newChkPnt->chkPntLat = calcLat;
        newChkPnt->chkPntLong = calcLong;
        newChkPnt->chkPntNo = num;
        newChkPnt->prev = prevChkPnt;
        newChkPnt->next = NULL;
        ++numberOfChkPnts;

            if(NULL == firstChkPnt)
            {
                firstChkPnt = newChkPnt;
                giveCheckPoint = firstChkPnt;
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

uint8_t getPresentChkPnt()
{
    //giveCheckPoint = firstChkPnt;

   /* while(giveCheckPoint != NULL)
    {
        if(giveCheckPoint == prevChkPnt)
            return giveCheckPoint->chkPntNo;
        else
            giveCheckPoint = giveCheckPoint->next;
    }*/
    return giveCheckPoint->chkPntNo;
}

float_t getLongitude(uint8_t longitudeNumber)
{
    checkPointData_t *pntToGetLong = NULL;
    pntToGetLong = firstChkPnt;

    while(pntToGetLong != NULL)
    {
        if(pntToGetLong->chkPntNo == longitudeNumber)
            return pntToGetLong->chkPntLong;
        else
            pntToGetLong = pntToGetLong->next;
    }
}

float_t getLatitude(uint8_t latitudeNumber)
{
    checkPointData_t *pntToGetLat = NULL;
    pntToGetLat = firstChkPnt;

    while(pntToGetLat != NULL)
    {
        if(pntToGetLat->chkPntNo == latitudeNumber)
            return pntToGetLat->chkPntLat;
        else
            pntToGetLat = pntToGetLat->next;
    }
}

bool updateToNxtChkPnt()
{
    if(giveCheckPoint->next != NULL)
    {
        giveCheckPoint = giveCheckPoint->next;
        return true;
    }
    else
        return false;
}
