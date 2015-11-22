/*
 * motor.hpp
 *
 *  Created on: Nov 14, 2015
 *      Author: Bhargav
 */

#ifndef L5_APPLICATION_SOURCE_MOTOR_HPP_
#define L5_APPLICATION_SOURCE_MOTOR_HPP_
#include "CAN_base_class.hpp"


typedef struct{

    uint64_t stop;        ///< B0:0  Min: 0 Max: 1   Destination: MOTOR
    uint64_t forward;     ///< B1:1  Min: 0 Max: 1   Destination: MOTOR
    uint64_t usecustom;   ///< B2:2  Min: 0 Max: 1   Destination: MOTOR
    uint64_t custom;      ///< B4:3  Min: 0 Max: 1   Destination: MOTOR
    uint64_t incr;        ///< B5:5  Min: 0 Max: 1   Destination: MOTOR

}throttle_can_payload __attribute__((packed));

class motor_class:public CAN_base_class
{
public:
	uint16_t id_heart_beat = 0x100;
			uint16_t id_motor_status = 0x102;
			uint16_t id_motor_throttle = 0x22;
			uint16_t id_motor_steering = 0x21;
			uint32_t motor_speed = 0;
			uint32_t motor_rpm = 0;
			uint8_t motor_steering = 0;
			can_msg_t motor_throttle_can_mess;
			can_msg_t motor_steering_can_mess;
	static motor_class* getInstance();
	bool motor_class_init();
	bool get_motor_status();
	bool send_motor_throttle();
	bool send_motor_steering();
	motor_class();

private:
	static motor_class *single;

		throttle_can_payload motor_throttle_payload;
};

#endif /* L5_APPLICATION_SOURCE_MOTOR_HPP_ */
