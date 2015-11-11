/*
 * CompassGPS_calculation.hpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhishek Gurudutt, Tejeshwar
 *
 *  For the project CMPE243: Self drive car
 *  This file performs calculations for the heading direction and total distance.
 */

#ifndef L5_APPLICATION_COMPASSGPS_CALCULATION_HPP_
#define L5_APPLICATION_COMPASSGPS_CALCULATION_HPP_

#include "math.h"

/*
 * Function to calculate the current direction!!
 * This function should take care of the direction of the car when it is heading towards destination
 * or intermediate check points.
 * Implementing the heading formula to compute traveling direction
 * @latitude1  : latitude of current position
 * @longitude1 : longitude of current position
 * @latitude2  : latitude of upcoming checkpoint
 * @longitude2 : longitude of upcoming checkpoint
*/
double_t headingdir(double_t latitude1, double_t longitude1, double_t latitude2, double_t longitude2);

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
 * @currentLat : latitude of current position
 * @currentLong : longitude of current position
 * @chkPntLat : latitude of the upcoming checkpoint
 * @chkPntLong : longitude of the upcoming checkpoint
 */
float_t calcDistToNxtChkPnt(double_t currentLat, double_t currentLong, double_t chkPntLat, double_t chkPntLong);

/*
 * Calculates the distance to final destination
 * This is the sum of distance of all the checkpoints from the present location
 * @distToNxtChkPnt : distance to next checkpoint
 */
float_t calcDistToFinalDest(float_t distToNxtChkPnt);

/*
 * Checks if the checkpoint has reached
 * @currentLat : latitude of current position
 * @currentLong : longitude of current position
 * @chkPntLat : latitude of the upcoming checkpoint
 * @chkPntLong : longitude of the upcoming checkpoint
 *
 * Returns true if reached.
 */
bool checkPntReached(float_t distance);

/*
 * This updates the checkpoint number
 */
bool updateDestPoints(bool isFinal);

/*
 * update message telling destination is reached
 */
void destReached();

#endif /* L5_APPLICATION_COMPASSGPS_CALCULATION_HPP_ */
