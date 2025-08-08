/*
 * LCD.h
 *
 * Created: 18/07/2025 11:05:56
 *  Author: Luis Sactic
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>
#include <util/delay.h>

#define LCD_RS PD0
#define LCD_EN PD1
#define LCD_D0 PD2
#define LCD_D1 PD3
#define LCD_D2 PD4
#define LCD_D3 PD5
#define LCD_D4 PD6
#define LCD_D5 PD7
#define LCD_D6 PB0
#define LCD_D7 PB1

void LCD_Init();
void LCD_Cmd(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_String(const char *str);
void LCD_SetCursor(uint8_t x, uint8_t y);
void LCD_Clear();

#endif /* LCD_H_ */