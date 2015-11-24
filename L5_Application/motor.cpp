#include "io.hpp"
#include "eint.h"
#include "gpio.hpp"
#include "lpc_pwm.hpp"
#include "motor.hpp"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

bool floatIsSame(float a, float b)
{
    return fabs(a - b) < 0.001;
}

DirectionCtrl * DirectionCtrl::m_pInstance = NULL;
DirectionCtrl::DirectionCtrl(void) :
        directionPWM(PWM(PWM::pwm1, STANDARD_FREQ)), pin0_29(P0_29), pin0_30(
                P0_30)
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
    switch (dir)
    {
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
    //printf("PWM %f\n", dirPWM);
    directionPWM.set(dirPWM);

};

SpeedCtrl* SpeedCtrl::m_pInstance = NULL;
SpeedCtrl::SpeedCtrl() :
        throttlePWM(PWM(PWM::pwm2, STANDARD_FREQ)),
        pin1_22(P1_22), pin1_23(P1_23)
{
    speedPWM = basePWM;
    throttlePWM.set(basePWM);
    //LD.setNumber(speedPWM);
    desiredDirection = goStop;
    desiredCustom = 0;
    desiredRpm = 0;
    pin1_22.setAsOutput();
    pin1_23.setAsOutput();
}

SpeedCtrl* SpeedCtrl::getInstance()
{
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
    //printf("Check speed %f\n", pwm);
    if ((pwm > backLimitPWM) && (pwm < frontLimitPWM))
    {
        if (basePWM - pwm > 0.5)
        {
            pin1_22.setHigh();
            pin1_23.setHigh();
        }
        else
        {
            pin1_22.setLow();
            pin1_23.setLow();
        }
        return true;
    }
    return false;
}
void SpeedCtrl::selfTuningSpeed()
{
    if (startSelfTuning!=0)
    {
        if (startSelfTuning == 1)
            startSelfTuning++;
        else
        {
            startSelfTuning = 0;
            selfTuning = true;
        }
    }
    if (selfTuning)
    {
        int readbackRpm = 0;
        float readbackSpeed = 0;

        SpeedMonitor::getInstance()->getSpeed(&readbackRpm, &readbackSpeed);
        if (desiredRpm == 0 && (int)readbackRpm != 0)
        {
            setSpeedPWM(basePWM, selfTuning);
            desiredDirection = goStop;
        }
        else
        {
            int deltaRpm = desiredRpm - readbackRpm;
            int updateTime = 0;
            int updateStep = 0;
            if ( abs(deltaRpm) > RPM_THERHOLD_3 )
            {
                updateTime = 1;
                updateStep = 3;
            }
            else if ( abs(deltaRpm) > RPM_THERHOLD_2 )
            {
                updateTime = 4;
                updateStep = 2;
            }
            else if (abs(deltaRpm) > RPM_THERHOLD_1)
            {
                updateTime = 6;
                updateStep = 1;
            }

            if (updateTime != 0 && updateStep != 0)
            {
                if (selfTuningTimer > updateTime)
                {
                    if (deltaRpm > 0)
                    {
                        incrSpeedPWM(updateStep);
                    }
                    else
                    {
                        descrSpeedPWM(updateStep);
                    }
                    selfTuningTimer = 0;
                }
                else
                    selfTuningTimer ++;
            }
            else
                selfTuningTimer = 0;
        }
    }
}

void SpeedCtrl::setSpeedPWM(float pwm, bool selfTuning_f)
{
    if (checkPWM(pwm))
    {
        if (((speedPWM < basePWM) && (pwm > basePWM))
                || ((speedPWM > basePWM) && (pwm < basePWM)))
        { //Change forward to backward or backward to forward
            throttlePWM.set(basePWM - 0.3);
            vTaskDelay(1000);
            throttlePWM.set(basePWM);
            vTaskDelay(1000);
        }
        throttlePWM.set(pwm);
        speedPWM = pwm;
        if (! selfTuning_f)
        {
            selfTuning = selfTuning_f;
            desiredCustom = 0;
            desiredDirection = goStop;
        }

    }
}
void SpeedCtrl::setStop()
{
    desiredDirection = goStop;
    speedPWM = basePWM;
    throttlePWM.set(basePWM);
    selfTuning = false;
}
void SpeedCtrl::setSpeedCustom(bool forward, uint8_t speedSetting)
{
    if (forward)
    {
        if (speedSetting > 0 && speedSetting < 4)
        {
            if (desiredDirection != goForward || desiredCustom != speedSetting )
            {

                desiredCustom = speedSetting;
                desiredDirection = goForward;
                desiredRpm = rpm_forward_custom[desiredCustom - 1];
                setSpeedPWM(pwm_forward_custom[desiredCustom - 1], true);
                startSelfTuning = 1;
            }
        }
    }
    else
    {
        if (speedSetting > 0 && speedSetting < 4)
        {
            if (desiredDirection != goBackward || desiredCustom != speedSetting)
            {
                desiredCustom = speedSetting;
                desiredDirection = goBackward;
                setSpeedPWM(pwm_backward_custom[desiredCustom - 1], true);
                desiredRpm = rpm_backward_custom[desiredCustom- 1];
                startSelfTuning = 1;
            }
        }
    }

}
void SpeedCtrl::incrSpeedPWM(int step)
{
    float pwm = speedPWM + ((float)PWMStep* (float)step);
    if (checkPWM(pwm))
    {
        if (desiredCustom != 0)
        {
            if (desiredDirection == goForward)
                pwm_forward_custom[desiredCustom - 1] = pwm;
            else
                pwm_backward_custom[desiredCustom - 1] = pwm;
        }
        //printf("Set speed %f\n", pwm);
        speedPWM = pwm;
        throttlePWM.set(pwm);
    }
#if 0
    else
    {
        printf("This(PWM:%f) is out of limit\n", pwm);
    }
#endif
}

