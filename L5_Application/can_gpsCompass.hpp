/*
 * can_gpsCompass.hpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhi
 */

#ifndef L5_APPLICATION_CAN_GPSCOMPASS_HPP_
#define L5_APPLICATION_CAN_GPSCOMPASS_HPP_

#include "stdint.h"
#include "math.h"

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
 */
void addChkPnts(uint8_t latDec, uint32_t latFloat, uint8_t lonDec, uint32_t lonFloat, uint8_t num);

/* returns the number of checkpoints added */
uint8_t getNumOfChkPnts();

/* returns the present check point */
uint8_t getPresentChkPnt();

/* returns the present longitude value */
double_t getLongitude(uint8_t longitudeNumber);

/* returns the present latitude value */
double_t getLatitude(uint8_t latitudeNumber);

/* Updates to next checkpoint */
bool updateToNxtChkPnt();
#endif /* L5_APPLICATION_CAN_GPSCOMPASS_HPP_ */
