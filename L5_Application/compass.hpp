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

void compassbearing_reading();
void compassbearingreading_highlowbytes();
void pitchangle();
void rollangle();
void temperature();
int calibrate_compass(int mode);
int headingmode_compass();
float headingdir(float latitude,float longitude);

#endif /* L5_APPLICATION_COMPASS_HPP_ */
