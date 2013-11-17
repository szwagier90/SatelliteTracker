#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "stm32f0xx_rcc.h"

#define DDE_STRING_LENGTH 7

const uint8_t ASCII_ZERO = '0';
const uint8_t ASCII_DDE_STRING_BEGIN = 'W';
const uint8_t ASCII_DDE_STRING_END = '\r';

volatile enum
{
    WORD_WAIT_FOR_RESET = 0x01,
    WORD_READING = 0x55,
    WORD_ERROR = 0xff
} dde_state = WORD_WAIT_FOR_RESET;

volatile uint8_t received = 0x0;

volatile uint8_t azimuth = 0x0;
volatile uint8_t elevation = 0x0;
volatile uint8_t azimuth_buffer = 0x0;
volatile uint8_t elevation_buffer = 0x0;

volatile uint8_t dde_string[DDE_STRING_LENGTH];
volatile uint8_t dde_string_counter = 0x0;

void SysTick_Handler(void);
void USART1_IRQHandler(void);

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

#endif
