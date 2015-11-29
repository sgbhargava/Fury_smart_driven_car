/*
 * hasdDefine.hpp
 *
 *  Created on: Nov 8, 2015
 *      Author: Tejeshwar
 */

#ifndef L5_APPLICATION_HASHDEFINE_HPP_
#define L5_APPLICATION_HASHDEFINE_HPP_

/*
 * This file contains all the #define values
 * */

// #defines to enable or disable certain maodules
#define  TESTCODE               1
#define  GPSMODULE              1
#define  CAN_USAGE              1

// #defines for compass calibration
#define  BEARINGMODE            0
#define  CALIBRATIONMODE        1
#define  HEADINGMODE            2
#define  MODE_THREE             3          // RANDOMN VALUE

// #defines for conversion and calculation
#define  TO_DEG                 (180 / 3.14159)
#define  TO_RAD                 (3.14159 / 180)
#define  RADIUS                 6371000     // RADIUS OF EARTH IN METERS
#define  MINUTES                60         // TO MINUTES
#define  TEN_6                  1000000    // 10^6
#define  TEN_2                  100
#define  MAGENTIC_DECLINTAION   14
#define  SCALE                  30
#define  TWO_METERS             2

// Message IDs and data length for CAN communication
#define  HEARTBEAT_ID           0x160
#define  COMPASS_DIST_ID        0x162
#define  GPS_DATA_ID            0x461
#define  COMM_GPSDATA_ID        0x382
#define  MASTER_RESET_ID        0x320
#define  DATA_LEN_EIGHT         8
#define  DATA_LEN_SIX           6
#define  DATA_LEN_ZERO          0

#endif /* L5_APPLICATION_HASHDEFINE_HPP_ */
