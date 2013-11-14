#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "stm32f0xx_rcc.h"

volatile uint8_t sign = '0';
volatile uint8_t w_received = 0;

void SysTick_Handler(void);
void USART1_IRQHandler(void);

#endif
