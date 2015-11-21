/*
 * battery.hpp
 *
 *  Created on: Nov 21, 2015
 *      Author: Christopher
 */
#include "adc0.h"
#include "LPC17xx.h"

#ifndef L5_APPLICATION_BATTERY_HPP_
#define L5_APPLICATION_BATTERY_HPP_

void initBattery()
{
    LPC_PINCON->PINSEL3 |= (3<<28); //PIN 1.30
    adc0_init();
}

uint16_t GetBatteryValueADC()
{
    return ((adc0_get_reading(4)/4096)*3.3);
}



#endif /* L5_APPLICATION_BATTERY_HPP_ */
