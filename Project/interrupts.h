#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"

#define DDE_STRING_LENGTH 7

const uint8_t ASCII_ZERO = '0';
const uint8_t ASCII_DDE_STRING_BEGIN = 'W';
const uint8_t ASCII_DDE_STRING_END = '\r';

const uint16_t SERVO_PULSE_CENTER = 1500;

const uint16_t ANTENNA_NORTH_MIN = 270;
const uint16_t ANTENNA_NORTH_CENTER = 0;
const uint16_t ANTENNA_NORTH_MAX = 90;

const uint16_t ANTENNA_EAST_MIN = 0;
const uint16_t ANTENNA_EAST_CENTER = 90;
const uint16_t ANTENNA_EAST_MAX = 180;

const uint16_t ANTENNA_SOUTH_MIN = 90;
const uint16_t ANTENNA_SOUTH_CENTER = 180;
const uint16_t ANTENNA_SOUTH_MAX = 270;

const uint16_t ANTENNA_WEST_MIN = 180;
const uint16_t ANTENNA_WEST_CENTER = 270;
const uint16_t ANTENNA_WEST_MAX = 0;

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
    ANTENNA_STATE_BEFORE_INIT = 0x00, ANTENNA_STATE_NORMAL = 0x80, ANTENNA_STATE_CALIBRATION = 0x10
} antenna_state = ANTENNA_STATE_BEFORE_INIT;

volatile uint8_t received = 0x0;

volatile uint8_t azimuth = 0x0;
volatile uint8_t elevation = 0x0;
volatile uint8_t azimuth_buffer = 0x0;
volatile uint8_t elevation_buffer = 0x0;

volatile uint8_t dde_string[DDE_STRING_LENGTH];
volatile uint8_t dde_string_counter = 0x0;

TIM_OCInitTypeDef TIM_OCInitStructure;

volatile uint16_t azimuth_pulse = SERVO_PULSE_CENTER; //correct to pulse
volatile uint16_t elevation_pulse = SERVO_PULSE_CENTER; //correct to pulse

void interrupts_init(void);
inline void init_and_power_on_servos(void);

void SysTick_Handler(void);
void USART1_IRQHandler(void);

inline void set_calibration_mode();

inline void word_reset();
inline void word_reading();
inline void word_end();
inline void word_error();

inline void parse_azimuth();
inline void parse_elevation();
inline uint8_t atoi(uint8_t);
inline uint8_t is_digit(uint8_t);

inline uint8_t is_coordinates_correct();
inline uint8_t is_azimuth_correct();
inline uint8_t is_elevation_correct();

inline void store_coordinates();

inline void set_pulses();
inline void set_GPIO(void);

inline void set_next_antenna_direction(void);

#endif
