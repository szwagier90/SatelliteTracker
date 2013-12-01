#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"
#include "servo.h"

#define DDE_STRING_LENGTH 7

const uint8_t ASCII_ZERO = '0';
const uint8_t ASCII_DDE_STRING_BEGIN = 'W';
const uint8_t ASCII_DDE_STRING_END = '\r';

volatile enum
{
    WORD_WAIT_FOR_RESET, WORD_READING, WORD_ERROR
} dde_state = WORD_WAIT_FOR_RESET;

volatile enum
{
    ANTENNA_DIRECTION_NORTH = 0x01,
    ANTENNA_DIRECTION_EAST = 0x02,
    ANTENNA_DIRECTION_SOUTH = 0x04,
    ANTENNA_DIRECTION_WEST = 0x08
} antenna_direction = ANTENNA_DIRECTION_NORTH;

volatile enum
{
    ANTENNA_STATE_NORMAL = 0x80, ANTENNA_STATE_WARNING = 0xc0, ANTENNA_STATE_ERROR = 0x20, ANTENNA_STATE_CALIBRATION = 0x10
} antenna_state = ANTENNA_STATE_NORMAL;

volatile uint8_t received = 0x0;

volatile int16_t azimuth = 0x0;
volatile int16_t altitude = 0x0;

volatile uint8_t dde_string[DDE_STRING_LENGTH];
volatile uint8_t dde_string_counter = 0x0;

volatile uint16_t antenna_direction_angle = 0;

volatile uint16_t azimuth_pulse = SERVO_PULSE_CENTER;
volatile uint16_t altitude_pulse = SERVO_PULSE_CENTER;

void interrupts_init(void);
inline void init_and_power_on_servos(void);

void SysTick_Handler(void);
void USART1_IRQHandler(void);

inline void word_reset();
inline void word_reading();
inline void word_end();
inline void word_error();

inline void parse_azimuth();
inline void parse_altitude();
inline uint8_t atoi(uint8_t);
inline uint8_t is_digit(uint8_t);

inline uint8_t is_coordinates_correct();
inline uint8_t is_azimuth_correct();
inline uint8_t is_altitude_correct();

inline void set_GPIO(void);

inline void set_next_antenna_direction(void);
inline void set_up_antenna(void);

#endif
