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
    uint64_t backward:1;        // use the customized backward
    uint64_t incrBackward:1;    // if backward is true, and incrBackward is true, it means increasing the speed.
    uint64_t customBackward:2;  // if backward is true, this indicates the customized code
                                // if backward is true, and incrBackward is false, this is 0x3 means decreasing
    uint64_t forward:1;
    uint64_t incrForward:1;
    uint64_t customForward:2;
}throttle_can_payload __attribute__((packed));

class motor_class:public CAN_base_class
{
public:
	uint16_t id_heart_beat = 0x100;
	uint16_t id_motor_status = 0x102;
	uint16_t id_motor_throttle = 0x21;
	uint16_t id_motor_steering = 0x22;
	uint32_t motor_speed = 0;
	uint32_t motor_rpm = 0;
	can_msg_t motor_throttle_can_mess;
	can_msg_t motor_steering_can_mess;
	throttle_can_payload motor_throttle_payload;
	bool motor_class_init();
	bool get_motor_status();
	bool send_motor_throttle();
	bool send_motor_steering();


};

#endif /* L5_APPLICATION_SOURCE_MOTOR_HPP_ */
