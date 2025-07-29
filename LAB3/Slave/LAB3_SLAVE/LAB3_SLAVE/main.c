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


char data_S = 'a';

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
		
		
		
	}
}

/****************************************/
// NON-Interrupt subroutines


void setup(){
	
	cli();
	UART_init();
	SPI_init(SLAVE_SS, DATA_ORDER_MSB, CLK_LOW, CLK_FIRST_EDGE);
	
	SPCR |= (1 << SPIE); // habilitar interrupcion SPI
	
	sei();
}

/****************************************/
// Interrupt routines


ISR (SPI_STC_vect){
	
	uint8_t SPI_valor = SPDR;
	
	if (SPI_valor == 'c')
	{
		SPI_Write(data_S);
	}
	
}