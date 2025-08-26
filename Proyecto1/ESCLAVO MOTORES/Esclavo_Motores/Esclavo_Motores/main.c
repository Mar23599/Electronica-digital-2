/*
 * I2C Slave - Control de LEDs (PD5 y PB0)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "I2C.h"
#include "ServoLib.h"

#define I2C_SLAVE_ADDRESS 0x41  // Dirección de 7 bits del esclavo

// Variables globales
volatile uint8_t received_command = 0;
volatile uint8_t command_received_flag = 0;

// Prototipos
void setup(void);
void process_command(uint8_t command);

//------------------------------------------------------
// Main
//------------------------------------------------------
int main(void)
{
    setup();

    while (1)
    {
		
		  
        if (command_received_flag)
        {
            // Procesar el comando recibido
            process_command(received_command);

            // Limpiar flag
            command_received_flag = 0;

            // Rehabilitar la interfaz I2C
            TWCR |= (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
        }
    }
}

//------------------------------------------------------
// Inicialización
//------------------------------------------------------
void setup(void)
{
    cli();
	I2C_Slave_Init(I2C_SLAVE_ADDRESS);

    // Configurar PD5 y PB0 como salidas
    DDRD |= (1 << DDD5);
    DDRB |= (1 << DDB0);

    // Inicialmente apagados
    PORTD &= ~(1 << PORTD5);
    PORTB &= ~(1 << PORTB0);
	
	servo_init(); //Inializar servo

    sei();
}

//------------------------------------------------------
// Procesar comandos recibidos
//------------------------------------------------------
void process_command(uint8_t command)
{
	
	
    switch (command)
    {
        case 'A':  // Encender PD5
            PORTD |= (1 << PORTD5);
            break;

        case 'B':  // Apagar PD5
            PORTD &= ~(1 << PORTD5);
            break;

        case 'C':  // Abrir puerta con el servo
            PORTB |= (1 << PORTB0);
			servo_175_grados();
			
            break;

        case 'D':  // Cerrar puerta con el Servo
            PORTB &= ~(1 << PORTB0);
			servo_0_grados();
            break;

        default:
            // Comando no reconocido - ignorar
            break;
    }
}

//------------------------------------------------------
// ISR de I2C (TWI)
//------------------------------------------------------
ISR(TWI_vect)
{
    uint8_t status = TWSR & 0xF8;

    switch (status)
    {
        case 0x60:  // SLA+W recibido, ACK enviado
        case 0x68:  // SLA+W recibido tras arbitraje perdido
        case 0x70:  // Llamada general recibida, ACK enviado
        case 0x78:
            // Listo para recibir datos
            TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
            break;

        case 0x80:  // Dato recibido, ACK enviado
        case 0x90:
            received_command = TWDR;
            command_received_flag = 1;
            TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
            break;

        case 0x88:  // Dato recibido, NACK enviado
        case 0x98:
            received_command = TWDR;
            command_received_flag = 1;
            break;

        case 0xA0:  // STOP o REPEATED START recibido
            TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
            break;

        default:    // Estado no manejado ? resetear interfaz
            TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
            break;
    }
}
