/*
 * ServoLib.c
 *
 * Created: 26/08/2025 12:18:17
 *  Author: aleja
 */ 

#include "ServoLib.h"
void servo_init(void) {
	// Configurar PB1 (OC1A) como salida
	DDRB |= (1 << PB1);
	
	// Configurar Timer1 en modo PWM de 10 bits (Fast PWM)
	// TOP = ICR1, OC1A no invertido
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler 8
	
	// Configurar frecuencia de PWM para servos (50Hz)
	ICR1 = 39999; // Para 50Hz con prescaler 8 y F_CPU = 16MHz
	
	// Inicializar servo en posición neutra (90°)
	OCR1A = 3000; // Valor inicial aproximado para 90°
	_delay_ms(500); // Esperar a que el servo se estabilice
}

void servo_0_grados(void) {
	// Posición para 0° (aprox. 1ms pulso)
	OCR1A = 2000; // 2000 counts = 1ms
	_delay_ms(100); // Esperar a que el servo alcance la posición
}

void servo_175_grados(void) {
	// Posición para 175° (aprox. 2ms pulso)
	OCR1A = 4000; // 4000 counts = 2ms
	_delay_ms(100); // Esperar a que el servo alcance la posición
}