/*
 * Ale_lib_LCD8.c
 *
 * Created: 18/07/2025 06:37:44
 *  Author: aleja
 */ 

/*
PINOUT
PD0-PD3: PC0 a PC3
PD4-PD7: PB0-PB3

RS:PB4
RW:GND
E: PB5

VSS: GND
VDD: 5V
VEE: POT

K: GND
A: 5V

*/

#include "Ale-lib-LCD8.h"

void LCD_command( uint8_t a){
	
	
	uint8_t aH, aL; // aHIGH y a LOW: variables que guardan bit alto y bajo del parametro de entrada.
	
	// RS = 0: Inidicar que se envia un comando
	PORTB &= ~(1 << PORTB4); 
	
	//Mascaras para dividir a en aHIGH y aLOW
	aL = 0x0F & a; // Mascara para conservar los 4 bits menos significativos en aL. aH se deberia ver como: [0,0,0,0,a3,a2,a1,a0]
	aH = (a >> 4) & 0x0F ; // Mascara para conservar los 4 bits mas significativos. Se mueven a la izquierda para que aH sea: [0,0,0,0,a7,a6,a5,a4]
	
	//Enviar el comando a PD0-PD7 en la LCD
	PORTC = (0xF0 & PORTC) | aL; // Conservar bits mas significativos de PORTC. Alterar los bits menos significativos con aL.
	PORTB = (0xF0 & PORTB) | aH; // Conservar bits mas significativos de PORTB. Alterar los bits menos significativos con aH.
	
	
	// Pulso para habilitar el envio del comando
	PORTB |= (1 << PORTB5); // E = 1
	_delay_ms(1);
	PORTB &= ~(1 << PORTB5); // E = 0
	_delay_ms(2);
	
	
}

void LCD_init8(void){
	
	//Declaracion de pines de salida utilizados
	
	DDRB |= (1 << PORTB0)|(1 << PORTB1)|(1 << PORTB2)|(1 << PORTB3); //Pines que se conectan a PD4-PD7
	DDRC |= (1 << PORTC0)|(1 << PORTC1)|(1 << PORTC2)|(1 << PORTC3); //Pines que se conectan a PD0-PD3
	
	DDRB |= (1 << PORTB4)|(1 << PORTB5); // RS4 = PB4 ; E = PB5
	
	
	// Procedimiento de inicializacion
	
	_delay_ms(20); // Espera inicial. LCD arrancando.
	
	LCD_command(0b00111000); // Funcion SET: 8bits. 2 lineas. Arreglos 5x8
	_delay_ms(1);
	LCD_command(0b00001100); // Display ON. CURSOR OFF. Parpadeo OFF.
	_delay_ms(1);
	LCD_command(0b00000110); // 1/D = 1 incremento. Sin desplazamiento.
	_delay_ms(1);
	LCD_command(0b00000001); // Limpiar pantalla
	_delay_ms(5);
	
	
}

void LCD_PORT_data(uint8_t b){
	
	
	uint8_t bH, bL; // bHIGH y bLOW
	
	bL = 0X0F & b; // bL guarda los bits menos significativos de b. bL = [0,0,0,0,D3,D2,D1,D0]
	bH = (b >> 4) & 0x0F; // bH guarda los bits mas significativos de b. bH =[0,0,0,0,D7,D6,D5,D4]
	
	PORTB |= (1 << PORTB4); // RS = 1 -> Enviar un dato
	
	PORTC = (PORTC & 0xF0)|bL; //Enviar bit bajo
	PORTB = (PORTB & 0xF0)|bH; // Enviar bit alto
	
	
	// Pulso para habilitar el envio del comando
	PORTB |= (1 << PORTB5); // E = 1
	_delay_ms(2);
	PORTB &= ~(1 << PORTB5); // E = 0
	_delay_ms(2);
	
}

void LCD_PORT_str(const char* str){
	
	// Repetir la instruccion LCD_PORT_data por cada caracter del string
	while (*str)
	{
		LCD_PORT_data(*str++);
	}
	
}

void LCD_command_cursor( uint8_t fila, uint8_t columna){
	
	uint8_t dir;
	
	if ( fila == 0)
	{
		dir = 0x00 + columna; // fila 1
	}
	
	else if (fila == 1)
	{
		dir = 0x40 + columna; // Fila 2
	}
	
	else
	{
		dir = 0x00; // Si la fila es inválida -> por defecto se coloca el cursor en (0,0)
	}
	
	LCD_command( 0x80 | dir); // Enviar comando para mover el cursor. Mascara para setear el cursor
	
}

