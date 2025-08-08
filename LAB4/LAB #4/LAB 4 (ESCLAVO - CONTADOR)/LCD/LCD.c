/*
 * LCD.c
 *
 * Created: 18/07/2025 11:05:46
 *  Author: Luis Sactic
 */ 

#include "LCD.h"

void LCD_Init() {
	DDRD |= (1<<LCD_RS)|(1<<LCD_EN)|(1<<LCD_D0)|(1<<LCD_D1)|(1<<LCD_D2)|(1<<LCD_D3)|(1<<LCD_D4)|(1<<LCD_D5);
	DDRB |= (1<<LCD_D6)|(1<<LCD_D7);
	
	// Secuencia de inicialización para 8 bits
	LCD_Cmd(0x38); // Function set: 8-bit, 2 line, 5x8 dots
	_delay_us(100);
	LCD_Cmd(0x0C); // Display on, cursor off
	_delay_us(100);
	LCD_Cmd(0x06); // Entry mode: increment, no shift
	_delay_us(100);
	LCD_Clear();    // Clear display
	_delay_ms(5);
	
	/*
	// Secuencia de inicialización para 4 bits
	LCD_Cmd(0x02); // Modo 4 bits
	LCD_Cmd(0x28); // 2 líneas, 5x8 puntos
	LCD_Cmd(0x0C); // Display ON, cursor OFF
	LCD_Cmd(0x06); // Incremento automático
	LCD_Cmd(0x01); // Limpiar display
	_delay_ms(5);
	*/
}

void LCD_Cmd(uint8_t cmd) {
	PORTD = (PORTD & 0x03) | ((cmd & 0x3F) << 2);
	PORTB = (PORTB & 0xFC) | ((cmd >> 6) & 0x03);
	
	PORTD &= ~(1<<LCD_RS); // Modo comando
	PORTD |= (1<<LCD_EN);  // Enable high
	_delay_us(1);
	PORTD &= ~(1<<LCD_EN); // Enable low
	_delay_ms(2);
}

void LCD_Data(uint8_t data) {
	PORTD = (PORTD & 0x03) | ((data & 0x3F) << 2);
	PORTB = (PORTB & 0xFC) | ((data >> 6) & 0x03);
	
	PORTD |= (1<<LCD_RS);  // Modo datos
	PORTD |= (1<<LCD_EN);  // Enable high
	_delay_us(1);
	PORTD &= ~(1<<LCD_EN); // Enable low
	_delay_us(100);
}

void LCD_String(const char *str) {
	while(*str) {
		LCD_Data(*str++);
	}
}

void LCD_SetCursor(uint8_t x, uint8_t y) {
	uint8_t addr = (y == 0) ? 0x80 + x : 0xC0 + x;
	LCD_Cmd(addr);
}

void LCD_Clear() {
	LCD_Cmd(0x01);
	_delay_ms(5);
}