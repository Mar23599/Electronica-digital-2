/*
 * ADC.h
 *
 * Created: 18/07/2025 10:56:27
 *  Author: Luis Sactic
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

void ADC_Init();
uint16_t ADC_Read(uint8_t channel);

#endif /* ADC_H_ */