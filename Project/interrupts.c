#include "interrupts.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_tim.h"

void interrupts_init(void)
{
    init_and_power_on_servos();
    set_GPIO();
}

inline void init_and_power_on_servos(void)
{
    setServoAngle(0, COORDINATE_AZIMUTH);
    setServoAngle(0, COORDINATE_ALTITUDE);

    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void SysTick_Handler(void)
{
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        received = USART_ReceiveData(USART1);

        if('k' == received)
        {
            if(ANTENNA_STATE_CALIBRATION != antenna_state)
                antenna_state = ANTENNA_STATE_CALIBRATION;
            else
                antenna_state = ANTENNA_STATE_NORMAL;
        }
        else if('n' == received)
        {
            if(ANTENNA_STATE_CALIBRATION == antenna_state)
                set_next_antenna_direction();
        }

        if(ANTENNA_STATE_NORMAL == antenna_state)
        {
            if(ASCII_DDE_STRING_BEGIN == received)
                word_reset();
            else if(ASCII_DDE_STRING_END == received)
                word_end();
            else if(WORD_READING == dde_state)
                word_reading();
            else if(WORD_ERROR == dde_state)
                word_error();
        }

        set_GPIO();
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
    parse_altitude();
    if(is_coordinates_correct())
    {
        set_up_antenna();
        dde_state = WORD_WAIT_FOR_RESET;
    }
    else
        dde_state = WORD_ERROR;
}

inline void parse_azimuth()
{
    if(is_digit(dde_string[0]))
        azimuth = 100 * atoi(dde_string[0]);
    else
        dde_state = WORD_ERROR;

    if(is_digit(dde_string[1]))
        azimuth += 10 * atoi(dde_string[1]);
    else
        dde_state = WORD_ERROR;

    if(is_digit(dde_string[2]))
        azimuth += atoi(dde_string[2]);
    else
        dde_state = WORD_ERROR;
}

inline void parse_altitude()
{
    if(is_digit(dde_string[4]))
        altitude = 100 * atoi(dde_string[4]);
    else
        dde_state = WORD_ERROR;

    if(is_digit(dde_string[5]))
        altitude += 10 * atoi(dde_string[5]);
    else
        dde_state = WORD_ERROR;

    if(is_digit(dde_string[6]))
        altitude += atoi(dde_string[6]);
    else
        dde_state = WORD_ERROR;
}

inline uint8_t is_coordinates_correct()
{
    if(WORD_ERROR == dde_state)
        return (uint8_t)0;
    if(!is_azimuth_correct())
        return (uint8_t)0;
    if(!is_altitude_correct())
        return (uint8_t)0;
    return (uint8_t)1;
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
    if((azimuth >= 0) && (azimuth <= 360))
        return (uint8_t)1;
    else
        return (uint8_t)0;
}

inline uint8_t is_altitude_correct()
{
    if((altitude >= 0) && (altitude <= 90))
        return (uint8_t)1;
    else
        return (uint8_t)0;
}

inline uint8_t atoi(uint8_t a)
{
    a -= ASCII_ZERO;
    return a;
}

inline void set_GPIO(void)
{
    GPIO_Write(GPIOA, antenna_state | antenna_direction);
}

inline void set_next_antenna_direction(void)
{
    if(ANTENNA_DIRECTION_NORTH == antenna_direction)
    {
        antenna_direction = ANTENNA_DIRECTION_EAST;
        antenna_direction_angle = 90;
    }
    else if(ANTENNA_DIRECTION_EAST == antenna_direction)
    {
        antenna_direction = ANTENNA_DIRECTION_SOUTH;
        antenna_direction_angle = 180;
    }
    else if(ANTENNA_DIRECTION_SOUTH == antenna_direction)
    {
        antenna_direction = ANTENNA_DIRECTION_WEST;
        antenna_direction_angle = 270;
    }
    else if(ANTENNA_DIRECTION_WEST == antenna_direction)
    {
        antenna_direction = ANTENNA_DIRECTION_NORTH;
        antenna_direction_angle = 0;
    }
}

inline void set_up_antenna(void)
{
    setServoAngle(azimuth - antenna_direction_angle, COORDINATE_AZIMUTH);
    setServoAngle(altitude, COORDINATE_ALTITUDE);
}
