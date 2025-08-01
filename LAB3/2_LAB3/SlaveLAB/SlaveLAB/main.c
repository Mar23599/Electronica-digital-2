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
#include <util/delay.h>
#include <stdint.h>

#include "Ale-lib-SPI.h"
#include "Ale-lib-UART.h"
#include "Ale-Lib-ADC.h"

volatile uint8_t contador = 0;

volatile uint8_t data_type = 0;

volatile uint8_t POTA = 0;
volatile uint8_t POTB = 0;



volatile union {
	
	uint8_t u8;
	int i;
	char c;
	
	} dato_recibido;

uint8_t UART_recByte(); 




/****************************************/
// Function prototypes

void setup();
void print_contador_master(uint8_t counter); 
char UART_ReChar(void);




/****************************************/
// Main Function

int main(void)
{
	
	setup();
	/* Replace with your application code */
	
	
	
	while (1)
	{
		
		POTA = ADC_read(6);
		POTB = ADC_read(7);
		
		
	}
}

/****************************************/
// NON-Interrupt subroutines


void setup(){
	
	cli();
	
	//Inicializacion de SPI
	SPI_init(SLAVE_SS, DATA_ORDER_MSB, CLK_LOW, CLK_FIRST_EDGE); 
	SPCR |= (1 << SPIE); // Habilitar interrupciones
	
	//Inicializacion ADC
	ADC_init();
	DDRC &= ~(1 << PC6);
	
	
	//Inicializacion de pines de salida
	
	DDRD |= (1 << PORTD2)|(1 << PORTD3)|(1 << PORTD4)|(1 << PORTD5)|(1 << PORTD6)|(1 << PORTD7);
	DDRC |= (1 << PORTC0)|(1 << PORTC1);
	
	UART_init();
	
	
	sei();
	
}


void print_contador_master(uint8_t counter){
	
	/*
	MASTER PINOUT

	ESCLAVO PINOUT

	LSB

	PD2
	PD3
	PD4
	PD5
	PD6
	PD7
	PC0
	PC1

	MSB
	*/
	
	
	if (counter & 0b00000001)
	{
		PORTD |= (1 << PORTD2);
	} else {
		
		PORTD &= ~(1 << PORTD2);
	}
	
	if (counter & 0b00000010)
	{
		PORTD |= (1 << PORTD3);
		} else {
		
		PORTD &= ~(1 << PORTD3);
	}
	
	if (counter & 0b00000100)
	{
		PORTD |= (1 << PORTD4);
		} else {
		
		PORTD &= ~(1 << PORTD4);
	}
	
	if (counter & 0b00001000)
	{
		PORTD |= (1 << PORTD5);
		} else {
		
		PORTD &= ~(1 << PORTD5);
	}
	
	if (counter & 0b00010000)
	{
		PORTD |= (1 << PORTD6);
		} else {
		
		PORTD &= ~(1 << PORTD6);
	}
	
	if (counter & 0b00100000)
	{
		PORTD |= (1 << PORTD7);
		} else {
		
		PORTD &= ~(1 << PORTD7);
	}
	
	if (counter & 0b01000000)
	{
		PORTC |= (1 << PORTC0);
		} else {
		
		PORTC &= ~(1 << PORTC0);
	}
	
	if (counter & 0b10000000)
	{
		PORTC |= (1 << PORTC1);
		} else {
		
		PORTC &= ~(1 << PORTC1);
	}
	
	
	
}




/****************************************/
// Interrupt routines



ISR(SPI_STC_vect) {
	// Rutina de interrupción por transferencia SPI completa
	
	char raw = SPDR;
	dato_recibido.c = raw; //Interpretacion del dato
	
	 if (data_type == 1) {
		 // Instrucciones para uint8_t
		 contador = SPDR; // Lee el byte recibido
		 print_contador_master(contador); // Actualiza el contador físico
		 
	 }
	 else if (data_type == 2) {
		 // Instrucciones para int
		contador = SPDR; // Lee el byte recibido
		 print_contador_master(contador); // Actualiza el contador físico
		 
	 }
	 else if (data_type == 3) {
		 // Instrucciones para char
		 if (raw == 'A')
		 {
			 SPI_Write(POTA);
			 SPI_Read();
		 } 
		 
		 else if (raw == 'B')
		 {
			 SPI_Write(POTB);
			 SPI_Read();
		 }
		 
	 }
	
	
	
}