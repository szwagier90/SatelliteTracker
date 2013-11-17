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

        if(ASCII_DDE_STRING_BEGIN == received)
            word_reset();
        else if(ASCII_DDE_STRING_END == received)
            word_end();
        else if(WORD_READING == dde_state)
            word_reading();
        else if(WORD_ERROR == dde_state)
            word_error();

        USART_SendData(USART1, received);
    }
}

inline void word_reset()
{
    dde_string_counter = 0x0;
    dde_state = WORD_READING;
}

inline void word_reading()
{
    if(dde_string_counter < DDE_STRING_LENGTH)
    {
        dde_string[dde_string_counter] = received;
        ++dde_string_counter;
    }
}

inline void word_end()
{
    parse_azimuth();
    parse_elevation();
    if(is_coordinates_correct())
    {
        store_coordinates();
        dde_state = WORD_WAIT_FOR_RESET;
    }
    else
        dde_state = WORD_ERROR;
}

inline void parse_azimuth()
{
    if(is_digit(dde_string[0]))
        azimuth_buffer = 100 * atoi(dde_string[0]);
    else
        dde_state = WORD_ERROR;

    if(is_digit(dde_string[1]))
        azimuth_buffer += 10 * atoi(dde_string[1]);
    else
        dde_state = WORD_ERROR;

    if(is_digit(dde_string[2]))
        azimuth_buffer += atoi(dde_string[2]);
    else
        dde_state = WORD_ERROR;
}

inline void parse_elevation()
{
    if(is_digit(dde_string[4]))
        elevation_buffer = 100 * atoi(dde_string[4]);
    else
        dde_state = WORD_ERROR;

    if(is_digit(dde_string[5]))
        elevation_buffer += 10 * atoi(dde_string[5]);
    else
        dde_state = WORD_ERROR;

    if(is_digit(dde_string[6]))
        elevation_buffer += atoi(dde_string[6]);
    else
        dde_state = WORD_ERROR;
}

inline uint8_t is_coordinates_correct()
{
    if(WORD_ERROR == dde_state)
        return (uint8_t)0;
    if(!is_azimuth_correct())
        return (uint8_t)0;
    if(!is_elevation_correct())
        return (uint8_t)0;
    return (uint8_t)1;
}

inline void store_coordinates()
{
    azimuth = azimuth_buffer;
    elevation = elevation_buffer;
}

inline void word_error()
{
    //
}

inline uint8_t is_digit(uint8_t c)
{
    if((c >= '0') && (c <= '9'))
        return (uint8_t)1;
    else
        return (uint8_t)0;
}

inline uint8_t is_azimuth_correct()
{
    if((azimuth_buffer >= 0) && (azimuth_buffer <= 360))
        return (uint8_t)1;
    else
        return (uint8_t)0;
}

inline uint8_t is_elevation_correct()
{
    if((elevation_buffer >= 0) && (elevation_buffer <= 90))
        return (uint8_t)1;
    else
        return (uint8_t)0;
}

inline uint8_t atoi(uint8_t a)
{
    a -= ASCII_ZERO;
    return a;
}
