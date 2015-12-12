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

typedef union
{
        struct
        {
                uint32_t lat_dec :8;
                uint32_t lat_float :20;
                uint32_t long_dec :8;
                uint32_t long_float :20;
                uint32_t chkPoint :7;
                uint32_t bIsFinal :1;
        }__attribute__((packed));
        uint64_t gpsData;
} lat_long_info;


typedef union
{
        struct
        {
                int32_t turnDecision     : 8;
                uint32_t checkpoint      : 7;
                uint32_t isFinal         : 1;
                uint32_t dist_finalDest : 16;
                uint32_t dist_nxtPnt    : 16;
        }__attribute__((packed));
        uint64_t compassData;
} compass_distance_info;

typedef struct
{
        uint32_t desiredAngle   : 16;
        uint32_t currentAngle   : 16;
}__attribute__((packed)) degree_info;

// function to initialize can1 for communication with other modules
bool can_communicationInit();

// function to add messages
bool can_addMsgIDs(uint16_t id1, uint16_t id2);

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
void sendCompass_data(int8_t turn, uint8_t presentChkPnt, float_t nxtChkPntDist, float_t finalDestDist, bool isFinal);

/* To send compass data to comm board
 * @desiredHeading  : heading at which the car has to head.
 * @currentHeading  : heading at which is pointing.
 */
void sendDegrees_data(uint16_t desiredHeading, uint16_t currentHeading);

/*Receives data from CANBUS*/
bool can_receive(uint16_t id, uint64_t *data);

/* Transmits data to master
 * @msgID       :   Message ID
 * @data        :   data to be transmitted
 * @dataLength  :   length of data
 */
void can_transmit(uint32_t msgID, uint64_t *data, uint8_t dataLength);

/*Transmit heartbeat message to master*/
void heartbeat();

void can_checkBusOff(uint32_t a);

//Parse data into checkpoints
bool can_addGPSData(uint64_t *data);

// Transmit Ack message to comm
void can_sendAck();

#endif /* RECEIVE_CANMSG_HPP_ */
