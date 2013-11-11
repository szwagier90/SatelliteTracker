#include "stm32f0xx_gpio.h"

#include "HD44780.h"


void _LCD_SetNibble(unsigned char nibbleToWrite)
{
	GPIO_WriteBit(LCD_GPIO, LCD_D4, (nibbleToWrite & 0x01));
	GPIO_WriteBit(LCD_GPIO, LCD_D5, (nibbleToWrite & 0x02));
	GPIO_WriteBit(LCD_GPIO, LCD_D6, (nibbleToWrite & 0x04));
	GPIO_WriteBit(LCD_GPIO, LCD_D7, (nibbleToWrite & 0x08));
}

unsigned char _LCD_GetNibble(void)
{
	unsigned char tmp = 0;

	GPIO_WriteBit(LCD_GPIO, LCD_DB4)
if(LCD_DB4_PIN & LCD_DB4)
	tmp |= (1 << 0);
if(LCD_DB5_PIN & LCD_DB5)
	tmp |= (1 << 1);
if(LCD_DB6_PIN & LCD_DB6)
	tmp |= (1 << 2);
if(LCD_DB7_PIN & LCD_DB7)
	tmp |= (1 << 3);
return tmp;
}

void _LCD_Write(unsigned char dataToWrite)
{
	GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_SET);
	_LCD_SetNibble(dataToWrite >> 4);
	GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_RESET);
	GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_SET);
	_LCD_SetNibble(dataToWrite);
	GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_RESET);
//	_delay_us(50);
}

void LCD_WriteCommand(unsigned char commandToWrite)
{
	GPIO_WriteBit(LCD_GPIO, LCD_RS, Bit_RESET);
	_LCD_Write(commandToWrite);
}

void LCD_WriteData(unsigned char dataToWrite)
{
	GPIO_WriteBit(LCD_GPIO, LCD_RS, Bit_SET);
	_LCD_Write(dataToWrite);
}

void LCD_WriteText(char * text)
{
	while(*text)
	{
		LCD_WriteData(*text++);
	}
}

void LCD_GoTo(unsigned char x, unsigned char y)
{
	LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}

void LCD_Clear(void)
{
	LCD_WriteCommand(HD44780_CLEAR);
//	_delay_ms(2);
}

void LCD_Home(void)
{
	LCD_WriteCommand(HD44780_HOME);
//	_delay_ms(2);
}

void LCD_Initalize(void)
{
	unsigned char i;

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  LCD_DB4 | LCD_DB5 | LCD_DB6 | LCD_DB7 | LCD_RS | LCD_E
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_GPIO, &GPIO_InitStructure);

//	_delay_ms(15); // oczekiwanie na ustalibizowanie sie napiecia zasilajacego
	GPIO_WriteBit(LCD_GPIO, LCD_RS, Bit_RESET);
	GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_RESET);

	for(i = 0 ; i < 3 ; ++i) // trzykrotne powtórzenie bloku instrukcji
	{
		GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_SET);
		_LCD_SetNibble(0x03); // tryb 8-bitowy
		GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_RESET);
//		_delay_ms(5); // czekaj 5ms
	}

	GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_SET);
	_LCD_SetNibble(0x02); // tryb 4-bitowy
	GPIO_WriteBit(LCD_GPIO, LCD_E, Bit_RESET);
//	_delay_ms(1); // czekaj 1ms

	LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
	LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // wylaczenie wyswietlacza
	LCD_WriteCommand(HD44780_CLEAR); // czyszczenie zawartosci pamieci DDRAM
//	_delay_ms(2);

	LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);// inkrementaja adresu i przesuwanie kursora
	LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK); // wlacz LCD, bez kursora i mrugania
}
