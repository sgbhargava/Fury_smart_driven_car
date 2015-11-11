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
actual_headingdir *structptr = new actual_headingdir;

uint8_t compassBearing_fullCircle()
{
//    LE.off(1);LE.off(1);
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
    return (compassangle/10);
}

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

uint8_t compass_calibrationMode(uint8_t compassMode)
{
    /**Here register address 0 indicates its a command register**/
     LE.on(1);LE.off(2);
     compass.writeReg(0xC0,0,0xF0);
     delay_ms(20);
     compass.writeReg(0xC0,0,0xF5);
     delay_ms(20);
     compass.writeReg(0xC0,0,0xF6);
     delay_ms(20);

/*
    LE.on(1);LE.off(2);
    if (SW.getSwitch(2))
    {
        To come out of calibration mode
        headingmode_compass();
        compassMode = 2;
        //LE.off(1);
    }
*/
    return MODE_THREE;
}

uint8_t compass_headingMode()
{
    compass.writeReg(0xC0,0,0xF8);
    uint8_t headingMode = 0;
//    LE.on(2);
    return headingMode;
}

#if 1
void compass_actualHeadingDir(double_t headingAngle)
{
    structptr->current_angle = compassBearing_inDeg();
    structptr->desired_angle = headingAngle;
}
#endif

