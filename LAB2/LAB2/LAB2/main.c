/*
 * LAB2.c
 *
 * Created: 17/07/2025 22:44:26
 * Author : aleja
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <stdio.h>
#include "Ale-lib-UART.h"

#define F_CPU 16000000

uint8_t adc_read_A = 0;
uint8_t adc_read_B = 0;

volatile uint8_t adc_read_C = 0;

float voltajeA;
float voltajeB;
float voltajeC;

char bufferA[5];
char bufferB[5];
char bufferC[5];




/****************************************/
// Function prototypes

void setup();
void LCD_voltage_PRINT( uint8_t adc_read_val, uint8_t fila, uint8_t col); 
void float_to_string(uint8_t valor, char* buffer);



/****************************************/
// Main Function

int main(void)
{
	
	setup();
	
	LCD_command_cursor(0,1); // Colocar cursor en 0,0
	LCD_PORT_str("S1"); // Escribir
	LCD_command_cursor(1,0); // Colocar cursor en 1, 0
	LCD_PORT_str("0.00"); 
	
	LCD_command_cursor(0,6); // Colocar cursor en 0,7
	LCD_PORT_str("S2"); // Escribir
	LCD_command_cursor(1,5); // Colocar cursor en 1, 6
	LCD_PORT_str("0.00");
	
	LCD_command_cursor(0,11); // Colocar cursor en 0,11
	LCD_PORT_str("S3"); // Escribir
	LCD_command_cursor(1,10); // Colocar cursor en 1, 10
	LCD_PORT_str("0.00");
	
	LCD_command_cursor(1,15); // Colocar cursor en 1, 10
	LCD_PORT_str("V");
	
	
	
	
	
	while (1)
	{
		// Lectura de ADC	
		adc_read_A = ADC_read(6);
		adc_read_B = ADC_read(7);
		
		
		
		
		
		// Imprimir en LCD
		LCD_voltage_PRINT( adc_read_B, 1, 0); 
		LCD_voltage_PRINT( adc_read_A, 1, 5);
		LCD_voltage_PRINT( adc_read_C, 1, 10);
		
		//Envio por medio de monitor serial
		
		//S1
		UART_SendString("S1: ");
		
		float_to_string(adc_read_B, bufferB);
		UART_SendString(bufferB);
		
		UART_SendString("\n");
		
		// S2
		UART_SendString("S2: ");
		
		float_to_string(adc_read_A, bufferA);
		UART_SendString(bufferA);
		
		UART_SendString("\n");
		
		//S3
		
		UART_SendString("S3: ");
		
		float_to_string(adc_read_C, bufferC);
		UART_SendString(bufferC);
		
		UART_SendString("\n");
		
		
		UART_SendString("\n");
		
		
		_delay_ms(100);
		
		
		
		
		

	}
}

/****************************************/
// NON-Interrupt subroutines


void setup(){
	
	cli();
	
	UART_init(1); // Inicializacion de UART con interrupciones habilitadas
	ADC_init(); // Inicializacion de ADC
	LCD_init8(); // Inicializacion de pantalla LCD
	
	
	sei();
}



void LCD_voltage_PRINT(uint8_t adc_read_val, uint8_t fila, uint8_t col) {
	float voltaje = ((float)adc_read_val / 255.0) * 5.0; 

	uint8_t entero = (uint8_t)voltaje;
	uint8_t decimal = (uint8_t)((voltaje - entero) * 100);

	char buffer[6]; // x.xx + '\0'
	snprintf(buffer, sizeof(buffer), "%d.%02d", entero, decimal);

	LCD_command_cursor(fila, col);
	LCD_PORT_str(buffer);
}

void float_to_string(uint8_t valor, char* buffer){
	
	
	float voltaje = (valor * 5.0f)/255.0f ; // Mapeo del valor a float 0-255 a 0.00-5.00
	
	//Limitar maximo y mínimo
	if (voltaje > 5.0f) voltaje = 5.0f;   
	if (voltaje < 0.0f) voltaje = 0.0f;
	
	//Inicio de conversion
	
	uint16_t temp = (uint16_t)(voltaje * 100);
	
	uint8_t entero = temp/100; // obtener entero
	uint8_t Fdecimal = (temp/10) % 10; // Primer deciaml
	uint8_t Sdecimal = temp % 10; // Segundo deciaml
	
	
	buffer[0] = '0' + entero;
	buffer[1] = '.';
	buffer[2] = '0' + Fdecimal; // Primer decimal
	buffer[3] = '0' + Sdecimal; // Segundo deciaml
	buffer[4] = '\0';
	
}



/****************************************/
// Interrupt routines


ISR(USART_RX_vect) {
	char recibido = UDR0; // Leer el dato recibido

	if (recibido == '+') {
		if (adc_read_C < 255) adc_read_C++; // Incrementar solo si no está en el máximo
	}
	else if (recibido == '-') {
		if (adc_read_C > 0) adc_read_C--;   // Decrementar solo si no está en el mínimo
	}
}
