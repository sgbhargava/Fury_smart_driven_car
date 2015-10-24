/*
 * sonic_sensor.hpp
 *
 *  Created on: Oct 15, 2015
 *      Author: Christopher
 */

#ifndef L5_APPLICATION_SONIC_SENSOR_HPP_
#define L5_APPLICATION_SONIC_SENSOR_HPP_
#include "eint.h"
#include "gpio.hpp"
#include "utilities.h"
#include "stdio.h"
#include "tasks.hpp"
#include "SensorDataType.h"
#include "lpc_timers.h"


uint32_t timerValue1, timerValue2, timerValue3;

void StartTimeSensor1_isr(void)
{
    lpc_timer_set_value(lpc_timer0, 0);
}

void EndTimeSensor1_isr(void)
{
    timerValue1 = (lpc_timer_get_value(lpc_timer0)/147);
}

void StartTimeSensor2_isr(void)
{
    lpc_timer_set_value(lpc_timer2, 0);
}

void EndTimeSensor2_isr(void)
{
    timerValue2 = (lpc_timer_get_value(lpc_timer2)/147);
}

void StartTimeSensor3_isr(void)
{
    lpc_timer_set_value(lpc_timer3, 0);
}

void EndTimeSensor3_isr(void)
{
    timerValue3 = (lpc_timer_get_value(lpc_timer3)/147);
}

class SonicSensorTask : public scheduler_task
{
    public:
        SonicSensorTask(uint8_t priority) :
            scheduler_task("SonicSensor", 1024, priority), pwmSensor1(P2_0), pwmSensor2(P2_2), pwmSensor3(P2_4),
            SensorTrig1(P2_1), SensorTrig2(P2_3), SensorTrig3(P2_5), sensor_data_q(NULL)
        {
            sensor_data.SonicSensor1 = 0;
            sensor_data.SonicSensor2 = 0;
            sensor_data.SonicSensor3 = 0;
        }

        bool init(void)
        {
            lpc_timer_enable(lpc_timer0, 1);
            lpc_timer_enable(lpc_timer2, 1);
            lpc_timer_enable(lpc_timer3, 1);

            pwmSensor1.setAsInput();
            pwmSensor2.setAsInput();
            pwmSensor3.setAsInput();
            SensorTrig1.setAsOutput();
            SensorTrig2.setAsOutput();
            SensorTrig3.setAsOutput();

            sensor_data_q = xQueueCreate(2, sizeof(sensor_data));
            addSharedObject("sonic_queue", sensor_data_q);

            //Interrupt for Sensor 1
            eint3_enable_port2(0, eint_rising_edge, StartTimeSensor1_isr);
            eint3_enable_port2(0, eint_falling_edge, EndTimeSensor1_isr);
            //Interrupt for Sensor 2
            eint3_enable_port2(2, eint_rising_edge, StartTimeSensor2_isr);
            eint3_enable_port2(2, eint_falling_edge, EndTimeSensor2_isr);
            //Interrupt for Sensor 3
            eint3_enable_port2(4, eint_rising_edge, StartTimeSensor3_isr);
            eint3_enable_port2(4, eint_falling_edge, EndTimeSensor3_isr);

            return true;
        }

        bool run(void *p)
        {
            //Sensor 1
            SensorTrig1.setHigh();
            delay_us(TriggerDelay_us);
            SensorTrig1.setLow();
            sensor_data.SonicSensor1 = timerValue1;
            delay_ms(DelayForSensor_ms);

            //Sensor 2
            SensorTrig2.setHigh();
            delay_us(TriggerDelay_us);
            SensorTrig2.setLow();
            sensor_data.SonicSensor2 = timerValue2;
            delay_ms(DelayForSensor_ms);

            //Sensor 3
            SensorTrig3.setHigh();
            delay_us(TriggerDelay_us);
            SensorTrig3.setLow();
            sensor_data.SonicSensor3 = timerValue3;
            delay_ms(DelayForSensor_ms);

            xQueueSend(sensor_data_q, &sensor_data, 0);

            return true;
        }
    private:
        SonicSensors_t sensor_data;
        GPIO pwmSensor1, pwmSensor2, pwmSensor3;
        GPIO SensorTrig1, SensorTrig2, SensorTrig3;
        QueueHandle_t sensor_data_q;
        static const int TriggerDelay_us = 25;
        static const int DelayForSensor_ms = 70;
};

#endif /* L5_APPLICATION_SONIC_SENSOR_HPP_ */
