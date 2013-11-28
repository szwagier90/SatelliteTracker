#include "servo.h"

inline void setServoAngle(int16_t angle, enum servoCoordinate coordinate)
{
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;

    TIM_OCInitStructure.TIM_Pulse = angleToPulse(angle);

    if(COORDINATE_AZIMUTH == coordinate)
        TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    else if(COORDINATE_ALTITUDE == coordinate)
        TIM_OC4Init(TIM1, &TIM_OCInitStructure);
}

inline uint16_t angleToPulse(int16_t angle)
{
    int16_t angle_multiplier = angle;
    int16_t pulse = SERVO_PULSE_CENTER;

    if(angle_multiplier < 0)
        angle_multiplier *= -1;

    if(angle_multiplier >= 45)
        angle_multiplier -= 45;

    if(angle <= -90)
    {
        pulse = SERVO_ANGLE_NEG_90;
    }
    else if(angle < -45)
    {
        pulse = SERVO_ANGLE_NEG_90
        pulse -= SERVO_ANGLE_NEG_45;
        pulse /= 45;
        pulse *= angle_multiplier;
        pulse += SERVO_ANGLE_NEG_90;
    }
    else if(angle < 0)
    {
        pulse = SERVO_ANGLE_NEG_45
        pulse -= SERVO_ANGLE_POS_0;
        pulse /= 45;
        pulse *= angle_multiplier;
        pulse += SERVO_ANGLE_NEG_45;
    }
    else if(angle < 45)
    {
        pulse = SERVO_ANGLE_POS_0
        pulse -= SERVO_ANGLE_POS_45;
        pulse /= 45;
        pulse *= angle_multiplier;
        pulse += SERVO_ANGLE_POS_0;
    }
    else if(angle < 90)
    {
        pulse = SERVO_ANGLE_POS_45
        pulse -= SERVO_ANGLE_POS_90;
        pulse /= 45;
        pulse *= angle_multiplier;
        pulse += SERVO_ANGLE_POS_45;
    }
    else if(angle >= 90)
    {
        pulse = SERVO_ANGLE_POS_90;
    }
    else
        pulse = SERVO_ANGLE_POS_0;

    return pulse;
}
