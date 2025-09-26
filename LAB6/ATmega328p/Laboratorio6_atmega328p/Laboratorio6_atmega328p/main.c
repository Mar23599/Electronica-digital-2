/*
 * NombreProgra.c
 *
 * Created: 
 * Author: 
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)

#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART.h"

volatile uint8_t debounce_flag = 0; // Tiempo de espera para antirebote en ms

volatile uint8_t prev_estado = 0b00111111; //EL estado inicial es todos encendidos por el pull up

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
	//Inicializar botones de entrada
	
	DDRC &= ~((1 << PC0)|(1 << PC1)|(1 << PC2)|(1 << PC3)|(1 << PC4)|(1 << PC5)); 
	PORTC |= ((1 << PC0)|(1 << PC1)|(1 << PC2)|(1 << PC3)|(1 << PC4)|(1 << PC5)); // Colocar pull-up
	
	// Habilitar interrupciones y configurar mascaras
	 PCICR |= (1 << PCIE1); //habilitar interrupciones por pin change en puerto C
	 
	  PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) | (1 << PCINT11) | (1 << PCINT12) | (1 << PCINT13); // Mascaras para interrupción en cada pin
	
	//inicializacion de comunicación serial
	
	UART_init(0); // Desabilitar interrupciones por recepción
	
	// Configuración del timer0 para interrupcion de 1ms. (ANTIREBOTE)
	
	TCCR0A |= (1 << WGM01); // Activar modo CTC
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler de 64 -> Freq = 1Khz
	OCR0A = 249; // (16Mhz / 64 * 1k) -1 = 249 para interrumpir en 1ms
	TIMSK0 |= (1 << OCIE0A); // Habilitar interrupciones por comparación A
	
	
	sei(); 
	
}

/****************************************/
// Interrupt routines

ISR(PCINT1_vect){
	if (!debounce_flag)
	{
		uint8_t estado = PINC & 0b00111111;
		uint8_t cambios = prev_estado ^ estado; // prev_estado XOR estado: Determinar que bit cambio

		if ( (cambios & (1 << PC0)) && !(estado & (1 << PC0)) ) {
			UART_SendChar('A');
		}
		if ( (cambios & (1 << PC1)) && !(estado & (1 << PC1)) ) {
			UART_SendChar('B');
		}
		if ( (cambios & (1 << PC2)) && !(estado & (1 << PC2)) ) {
			UART_SendChar('C');
		}
		if ( (cambios & (1 << PC3)) && !(estado & (1 << PC3)) ) {
			UART_SendChar('D');
		}
		if ( (cambios & (1 << PC4)) && !(estado & (1 << PC4)) ) {
			UART_SendChar('E');
		}
		if ( (cambios & (1 << PC5)) && !(estado & (1 << PC5)) ) {
			UART_SendChar('F');
		}

		prev_estado = estado;   //  actualizar para la próxima interrupción
		debounce_flag = 20;     // 20ms de bloqueo
	}
}



ISR(TIMER0_COMPA_vect){
	
	if (debounce_flag)
	{
		debounce_flag--; // Decrementar cada 1ms 
	}
	
} 

