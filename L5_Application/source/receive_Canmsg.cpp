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

// Generally, pointer is initialized to NULL and in the init() function they get new type

lat_long_info          *receive_gpsData      = new lat_long_info;
lat_long_info          *transmit_gpsData     = new lat_long_info;
compass_distance_info  *transmit_compassData = new compass_distance_info;


bool can_communicationInit()
{
    bool ok = CAN_init(can1, 100, 10, 10, can_checkBusOff, NULL);

    if(ok)
    {
        printf("can init\n");
    }

    return ok;

}

bool can_addMsgIDs(uint16_t id1, uint16_t id2)
{
    bool canSetup = CAN_fullcan_add_entry(can1,
                CAN_gen_sid(can1, id1),
                CAN_gen_sid(can1, id2));

    if(canSetup)
    {
        printf("Msges added\n");
    }

    return canSetup;

}

void can_transmit(uint32_t msgID, uint64_t *data, uint8_t dataLength)
{
    can_msg_t gpsData;
    gpsData.msg_id                = msgID;
    gpsData.data.qword            = *data;
    gpsData.frame_fields.data_len = dataLength;
    CAN_tx(can1, &gpsData, 10);
}

void sendGPS_data(uint8_t *currentChkPnt,double_t *currentLat, double_t *currentLon, bool isFinal)
{
    uint32_t sendLat_dec   = (uint32_t) *currentLat;
    uint32_t sendLat_float = (*currentLat - sendLat_dec) * (TEN_6);
    uint32_t sendLon_dec   = (uint32_t) *currentLon;
    uint32_t sendLon_float = (*currentLon - sendLon_dec) * (TEN_6);

    // with dbc: you can use scale of 0.000001
    // 123456789 ==> 123.456789
    transmit_gpsData->lat_dec    = sendLat_dec;
    transmit_gpsData->lat_float  = sendLat_float;
    transmit_gpsData->long_dec   = sendLon_dec;
    transmit_gpsData->long_float = sendLon_float;
    transmit_gpsData->chkPoint   = *currentChkPnt;
    transmit_gpsData->bIsFinal   = isFinal;

    uint64_t presentGps_data = *(uint64_t *) (transmit_gpsData);
    uint32_t msgID = GPS_DATA_ID;

    can_transmit(msgID, &presentGps_data, DATA_LEN_EIGHT);
}

void sendCompass_data(int8_t turn, uint8_t presentChkPnt,
                                        float_t nxtChkPntDist, float_t finalDestDist)
{
    /** Positive number indicates the car has to car turn right.
     *  Negative number indicates the car has to turn left.
     *  Else head straight.
     */

    transmit_compassData->checkpoint        =   presentChkPnt;
    transmit_compassData->turnDecision      =   turn;
    transmit_compassData->dist_nxtPnt       =   (uint16_t) nxtChkPntDist;
    transmit_compassData->dist_finalDest    =   (uint16_t) finalDestDist;

    uint64_t presentCompassDist_data = *(uint64_t *) (transmit_compassData);
    uint32_t msgID = COMPASS_DIST_ID;

    can_transmit(msgID, &presentCompassDist_data, DATA_LEN_SIX);
}

bool can_receive(uint16_t id, uint64_t *data)
{

    bool received = false;
    can_fullcan_msg_t temp;
    can_fullcan_msg_t *data_updated = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, id));

    received = CAN_fullcan_read_msg_copy(data_updated, &temp);

    if(received)
    {
        data = (uint64_t *)temp.data.qword;
    }

    return received;
}

bool can_addGPSData(uint64_t *data)
{
    bool added;
    receive_gpsData = (lat_long_info*) (data);
    added =  addChkPnts(receive_gpsData->lat_dec,receive_gpsData->long_dec,receive_gpsData->long_dec,
                                      receive_gpsData->long_float,receive_gpsData->chkPoint);

    return added;
}

void heartbeat()
{
    uint64_t data = 0;
    can_transmit(HEARTBEAT_ID, &data, DATA_LEN_ZERO);
}

void can_checkBusOff(uint32_t a)
{
    if(CAN_is_bus_off(can1))
    {
        CAN_reset_bus(can1);
    }
}
