/*
 * LAB 4 (ESCLAVO).c
 *
 * 
 */ 



#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "I2C/I2C.h"

#define SLAVE_ADDRESS 0x20

// Definir pines para botones y LEDs
#define BUTTON_DEC PC2
#define BUTTON_INC PC3
#define LED0 PD2
#define LED1 PD3
#define LED2 PD4
#define LED3 PD5

volatile uint8_t counter = 0;
volatile uint8_t display_value = 0; // Valor que se envía al maestro (0-15)

void init_ports(void) {
    // Configurar botones como entradas con pull-up
    DDRC &= ~((1 << BUTTON_INC) | (1 << BUTTON_DEC));
    PORTC |= (1 << BUTTON_INC) | (1 << BUTTON_DEC);
    
    // Configurar LEDs como salidas
    DDRD |= (1 << LED0) | (1 << LED1) | (1 << LED2) | (1 << LED3);
    PORTD &= ~((1 << LED0) | (1 << LED1) | (1 << LED2) | (1 << LED3));
}

void update_leds(void) {
    // Mostrar solo los 4 bits menos significativos
    PORTD = (PORTD & 0xC3) | ((display_value & 0x0F) << 2);
}

uint8_t read_buttons(void) {
    return (~PINC) & ((1 << BUTTON_INC) | (1 << BUTTON_DEC));
}

ISR(TWI_vect) {
    switch(TWSR & 0xF8) {
        case 0x60:  // SLA+W recibido
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;
            
        case 0x80:  // Dato recibido
            counter = TWDR;
            display_value = counter & 0x0F; // Mantener entre 0-15
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;
            
        case 0xA8:  // SLA+R recibido
            TWDR = display_value; // Enviar siempre valor entre 0-15
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;
            
        default:
            TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
            break;
    }
}

int main(void) {
    init_ports();
    
    // Inicializar I2C como esclavo
    TWAR = (SLAVE_ADDRESS << 1);
    TWCR = (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
    
    sei();  // Habilitar interrupciones globales
    
    while(1) {
        uint8_t buttons = read_buttons();
        
        if(buttons & (1 << BUTTON_INC)) {
            counter++;
            display_value = counter & 0x0F; // Forzar a 4 bits
            while(read_buttons() & (1 << BUTTON_INC));
            _delay_ms(50);
            update_leds();
        }
        
        if(buttons & (1 << BUTTON_DEC)) {
            counter--;
            display_value = counter & 0x0F; // Forzar a 4 bits
            while(read_buttons() & (1 << BUTTON_DEC));
            _delay_ms(50);
            update_leds();
        }
    }
}