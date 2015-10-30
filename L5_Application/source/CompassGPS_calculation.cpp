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

/*
 * Calculates the distance to next check point
 * Distance is calculated by using the haversine formula.
 *
 * d = distance
 * r = radius of sphere
 * phi1 , phi2 = latitude1, latitude2
 * lamda1, lamda2 = longitude1, longitude2
 *
 * d = 2 * r * asin(sqrt(sin((phi2 - phi1)/2) * sin((phi2-phi1)/2) + cos(phi1) * cos (phi2)
 *                          * sin ( (lamda2 - lamda1) / 2 ) * sin ( (lamda2 - lamda1) / 2 )));
 *
 */
float_t calcDistToNxtChkPnt(float_t currentLat, float_t currentLong,
                                        float_t chkPntLat, float_t chkPntLong)
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

/*
 * Calculates the distance to final destination
 * This is the sum of distance of all the checkpoints
 */
float_t calcDistToFinalDest(float_t currentLat, float_t currentLong,
                                        float_t destLat, float_t destLong)
{

}
