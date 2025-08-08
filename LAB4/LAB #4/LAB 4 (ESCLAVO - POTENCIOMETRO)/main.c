/*
 * LAB 4 (ESCLAVO - POTENCIOMETRO).c
 *
 * 
 *
 */ 



#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "I2C/I2C.h"
#include "ADC/ADC.h"

#define SLAVE_ADDRESS 0x21

volatile uint16_t pot_value = 0;

void ADC_Init() {
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, prescaler 128
    ADMUX = (1 << REFS0); // AVcc como referencia
}

uint16_t ADC_Read(uint8_t channel) {
    ADMUX = (1 << REFS0) | (channel & 0x07); // Configurar canal
    ADCSRA |= (1 << ADSC); // Iniciar conversión
    while(ADCSRA & (1 << ADSC)); // Esperar conversión
    return ADC; // Retornar valor de 10 bits (0-1023)
}

ISR(TWI_vect) {
    switch(TWSR & 0xF8) {
        case 0xA8:  // SLA+R recibido
            TWDR = (pot_value >> 8); // Byte alto
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;
            
        case 0xB8:  // Byte alto enviado
            TWDR = (pot_value & 0xFF); // Byte bajo
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;
            
        default:
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;
    }
}

int main(void) {
    ADC_Init();
    
    // Inicializar I2C como esclavo
    TWAR = (SLAVE_ADDRESS << 1);
    TWCR = (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
    
    sei();  // Habilitar interrupciones globales
    
    while(1) {
        pot_value = ADC_Read(0); // Leer canal ADC0 (0-1023)
        
        // Filtro simple para valores erróneos
        if(pot_value > 1023) pot_value = 1023;
        
        _delay_ms(50);
    }
}