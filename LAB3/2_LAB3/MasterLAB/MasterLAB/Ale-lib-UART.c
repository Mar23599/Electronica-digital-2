/*
 * Ale_lib_UART.c
 *
 * Created: 21/07/2025 22:25:08
 *  Author: aleja
 */ 


#include "Ale-lib-UART.h"

void UART_init(uint8_t IE){
	
	UBRR0H = 0x00;
	UBRR0L = 0x67; // UBRR0 = 103 = 0x0067 para 9600 bauds
	
	//Limpiar registros de configuraciones previas
	
	UCSR0B = 0x00;
	UCSR0C = 0x00; 
	
	//Configurar modo asíncrono, 8 bits, 1 stop bit, sin pararidad
	
	UCSR0C &= ~( (1 << UMSEL01) | (1 << UMSEL00) ); // Configuracion de modo ASINCRONO
	
	UCSR0C &= ~(	(1 << UPM01)|(1 << UPM00)	); // Sin paridad
	
	UCSR0C &= ~(1 << USBS0); // 1 bit de stop
	
	
	UCSR0C |= (1 << UCSZ01)|(1 << UCSZ00); // 8 bits de datos
	UCSR0B |= (1 << TXEN0)|(1 << RXEN0); // Habilitar transmisor y receptor
	
	if ( IE == 1)
	{
		UCSR0B |= (1 << RXCIE0); // Si IE es 1 -> activar las interrupciones por UART
	} 
	
	
}



void UART_SendChar(char c){
	
	
	while (	!((UCSR0A) & (1 << UDRE0))	); // Esperara que el buffer esté vacio
	UDR0 = c; //Escribir el caracter a transmitir
	
}

void UART_SendString(const char *str) {
	if (str == NULL) return;  // Protección contra NULL
	
	while (*str != '\0') {
		UART_SendChar(*str++);
	}
}