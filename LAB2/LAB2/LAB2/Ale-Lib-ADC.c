/*
 * Ale_Lib_ADC.c
 *
 * Created: 17/07/2025 22:49:53
 *  Author: aleja
 */ 


#include "Ale-Lib-ADC.h"

void ADC_init(void){
	
	ADMUX |= (1 << REFS0); // 5v como referecnia
	ADMUX |= (1 << ADLAR); // Justifiacion a la izquierda
	
	ADCSRA |= (1 << ADEN); // Habilitar ADC
	ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0); // Configuracion de prescalador = 128
	
	
}

uint8_t ADC_read(uint8_t canal ){
	
	
	if (canal > 7)
	{
		canal = 7; // En caso de escojer un canal fuera de rango, colocar como predeterminado el 7.
	}
	
	ADMUX = (ADMUX & 0b11100000)|(canal & 0x07); // Seleccionar canal
	
	DIDR0 |= (1 << canal); // Desactivar buffer digital del canal seleccionado
	
	ADCSRA |= (1 << ADSC); // Iniciar conversion
	
	while ( ADCSRA & (1 << ADSC)); // Esperar a que termine la conversion
	
	return ADCH; // Retornar la medicion jusitificada a la izquierda. 
	
	
}