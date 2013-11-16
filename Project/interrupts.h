#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "stm32f0xx_rcc.h"

const uint8_t ASCII_ZERO = '0';
const uint8_t ASCII_DDE_STRING_BEGIN = 'W';
const uint8_t ASCII_DDE_STRING_END = '\r';

volatile enum
{
    WORD_WAIT_FOR_DATA,
    WORD_BEGIN = 0x01,
    WORD_AZIMUTH = 0x02,
    WORD_AZIMUTH_CHECKED = 0x04,
    WORD_ELEVATION = 0x08,
    WORD_COORDINATES_STORED = 0x10
} dde_word_state = WORD_WAIT_FOR_DATA;

volatile uint8_t received = 0x0;

volatile uint8_t azimuth = 0x0;
volatile uint8_t elevation = 0x0;
volatile uint8_t azimuth_buffer = 0x0;
volatile uint8_t elevation_buffer = 0x0;


void SysTick_Handler(void);
void USART1_IRQHandler(void);

inline void word_wait_for_data();
inline void word_begin();
inline void word_azimuth();
inline void word_azimuth_checked();
inline void word_elevation();
inline void word_coordinates_stored();

inline uint8_t is_digit(uint8_t);
inline uint8_t is_azimuth_ok();
inline uint8_t is_elevation_ok();
inline uint8_t atoi(uint8_t);


#endif
