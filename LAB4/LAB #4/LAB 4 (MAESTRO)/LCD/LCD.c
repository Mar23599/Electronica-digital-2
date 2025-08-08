/*
 * LCD.c
 *
 * Created: 18/07/2025 11:05:46
 *  Author: Luis Sactic
 */ 


#include "LCD.h"

void LCD_PulseEnable(void) {
	PORTD |= (1 << LCD_EN);
	_delay_us(1);
	PORTD &= ~(1 << LCD_EN);
	_delay_us(100);
}

void LCD_SendByte(uint8_t byte) {
	// Escribir en PORTD (D0-D5)
	PORTD = (PORTD & 0x03) | ((byte & 0x3F) << 2);
	
	// Escribir en PORTB (D6-D7)
	PORTB = (PORTB & 0xFC) | ((byte >> 6) & 0x03);
	
	LCD_PulseEnable();
}

void LCD_Init(void) {
	// 1. Configurar pines como salidas
	DDRD |= (1 << LCD_RS) | (1 << LCD_EN) |
	(1 << LCD_D0) | (1 << LCD_D1) |
	(1 << LCD_D2) | (1 << LCD_D3) |
	(1 << LCD_D4) | (1 << LCD_D5);
	
	DDRB |= (1 << LCD_D6) | (1 << LCD_D7);

	// 2. Espera inicial extendida
	_delay_ms(50);

	// 3. Secuencia de inicialización en modo 8 bits
	PORTD &= ~(1 << LCD_RS); // Modo comando
	
	// Secuencia de reset (3 veces 0x30)
	LCD_SendByte(0x30);
	_delay_ms(5);
	LCD_SendByte(0x30);
	_delay_us(100);
	LCD_SendByte(0x30);
	_delay_us(100);
	
	// Configuración final
	LCD_SendByte(0x38); // 8-bit, 2 line, 5x8 dots
	_delay_us(100);
	LCD_SendByte(0x0C); // Display ON, cursor OFF
	_delay_us(100);
	LCD_SendByte(0x06); // Increment, no shift
	_delay_us(100);
	LCD_Clear();
}

void LCD_Cmd(uint8_t cmd) {
	PORTD &= ~(1 << LCD_RS);
	LCD_SendByte(cmd);
}

void LCD_WriteChar(uint8_t data) {
	PORTD |= (1 << LCD_RS);
	LCD_SendByte(data);
}

void LCD_WriteString(const char *str) {
	while(*str) {
		LCD_WriteChar(*str++);
	}
}

void LCD_SetCursor(uint8_t x, uint8_t y) {
	uint8_t address = (y == 0) ? 0x80 + x : 0xC0 + x;
	LCD_Cmd(address);
}

void LCD_Clear(void) {
	LCD_Cmd(0x01);
	_delay_ms(5);
}