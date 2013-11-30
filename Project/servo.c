#include "servo.h"

inline void initServo(int16_t angle, enum servoCoordinate coordinate)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;

    TIM_OCInitStructure.TIM_Pulse = angleToPulse(angle);

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
}

inline void setServoAngle(int16_t angle, enum servoCoordinate coordinate)
{
    if(COORDINATE_AZIMUTH == coordinate)
        TIM1->CCR1 = angleToPulse(angle);
    else if(COORDINATE_ALTITUDE == coordinate)
        TIM1->CCR4 = angleToPulse(angle);
}

inline uint16_t angleToPulse(int16_t angle)
{
    int32_t pulse = SERVO_PULSE_CENTER;

    if(angle <= -90)
        pulse = SERVO_ANGLE_NEG_90;
    else if(angle < -45) // -90 < angle < -45
    {
        angle *= -1;
        angle -= 45;
        pulse = calculateNegativePulse(SERVO_ANGLE_NEG_90, SERVO_ANGLE_NEG_45, angle);
    }
    else if(angle < 0) // -45 <= angle < 0
    {
        angle *= -1;
        pulse = calculateNegativePulse(SERVO_ANGLE_NEG_45, SERVO_ANGLE_POS_0, angle);
    }
    else if(angle < 45) // 0 <= angle < 45
        pulse = calculatePositivePulse(SERVO_ANGLE_POS_0, SERVO_ANGLE_POS_45, angle);
    else if(angle < 90) // 45 <= angle < 90
    {
        angle -= 45;
        pulse = calculatePositivePulse(SERVO_ANGLE_POS_45, SERVO_ANGLE_POS_90, angle);
    }
    else if(angle >= 90)
        pulse = SERVO_ANGLE_POS_90;
    else
        pulse = SERVO_ANGLE_POS_0;

    return pulse;
}

inline uint32_t calculatePositivePulse(uint16_t leftEdgePulse, uint16_t rightEdgePulse, uint16_t angleMultiplier)
{
    int32_t pulse;

    pulse = rightEdgePulse - leftEdgePulse;
    pulse *= angleMultiplier;
    pulse /= 45;
    pulse += leftEdgePulse;

    return pulse;
}

inline uint32_t calculateNegativePulse(uint16_t leftEdgePulse, uint16_t rightEdgePulse, uint16_t angleMultiplier)
{
    int32_t pulse;

    pulse = leftEdgePulse - rightEdgePulse;
    pulse *= angleMultiplier;
    pulse /= 45;
    pulse += rightEdgePulse;

    return pulse;
}
