/*
 * Ale_lib_LCD8.h
 *
 * Created: 18/07/2025 06:38:43
 *  Author: aleja
 */ 


#ifndef ALE-LIB-LCD8_H_
#define ALE-LIB-LCD8_H_

// Liberia para manejo de pantalla LCD por medio de 8 bits.

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/delay.h>


// Funcion para enviar un comando
void LCD_command( uint8_t a); 

//Funcion para inicializar la pantalla LCD
void LCD_init8( void );

//Funcion para enviar un dato 
void LCD_PORT_data(uint8_t b);

//Funcion para enviar un string
void LCD_PORT_str(const char* str);

//Ubicar al cursor

void LCD_command_cursor( uint8_t fila, uint8_t columna);



#endif /* ALE-LIB-LCD8_H_ */