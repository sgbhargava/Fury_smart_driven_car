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

#define RADIUS  6371                // This is the radius of earth in km.
#define TO_RAD  (3.14159 / 180)     // value of PI

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


float_t calcDistToFinalDest(float_t currentLat, float_t currentLong, float_t destLat, float_t destLong)
{

    float_t finalDist;


    return finalDist;
}
