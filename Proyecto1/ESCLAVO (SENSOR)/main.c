/*
 * ESCLAVO (SENSOR).c
 *
 * Created: 8/08/2025 12:29:50
 * Author : Luis Sactic
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "I2C/I2C.h"

#define DHT11_PIN PD6
#define SLAVE_ADDRESS 0x40
#define STATUS_LED PB5

volatile int8_t temperature = 0;
volatile uint8_t status_code = 0;

void set_led(uint8_t state) {
    DDRB |= (1 << STATUS_LED);
    if(state) {
        PORTB |= (1 << STATUS_LED);
    } else {
        PORTB &= ~(1 << STATUS_LED);
    }
}

void blink_pattern(uint8_t code) {
    for(uint8_t i=0; i<code; i++) {
        set_led(1);
        _delay_ms(200);
        set_led(0);
        _delay_ms(200);
    }
}

int8_t read_dht11() {
    uint8_t data[5] = {0};
    uint16_t timeout;
    
    // Start signal
    DDRD |= (1 << DHT11_PIN);
    PORTD &= ~(1 << DHT11_PIN);
    _delay_ms(18);
    PORTD |= (1 << DHT11_PIN);
    _delay_us(40);
    DDRD &= ~(1 << DHT11_PIN);

    // Wait for response
    timeout = 1000;
    while((PIND & (1 << DHT11_PIN)) && (timeout > 0)) {
        timeout--;
        _delay_us(1);
    }
    if(timeout == 0) return -1;
    
    timeout = 1000;
    while(!(PIND & (1 << DHT11_PIN)) && (timeout > 0)) {
        timeout--;
        _delay_us(1);
    }
    if(timeout == 0) return -2;
    
    timeout = 1000;
    while((PIND & (1 << DHT11_PIN)) && (timeout > 0)) {
        timeout--;
        _delay_us(1);
    }
    if(timeout == 0) return -3;

    // Read 40 bits
    for(uint8_t i=0; i<5; i++) {
        for(uint8_t j=0; j<8; j++) {
            while(!(PIND & (1 << DHT11_PIN))) {
                // Wait for pin to go high
            }
            _delay_us(30);
            
            if(PIND & (1 << DHT11_PIN)) {
                data[i] |= (1 << (7-j));
                while(PIND & (1 << DHT11_PIN)) {
                    // Wait for pin to go low
                }
            }
        }
    }

    // Verify checksum
    if(data[4] != (data[0] + data[1] + data[2] + data[3])) {
        return -4;
    }
    
    return data[2]; // Return temperature
}

ISR(TWI_vect) {
    uint8_t status = TWSR & 0xF8;
    switch(status) {
        case 0x60: // SLA+W received
        case 0x80: // Data received
            TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);
            break;
            
        case 0xA8: // SLA+R received
            TWDR = temperature;
            TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);
            break;
            
        case 0xB8: // Data transmitted
            TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);
            break;
            
        default:
            TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);
            break;
    }
}

int main(void) {
    // Setup
    set_led(0);
    I2C_Slave_Init(SLAVE_ADDRESS);
    sei();
    
    while(1) {
        int8_t result = read_dht11();
        
        if(result >= 0) {
            temperature = result;
            status_code = 0;
            set_led(1);
            _delay_ms(50);
            set_led(0);
        } else {
            status_code = -result; // Store error code
            blink_pattern(status_code);
        }
        
        _delay_ms(2000);
    }
    
    return 0; // Never reached
}