/*
 * compass.h
 *
 *  Created on: Oct 24, 2015
 *      Author: Tejeshwar
 */

#ifndef L5_APPLICATION_COMPASS_H_
#define L5_APPLICATION_COMPASS_H_

#include "scheduler_task.hpp"
#include "i2c2.hpp"
#include "i2c2_device.hpp"
#include "i2c_base.hpp"
#include "stdio.h"

I2C2 &compass = I2C2::getInstance();

void compassbearing_reading()
{
    uint8_t data;
    data = compass.readReg(0xc0,1);
    printf("compass bearing:%d\n",data);
}

void compassbearingreading_highlowbytes()
{
    uint8_t shiftreg;
    uint8_t shiftreg1;
    float compassangle;
    shiftreg = compass.readReg(0xC0,2);
    shiftreg1 = compass.readReg(0xC0,3);

    compassangle = (shiftreg1<<0) + (shiftreg<<8);
    //compassangle = shiftreg<<8;
    printf("compassbearingreading_highlowbytes:%f\n",(compassangle/10));
}


#endif /* L5_APPLICATION_COMPASS_H_ */