void SpeedCtrl::descrSpeedPWM(int step)
{
    float pwm = speedPWM - ((float)PWMStep* (float)step);
    if (checkPWM(pwm))
    {
        //printf("Set speed %f\n", pwm);
        if (desiredCustom != 0)
        {
            if (desiredDirection == goForward)
                pwm_forward_custom[desiredCustom - 1] = pwm;
            else
                pwm_backward_custom[desiredCustom - 1] = pwm;
        }
        speedPWM = pwm;
        throttlePWM.set(pwm);
    }
#if 0
    else
    {
        printf("This(PWM:%f) is out of limit\n", pwm);
    }
#endif
}

int SpeedCtrl::getGoDesiredDirection()
{
    return desiredDirection;
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

    m_rpmRecorded = 0;
}
SpeedMonitor* SpeedMonitor::getInstance()
{
    if (m_pInstance == NULL)
        m_pInstance = new SpeedMonitor();
    return m_pInstance;
}

void SpeedMonitor::getSpeed(int* rpm, float* speed)
{
    *rpm = m_rpm;
    *speed = m_speed;
}
void SpeedMonitor::getSpeedEachSecond(int* rpm, float* speed)
{
    const uint64_t ONE_SECOND = 1000;
    uint64_t cur_time = sys_get_uptime_ms();
    if (((!floatIsSame(m_speed, 0)) || (m_rpm != 0))
            && (cur_time - m_last_time > ONE_SECOND))
    {
        m_speed = 0;
        m_rpm = 0;
    }
    else
    {
        int rpmCal = 0;
        int rpmCur = 0;
        int rpmTempArray[100];
        int rpmRecorded = (m_rpmRecorded == 0)? 0: m_rpmRecorded+1;
        int totalRecorded = rpmRecorded;
        memcpy(rpmTempArray, m_rpmArray, 100*sizeof(int));
        m_rpmRecorded = 0;

        for (int i = 0; i < rpmRecorded; i++)
        {
            if (i == 0 || (abs (rpmCur - rpmTempArray[i]) < RPM_THERHOLD_1))
            {
                rpmCur = rpmTempArray[i];
                rpmCal += rpmTempArray[i];
            }
            else
            {
                if (i +1 < rpmRecorded)
                {
                    if (abs(rpmTempArray[i] -rpmTempArray[i+1]) < RPM_THERHOLD_1)
                    {
                        rpmCur = rpmTempArray[i];
                        rpmCal += rpmTempArray[i];
                    }
                    else
                        totalRecorded --;
                }
                else
                    totalRecorded --;
            }

        }
        if (totalRecorded <= 0 || rpmCal == 0)
            m_rpm = 0;
        else
        {
            m_rpm = *rpm = rpmCal/totalRecorded;
        }

    }
    *rpm = m_rpm;
    *speed = m_speed;
}
int SpeedMonitor::getRpm()
{
    return m_rpm;
}

void SpeedMonitor::calSpeed()
{
    uint64_t cur_time = sys_get_uptime_ms();
    uint64_t time_diff = cur_time - m_last_time;


    int rpm = 1.0 / ((float) time_diff * MS_TO_MINS);
    //m_speed = DIAMETER * PI * (m_rpm / 60); // m/hr
    //m_speed = m_speed_meter /1000 * KM_TO_MILES;
    m_last_time = cur_time;
    m_rpmArray[m_rpmRecorded++] = rpm;


}
