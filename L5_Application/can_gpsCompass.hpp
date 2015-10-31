/*
 * can_gpsCompass.hpp
 *
 *  Created on: Oct 28, 2015
 *      Author: Abhi
 */

#ifndef L5_APPLICATION_CAN_GPSCOMPASS_HPP_
#define L5_APPLICATION_CAN_GPSCOMPASS_HPP_

#include "math.h"
#include "stdint.h"

typedef struct checkPointData{
        float_t chkPntLat;          // latitude of checkpoint
        float_t chkPntLong;         // longitude of checkpoint
        uint8_t chkPntNo;           // checkpoint number
        checkPointData *next;       // pointing to next node
        checkPointData *prev;       // pointing to previous node
}checkPointData_t;

/*
 * Adds checkpoint data
 * @lat : latitude
 * @lon : longitude
 * @num : checkpoint number
 */
void addChkPnts(float_t lat, float_t lon, uint8_t num);

/* returns the number of checkpoints added */
uint8_t getNumOfChkPnts();

/*
 *
 */

#endif /* L5_APPLICATION_CAN_GPSCOMPASS_HPP_ */
