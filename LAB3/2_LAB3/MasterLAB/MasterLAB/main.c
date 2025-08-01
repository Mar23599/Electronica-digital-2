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

#define SS_eneble 0
#define  SS_disable 1

#include "Ale-lib-SPI.h"
#include "Ale-lib-UART.h"

volatile uint8_t contador = 0;
volatile uint8_t POTA = 0;
volatile uint8_t POTB = 0;

volatile char FLAG; 

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
		
		
		switch (FLAG){
			
			case 'L': 
			contador = UART_recByte(); // Convertir el contador de ascci a Uint8_t
			print_contador_master(contador); // Imprimie el contador
			SPI_SS_control(SS_eneble);
			SPI_Write(contador);
			SPI_Read();
			SPI_SS_control(SS_disable);
			break;
			
			case 'P':
			
			SPI_SS_control(SS_eneble);
			SPI_Write('A');
			SPI_Read(); // Espera para enviar dato
			SPI_SS_control(SS_disable); // Enviar A y esperar a recibir un dato
			SPI_Read();
			UART_SendString("POTA: ");
			UART_SendChar(SPDR);
			UART_SendString("\n");
			
			SPI_SS_control(SS_eneble);
			SPI_Write('B');
			SPI_Read(); // Esperar para enviar el dato
			SPI_SS_control(SS_disable); // Enviar B y esperar a recibir un dato
			SPI_Read(); 
			
			UART_SendString("POTB: ");
			UART_SendChar(SPDR);
			UART_SendString("/n");
			
			
			
		}
		
		
		
		
		
		
		
	}
}

/****************************************/
// NON-Interrupt subroutines


void setup(){
	
	cli();
	
	//Inicializacion de SPI
	SPI_init(MS_OSC_DIV128, DATA_ORDER_MSB, CLK_LOW, CLK_FIRST_EDGE); 
	
	
	
	//Inicializacion UART
	UART_init(0); 
	
	//Inicializacion de pines de salida
	
	DDRD |= (1 << PORTD3)|(1 << PORTD4)|(1 << PORTD5)|(1 << PORTD6)|(1 << PORTD7);
	DDRC |= (1 << PORTC0)|(1 << PORTC1)|(1 << PORTC2);
	
	
	sei();
	
}


void print_contador_master(uint8_t counter){
	
	/*
	MASTER PINOUT

	LSB

	PD3
	PD4
	PD5
	PD6
	PD7
	PC0
	PC1
	PC2

	MSB
	*/
	
	
	if (counter & 0b00000001)
	{
		PORTD |= (1 << PORTD3);
	} else {
		
		PORTD &= ~(1 << PORTD3);
	}
	
	if (counter & 0b00000010)
	{
		PORTD |= (1 << PORTD4);
		} else {
		
		PORTD &= ~(1 << PORTD4);
	}
	
	if (counter & 0b00000100)
	{
		PORTD |= (1 << PORTD5);
		} else {
		
		PORTD &= ~(1 << PORTD5);
	}
	
	if (counter & 0b00001000)
	{
		PORTD |= (1 << PORTD6);
		} else {
		
		PORTD &= ~(1 << PORTD6);
	}
	
	if (counter & 0b00010000)
	{
		PORTD |= (1 << PORTD7);
		} else {
		
		PORTD &= ~(1 << PORTD7);
	}
	
	if (counter & 0b00100000)
	{
		PORTC |= (1 << PORTC0);
		} else {
		
		PORTC &= ~(1 << PORTC0);
	}
	
	if (counter & 0b01000000)
	{
		PORTC |= (1 << PORTC1);
		} else {
		
		PORTC &= ~(1 << PORTC1);
	}
	
	if (counter & 0b10000000)
	{
		PORTC |= (1 << PORTC2);
		} else {
		
		PORTC &= ~(1 << PORTC2);
	}
	
	
	
}

char UART_ReChar(void){
	
	
	while (	!(UCSR0A & (1 << RXC0))	); //Esperar a que exista un dato dispoible
	return UDR0;
	
}

uint8_t UART_recByte(){
	
	char str[4] = {0};
	for( uint8_t i = 0; i < 3; i++){
		str[i] = UART_ReChar();
	}
	
	return (uint8_t)atoi(str);
}

/****************************************/
// Interrupt routines

ISR (USART_RX_vect){
	
	FLAG = UDR0;
	
}

