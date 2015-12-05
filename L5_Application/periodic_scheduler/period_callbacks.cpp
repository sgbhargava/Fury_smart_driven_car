/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */
#define sonar_hard_threshold 30
#define lidar_hard_threshold 30
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "stdio.h"
#include "utilities.h"
#include "file_logger.h"
#include "queue.h"
#include "can.h"
#include "printf_lib.h"
#include "sensors.hpp"
#include "motor.hpp"
#include "IO_bridge.hpp"
#include "geo_controller.hpp"
#include "tlm/c_tlm_comp.h"
#include "tlm/c_tlm_var.h"
#include "can_gpsCompass.hpp"
#include "master_class.hpp"

CAN_base_class my_can;
master_class *master;
sensor_class *sensor;
motor_class *motor;
geo_controller_class *geo_controller;
IO_base_class *IO_controller;
checkPointData_t checkPoints;
can_msg_t motor_throttle;
can_msg_t motor_steer;
can_msg_t heart_beat;

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (2*512 * 4);
#if 1

void Obstruction_avoidance_algorithm(void) {
printf("geo %d\n", geo_controller->turnDecision);
if(geo_controller->turnDecision < 0)
{
	motor->motor_steering = 4;
	//motor->send_motor_steering();
}
else if(geo_controller->turnDecision > 0)
{
	motor->motor_steering = 1;
	//motor->send_motor_steering();
}
else
{
	motor->motor_steering = 0;
	//motor->send_motor_steering();*/
}
	motor->get_motor_status();
	sensor->lidar_threshold = 70 + motor->motor_rpm / 2;
	sensor->sensor_threshold = 50 + motor->motor_rpm / 3;

	if ((sensor->lidar_threshold > 120) | (sensor->sensor_threshold > 80)) {
		sensor->lidar_threshold = 120;
		sensor->sensor_threshold = 80;
	}

	if (sensor->lidar < sensor->lidar_threshold) {

		if ((sensor->left < sensor->sensor_threshold)
				&& (sensor->right < sensor->sensor_threshold)) {
			//printf("stop\n");
			motor->motor_steering = 0;
			motor->stop();
		}

		else if (sensor->right < sensor->sensor_threshold) {
			motor->motor_steering = 4; //left
			motor->custom_1();
		} else if (sensor->left < sensor->sensor_threshold) {
			motor->motor_steering = 1; //right
			motor->custom_1();
		}

	} else {
		if (sensor->right < sensor->sensor_threshold) {
			motor->motor_steering = 3;
		} else if (sensor->left < sensor->sensor_threshold) {
			motor->motor_steering = 2;
		} else {
			//motor->motor_steering = 0;
		}
		motor->custom_1();
	}

	/*printf("motor steer %d\n", motor->motor_steering);

	printf("rpm is %d\n", motor->motor_rpm);*/
	if (!motor->send_motor_steering()) {
		//printf("ERROR failed to send 21\n");
	}

	if (!motor->send_motor_throttle())
	{
		//printf("ERROR failed to send 22\n");
	}
}

#endif

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void) {
	motor = motor_class::getInstance();
	sensor = sensor_class::getInstance();
	geo_controller = geo_controller_class::getInstance();
	IO_controller = IO_base_class::get_Instance();
	master = master_class::getInstance();
	my_can.CAN_base_class_init();

	return true; // Must return true upon success
}

//Register any telemetry variables
bool period_reg_tlm(void) {

	// Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
	TLM_REG_VAR(tlm_component_get_by_name("disk"), sensor->left, tlm_int);
	TLM_REG_VAR(tlm_component_get_by_name("disk"), sensor->right, tlm_int);
	TLM_REG_VAR(tlm_component_get_by_name("disk"), sensor->lidar, tlm_int);
	TLM_REG_VAR(tlm_component_get_by_name("disk"), sensor->back, tlm_int);
	TLM_REG_VAR(tlm_component_get_by_name("disk"), motor->motor_steering,tlm_int);
	//TLM_REG_VAR(tlm_component_get_by_name("disk"), localCompassData.turnDecision , tlm_int);
	// TLM_REG_VAR(tlm_component_get_by_name("disk"), sensor.sensor_threshold,tlm_int);
	// TLM_REG_VAR(tlm_component_get_by_name("disk"), sensor.lidar_threshold,tlm_int);
	return true; // Must return true upon success
}

void period_1Hz(void) {


	//geo_controller->reset();

	static uint8_t timeout =0;
	timeout++;
	timeout %= 2;
	if(timeout == 1)
	{

		if(!geo_controller->get_heartbeat())
			geo_controller->reset();

		if(!IO_controller->get_heartbeat())
			IO_controller->reset();

		if(!sensor->get_heartbeat())
			sensor->reset();

		if(!motor->get_heartbeat())
			motor->reset();
	}

	sensor->get_battery();
	printf("bat %d\n", sensor->battery);
}

void period_10Hz(void) {

	if (!sensor->get_sensor_reading())
		u0_dbg_printf("ERROR failed to get sensor data\n");

	if(!geo_controller->get_compass_data())
				u0_dbg_printf("ERROR failed to get compass data\n");
	Obstruction_avoidance_algorithm();


}

void period_100Hz(void) {

}

void period_1000Hz(void) {
	//LE.toggle(4);
}
