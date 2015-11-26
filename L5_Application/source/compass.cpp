/*
 * compass.c
 *
 *  Created on: Oct 30, 2015
 *      Author: Tejeshwar
 */


#include <compass.hpp>
#include "scheduler_task.hpp"
#include "i2c2.hpp"
#include "i2c2_device.hpp"
#include "i2c_base.hpp"
#include "stdio.h"
#include "io.hpp"
#include "utilities.h"
#include <gps.hpp>
#include <hashDefine.hpp>
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "math.h"
#include "CompassGPS_calculation.hpp"


/**Few functions here are just to make sure the I2C is working is fine**/
I2C2 &compass = I2C2::getInstance();

uint8_t compassBearing_fullCircle()
{
    uint8_t data;
    data = compass.readReg(0xc0, 1);
    return data;
}

float_t compassBearing_inDeg()
{
    uint8_t shiftreg;
    uint8_t shiftreg1;
    float_t compassangle;
    shiftreg = compass.readReg(0xC0, 2);
    shiftreg1 = compass.readReg(0xC0, 3);

    compassangle = (shiftreg1 << 0) + (shiftreg << 8);

    return fmodf(((compassangle/10) + MAGENTIC_DECLINTAION ), 360);
}

#if 0
void compass_pitchAngle()
{
    float_t pitchangle, pitchangle1;
    pitchangle = compass.readReg(0xC0, 4);
    pitchangle1 = compass.readReg(0xC0, 26);
}

void compass_rollAngle()
{
    float_t rollangle1, rollangle2;
    rollangle1 = compass.readReg(0xC0, 5);
    rollangle2 = compass.readReg(0xC0, 27);
}

void temperature()
{
    signed int temperature_highbyte, temperature_lowbyte;
    signed int temperature;
    temperature_highbyte = compass.readReg(0xC0, 24);
    temperature_lowbyte = compass.readReg(0xC0, 25);
    temperature = (temperature_highbyte << 8) + (temperature_lowbyte >> 0);
}
#endif

uint8_t compass_calibrationMode(uint8_t compassMode)
{
    /**Here register address 0 indicates its a command register**/
     compass.writeReg(0xC0,0,0xF0);
     delay_ms(20);
     compass.writeReg(0xC0,0,0xF5);
     delay_ms(20);
     compass.writeReg(0xC0,0,0xF6);
     delay_ms(20);

    return MODE_THREE;
}

uint8_t compass_headingMode()
{
    compass.writeReg(0xC0,0,0xF8);
    uint8_t headingMode = 0;
    return headingMode;
}


