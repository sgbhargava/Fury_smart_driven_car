/*
 * can_gpsCompass.hpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhishek Gurudutt, Tejeshwar
 */

#ifndef L5_APPLICATION_CAN_GPSCOMPASS_HPP_
#define L5_APPLICATION_CAN_GPSCOMPASS_HPP_

#include "stdint.h"
#include "math.h"
#include "geo_controller.hpp"

typedef struct checkPointData{
        double_t chkPntLat;          // latitude of checkpoint
        double_t chkPntLong;         // longitude of checkpoint
        uint8_t chkPntNo;           // checkpoint number
        checkPointData *next;       // pointing to next node
        checkPointData *prev;       // pointing to previous node
}checkPointData_t;


/*
 * Adds checkpoint data
 * @latDec      : Decimal part of latitude
 * @latFloat    : Floating part of latitude
 * @lonDec      : Decimal part of longitude
 * @lonFloat    : Floating part of longitude
 * @num         : checkpoint number
 * returns true if added, false if not
 */
bool addChkPnts(long_lat *lat_long_data);

/* returns the number of checkpoints added */
uint8_t getNumOfChkPnts();

/* returns the present check point */
uint8_t getPresentChkPnt();

/* returns the present longitude value */
double_t getLongitude(uint8_t longitudeNumber);

/* returns the present latitude value */
double_t getLatitude(uint8_t latitudeNumber);

/* Updates to next checkpoint
 * returns true if it is intermediate checkpoint
 * returns false if it is a final checkpoint
 */
bool updateToNxtChkPnt();

/* Updates to previous checkpoint
 * returns true if it is intermediate checkpoint
 * returns false if it is the first checkpoint
 */
bool updateToPrevChkPnt();

#endif /* L5_APPLICATION_CAN_GPSCOMPASS_HPP_ */
