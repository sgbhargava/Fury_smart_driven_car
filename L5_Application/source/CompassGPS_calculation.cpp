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
#define To_DEG  (180/3.14159)

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
 * This is the sum of distance of all the checkpoints from the present location
 */
float_t calcDistToFinalDest(float_t currentLat, float_t currentLong,
                                        float_t destLat, float_t destLong)
{

    float_t finalDist;


    return finalDist;
}

/*
 * Function to calculate the current direction!!
 * This function should take care of the direction of the car when it is heading towards destination
 * or intermediate check points.
 * Implementing the heading formula to compute traveling direction
*/

float_t headingdir(float_t latitude1, float_t longitude1,float_t latitude2,float_t longitude2)
{
    double_t delta_longitude,firstterm,secondterm,firstproduct,secondproduct,headingdirection;

    checkPointData_t *getnextcheckpnt = new checkPointData_t;
    latitude2 = getnextcheckpnt->chkPntLat;
    longitude2 = getnextcheckpnt->chkPntLong;

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
    headingdirection = (float_t) To_DEG * (headingdirection);

    headingdirection = fmodf((headingdirection+360),360);
    return headingdirection;
}
