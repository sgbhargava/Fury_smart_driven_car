/*
 * receive_Canmsg.cpp
 *
 *  Created on: Nov 10, 2015
 *      Author: Tejeshwar, Abhishek
 */

#include <receive_Canmsg.hpp>
#include "can.h"
#include "stdio.h"

lat_long_info *temp = new lat_long_info;


void getdata()
{
    can_msg_t *data = NULL;
    data->msg_id = 0;
    data->data.qword = 0;
    data->data.bytes[8] = 0;
    CAN_rx(can1,data,100);


/* Wrong Idea of doing
 *  data = (uint32_t) (data->data.qword>>8) && (0x0000000000000ff);
    temp->lat_dec = data;
    data = (uint32_t) (data->data.qword>>20) && (0x0000000000fffff);
    temp->lat_float = data;
    data = (uint32_t) (data->data.qword>>8) && (0x0000000000ffff);
    temp->long_dec = data;
    data = (uint32_t) (data->data.qword>>20) && (0x0000fffff);
    temp->long_float =data;*/


    temp->lat_dec =    (uint32_t)  (data->data.qword>>8)    & (0x000000000000ff);
    temp->lat_float =  (uint32_t)  (((data->data.qword>>28) & (0x00000000fffff00))>>8);
    temp->long_dec =   (uint32_t)  (((data->data.qword>>36) & (0x0000000ff0000000))>>28);
    temp->long_float = (uint32_t)  (((data->data.qword>>56) & (0x00fffff000000000))>>36);
    temp->chkPoint =   (uint32_t)  (((data->data.qword)     & (0xff00000000000000))>>56);
}
