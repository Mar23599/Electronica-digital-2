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

uint8_t display_7seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

uint8_t sec_counter = 0; // contador de 0 a 20, para hacer un segundo
uint8_t time_counter = 0;  // contador de 0 a 5, para hacer la cuenta regresiva

uint8_t flag_game_start = 0; // Bandera para iniciar juego

uint8_t counter_playerA = 0;
uint8_t counter_playerB = 0;

uint8_t flag_winner = 0; // Si A gana, 1. Si B gana, 2




/****************************************/
// Function prototypes

void setup();
void PrintA(	uint8_t contadorA	);
void PrintB(	uint8_t contadorB	);
void winner(	uint8_t winner_flag	);

/****************************************/
// Main Function

int main(void)
{
	
	setup();
	/* Replace with your application code */
	while (1)
	{
		
		PrintA( counter_playerA );
		PrintB( counter_playerB );
		
		if ( flag_winner != 0)
		{
			cli();
			winner(flag_winner);
		}
		
		
	}
}

/****************************************/
// NON-Interrupt subroutines


void setup(){
	
	cli();
	
	//Configuración del PRESCALADOR del sistema
	
	CLKPR = (1 << CLKPCE); // Habilitar cambio de prescaldor
	CLKPR = (1 << CLKPS1); // Aplicar prescalador de 4 -> FR_CPU 4MHz
	
	
	//Salidas
	
	DDRD = 0xFF; // Todo puerto D sera un salida
	PORTD = 0xFF; // Puerto D inicia totalmente apagado
	
	DDRB |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3); // PB0, PB1, PB2 y PB3 configurados como salidas del jugador A
	PORTB &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Inicialmente apagados
	
	DDRC |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3); // PC0, PC1, PC2 y PC3 configurados como salidas del jugador B
	PORTC &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Inicialmente apagados
	
	
	//Entradas
	
	DDRC &= ~((1 << 4)|(1 << 5)); // COnfigurar PC4 y PC5 como entrada para jugador A y B
	PORTC |= (1 << 4)|(1 << 5); // Activar pull ups
	
	DDRB &= ~(1 << 4);// Configurar PB5 como entrada 
	PORTB |=(1 << 4); // Activar pull up
	
	//Interrupciones
	
	//interrupciones pin change en puerto C y mascara para PC4 y PC5. Puerto B PB5.
	
	PCICR |= (1 << PCIE1)|(1 << PCIE0); // Habilitar interrupciones por pin change en puerto C y B
	
	PCMSK1 |= (1 << PCINT12)|(1 << PCINT13); // Mascara para habilitar interrupciones en PC4 y PC5
	PCMSK0 |= (1 << PCINT4); // Mascara para habilitar interrupciones en PB5
	
	// Interrupciones de modo CTC del timer0 y configuracion del timer0
	
	
	TCCR0A |= (1 << WGM01); // Configurar timer en modo CTC
	TCCR0B |= (1 << CS02) | (1 << CS00); //Configurar prescalador de 1024
	OCR0A = 195; // Se busca interrumpir cada 5 ms
	
	TIMSK0 &= ~(1 << OCIE0A); // NO habilitar interrupciones por comparacion A, se habilitará despues
	
	
	
	
	
	
	
	sei();
}


void PrintA(	uint8_t contadorA	){
	
	switch (contadorA){
		
	case 1:
	PORTB &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Apagar todos los pines
	PORTB |= (1 << 0); //Encender el primer bit
	break;
	
	case 2:
	PORTB &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Apagar todos los pines
	PORTB |= (1 << 1); //Encender el segundo bit
	break;
	
	case 3:
	PORTB &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Apagar todos los pines
	PORTB |= (1 << 2); //Encender el tercer bit
	break;
	
	case 4:
	PORTB &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Apagar todos los pines
	PORTB |= (1 << 3); //Encender el cuarto bit
	
	flag_winner = 1;
	
	break;

		
	}
	
	
	
	
}

void PrintB(	uint8_t contadorB	){
	
	switch (contadorB){
		
	case 1:
	PORTC &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Apagar todos los pines
	PORTC |= (1 << 0); //Encender el primer bit
	break;
	
	case 2:
	PORTC &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Apagar todos los pines
	PORTC |= (1 << 1); //Encender el segundo bit
	break;
	
	case 3:
	PORTC &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Apagar todos los pines
	PORTC |= (1 << 2); //Encender el tercer bit
	break;
	
	case 4:
	PORTC &= ~((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)); // Apagar todos los pines
	PORTC |= (1 << 3); //Encender el cuarto bit
	
	flag_winner = 2;
	
	break;
		
		
	}
	
	
	
}

void winner(	uint8_t winner_flag		){
	
	
	if ( winner_flag == 1)
	{
		// Si gana jugador A
		PORTB |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3); // Encender los pines del ganador A
		PORTC &= ~(	(1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)	); //Apagar todos los pines del perdedor B
		PORTD = display_7seg[1];
		
	} 
	else if ( winner_flag == 2)
	{
		// Si gana jugador B
		PORTC |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3); // Encender los pines del ganador B
		PORTB &= ~(	(1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)	); //Apagar todos los pines del perdedor A
		PORTD = display_7seg [2];
	}
	
	
}



/****************************************/
// Interrupt routines

// Interrupciones pin change en puerto C

ISR (	PCINT1_vect	){
	
	if ( flag_game_start == 1)
	{
	
	
	
		if (	!(PINC & (1 << 4))	)
		{
		counter_playerA = counter_playerA + 1; // incrementar el contador de jugador A
		} 
	
		if (	!(PINC & (1 << 5))	)
		{
		counter_playerB = counter_playerB + 1; // Incrementar contador de jugador B
		}
	
	}
	
	
	}
	
	
// Interrupciones pin change en puerto B
ISR (	PCINT0_vect	){

	
	if (	!(PINB & (1 << 4))	)
	{	
		TIMSK0 |= (1 << OCIE0A); // habilitar interrupciones del timer 0
	}


}
	
	


ISR (TIMER0_COMPA_vect){
	
	sec_counter = sec_counter +1 ; // Cada 5 ms aumentar la cuenta de sec_counter
	
	
	if (sec_counter == 20) // Cuando se alcanze un segundo
	{
		time_counter = time_counter +1 ; // Cada segundo, incrementar la cuenta de time counter.
		sec_counter = 0; //Reiniciar la cuenta desde 0
	}
	
	switch (time_counter){
		
		
		case 1: PORTD = display_7seg[5];
		break;
		
		case 2: PORTD = display_7seg[4];
		break;
		case 3: PORTD = display_7seg[3];
		break;
		case 4: PORTD = display_7seg[2];
		break;
		case 5: PORTD = display_7seg[1];
		break;
		case 6: PORTD = display_7seg[0];
		
		//Desabilitar el timer0 y levantar bandera de inicio de juego
		
		flag_game_start = 1; // Iniciar el juego
		TIMSK0 &= ~(1 << OCIE0A); // Desahbilitar interrupciones del timer0
		
		break;
		
		
		
	}
	
	
}


