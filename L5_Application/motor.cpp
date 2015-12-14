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
    //LD.setNumber(dir);
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

}
bool SpeedCtrl::checkPWM(float pwm)
{
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
    int readbackRpm = SpeedMonitor::getInstance()->getRpm();
    int rpmDiff = readbackRpm - desiredRpm;
    if (selfTuning && rpmDiff >= RPM_THRESHOLD_3)
    {
        setSpeedPWM(8.65, selfTuning);
        LD.setNumber(99);
        selfTuningTimer = 0;
    }
    else if (startSelfTuning!=0)
    {
        if (startSelfTuning <= 5 )
            startSelfTuning ++;
        else
        {
            startSelfTuning = 0;
            selfTuning = true;
        }
        LD.setNumber(88);
        selfTuningTimer = 0;
    }
    else if (selfTuning && desiredDirection != goStop)
    {
        if (desiredRpm == 0 && (int)readbackRpm != 0)
        {
            setSpeedPWM(basePWM, selfTuning);
            lastSelfTuning = 0;
            LD.setNumber(lastSelfTuning);
            selfTuningTimer = 0;
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

                if ((lastSelfTuning != 5) || (selfTuningTimer == 0))
                    selfTuningTimer = updateTime;
                lastSelfTuning = 5;

            }
            else if ( (abs(deltaRpm) >= RPM_THRESHOLD_3) &&
                    ( ((lastSelfTuning > 3) && (selfTuningTimer == 0)) ||
                      (lastSelfTuning <= 3) ) )
            {
                updateTime = 6;
                updateStep = 3;
                if ((lastSelfTuning != 3) || (selfTuningTimer == 0))
                    selfTuningTimer = updateTime;
                lastSelfTuning = 3;
            }
            else if ( (abs(deltaRpm) >= RPM_THRESHOLD_2 )  &&
                    ( ((lastSelfTuning > 2) && (selfTuningTimer == 0)) ||
                      (lastSelfTuning <= 2) ) )
            {
                updateTime = 5;
                updateStep = 2;
                if ((lastSelfTuning != 2) || (selfTuningTimer == 0))
                    selfTuningTimer = updateTime;
                lastSelfTuning = 2;
            }
            else if ((abs(deltaRpm) >= RPM_THRESHOLD_1)  &&
                    ( ((lastSelfTuning > 1) && (selfTuningTimer == 0)) ||
                      (lastSelfTuning <= 1) ) )
            {
                updateTime = 4;
                updateStep = 1;
                if ((lastSelfTuning != 1) || (selfTuningTimer == 0))
                    selfTuningTimer = updateTime;
                lastSelfTuning = 1;
            }
            else
            {
                lastSelfTuning = 0;
            }
            LD.setNumber(lastSelfTuning);
            if (updateTime != 0 && updateStep != 0)
            {
                if (selfTuningTimer <= 0)
                {
                    if (deltaRpm > 0)
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
                    selfTuningTimer = updateTime;
                }
                else
                    selfTuningTimer --;
            }
            else
                selfTuningTimer = 0;
        }
    }
}

void SpeedCtrl::setSpeedPWM(float pwm, bool selfTuning_f, bool backSeq)
{
    if (!backSeq || (backwardSequence && backSeq))
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

            curDirection = (pwm < basePWM )? goBackward: ((floatIsSame(pwm,basePWM) && !selfTuning)? goStop: goForward);

            desiredDirection = curDirection;
            selfTuning = selfTuning_f;

            if (wasForward && curDirection == goBackward && (backwardSequence == false))
            {
                printf("Get Backward\n");
                selfTuning = false;
                backwardSequence = true;
                xQueueSend(scheduler_task::getSharedObject(shared_directionQueue), &pwm, 10);
                startSelfTuning = 0;
            }
            else
            {
                throttlePWM.set(pwm);
                speedPWM = pwm;
                backwardSequence = backSeq;
            }

            if (! selfTuning_f)
            {
                desiredCustom = 0;
            }
        }
    }
}
void SpeedCtrl::setStop()
{
    selfTuning = false;
    desiredDirection = goStop;
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
}

void SpeedCtrl::setDesiredDirection(int dir)
{
    desiredDirection = dir;
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
void SpeedCtrl::setSpeedPWMDirect(float pwm, bool backSeq)
{
    if (!backSeq || ( backwardSequence && backSeq))
    {
        speedPWM = pwm;
        selfTuning = false;
        throttlePWM.set(pwm);
        backwardSequence = backSeq;
    }
}
void SpeedCtrl::incrSpeedPWM(int step)
{
    float pwm = speedPWM + ((float)PWMStep* (float)step);
    if (checkPWM(pwm))
    {
        if (desiredCustom != 0 && desiredDirection != goStop)
        {
            if (desiredDirection == goForward)
                pwm_forward_custom[desiredCustom - 1] = pwm;
            else
                pwm_backward_custom[desiredCustom - 1] = pwm;
        }
        speedPWM = pwm;
        throttlePWM.set(pwm);
    }
}

void SpeedCtrl::descrSpeedPWM(int step)
{
    float pwm = speedPWM - ((float)PWMStep* (float)step);
    if (checkPWM(pwm))
    {
        //printf("Set speed %f\n", pwm);
        if (desiredCustom != 0 && desiredDirection != goStop)
        {
            if (desiredDirection == goForward)
                pwm_forward_custom[desiredCustom - 1] = pwm;
            else
                pwm_backward_custom[desiredCustom - 1] = pwm;
        }
        speedPWM = pwm;
        throttlePWM.set(pwm);
    }
}

int SpeedCtrl::getGoDesiredDirection()
{
    return desiredDirection;
}

void SpeedCtrl::setBackwardSequence(bool bs)
{
    backwardSequence = bs;
}
SpeedMonitor* SpeedMonitor::m_pInstance = NULL;
void speed_pulse_start(void)
{
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
    *rpm = m_rpm;
    *speed = m_rpm;
}
void SpeedMonitor::periodGetSpeed(float* rpm, float* speed)
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


