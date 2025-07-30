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


uint8_t data_r; // Dato recivido

/****************************************/
// Function prototypes

void setup();

/****************************************/
// Main Function

int main(void)
{
	
	setup();
	
	UART_SendString("Inicio \n");
	
	
	while (1)
	{
		
		SPI_SS_CONTROL(SS_ENEBLE);
		SPI_Write('A');
		UART_SendString("PRUEBA1");
		data_r = SPI_Read();
		UART_SendString("PRUEBA2");
		SPI_SS_CONTROL(SS_DISABLE);
		
		UART_SendString("Se recibio: ");
		UART_SendChar(data_r);
		UART_SendString("\n");
		
		
		
	}
}

/****************************************/
// NON-Interrupt subroutines


void setup(){
	
	cli();
	
	SPI_init(MS_OSC_DIV16, DATA_ORDER_MSB, CLK_LOW, CLK_FIRST_EDGE);
	SPCR |= (1 << SPIE); // HABILITAR INTERRUPCIONES DEL SPI
	UART_init();
	
	sei();
	
}

/****************************************/
// Interrupt routines


ISR(SPI_STC_vect){
	

	
	
}