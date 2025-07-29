/*
 * NombreProgra.c
 *
 * Created: 
 * Author: 
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)


#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#include "Ale-lib-SPI.h"
#include "Ale-lib-UART.h"


uint8_t data_s;

/****************************************/
// Function prototypes

void setup();

/****************************************/
// Main Function

int main(void)
{
	
	setup();
	
	
	while (1)
	{
		PORTB &= ~(1 << PORTB2); //SS E = 1
		
		SPI_Write('c');
		data_s = SPI_Read();
		UART_SendString("\n");
		UART_SendChar(data_s);
		
		PORTB |= (1 << PORTB2); //SS E = 0
		
		_delay_ms(500);
		
		
	}
}

/****************************************/
// NON-Interrupt subroutines


void setup(){
	
	cli();
	
	SPI_init(MS_OSC_DIV16, DATA_ORDER_MSB, CLK_LOW, CLK_FIRST_EDGE);
	UART_init();
	
	sei();
	
}

/****************************************/
// Interrupt routines