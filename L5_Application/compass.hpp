/*
 * compass.h
 *
 *  Created on: Oct 24, 2015
 *      Author: Tejeshwar
 */

#ifndef L5_APPLICATION_COMPASS_HPP_
#define L5_APPLICATION_COMPASS_HPP_

#include "scheduler_task.hpp"
#include "i2c2.hpp"
#include "i2c2_device.hpp"
#include "i2c_base.hpp"
#include "stdio.h"
#include "io.hpp"
#include "utilities.h"
#include "math.h"

/* Calculates heading direction for a full circle!!
 * Returns values between 0 to 255
*/
uint8_t compassBearing_fullCircle();

/* Calculates heading direction!!
 * Returns values between 0 to 360
*/
float_t compassBearing_inDeg();

/* Calculates pitch angle
 * */
void compass_pitchAngle();

/* Calculates roll angle*/
void compass_rollAngle();

/* Calculates temperature*/
void temperature();

/* Compass enters into calibration when this function is called.
 * We can calibrate the magnetometer, accelerometer and gyro*/
uint8_t compass_calibrationMode(uint8_t mode);

/* Compass enters into heading mode when this function is called.
 * This function is required to get the compass module out of the
 * calibration mode.
 * */
uint8_t compass_headingMode();


typedef struct {
        float_t current_angle;
        double_t desired_angle;
        //uint8_t destination_reached;
        //uint8_t is_valid;
}actual_headingdir;

/*Sends current angle and desired angle to master to make turn or not*/
void compass_actualHeadingDir(double_t headingAngle);

#endif /* L5_APPLICATION_COMPASS_HPP_ */
