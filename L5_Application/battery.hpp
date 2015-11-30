/*
 * battery.hpp
 *
 *  Created on: Nov 21, 2015
 *      Author: Christopher
 */
#include "adc0.h"
#include "LPC17xx.h"
#include "eint.h"
#include "tasks.hpp"
#include "SensorDataType.h"
#include "printf_lib.h"

#ifndef L5_APPLICATION_BATTERY_HPP_
#define L5_APPLICATION_BATTERY_HPP_

uint8_t BoardBatteryFlag = 0;

//If voltage drops below 5% of 5V flag bit will be set by regulator
void BoardBattery_isr(void)
{
    BoardBatteryFlag = 0xFF;
}

class BatteryTask : public scheduler_task
{
    public:
        BatteryTask(uint8_t priority) :
            scheduler_task("Battery", 1024, priority), battery_data_q(NULL)
        {
            /* Nothing to init */
        }

        bool init(void)
        {
            battery_data_q = xQueueCreate(1, sizeof(BattSen));
            addSharedObject("battery_queue", battery_data_q);

            //Interrupt for Sensor 1
            eint3_enable_port2(6, eint_rising_edge, BoardBattery_isr);

            LPC_PINCON->PINSEL3 |= (3<<28); //PIN 1.30
            LPC_PINCON->PINSEL3 |= (3<<30); //PIN 1.31
            adc0_init();
            return true;
        }

        bool run(void *p)
        {
            BattSen.MotorBattery = ((adc0_get_reading(4)*7.4 * 10) / 4096);
            BattSen.BoardBattery = ((adc0_get_reading(5)* 5 * 10) / 4096);
            BattSen.BatteryFlag = BoardBatteryFlag;
            xQueueSend(battery_data_q, &BattSen, 0);
            return true;
        }

    private:
        QueueHandle_t battery_data_q;
        BatterySensors_t BattSen;
};

#endif /* L5_APPLICATION_BATTERY_HPP_ */
