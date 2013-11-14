#include "interrupts.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_misc.h"

void SysTick_Handler(void)
{
    if(w_received)
    {
        sign = 'W';
        GPIO_Write(GPIOA, sign);
    }
    else
    {
        sign += 1;
        GPIO_Write(GPIOA, sign);

    }

    USART_SendData(USART1, sign);
}

void USART1_IRQHandler(void)
{
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
    {
        sign = USART_ReceiveData(USART1);
        if(sign == 'W')
        {
            w_received = 1;
        }

        USART_SendData(USART1, '!');
    }
}
