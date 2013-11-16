#include "interrupts.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_misc.h"

void SysTick_Handler(void)
{
    GPIO_Write(GPIOA, azimuth);
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        received = USART_ReceiveData(USART1);

        if(WORD_WAIT_FOR_DATA == dde_word_state)
            word_wait_for_data();
        else if(WORD_BEGIN == dde_word_state)
            word_begin();
        else if(WORD_AZIMUTH == dde_word_state)
            word_azimuth();
        else if(WORD_AZIMUTH_CHECKED == dde_word_state)
            word_azimuth_checked();
        else if(WORD_ELEVATION == dde_word_state)
            word_elevation();
        else if(WORD_COORDINATES_STORED == dde_word_state)
            word_coordinates_stored();

        USART_SendData(USART1, received);
    }
}

inline void word_wait_for_data()
{
    if(ASCII_DDE_STRING_BEGIN == received)
        dde_word_state = WORD_BEGIN;
}

inline void word_begin()
{
    if(is_digit(received))
    {
        azimuth_buffer = atoi(received);
        dde_word_state = WORD_AZIMUTH;
    }
    else if(received != 'W')
        dde_word_state = WORD_WAIT_FOR_DATA;
}

inline void word_azimuth()
{
    if(is_digit(received))
    {
        azimuth_buffer *= 10;
        azimuth_buffer += atoi(received);
    }
    else if(ASCII_DDE_STRING_BEGIN == received)
        dde_word_state = WORD_BEGIN;
    else if(' ' == received)
    {
        if(is_azimuth_ok())
            dde_word_state = WORD_AZIMUTH_CHECKED;
        else
            dde_word_state = WORD_WAIT_FOR_DATA;
    }
    else
    {
        dde_word_state = WORD_WAIT_FOR_DATA;
    }
}

inline void word_azimuth_checked()
{
    if(is_digit(received))
    {
        elevation_buffer = atoi(received);
        dde_word_state = WORD_ELEVATION;
    }
    else if(ASCII_DDE_STRING_BEGIN == received)
        dde_word_state = WORD_BEGIN;
    else
        dde_word_state = WORD_WAIT_FOR_DATA;
}

inline void word_elevation()
{
    if(is_digit(received))
    {
        elevation_buffer *= 10;
        elevation_buffer += atoi(received);
    }
    else if(ASCII_DDE_STRING_BEGIN == received)
        dde_word_state = WORD_BEGIN;
    else if(ASCII_DDE_STRING_END == received)
    {
        if(is_elevation_ok())
        {
            azimuth = azimuth_buffer;
            elevation = elevation_buffer;
            dde_word_state = WORD_COORDINATES_STORED;
        }
        else
            dde_word_state = WORD_WAIT_FOR_DATA;

    }
    else
        dde_word_state = WORD_WAIT_FOR_DATA;
}

inline void word_coordinates_stored()
{
    if(ASCII_DDE_STRING_BEGIN == received)
        dde_word_state = WORD_BEGIN;
    else
        dde_word_state = WORD_WAIT_FOR_DATA;
}

inline uint8_t is_digit(uint8_t c)
{
    if((c >= '0') && (c <= '9'))
        return (uint8_t)1;
    else
        return (uint8_t)0;
}

inline uint8_t is_azimuth_ok()
{
    if((azimuth_buffer >= 0) && (azimuth_buffer <= 360))
        return 1;
    else
        return 0;
}

inline uint8_t is_elevation_ok()
{
    if((elevation_buffer >= 0) && (elevation_buffer <= 360))
        return 1;
    else
        return 0;
}

inline uint8_t atoi(uint8_t a)
{
    return a - ASCII_ZERO;
}
