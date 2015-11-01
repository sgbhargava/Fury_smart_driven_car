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
    static float_t finalDist;
    uint8_t chkPnt = getPresentChkPnt();
    uint8_t totalChkPnts = getNumOfChkPnts();
    static uint8_t prevChkPnt;

    if(prevChkPnt != chkPnt){
        for (int i = chkPnt; i < totalChkPnts; i++)
        {
            finalDist = calcDistToNxtChkPnt(getLongitude(i), getLatitude(i), getLongitude(i+1), getLatitude(i+1));
        }
    }
    finalDist = finalDist + distToChkPnt;
    prevChkPnt = chkPnt;

    return finalDist;
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
