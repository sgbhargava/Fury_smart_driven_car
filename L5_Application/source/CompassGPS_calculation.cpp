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
#include "math.h"

#define RADIUS  6371        // This is the radius of earth in km.
#define PI      3.14159     // value of PI

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
float calcDistToNxtChkPnt(float currentLat, float currentLong, float chkPntLat, float chkPntLong)
{
    float dist;


}

/*
 * Calculates the distance to final destination
 * This is the sum of distance of all the checkpoints
 */
float calcDistToFinalDest(float currentLat, float currentLong, float destLat, float destLong)
{

}
