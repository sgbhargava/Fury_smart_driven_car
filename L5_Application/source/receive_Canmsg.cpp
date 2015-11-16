/*
 * receive_Canmsg.cpp
 *
 *  Created on: Nov 10, 2015
 *      Author: Tejeshwar, Abhishek
 */

#include  <receive_Canmsg.hpp>
#include  "can.h"
#include  "stdio.h"
#include  "can_gpsCompass.hpp"
#include  "gps.hpp"
#include  "math.h"
#include  "hashDefine.hpp"
#include  "inttypes.h"
#include  "lpc_sys.h"

lat_long_info          *receive_gpsdata      = new lat_long_info;
lat_long_info          *transmit_gpsdata     = new lat_long_info;
compass_distance_info  *transmit_compassdata = new compass_distance_info;


void can_transmit(uint32_t msgID,uint64_t *updateGps_data)
{
    can_msg_t *gpsData = NULL;
    gpsData->msg_id = msgID;
    gpsData->data.qword = *updateGps_data;
    CAN_tx(can1,gpsData,10);
}

void sendGPS_data(uint8_t *currentChkPnt,double_t *currentLat, double_t *currentLon)
{
    uint32_t sendLat_dec   = (uint32_t) *currentLat;
    uint32_t sendLat_float = (*currentLat - sendLat_dec) * (TEN_6);
    uint32_t sendLon_dec   = (uint32_t) *currentLon;
    uint32_t sendLon_float = (*currentLon - sendLon_dec) * (TEN_6);

    transmit_gpsdata->lat_dec    = sendLat_dec;
    transmit_gpsdata->lat_float  = sendLat_float;
    transmit_gpsdata->long_dec   = sendLon_dec;
    transmit_gpsdata->long_float = sendLon_float;
    transmit_gpsdata->chkPoint   = *currentChkPnt;

    uint64_t presentGps_data = *(uint64_t *) (transmit_gpsdata);
    uint32_t msgID = GPS_DATA_ID;

    can_transmit(msgID,&presentGps_data);
}

void sendCompass_data(float_t currentDir, double_t desiredDir, uint8_t presentChkPnt,
        float_t nxtChkPntDist, float_t finalDestDist)
{
    transmit_compassdata->checkpoint        =   presentChkPnt;
    transmit_compassdata->current_angle     =   (uint16_t) currentDir;
    transmit_compassdata->desired_angle     =   (uint16_t) desiredDir;
    transmit_compassdata->dist_nxtpnt       =   (uint16_t) nxtChkPntDist;
    transmit_compassdata->dist_finaldest    =   (uint16_t) finalDestDist;

    uint64_t presentCompassDist_data = *(uint64_t *) (transmit_compassdata);
    uint32_t msgID = COMPASS_DIST_ID;

    can_transmit(msgID, &presentCompassDist_data);
}

void can_receive()
{
    can_msg_t *data = NULL;
    CAN_rx(can1,data,100);

    if(data->msg_id == MASTER_GPSDATA_ID)
        receive_gpsdata = (lat_long_info*)  &(data->data.qword);

    else if(data->msg_id == MASTER_RESET_ID)
        sys_reboot();

    else
        LD.setNumber(17);

    addChkPnts(receive_gpsdata->lat_dec,receive_gpsdata->long_dec,receive_gpsdata->long_dec,
                                                receive_gpsdata->long_float,receive_gpsdata->chkPoint);
}

void heartbeat()
{
    can_msg_t *heartBeat_msg = NULL;
    heartBeat_msg->msg_id = HEARTBEAT_ID;

    CAN_tx(can1,heartBeat_msg,10);
}
