#include "io.hpp"
#include "eint.h"
#include "gpio.hpp"
#include "lpc_pwm.hpp"
#include "motor.hpp"

#include <cstdio>

DirectionCtrl * DirectionCtrl::m_pInstance = NULL;
DirectionCtrl::DirectionCtrl(void) :
        directionPWM(PWM(PWM::pwm1, STANDARD_FREQ)),
        pin0_29(P0_29), pin0_30(P0_30)
{
    dirPWM = basePWM;
    directionPWM.set(dirPWM);
    pin0_29.setAsOutput();
    pin0_30.setAsOutput();
}
DirectionCtrl * DirectionCtrl::getInstance()
{
    if (m_pInstance == NULL)
        m_pInstance = new DirectionCtrl();
    return m_pInstance;
}
void DirectionCtrl::setDirection(int dir)
{
    LD.setNumber(dir);
    switch (dir){
        case dirFarRight:
            dirPWM = farRight;
            pin0_29.setHigh();
            pin0_30.setLow();
            break;
        case dirRight:
            dirPWM = right;
            pin0_29.setHigh();
            pin0_30.setLow();
            break;
        case dirCenter:
            dirPWM = basePWM;
            pin0_29.setLow();
            pin0_30.setLow();
            break;
        case dirLeft:
            dirPWM = left;
            pin0_29.setHigh();
            pin0_30.setLow();
            break;
        case dirFarLeft:
            dirPWM = farLeft;
            pin0_29.setHigh();
            pin0_30.setLow();
            break;

    }
    printf("PWM %f\n", dirPWM);
    directionPWM.set(dirPWM);

};

SpeedCtrl* SpeedCtrl::m_pInstance = NULL;
SpeedCtrl::SpeedCtrl():
        throttlePWM(PWM(PWM::pwm2, STANDARD_FREQ)),
        pin1_22(P1_22), pin1_23(P1_23)
{
    speedPWM = basePWM;
    throttlePWM.set(basePWM);
    //LD.setNumber(speedPWM);
    pin1_22.setAsOutput();
    pin1_23.setAsOutput();

}

SpeedCtrl* SpeedCtrl::getInstance(){
    if (m_pInstance == NULL)
        m_pInstance = new SpeedCtrl();
    return m_pInstance;
}

void SpeedCtrl::initESC()
{

    printf("INIT\n");
    throttlePWM.set(SpeedCtrl::backLimitPWM);
    vTaskDelay(1000);
    throttlePWM.set(SpeedCtrl::frontLimitPWM);
    vTaskDelay(1000);
    printf("INIT Done\n");
    throttlePWM.set(SpeedCtrl::basePWM);
    speedPWM = SpeedCtrl::basePWM;
    LE.on(1);
    //LD.setNumber((int)speedPWM);

}
bool SpeedCtrl::checkPWM(float pwm)
{
    printf("Check speed %f\n", pwm);
    //if ((pwm > backLimitPWM) && (pwm < frontLimitPWM))
    //{
        if ( basePWM - pwm > 0.5)
        {
            pin1_22.setHigh();
            pin1_23.setHigh();
        }
        else{
            pin1_22.setLow();
            pin1_23.setLow();
        }
        return true;
    //}
    //return false;
}

void SpeedCtrl::setSpeedPWM(float pwm)
{
    if (checkPWM(pwm)){
        printf("Set speed %f\n", pwm);
        if ((( speedPWM < SpeedCtrl::basePWM ) && (pwm > SpeedCtrl::basePWM)) ||
                (( speedPWM > SpeedCtrl::basePWM ) && (pwm < SpeedCtrl::basePWM)))
        { //Change forward to backward or backward to forward
            throttlePWM.set(SpeedCtrl::basePWM-0.3);
            vTaskDelay(1000);
            throttlePWM.set(SpeedCtrl::basePWM);
            vTaskDelay(1000);
        }
        throttlePWM.set(pwm);
        speedPWM = pwm;
    }
}

void SpeedCtrl::incrSpeedPWM()
{
    float pwm = speedPWM + PWMStep;
    if (checkPWM(pwm)){
        printf("Set speed %f\n", pwm);
        speedPWM = pwm;
        throttlePWM.set(pwm);
    }
    else
    {
        printf("This(PWM:%f) is out of limit\n", pwm);
    }
}

void SpeedCtrl::descrSpeedPWM()
{
    float pwm = speedPWM - PWMStep;
    if (checkPWM(pwm)){
        printf("Set speed %f\n", pwm);
        speedPWM = pwm;
        throttlePWM.set(pwm);
    }
    else
    {
        printf("This(PWM:%f) is out of limit\n", pwm);
    }
}
SpeedMonitor* SpeedMonitor::m_pInstance = NULL;
void speed_pulse_start(void)
{
    SpeedMonitor::getInstance()->calSpeed();
}

SpeedMonitor::SpeedMonitor()
{
    //Singleton
    int port2_5 = 5;
    eint3_enable_port2(port2_5, eint_rising_edge, speed_pulse_start);
}

SpeedMonitor* SpeedMonitor::getInstance(){
    if (m_pInstance == NULL)
        m_pInstance = new SpeedMonitor();
    return m_pInstance;
}

void SpeedMonitor::setRpm(int rpmVal)
{
    m_rpm = rpmVal;
    m_speed = DIAMETER *PI *((float)m_rpm* 60); //meter
}
void SpeedMonitor::getSpeed(float* rpm, float* speed)
{
    const uint64_t FIVE_SECOND = 5* 1000;
    uint64_t cur_time = 0;//sys_get_uptime_ms();
    if ((m_speed != 0.0 && m_rpm != 0) && (cur_time - m_last_time > FIVE_SECOND))
    {
        m_speed = 0;
        m_rpm = 0;
    }
    *rpm = m_rpm;
    *speed = m_speed;
}
int SpeedMonitor::getRpm()
{
    return m_rpm;
}

void SpeedMonitor::calSpeed(){
    uint64_t cur_time = sys_get_uptime_ms();
    uint64_t time_diff = cur_time - m_last_time;

    m_rpm = 1.0/ ( (float)time_diff * MS_TO_MINS);
    m_speed = DIAMETER *PI * (m_rpm/ 60); // m/hr
    //m_speed = m_speed_meter /1000 * KM_TO_MILES;
    m_last_time = cur_time;
}

