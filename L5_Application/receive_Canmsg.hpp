/*
 * receive_Canmsg.hpp
 *
 *  Created on: Nov 10, 2015
 *      Author: Tejeshwar,Abhishek
 */

#ifndef RECEIVE_CANMSG_HPP_
#define RECEIVE_CANMSG_HPP_
#include "can.h"
#include "scheduler_task.hpp"
#include "math.h"

typedef struct {
        uint32_t lat_dec :8;
        uint32_t lat_float :20;
        uint32_t long_dec :8;
        uint32_t long_float :20;
        uint32_t chkPoint:7;
        uint32_t bIsFinal:1;
} __attribute__((packed)) lat_long_info ;


typedef struct{
        int8_t turnDecision     : 8;
        uint8_t checkpoint      : 8;
        uint32_t dist_finalDest : 16;
        uint32_t dist_nxtPnt    : 16;
}__attribute__((packed)) compass_distance_info;

// function to initialize can1 for communication with other modules
bool can_communicationInit();

/*
 * Function to pack the GPS data in a structure and transmit over CAN
 * @currentChkPnt : Present checkpoint value
 * @currentLat    : Present Latitude value
 * @currentLon    : Present Longitude value
 * @isFinal       : if it is the final checkpoint
 */
void sendGPS_data(uint8_t *currentChkPnt,double_t *currentLat, double_t *currentLon, bool isFinal);

/*
 * Function to pack the compass data in a structure and transmit over CAN
 * @turn          : deviation of current and heading degree
 * @presentChkPnt : present checkpoint value
 * @nxtChkOntDist : distance to next check point
 * @finalDestDist : distance to final destination.
 */
void sendCompass_data(int8_t turn, uint8_t presentChkPnt, float_t nxtChkPntDist, float_t finalDestDist);

/*Receives data from CANBUS*/
void can_receive();

/* Transmits data to master
 * @msgID       :   Message ID
 * @data        :   data to be transmitted
 * @dataLength  :   length of data
 */
void can_transmit(uint32_t msgID, uint64_t *data, uint8_t dataLength);

/*Transmit heartbeat message to master*/
void heartbeat();

void can_checkBusOff(uint32_t a);

#endif /* RECEIVE_CANMSG_HPP_ */
