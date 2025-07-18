/*
 * NombreProgra.c
 *
 * 11/07/2025
 * Autor: Alejandro Martínez 23599
 * Description: Juego de carreras con contador de decadas de 4 bits. El pin out se puede encontrar en el TXX. 
 */
/****************************************/
// Encabezado (Libraries)

#define PushB0 0
#define PushB1 1
#define PushB2 2
#define F_CPU 16000000UL  

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


uint8_t display_7segmentos_[16] ={
	
	0x3F, // 0
	0b00000110, // 1
	0x6D, // 2
	0x79, // 3
	0x66, // 4
	0x5B, // 5
	0x5F, // 6
	0x07, // 7
	0x7F, // 8
	0x77, // 9
	0x77, // A
	0x4F, // b
	0x39, // C
	0x5E, // d
	0x79, // E
	0x71  // F
};

	

uint8_t race_start_flag = 0;
	



/****************************************/
// Function prototypes

void setup();
void race_counter();

/****************************************/
// Main Function

int main(void)
{
	
	setup();
	
	
	
	

	
	while (1)
	{
		
		if ( race_start_flag == 1)
		{
			race_counter(); // Efecutar funcione de incio de carrera
			race_start_flag = 0; // apagar bandera de inicio de carrera
		}
		
		
	}
	
	
	
}

/****************************************/
// NON-Interrupt subroutines


void setup(){

	cli(); // Desabiliar interrupciones globales
	
	UCSR0B &= ~((1 << TXEN0) | (1 << RXEN0));// Desabilitar UART
	
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
	
	PCICR = (1 << PCIE1); // Habilitar interrupciones del Puerto C
	PCMSK1 = (1 << PCINT8)|(1 << PCINT9)|(1 << PCINT10); // Mascara para habilitarinterrupciones en PC0, PC1 y PC2
	
	
	
	// ***********************************************************************************************
	
	
	
	

	sei(); // Habilitar interrupciones globales
}


void race_counter(){
	
	// Arreglo que almacena contenido de display

	uint8_t display_7seg[16] = {
		
			0x3F, // 0
			0x06, // 1
			0b01011011, // 2
			0b01001111, // 3
			0x66, // 4
			0b01101101, // 5
			0x5F, // 6
			0x07, // 7
			0x7F, // 8
			0x77, // 9
			0x77, // A
			0x4F, // b
			0x39, // C
			0x5E, // d
			0x79, // E
			0x71  // F
		};
	
	PORTD =0x00;	
	PORTD = display_7seg[5]; // Imprimir 5
	_delay_ms(1000);
	
	PORTD =0x00;
	PORTD = display_7seg[4]; // Imprimir 4
	_delay_ms(1000);
	
	PORTD =0x00;
	PORTD = display_7seg[3]; // Imprimir 3
	_delay_ms(1000);
	
	PORTD =0x00;
	PORTD = display_7seg[2]; // Imprimir 2
	_delay_ms(1000);
	
	PORTD =0x00;
	PORTD = display_7seg[1]; // Imprimir 1
	_delay_ms(1000);
	
	PORTD =0x00;
	PORTD = display_7seg[0]; // Imprimir 0
	_delay_ms(1000);
	
	
}




/****************************************/
// Interrupt routines

//Interrupciones del puerto C


ISR(PCINT1_vect){
	
	if (	!(PINC & (1 << 0))	){
		
		race_start_flag = 1;
		
	}
	
	
}


