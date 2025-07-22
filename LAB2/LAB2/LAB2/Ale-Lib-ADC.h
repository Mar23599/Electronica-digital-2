/*
 * Ale_Lib_ADC.h
 *
 * Created: 17/07/2025 22:48:13
 *  Author: aleja
 */ 


#ifndef ALE-LIB-ADC_H_
#define ALE-LIB-ADC_H_

#include <avr/io.h>

void ADC_init (void); // Funcion para inicializar el ADC
uint8_t ADC_read(uint8_t canal ); // Funcion para utilziar un canal ADC 




#endif /* ALE-LIB-ADC_H_ */