
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
#define RPM_THERHOLD_1 25
#define RPM_THERHOLD_2 50
#define RPM_THERHOLD_3 100

class DirectionCtrl
{
    public:
        float dirPWM = 0;
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

    private:
        DirectionCtrl();
        static DirectionCtrl * m_pInstance;
        PWM directionPWM;
        GPIO pin0_29;
        GPIO pin0_30;
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
        enum
        {
            goStop,
            goForward,
            goBackward
        };
        float speedPWM = 0;
        static SpeedCtrl * getInstance();
        void initESC();
        bool checkPWM(float pwm);
        void selfTuningSpeed();
        void setSpeedPWM(float pwm, bool selfTuning_f = false);
        void setSpeedCustom(bool forward, uint8_t speedSetting);
        void incrSpeedPWM(int step = 1);
        void descrSpeedPWM(int step = 1);
        void setStop();
        int getGoDesiredDirection();

    private:
        SpeedCtrl();
        static SpeedCtrl * m_pInstance;
        PWM throttlePWM;
        GPIO pin1_22;
        GPIO pin1_23;
        bool selfTuning = false;
        int selfTuningTimer = 0;
        int startSelfTuning;

        const float PWMStep = 0.005;
        const float basePWM = BASE_DUTY_CYCLE;
        const float frontLimitPWM = MAX_DUTY_CYCLE;
        const float backLimitPWM = MIN_DUTY_CYCLE;
        int desiredRpm;
        int desiredCustom;
        int desiredDirection;
        float pwm_forward_custom[3] ={8.655, 8.7, 8.72}; //Level 2-3 requires tuning
        const int rpm_forward_custom[3] ={120, 150, 170};
        float pwm_backward_custom[3] ={7.8, 7.6, 7.4};
        const int rpm_backward_custom[3] ={110, 140, 170};
};


class SpeedMonitor
{
    public:
        float m_speed = 0;
        int m_rpm = 0;
        int m_rpmArray[50];
        int m_rpmRecorded = 0;
        static SpeedMonitor * getInstance();
        void calSpeed();
        int getRpm();
        void getSpeed(int* rpm, float* speed);
        void getSpeedEachSecond(int* rpm, float* speed);

    private:
        SpeedMonitor();
        static SpeedMonitor * m_pInstance;
        uint64_t m_last_time = 0;

};

#endif
