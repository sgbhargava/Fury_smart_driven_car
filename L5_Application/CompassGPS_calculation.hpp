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

float_t calcDistToNxtChkPnt(float_t currentLat, float_t currentLong, float_t chkPntLat, float_t chkPntLong);

/*
 * Calculates the distance to final destination
 * This is the sum of distance of all the checkpoints from the present location
 * @currentLat : latitude of current position
 * @currentLong : longitude of current position
 * @destLat : latitude of final destination
 * @destLong : longitude of final destination
 */
float_t calcDistToFinalDest(float_t currentLat, float_t currentLong, float_t destLat, float_t destLong);


#endif /* L5_APPLICATION_COMPASSGPS_CALCULATION_HPP_ */
