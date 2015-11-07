/*
 * can_gpsCompass.cpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhishek Gurudutt, Tejeshwar
 *
 *  CAN communication code
 */

#include "can_gpsCompass.hpp"


#define CONVERT_TOMIN       60              // convert deg to minute
#define TEN_6               1000000         // 10^6
#define TEN_2               100             // 10^2

//static checkPointData_t *nextChkPnt = NULL;
static checkPointData_t *prevChkPnt = NULL;
static checkPointData_t *firstChkPnt = NULL;
static uint8_t numberOfChkPnts = 0;
static checkPointData_t *giveCheckPoint = NULL;

void addChkPnts(uint8_t latDec, uint32_t latFloat, uint8_t lonDec, uint32_t lonFloat, uint8_t num)
{
    double_t calcLat, calcLong;
    checkPointData_t *newChkPnt = new checkPointData_t;
    if (NULL != newChkPnt)
    {
        // Calculating the floating part and concatenating in the form lat: DDMM.MMMM, long: DDDMM.MMMM
        calcLat = (latFloat / (float_t)TEN_6) * CONVERT_TOMIN;
        calcLat = (latDec * (float_t)TEN_2) + calcLat;

        calcLong = (lonFloat / (float_t)TEN_6) * CONVERT_TOMIN;
        calcLong = ((float_t)lonDec * TEN_2) + calcLong;

        // Storing the values in a structure.
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

double_t getLongitude(uint8_t longitudeNumber)
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
    return 0;
}

double_t getLatitude(uint8_t latitudeNumber)
{
    checkPointData_t *pntToGetLat = NULL;
    pntToGetLat = firstChkPnt;

    while(pntToGetLat != NULL)
    {
        return pntToGetLat->chkPntLat;
        if(pntToGetLat->chkPntNo == latitudeNumber)
            return pntToGetLat->chkPntLat;
        else
            pntToGetLat = pntToGetLat->next;
    }
    return 0;
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
