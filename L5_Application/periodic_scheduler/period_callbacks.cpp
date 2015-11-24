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

#include <stdlib.h>
#include <cstdio>
#include "sys_config.h"
#include "io.hpp"
#include "periodic_callback.h"
#include "motor.hpp"
#include "semphr.h"
#include "tasks.hpp"
#include "shared_handles.h"
#include "can.h"
#include "can_msg_process.hpp"
#include "tlm/c_tlm_comp.h"
#include "tlm/c_tlm_var.h"

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (1024 * 4*2);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    can_msg_process_init();
    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    SpeedCtrl *speed = SpeedCtrl::getInstance();
    DirectionCtrl* dir = DirectionCtrl::getInstance();
    SpeedMonitor* spdsensor = SpeedMonitor::getInstance();
    TLM_REG_VAR(tlm_component_get_by_name("disk"), speed->speedPWM, tlm_float);
    TLM_REG_VAR(tlm_component_get_by_name("disk"), dir->dirPWM, tlm_float);
    TLM_REG_VAR(tlm_component_get_by_name("disk"), spdsensor->m_speed, tlm_float);
    TLM_REG_VAR(tlm_component_get_by_name("disk"), spdsensor->m_rpm, tlm_int);

    return true; // Must return true upon success
}

void period_1Hz(void)
{
    sendSpeed();
    sendHeartBeat();
    readCANMsgs();
    SpeedCtrl::getInstance()->selfTuningSpeed();

}

void period_10Hz(void)
{
}

void period_100Hz(void)
{
    recvAndAnalysisCanMsg();
}

void period_1000Hz(void)
{
}
