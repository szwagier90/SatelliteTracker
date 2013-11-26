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
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;

    TIM_OCInitStructure.TIM_Pulse = azimuth_pulse;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_Pulse = elevation_pulse;
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);

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
                set_calibration_mode();
            else
                antenna_state = ANTENNA_STATE_NORMAL;
        }
        else if('n' == received)
        {
            if(ANTENNA_STATE_CALIBRATION == antenna_state)
                set_next_antenna_direction();
        }
        else if('*' == received)
        {
            azimuth_pulse += 100;
            set_pulses();
        }
        else if('/' == received)
        {
            azimuth_pulse -= 100;
            set_pulses();
        }

        if(ASCII_DDE_STRING_BEGIN == received)
            word_reset();
        else if(ASCII_DDE_STRING_END == received)
            word_end();
        else if(WORD_READING == dde_state)
            word_reading();
        else if(WORD_ERROR == dde_state)
            word_error();

        set_GPIO();
        USART_SendData(USART1, received);
    }
}

inline void set_calibration_mode()
{
    antenna_state = ANTENNA_STATE_CALIBRATION;
    azimuth_pulse = SERVO_PULSE_CENTER;
    elevation_pulse = SERVO_PULSE_CENTER;
    set_pulses();
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

inline void set_pulses()
{
    TIM_OCInitStructure.TIM_Pulse = azimuth_pulse;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_Pulse = elevation_pulse;
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
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

inline void set_GPIO(void)
{
    GPIO_Write(GPIOA, antenna_state | antenna_direction);
}

inline void set_next_antenna_direction(void)
{
    if(ANTENNA_DIRECTION_NORTH == antenna_direction)
        antenna_direction = ANTENNA_DIRECTION_EAST;
    else if(ANTENNA_DIRECTION_EAST == antenna_direction)
        antenna_direction = ANTENNA_DIRECTION_SOUTH;
    else if(ANTENNA_DIRECTION_SOUTH == antenna_direction)
        antenna_direction = ANTENNA_DIRECTION_WEST;
    else if(ANTENNA_DIRECTION_WEST == antenna_direction)
        antenna_direction = ANTENNA_DIRECTION_NORTH;
}
