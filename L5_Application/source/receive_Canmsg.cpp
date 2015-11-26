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
lat_long_info          *receive_gpsdata      = new lat_long_info;
lat_long_info          *transmit_gpsdata     = new lat_long_info;
compass_distance_info  *transmit_compassdata = new compass_distance_info;
can_fullcan_msg_t* ptr_toReadData;
can_std_id_t toget_Master_MsgID;
bool msgrec_master;

void can_transmit(uint32_t msgID, uint64_t *updateGps_data, uint8_t dataLength)
{
    can_msg_t gpsData;
    gpsData.msg_id                = msgID;
    gpsData.data.qword            = *updateGps_data;
    gpsData.frame_fields.data_len = dataLength;
    CAN_tx(can1, &gpsData, 10);
}

void sendGPS_data(uint8_t *currentChkPnt,double_t *currentLat, double_t *currentLon)
{
    uint32_t sendLat_dec   = (uint32_t) *currentLat;
    uint32_t sendLat_float = (*currentLat - sendLat_dec) * (TEN_6);
    uint32_t sendLon_dec   = (uint32_t) *currentLon;
    uint32_t sendLon_float = (*currentLon - sendLon_dec) * (TEN_6);

    // with dbc: you can use scale of 0.000001
    // 123456789 ==> 123.456789
    transmit_gpsdata->lat_dec    = sendLat_dec;
    transmit_gpsdata->lat_float  = sendLat_float;
    transmit_gpsdata->long_dec   = sendLon_dec;
    transmit_gpsdata->long_float = sendLon_float;
    transmit_gpsdata->chkPoint   = *currentChkPnt;

    uint64_t presentGps_data = *(uint64_t *) (transmit_gpsdata);
    uint32_t msgID = GPS_DATA_ID;

    can_transmit(msgID, &presentGps_data, DATA_LEN_EIGHT);
}

void sendCompass_data(float_t currentDir, double_t desiredDir, uint8_t presentChkPnt,
        float_t nxtChkPntDist, float_t finalDestDist)
{
    /** Positive number indicates the car has to car turn right.
     *  Negative number indicates the car has to turn left.
     *  Else head straight.
     */

    int8_t turn = ((desiredDir - currentDir)/30);

    transmit_compassdata->checkpoint        =   presentChkPnt;
    transmit_compassdata->turnDecision      =   turn;
    transmit_compassdata->dist_nxtpnt       =   (uint16_t) nxtChkPntDist;
    transmit_compassdata->dist_finaldest    =   (uint16_t) finalDestDist;

    uint64_t presentCompassDist_data = *(uint64_t *) (transmit_compassdata);
    uint32_t msgID = COMPASS_DIST_ID;

    can_transmit(msgID, &presentCompassDist_data, DATA_LEN_EIGHT);
}

void can_receive()
{
/*
    toget_Master_MsgID = CAN_gen_sid(can1,MASTER_GPSDATA_ID);
    CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1,MASTER_GPSDATA_ID));
    msgrec_master = CAN_fullcan_read_msg_copy(&ptr_toReadData,&data_master);

    if(msgrec_master == 1)
    {
        if(toget_Master_MsgID.id == MASTER_GPSDATA_ID)
        {
            receive_gpsdata = (lat_long_info*)  (ptr_toReadData.data.qword);
            addChkPnts(receive_gpsdata->lat_dec,receive_gpsdata->long_dec,receive_gpsdata->long_dec,
                                                            receive_gpsdata->long_float,receive_gpsdata->chkPoint);
        }
        else if(toget_Master_MsgID.id == MASTER_RESET_ID)
            sys_reboot();

        else
            LD.setNumber(17);
    }
    else
        LD.setNumber(15); // Indicates there is no new message in the CAN RAM
*/


    can_fullcan_msg_t* data_gpsUpdate = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, MASTER_GPSDATA_ID));
    can_fullcan_msg_t* data_sysReboot = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, MASTER_RESET_ID));


    if(CAN_fullcan_read_msg_copy(ptr_toReadData,data_gpsUpdate))
    {
        receive_gpsdata = (lat_long_info*) (ptr_toReadData->data.qword);
        addChkPnts(receive_gpsdata->lat_dec,receive_gpsdata->long_dec,receive_gpsdata->long_dec,
                                                                    receive_gpsdata->long_float,receive_gpsdata->chkPoint);
    }

    if(CAN_fullcan_read_msg_copy(ptr_toReadData,data_sysReboot))
    {
        sys_reboot();
    }


#if 0
    can_msg_t data;
    CAN_rx(can1, &data, 1);

    // START of checkpoints that resets all the checkpoint
    if(data.msg_id == MASTER_GPSDATA_ID)
    {
        receive_gpsdata = (lat_long_info*)  &(data.data.qword);
        addChkPnts(receive_gpsdata->lat_dec,receive_gpsdata->long_dec,receive_gpsdata->long_dec,
                                                        receive_gpsdata->long_float,receive_gpsdata->chkPoint);

    }
    else if(data.msg_id == MASTER_RESET_ID)
        sys_reboot();

    else
        LD.setNumber(17);
#endif
}

void heartbeat()
{
    uint64_t data = 0;
    can_transmit(HEARTBEAT_ID, &data, DATA_LEN_ZERO);
}
