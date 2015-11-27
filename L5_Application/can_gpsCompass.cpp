/*
 * can_gpsCompass.cpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhishek Gurudutt, Tejeshwar
 *
 *  functions to help in can communication.
 */

#include "can_gpsCompass.hpp"
#include "hashDefine.hpp"
#include "stdio.h"

static checkPointData_t *nextChkPnt = NULL;
static checkPointData_t *prevChkPnt = NULL;
static checkPointData_t *firstChkPnt = NULL;
static checkPointData_t *giveCheckPoint = NULL;
static uint8_t numberOfChkPnts = 0;

bool addChkPnts(long_lat *lat_long_data)
{

	uint8_t latDec = lat_long_data->lattitude_dec;
	uint32_t latFloat = lat_long_data->lattitude_float;
	uint8_t lonDec = lat_long_data->longitude_dec;
	uint32_t lonFloat = lat_long_data->longitude_float;
	uint8_t num = lat_long_data->checkpoint;
    double_t calcLat, calcLong;

    checkPointData_t *traverseChkPnt = NULL;

    // Check for multiple definition, also add the nodes in ascending order.
    traverseChkPnt = firstChkPnt;
    while(NULL != traverseChkPnt)
    {
        if((traverseChkPnt->chkPntNo < num) && (NULL != traverseChkPnt->next))
        {
            traverseChkPnt = traverseChkPnt->next;
            prevChkPnt = traverseChkPnt;
        }
        else if(traverseChkPnt->chkPntNo == num)
        {
            return false;
        }
        else if((traverseChkPnt->chkPntNo > num) && (NULL != traverseChkPnt->prev))
        {
            traverseChkPnt = traverseChkPnt->prev;
            prevChkPnt = traverseChkPnt;
        }
        else
        {
            if((NULL == traverseChkPnt->next) && (traverseChkPnt->chkPntNo < num))
            {
                nextChkPnt = NULL;
                prevChkPnt = traverseChkPnt;
                break;
            }
            else if((NULL == traverseChkPnt->prev) && (traverseChkPnt->chkPntNo > num))
            {
                nextChkPnt = traverseChkPnt;
                prevChkPnt = NULL;
                break;
            }
            else
            {
                nextChkPnt = traverseChkPnt;
                traverseChkPnt = traverseChkPnt->prev;
                prevChkPnt = traverseChkPnt;
                break;
            }
        }
    }

    // if there are no duplicate data, then create a new checkpoint
    checkPointData_t *newChkPnt = new checkPointData_t;
    if (NULL != newChkPnt)
    {
        // Calculating the floating part and concatenating in the form lat: DDMM.MMMM, long: DDDMM.MMMM
        calcLat = (latFloat / (float_t)TEN_6);
        calcLat = (latDec) + calcLat;

        calcLong = (lonFloat / (float_t)TEN_6);
        calcLong = ((float_t)lonDec) + calcLong;

        // Storing the values in a structure.
        newChkPnt->chkPntLat = calcLat;
        newChkPnt->chkPntLong = calcLong;
        newChkPnt->chkPntNo = num;
        newChkPnt->prev = prevChkPnt;
        newChkPnt->next = nextChkPnt;
        ++numberOfChkPnts;

            if(NULL == newChkPnt->prev)
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
            }

            if(NULL != nextChkPnt)
            {
                nextChkPnt->prev = newChkPnt;
            }
    }
    return true;
}

uint8_t getNumOfChkPnts()
{
    return numberOfChkPnts;
}

uint8_t getPresentChkPnt()
{
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

bool updateToPrevChkPnt()
{
    if (giveCheckPoint->prev != NULL)
    {
        giveCheckPoint = giveCheckPoint->prev;
        return true;
    }
    else
        return false;
}
