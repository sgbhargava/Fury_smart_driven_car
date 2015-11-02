/*
 * SensorDirection.hpp
 *
 *  Created on: Oct 31, 2015
 *      Author: Christopher
 */

#ifndef L5_APPLICATION_SENSORDIRECTION_HPP_
#define L5_APPLICATION_SENSORDIRECTION_HPP_
#include "SensorDataType.h"
#include "io.hpp"

//SonicSensor 1 -> left
//SonicSensor 2 -> right
//Lidar Sensor -> middle

uint8_t RCdirection(SonicSensors_t SonicData, uint16_t LidarData)
{
    enum direction {straight, farRight, right, left, FarLeft};
    int correctDirection = straight;
    if(LidarData < 75)
    {
        if(SonicData.SonicSensor1 < SonicData.SonicSensor2)
        {
            correctDirection = right;
//            LE.off(1);
//            LE.off(2);
//            LE.on(3);
        }
        else
        {
            correctDirection = left;
//            LE.on(1);
//            LE.off(2);
//            LE.off(3);
        }
    }
    else if(SonicData.SonicSensor1 < 75)
    {
        if(LidarData < SonicData.SonicSensor2)
        {
            correctDirection = right;
//            LE.on(3);
//            LE.off(1);
//            LE.off(2);
        }
        else
        {
            correctDirection = straight;
//            LE.on(2);
//            LE.off(1);
//            LE.off(3);
        }
    }
    else if(SonicData.SonicSensor2 < 75)
    {
        if(LidarData < SonicData.SonicSensor1)
        {
            correctDirection = left;
//            LE.on(1);
//            LE.off(2);
//            LE.off(3);
        }
        else
        {
            correctDirection = straight;
//            LE.on(2);
//            LE.off(1);
//            LE.off(3);
        }
    }
    else
    {
        correctDirection = straight;
//        LE.on(2);
//        LE.off(1);
//        LE.off(3);
    }
    return correctDirection;
}

#endif /* L5_APPLICATION_SENSORDIRECTION_HPP_ */
