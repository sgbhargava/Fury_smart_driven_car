/*
 * receive_Canmsg.hpp
 *
 *  Created on: Nov 10, 2015
 *      Author: Tejeshwar
 */

#ifndef RECEIVE_CANMSG_HPP_
#define RECEIVE_CANMSG_HPP_
#include "can.h"


typedef struct{
        uint64_t lat_dec     :8;
        uint64_t lat_float   :20;
        uint64_t long_dec    :8;
        uint64_t long_float  :20;
        uint64_t chkPoint    :8;
}__attribute__((packed)) lat_long_info;


void getdata();

#endif /* RECEIVE_CANMSG_HPP_ */
