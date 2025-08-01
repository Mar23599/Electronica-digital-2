/*
 * Ale_lib_UART.h
 *
 * Created: 21/07/2025 22:24:55
 *  Author: aleja
 */ 


#ifndef ALE-LIB-UART_H_
#define ALE-LIB-UART_H_

#include <avr/io.h>
#include <stddef.h>  


//Funcion de inicializacion de UART. 9600 bau, sin pardiad, un bit de stop, 8 bits de comunicación. 
//IE = 1 -> Habilitar interrupciones de UART. IE = 0 -> NO habilitar interrupciones

void UART_init(uint8_t IE); 

//Funcion para enviar un caracter
void UART_SendChar(char c);

//Funcion para enviar una cadena de caracteres
void UART_SendString(const char *str);



#endif /* ALE-LIB-UART_H_ */