#include "io.hpp"
#include "eint.h"
#include "gpio.hpp"
#include "lpc_pwm.hpp"
#include "motor.hpp"
#include "scheduler_task.hpp"
#include "shared_handles.h"

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
#if 0
void SpeedCtrl::selfTuningSpeed()
{
    if (startSelfTuning!=0)
    {
        if (startSelfTuning == 1)
            startSelfTuning ++;
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
        bool shouldDescr = false;

        SpeedMonitor::getInstance()->getSpeed(&readbackRpm, &readbackSpeed);
        if (desiredRpm == 0 && (int)readbackRpm != 0)
        {
            setSpeedPWM(basePWM, selfTuning);
            lastSelfTuning = 0;
        }
        else
        {
            int deltaRpm = desiredRpm - readbackRpm;
            int updateTime = 0;
            int updateStep = 0;

            if ( abs(deltaRpm) == desiredRpm )
            {
                updateTime = 2;
                updateStep = 4;

                if (lastSelfTuning != 4) selfTuningTimer = updateTime+1;
                lastSelfTuning = 4;

            }
            if ( abs(deltaRpm) > RPM_THERHOLD_3 )
            {
#ifdef incline_test
                if (AS.getY() < -50) //incline
                {
                    updateTime = 1;
                    updateStep = 4;
                    incline = true;
                    decline = false;
                }
                else if (AS.getY() > 50) //decline
                {
                    updateTime = 1;
                    updateStep = 8;
                    shouldDescr = true;
                    incline = false;
                    decline = true;
                }
                else
                {
                    if (incline)
                    {
                        updateTime = 1;
                        updateStep = 4;
                        shouldDescr = true;
                        incline = false;
                        decline = false;
                    }
                    else if (decline)
                    {
                        updateTime = 1;
                        updateStep = 8;
                        incline = false;
                        decline = false;
                    }
                    else
                    {
                        updateTime = 2;
                        updateStep = 3;
                        incline = false;
                        decline = false;
                    }
                }
                if (lastSelfTuning != 3) selfTuningTimer = updateTime+1;
                lastSelfTuning = 3;
#else

                updateTime = 2;
                updateStep = 3;
                if (lastSelfTuning != 3) selfTuningTimer = updateTime+1;
                lastSelfTuning = 3;
#endif
            }
            else if ( abs(deltaRpm) > RPM_THERHOLD_2 )
            {
                updateTime = 6;
                updateStep = 2;
                if (lastSelfTuning != 2) selfTuningTimer = updateTime+1;
                lastSelfTuning = 2;

#ifdef incline_test
                incline = false;
                decline = false;
#endif
            }
            else if (abs(deltaRpm) > RPM_THERHOLD_1)
            {
                updateTime = 4;
                updateStep = 1;
                if (lastSelfTuning != 1) selfTuningTimer = updateTime+1;
                lastSelfTuning = 1;

#ifdef incline_test
                incline = false;
                decline = false;
#endif
            }
            else
            {
                lastSelfTuning = 0;

#ifdef incline_test
                incline = false;
                decline = false;
#endif
            }

            if (updateTime != 0 && updateStep != 0)
            {
                if (selfTuningTimer > updateTime)
                {
                    if (deltaRpm > 0 && shouldDescr == false)
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
#else
void SpeedCtrl::selfTuningSpeed()
{
    if (startSelfTuning!=0)
    {
        if (startSelfTuning == 1)
            startSelfTuning ++;
        else
        {
            startSelfTuning = 0;
            selfTuning = true;
        }
    }
    if (selfTuning)
    {
        int readbackRpm = SpeedMonitor::getInstance()->getRpm();
        bool shouldDescr = false;

        if (desiredRpm == 0 && (int)readbackRpm != 0)
        {
            setSpeedPWM(basePWM, selfTuning);
            lastSelfTuning = 0;
        }
        else
        {
            int deltaRpm = desiredRpm - readbackRpm;
            int updateTime = 0;
            int updateStep = 0;

            if ( abs(deltaRpm) == desiredRpm )
            {
                updateTime = 2;
                updateStep = 4;

                if (lastSelfTuning != 5) selfTuningTimer = updateTime+1;
                lastSelfTuning = 5;

            }
            else if ( abs(deltaRpm) > RPM_THERHOLD_4 )
            {

                if (deltaRpm > 0)
                {
                    updateTime = 10;
                    updateStep = 0;

                    setSpeedPWMDirect(basePWM);
                }
                else
                {
                    updateTime = 5;
                    updateStep = 10;

                }
                if (lastSelfTuning != 4) selfTuningTimer = updateTime+1;
                lastSelfTuning = 4;
            }
            else if ( lastSelfTuning != 4 && abs(deltaRpm) > RPM_THERHOLD_3 )
            {
                /*
                int slope = AS.getY();
                if (slope < -50) //incline
                {
                    updateTime = 1;
                    updateStep = 4;
                    incline = true;
                    decline = false;
                }
                else if (slope > 50) //decline
                {
                    updateTime = 1;
                    updateStep = 10;
                    shouldDescr = true;
                    incline = false;
                    decline = true;
                }
                else
                {
                    if (incline)
                    {
                        updateTime = 1;
                        updateStep = 4;
                        shouldDescr = true;
                        incline = false;
                        decline = false;
                    }
                    else if (decline)
                    {
                        updateTime = 1;
                        updateStep = 8;
                        incline = false;
                        decline = false;
                    }
                    else
                    {
                        updateTime = 2;
                        updateStep = 3;
                        incline = false;
                        decline = false;
                    }
                }*/
                updateTime = 3;
                updateStep = 3;
                if (lastSelfTuning != 3) selfTuningTimer = updateTime+1;
                lastSelfTuning = 3;
            }
            else if ( abs(deltaRpm) > RPM_THERHOLD_2 )
            {
                updateTime = 6;
                updateStep = 2;
                if (lastSelfTuning != 2) selfTuningTimer = updateTime+1;
                lastSelfTuning = 2;

#ifdef incline_test
                incline = false;
                decline = false;
#endif
            }
            else if (abs(deltaRpm) > RPM_THERHOLD_1)
            {
                updateTime = 4;
                updateStep = 1;
                if (lastSelfTuning != 1) selfTuningTimer = updateTime+1;
                lastSelfTuning = 1;

#ifdef incline_test
                incline = false;
                decline = false;
#endif
            }
            else
            {
                lastSelfTuning = 0;

#ifdef incline_test
                incline = false;
                decline = false;
#endif
            }

            if (updateTime != 0 && updateStep != 0)
            {
                if (selfTuningTimer > updateTime)
                {
                    if (deltaRpm > 0 && shouldDescr == false)
                    {
                        if (desiredDirection == goForward)
                            incrSpeedPWM(updateStep);
                        else
                            descrSpeedPWM(updateStep);
                    }
                    else
                    {
                        if (desiredDirection == goForward)
                            descrSpeedPWM(updateStep);
                        else
                            incrSpeedPWM(updateStep);
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
#endif
void SpeedCtrl::checkSlope()
{
    int16_t slope = AS.getY();
    int16_t slope_thershold = 50;
    float readbackRpm = 0;
    float readbackSpeed = 0;
    int deltaRpm;

    SpeedMonitor::getInstance()->getSpeed(&readbackRpm, &readbackSpeed);

    if (abs(slope) > slope_thershold)
    {
        deltaRpm = desiredRpm - (int)readbackRpm;
        if ( abs(deltaRpm) > RPM_THERHOLD_4)
        {
            setSpeedPWM(basePWM, selfTuning);
        }
        else if ( abs(deltaRpm) > RPM_THERHOLD_3 )
        {
            if (deltaRpm > 0)
                incrSpeedPWM(4);
            else
                descrSpeedPWM(4);
        }
        else if ( abs(deltaRpm) > RPM_THERHOLD_2 )
        {
            if (deltaRpm > 0)
                incrSpeedPWM(1);
            else
                descrSpeedPWM(1);
        }
    }
}

void SpeedCtrl::setSpeedPWM(float pwm, bool selfTuning_f)
{
    if (checkPWM(pwm))
    {
        bool wasForward = false;
        int curDirection;

        if (desiredDirection == goStop)
        {
            wasForward = (speedPWM < basePWM );
        }
        else
        {
            wasForward = (desiredDirection == goForward);
        }

        curDirection = (pwm < basePWM )? goBackward: ((pwm > basePWM)? goForward:goStop );

        desiredDirection = curDirection;
        selfTuning = selfTuning_f;

        if (wasForward && curDirection == goBackward)
        {
            printf("wasForward\n");
            xQueueSend(scheduler_task::getSharedObject(shared_directionQueue), &pwm, portMAX_DELAY);
            selfTuning = false;
            startSelfTuning = 0;
        }
        else
        {
            throttlePWM.set(pwm);
            speedPWM = pwm;
        }

        if (! selfTuning_f)
        {
            desiredCustom = 0;
        }

    }
}
void SpeedCtrl::setStop()
{
    if (desiredDirection == goForward)
    {
        speedPWM = backLimitPWM;
        throttlePWM.set(backLimitPWM);
    }
    else
    {
        speedPWM = basePWM;
        throttlePWM.set(basePWM);
    }
    desiredDirection = goStop;
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
                desiredRpm = rpm_forward_custom[desiredCustom - 1];
                startSelfTuning = 1;
                setSpeedPWM(pwm_forward_custom[desiredCustom - 1], true);
            }
        }
    }
    else
    {
        if (speedSetting > 0 && speedSetting < 4)
        {
            if (desiredDirection != goBackward || desiredCustom != speedSetting)
            {
                startSelfTuning = 1;
                desiredCustom = speedSetting;
                setSpeedPWM(pwm_backward_custom[desiredCustom - 1], true);
                desiredRpm = rpm_backward_custom[desiredCustom- 1];
            }
        }
    }

}
void SpeedCtrl::setSelfTuning(bool tune)
{
    selfTuning = tune;
    if (tune)
        startSelfTuning = 1;
}
void SpeedCtrl::setSpeedPWMDirect(float pwm)
{
    speedPWM = pwm;
    throttlePWM.set(pwm);
    selfTuning = false;
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
    //SpeedMonitor::getInstance()->calSpeed();
    SpeedMonitor::getInstance()->addRpmCounter();
}

SpeedMonitor::SpeedMonitor()
{
    //Singleton
    int port2_5 = 5;
    eint3_enable_port2(port2_5, eint_rising_edge, speed_pulse_start);

    m_rpmRecorded = 0;
    m_rpmCounter = 0;
}
void SpeedMonitor::addRpmCounter()
{
    m_rpmCounter++;
}
int SpeedMonitor::getRpmCounter()
{
    return m_rpmCounter;
}
SpeedMonitor* SpeedMonitor::getInstance()
{
    if (m_pInstance == NULL)
        m_pInstance = new SpeedMonitor();
    return m_pInstance;
}

int SpeedMonitor::getRpm()
{
    return m_rpm;
}

void SpeedMonitor::getSpeed(float* rpm, float* speed)
{
    const uint64_t ONE_SECOND = 1000;
    uint64_t cur_time = sys_get_uptime_ms();
    if ((cur_time - m_last_time > ONE_SECOND/2) &&
            ((!floatIsSame(m_speed, 0)) || (!floatIsSame(m_rpm, 0))))
    {
        m_speed = 0;
        m_rpm = 0;
    }

    *speed = m_rpmCounter;
    m_rpmCounter = 0;
    *rpm = m_rpm = (*speed) /5*60*2;
    *speed = m_rpm * DIAMETER*2*PI /60;
}
