/*
 * ADC.c
 *
 * Created: 18/07/2025 10:56:17
 *  Author: Luis Sactic
 */ 

#include "ADC.h"
#include <avr/io.h>

void ADC_Init() {
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX = (1 << REFS0); // AVcc como referencia, canal 0 por defecto
}

uint16_t ADC_Read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Cambiar solo el canal
	ADCSRA |= (1 << ADSC); // Iniciar conversión
	while(ADCSRA & (1 << ADSC)); // Esperar fin de conversión
	return ADC; // Retornar valor de 10 bits
}