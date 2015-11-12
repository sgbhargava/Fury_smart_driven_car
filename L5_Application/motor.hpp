
#ifndef L5_APPLICATION_MOTOR_HPP_
#define L5_APPLICATION_MOTOR_HPP_
#include "gpio.hpp"
#include "lpc_pwm.hpp"

#define STANDARD_FREQ 55
#define PI 3.1415
#define DIAMETER 0.1 //meter
#define KM_TO_MILES 1/1.60934
#define MS_TO_MINS 1/(1000 * 60)
#define MAX_DUTY_CYCLE 11
#define MIN_DUTY_CYCLE 5.5
#define BASE_DUTY_CYCLE ((MAX_DUTY_CYCLE+MIN_DUTY_CYCLE)/2)

class DirectionCtrl
{
    public:
        enum
        {
            dirCenter,
            dirFarRight,
            dirRight,
            dirLeft,
            dirFarLeft
        };
        static DirectionCtrl * getInstance();
        void setDirection(int dir);
        void init(void);

    private:
        DirectionCtrl();
        static DirectionCtrl * m_pInstance;
        PWM directionPWM;
        GPIO pin0_29;
        GPIO pin0_30;
        float dirPWM = 0;
        const float halfTurn = ((MAX_DUTY_CYCLE - BASE_DUTY_CYCLE) / 2);
        const float basePWM = BASE_DUTY_CYCLE;
        const float farRight = MAX_DUTY_CYCLE;
        const float farLeft = MIN_DUTY_CYCLE;
        const float right = basePWM + halfTurn;
        const float left = basePWM - halfTurn;
};

class SpeedCtrl
{
    public:
        static SpeedCtrl * getInstance();
        void init();
        void initESC();
        bool checkPWM(float pwm);
        void setSpeedPWM(float pwm);
        void setSpeedCustom(bool forward, uint8_t speedSetting);
        void incrSpeedPWM();
        void descrSpeedPWM();
        void setStop();

    private:
        SpeedCtrl();
        static SpeedCtrl * m_pInstance;
        PWM throttlePWM;
        //GPIO pin1_22;
        //GPIO pin1_23;
        float speedPWM = 0;
        const float PWMStep = 0.1;
        const float basePWM = BASE_DUTY_CYCLE;
        const float frontLimitPWM = MAX_DUTY_CYCLE;
        const float backLimitPWM = MIN_DUTY_CYCLE;
        const float speed_forward_custom1 = 8.68;
        const float speed_forward_custom2 = 8.7;
        const float speed_forward_custom3 = 8.72;
        const float speed_backward_custom1 = 8.0;
        const float speed_backward_custom2 = 7.8;
        const float speed_backward_custom3 = 7.6;
};


class SpeedMonitor
{
    public:
        static SpeedMonitor * getInstance();
        void init();
        void setRpm(int rpmVal);
        void calSpeed();
        int getRpm();
        void getSpeed(float* rpm, float* speed);

    private:
        SpeedMonitor();
        static SpeedMonitor * m_pInstance;
        float m_speed = 0;
        float m_rpm = 0;
        uint64_t m_last_time = 0;

};

#endif
