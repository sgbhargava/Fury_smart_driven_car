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
#include "hashDefine.hpp"
#include "inttypes.h"

lat_long_info *receive_gpsdata = new lat_long_info;
lat_long_info *transmit_gpsdata = new lat_long_info;
compass_distance_info *transmit_compassdata = new compass_distance_info;


void gps_transmit(uint32_t msgID,uint64_t *updateGps_data)
{
    can_msg_t *gpsData = NULL;
    gpsData->msg_id = msgID;
    gpsData->data.qword = *updateGps_data;
    CAN_tx(can1,gpsData,10);

 //   gpsData->msg_id    = 0x00;/*Subject to change - this is just for my reference*/


/*    gpsData->data.qword = transmit_gpsdata->lat_dec;
    gpsData->data.qword = transmit_gpsdata->lat_float;*/


/*
     (gpsData->data.qword) = *(lat_long_info*) (transmit_gpadata->lat_dec);
     (gpsData->data.qword) = *(lat_long_info*) (transmit_gpadata->lat_float);
     (gpsData->data.qword) = *(lat_long_info*) (transmit_gpadata->long_dec);
     (gpsData->data.qword) = *(lat_long_info*) (transmit_gpadata->long_float);
     (gpsData->data.qword) = *(lat_long_info*) (transmit_gpadata->chkPoint);
*/

    //transmit_gpadata->lat_dec    =    *(lat_long_info*)  (&(gpsData->data.qword));
    //transmit_gpadata->lat_float  =    *(lat_long_info*)  (&(gpsData->data.qword));
    //transmit_gpadata->long_dec   =    *(lat_long_info*)  (&(gpsData->data.qword));
    //transmit_gpadata->long_float =    *(lat_long_info*)  (&(gpsData->data.qword));
    //transmit_gpadata->chkPoint   =    *(lat_long_info*)  (&(gpsData->data.qword));
}

void sendGPS_data(uint8_t *currentChkPnt,double_t *currentLat, double_t *currentLon)
{
    printf("1111\n");
    uint32_t sendLat_dec = (uint32_t) *currentLat;
    uint32_t sendLat_float = (*currentLat - sendLat_dec) * (TEN_6);
    uint32_t sendLon_dec = (uint32_t) *currentLon;
    uint32_t sendLon_float = (*currentLon - sendLon_dec) * (TEN_6);

    transmit_gpsdata->lat_dec    = sendLat_dec;
    transmit_gpsdata->lat_float  = sendLat_float;
    transmit_gpsdata->long_dec   = sendLon_dec;
    transmit_gpsdata->long_float = sendLon_float;
    transmit_gpsdata->chkPoint   = *currentChkPnt;


/*
    printf("%d %d %d %d %d\n",transmit_gpsdata->lat_dec,transmit_gpsdata->lat_float,
            transmit_gpsdata->long_dec,transmit_gpsdata->long_float,transmit_gpsdata->chkPoint);
*/



//    uint64_t Var = transmit_gpsdata->gps_transmit_data ;
    uint64_t presentGps_data = *(uint64_t *) (transmit_gpsdata);
    uint32_t msgID = 0x164;

//    getdata(&Var);
    gps_transmit(msgID,&presentGps_data);
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
    uint32_t msgID = 0x166;

    gps_transmit(msgID, &presentCompassDist_data);
}

void getdata()
{
    can_msg_t *data = NULL;
    CAN_rx(can1,data,100);

/*    receive_gpsdata = (lat_long_info*)  (temp);*/

    if(data->msg_id == 00)
        receive_gpsdata = (lat_long_info*)  &(data->data.qword);

    else
        printf("request for retransmission");

/*    printf("%d %d %d %d %d\n",receive_gpsdata->lat_dec,receive_gpsdata->long_dec,
            receive_gpsdata->long_float,receive_gpsdata->long_float,receive_gpsdata->chkPoint);*/

    addChkPnts(receive_gpsdata->lat_dec,receive_gpsdata->long_dec,receive_gpsdata->long_dec,
                                                receive_gpsdata->long_float,receive_gpsdata->chkPoint);
}

void compass_transmit()
{
    can_msg_t *compassData = NULL;
    compassData->msg_id = 0x00;/*Subject to change - this is just for my reference*/
    compassData->data.qword = 0;

/*
    *(lat_long_info*)  (&(compassData->data.qword)) = transmit_compassdata->current_angle;
    *(lat_long_info*)  (&(compassData->data.qword)) = transmit_compassdata->desired_angle;
    *(lat_long_info*)  (&(compassData->data.qword)) = transmit_compassdata->checkpoint;
    *(lat_long_info*)  (&(compassData->data.qword)) = transmit_compassdata->dist_finaldest;
    *(lat_long_info*)  (&(compassData->data.qword)) = transmit_compassdata->dist_nxtpnt;
*/

    CAN_tx(can1,compassData,10);
}

void heartbeat()
{
    can_msg_t *heartBeat_msg = NULL;
    heartBeat_msg->msg_id = 0x00;/*Subject to change - this is just for my reference*/

    CAN_tx(can1,heartBeat_msg,10);
}
