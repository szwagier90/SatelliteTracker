#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f0xx_tim.h"

#define SERVO_PULSE_CENTER 1550

#define SERVO_ANGLE_NEG_90 2700
#define SERVO_ANGLE_NEG_45 2100
#define SERVO_ANGLE_POS_0 1550
#define SERVO_ANGLE_POS_45 1000
#define SERVO_ANGLE_POS_90 600

enum servoCoordinate
{
    COORDINATE_AZIMUTH, COORDINATE_ALTITUDE
};

inline void initServo(int16_t, enum servoCoordinate);
inline void setServoAngle(int16_t, enum servoCoordinate);
inline uint16_t angleToPulse( int16_t);
inline uint32_t calculatePositivePulse(uint16_t, uint16_t, uint16_t);
inline uint32_t calculateNegativePulse(uint16_t, uint16_t, uint16_t);

#endif
