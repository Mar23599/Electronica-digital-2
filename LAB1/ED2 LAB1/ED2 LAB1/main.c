/*
 * NombreProgra.c
 *
 * 11/07/2025
 * Autor: Alejandro Martínez 23599
 * Description: Juego de carreras con contador de decadas de 4 bits. El pin out se puede encontrar en el TXX. 
 */
/****************************************/
// Encabezado (Libraries)

#include <avr/io.h>
#include <avr/interrupt.h>

#define PushB0 0
#define PushB1 1
#define PushB2 2



/****************************************/
// Function prototypes

void setup();

/****************************************/
// Main Function

int main(void)
{
	
	setup();
	/* Replace with your application code */
	while (1)
	{
	}
}

/****************************************/
// NON-Interrupt subroutines


void setup(){

	cli(); // Desabiliar interrupciones globales
	
	// ***********************************************************************************************
	//Configuración de pines de entrada
	
	//Puerto C con pull-up activado: PC0, PC1, PC2
	
	DDRC &= ~( (1 << PushB0)|(1 << PushB1)|(1 << PushB2) ); // Pines de PC declarados como entrada
	PORTC = (1 << PushB0)|(1 << PushB1)|(1 << PushB2);		//Activiar pull up
	
	//Configuración de pines de salida
	
	//Puerto D completo activado como salida. DP no se utiliza en esta versión. 
	
	DDRD = 0xFF; //Pines de PD declarados como salida
	
	
	//Puerto B para control de multiplexado: PB0, PB1, PB2
	
	DDRB = (1 << 0)| (1 << 1) | (1 << 2);
	
	// ***********************************************************************************************
	
	//Configuración de interrupciones
	
	//Interrupciones en el puerto C:
	
	PCICR = (1 << PCIE1) // Habilitar interrupciones del Puerto C
	PCMSK1 = (1 << PCINT8)|(1 << PCINT9)|(1 << PCINT10) // Mascara para habilitarinterrupciones en PC0, PC1 y PC2
	
	
	
	// ***********************************************************************************************
	
	
	
	

	sei(); // Habilitar interrupciones globales
}

/****************************************/
// Interrupt routines

//Interrupciones por el puerto C

ISR(){
	
	
	
	
}