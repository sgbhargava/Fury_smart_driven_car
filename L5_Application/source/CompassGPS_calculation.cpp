/*
 * CompassGPS_calculation.cpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhishek Gurudutt, Tejeshwar
 *
 *  For the project CMPE243: Self drive car
 *  This file performs calculations for the heading direction and total distance.
 */

#include "CompassGPS_calculation.hpp"
#include "can_gpsCompass.hpp"
#include "stdint.h"

#define TO_DEG  (180/3.14159)
#define RADIUS  6371000             // This is the radius of earth in meters.
#define TO_RAD  (3.14159 / 180)     // value of PI by angle


float_t calcDistToNxtChkPnt(float_t currentLat, float_t currentLong, float_t chkPntLat, float_t chkPntLong)
{
    float_t dist;

    double_t phi = (chkPntLat - currentLat) * TO_RAD;
    double_t phi1 = currentLat * TO_RAD;
    double_t phi2 = chkPntLat * TO_RAD;
    double_t lamda = (chkPntLong - currentLong) * TO_RAD;

    dist = (float_t) 2 * RADIUS * asin(sqrt((sin(phi/2) * sin(phi/2))
                                     + (cos(phi1) * cos(phi2) * sin(lamda/2) * sin(lamda/2))));

    return dist;

}


float_t calcDistToFinalDest(float_t distToChkPnt)
{
    static float_t finalDist = 0.0;
    uint8_t chkPnt = getPresentChkPnt();
    uint8_t totalChkPnts = getNumOfChkPnts();
    static uint8_t prevChkPnt = 0;

    // Calculating the total distance of the rest of checkpoints.
    if(prevChkPnt != chkPnt){
        for (uint8_t i = chkPnt; i < totalChkPnts; i++)
        {
            finalDist += calcDistToNxtChkPnt(getLongitude(i), getLatitude(i), getLongitude(i+1), getLatitude(i+1));
        }
    }

    // adding the present distance to checkpoint with the rest of the checkpoint distance
    finalDist = finalDist + distToChkPnt;

    prevChkPnt = chkPnt;

    return finalDist;
}


float_t headingdir(float_t latitude1, float_t longitude1,float_t latitude2,float_t longitude2)
{
    double_t delta_longitude,firstterm,secondterm,firstproduct,secondproduct,headingdirection;

    delta_longitude = TO_RAD * (longitude2 - longitude1);

    latitude1 = TO_RAD * (latitude1);
    latitude2 = TO_RAD * (latitude2);

    longitude1 = TO_RAD * (longitude1);
    longitude2 = TO_RAD * (longitude2);

    firstterm = sin(delta_longitude)*cos(latitude2);
    firstproduct = cos(latitude1)*sin(latitude2);
    secondproduct = (sin(latitude1)*cos(latitude2)*cos(delta_longitude));
    secondterm = firstproduct - secondproduct;

    headingdirection = atan2(firstterm,secondterm);
    headingdirection = (float_t) TO_DEG * (headingdirection);

    headingdirection = fmodf((headingdirection+360),360);
    return headingdirection;
}

bool checkPntReached(float_t currentLat, float_t currentLong, float_t chkPntLat, float_t chkPntLong)
{
    const float_t vicinity = 0.001;
    bool latInUpperBound, latInLowerBound, longInUpperBound, longInLowerBound;

    latInUpperBound = (currentLat <= (chkPntLat + vicinity)) && (currentLat >= chkPntLat);
    latInLowerBound = (currentLat >= (chkPntLat - vicinity)) && (currentLat <= chkPntLat);

    longInUpperBound = (currentLong <= (chkPntLat + vicinity)) && (currentLong >= chkPntLong);
    longInLowerBound = (currentLong >= (chkPntLat + vicinity)) && (currentLong <= chkPntLong);

    if((latInLowerBound || latInUpperBound) && (longInLowerBound || longInUpperBound))
        return true;

    return false;
}

bool updateDestPoints(bool isFinal)
{
    if(!isFinal)
    {
        isFinal = updateToNxtChkPnt();
        /*presentChkPnt = getPresentChkPnt();
        chkPntLat = getLongitude(presentChkPnt);
        chkPntLon = getLatitude(presentChkPnt);
        */
        // also update master.
    }
    if(isFinal)
    {
        //update reached
    }

    return isFinal;
}
