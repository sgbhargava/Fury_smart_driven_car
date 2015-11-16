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

typedef union{
        struct{
        uint32_t lat_dec     :8;
        uint32_t lat_float   :20;
        uint32_t long_dec    :8;
        uint32_t long_float  :20;
        uint32_t chkPoint    :8;
}__attribute__((packed));
    uint64_t gps_transmit_data;
}lat_long_info;


typedef union{
        struct{
        uint32_t current_angle  : 12;
        uint32_t desired_angle  : 12;
        uint8_t  checkpoint     : 8;
        uint32_t dist_finaldest : 16;
        uint32_t dist_nxtpnt    : 16;
}__attribute__((packed));
    uint64_t compass_dist_data;
}compass_distance_info;


void sendGPS_data(uint8_t *,double_t*, double_t*);

void sendCompass_data(float_t currentDir, double_t desiredDir, uint8_t presentChkPnt,
        float_t nxtChkPntDist, float_t finalDestDist);

/*Receives data from CANBUS*/
void can_receive();

/*Transmits data to master*/
void can_transmit(uint32_t,uint16_t *);

/*Transmit heartbeat message to master*/
void heartbeat();

#endif /* RECEIVE_CANMSG_HPP_ */
