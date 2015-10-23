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


uint32_t rise_time, diff_time;

void StartTime_isr(void)
{
    rise_time = sys_get_uptime_us();
}

void EndTime_isr(void)
{
    diff_time = (sys_get_uptime_us() - rise_time)/147;
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
            pwmSensor1.setAsInput();
            pwmSensor2.setAsInput();
            pwmSensor3.setAsInput();
            SensorTrig1.setAsOutput();
            SensorTrig2.setAsOutput();
            SensorTrig3.setAsOutput();

            sensor_data_q = xQueueCreate(2, sizeof(sensor_data));
            addSharedObject("sonic_queue", sensor_data_q);

            eint3_enable_port2(0, eint_rising_edge, StartTime_isr);
            eint3_enable_port2(0, eint_falling_edge, EndTime_isr);
            eint3_enable_port2(2, eint_rising_edge, StartTime_isr);
            eint3_enable_port2(2, eint_falling_edge, EndTime_isr);
            eint3_enable_port2(4, eint_rising_edge, StartTime_isr);
            eint3_enable_port2(4, eint_falling_edge, EndTime_isr);

            return true;
        }

        bool run(void *p)
        {
            //Sensor 1
            SensorTrig1.setHigh();
            delay_us(TriggerDelay_us);
            SensorTrig1.setLow();
            sensor_data.SonicSensor1 = diff_time;
            delay_ms(DelayForSensor_ms);

            //Sensor 2
            SensorTrig2.setHigh();
            delay_us(TriggerDelay_us);
            SensorTrig2.setLow();
            sensor_data.SonicSensor2 = diff_time;
            delay_ms(DelayForSensor_ms);

            //Sensor 3
            SensorTrig3.setHigh();
            delay_us(TriggerDelay_us);
            SensorTrig3.setLow();
            sensor_data.SonicSensor3 = diff_time;
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
