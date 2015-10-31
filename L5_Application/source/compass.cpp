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
#include "tasks.hpp"
#include "examples/examples.hpp"

/**Few functions here are just to make sure the I2C is working is fine**/
I2C2 &compass = I2C2::getInstance();

void compassbearing_reading()
{
    /**Calculates the heading
     * Important task**/
    uint8_t data;
    data = compass.readReg(0xc0, 1);
    printf("compass bearing:%d\n", data);
}

void compassbearingreading_highlowbytes()
{
    uint8_t shiftreg;
    uint8_t shiftreg1;
    float compassangle;
    shiftreg = compass.readReg(0xC0, 2);
    shiftreg1 = compass.readReg(0xC0, 3);

    compassangle = (shiftreg1 << 0) + (shiftreg << 8);
    printf("compassreadings_inangles:%f\n", (compassangle / 10));
}

void pitchangle()
{
    float pitchangle, pitchangle1;
    pitchangle = compass.readReg(0xC0, 4);
    pitchangle1 = compass.readReg(0xC0, 26);

    printf("pitchangle:%f\n", pitchangle);
    printf("pitchangle_withoutfilter:%f\n", pitchangle1);
}

void rollangle()
{
    float rollangle1, rollangle2;
    rollangle1 = compass.readReg(0xC0, 5);
    rollangle2 = compass.readReg(0xC0, 27);

    printf("rollangle:%f\n", rollangle1);
    printf("rollangle_withoutfilter:%f\n", rollangle2);
}

void temperature()
{
    signed int temperature_highbyte, temperature_lowbyte;
    signed int temperature;
    temperature_highbyte = compass.readReg(0xC0, 24);
    temperature_lowbyte = compass.readReg(0xC0, 25);
    temperature = (temperature_highbyte << 8) + (temperature_lowbyte >> 0);
    printf("temperature:%d\n", temperature);
}

int calibrate_compass(int mode)
{
    /**Here register address 0 indicates its a command register**/
    /*for(i=0;i<10000;i++)
     {
         compass.writeReg(0xC0,0,0xF0);
         delay_ms(20);
         compass.writeReg(0xC0,0,0xF5);
         delay_ms(20);
         compass.writeReg(0xC0,0,0xF6);
         delay_ms(20);
     }*/

    LE.on(1);LE.off(2);
    if (SW.getSwitch(2))
    {
        /*To come out of calibration mode*/
        //headingmode_compass();
        mode = 2;
        LE.off(1);
    }
    return mode;
}

int headingmode_compass()
{
/*0xF8 makes compass come out of calibration mode*/
//compass.writeReg(0xC0,0,0xF8);

    int mode = 0;
    LE.on(2);
    return mode;
}

void headingdata(float latitude, float longitude)
{
    /**Function to calculate the current direction!!
     * This function should take care of the direction of the car when it is heading towards destination
     * or intermediate check points.
     * Implementing the heading formula to compute traveling direction**/


}

