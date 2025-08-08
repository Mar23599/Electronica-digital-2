/*
 * I2C_LIB.c
 *
 * Created: 7/08/2025 09:35:12
 *  Author: aleja
 */ 


#include "I2C_LIB.h"


//Función para inicializar maestro
void I2C_Master_init(unsigned long SCL_Clock, uint8_t Prescaler){
	
	/*PinOut
	PC4 -> 
	PC5 -> 
	*/
	
	DDRC &= ~(	(1 << PC4)|(1 << PC5)	); // Colocar pines I2C como entrada
	
	switch(Prescaler){
		
		case 1:
		TWSR &= ~(1 << TWPS0);
		TWSR &= ~(1 << TWPS1);
		break;	
		
		case 4:
		TWSR |= (1 << TWPS0);
		TWSR &= ~(1 << TWPS1);
		break;	
		
		case 16:
		TWSR &= ~(1 << TWPS0);
		TWSR |= (1 << TWPS1);
		break;
		
		case 64:
		TWSR |= (1 << TWPS0);
		TWSR |= (1 << TWPS1);
		break;
		
		default:
		TWSR &= ~(1 << TWPS0);
		TWSR &= ~(1 << TWPS1);
		Prescaler = 1; // Colocar prescaler de 1 en caso de que la eleccion sea diferente a las opciones
		break;
	}
	
	TWBR = ((F_CPU/SCL_Clock)-16)/(2*Prescaler);
	TWCR |= (1 << TWEN);
	
	
	
}

// Funcion de inicio de comunicacion I2C
void I2C_Master_Start(void){
	
	// uint8_t estado;
	// Limpiar bandera de interrupción-Inicio de Start-Verificar que el módulo este encendido
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN); 
	while(!(TWCR & (1 << TWINT))); // Esperar a que la bandera de interrupcion se apague para contiunar
	
	/*
	estado = TWSR & 0xF8; // Verificar estado
	
	if ((estado != 0x08) || (estado != 0x10))
	{
		return 1
	} else {
		return estado;
	}
	
	//Si se usa el estado, cambiar el tipo de dato correspondiente a la función
	*/
		
	
}

//Funcion de parada de la comunicacion I2C
void I2C_Master_Stop(void){

// Colocar activado-iniciar secuencia de interrupcion-iniciar secuencia de stop
	TWCR = (1 << TWEN)|(1 << TWINT)|(1 << TWSTO );
	
}

/*Funcion de transmición de datos: Maestro -> esclavo
Si devuelve 1 si el esclavo a recibido el dato
*/
uint8_t I2C_Master_Write(uint8_t dato){
	
	uint8_t estado;
	
	TWDR = dato; // Cargar el dato desde el registro
	TWCR = (1 << TWEN)|(1 << TWINT); //Iniciar envio
	
	while (!(TWCR & (1 << TWINT))); // Esperar al flag TWINT
	
	estado = TWSR & 0xF8; //Verificar estado
	
	if (estado == 0x18 || estado == 0x28 || estado == 0x40)
	{
		return 1;
	} else {
		return estado;
	}
	
}

//Funcion de recepcion de datos enviados: Esclavo -> maestro
//Lectura de datos en el esclavo

uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack){
	
	uint8_t estado;
	
	// ack = 1 para activiar secuencia de ack, ack = 0 no enviar secuencia de ack
	if (ack)
	{
		TWCR |= (1 << TWEA); // Lectura de ACK
	} else {
		TWCR &= ~(1 << TWEA); // Lectura sin ACK
	}
	
	TWCR |= (1 << TWINT); //Iniciar lectura
	
	while( !(TWCR & (1 << TWINT)) ); //Esperar al flag TWINT
	
	estado = TWSR & 0xF8; //Verificar estado
	
	//Verificar el dato leído con ACK o sin ACK
	
	if (estado == 0x58 || estado == 0x50)
	{
		*buffer = TWDR;
		return 1;
	} else {
		return estado;
	}
	
	
	
	
}

//Función de inicialización como Esclavo

void I2C_Slave_init(uint8_t address){
	
	/*PinOut
	PC4 -> 
	PC5 -> 
	*/
	
	DDRC &= ~(	(1 << PC4)|(1 << PC5)	); // Colocar pines I2C como entrada
	
	TWAR = address << 1; // Se asigna la dirreccion que tendra
	//TWAR = (address << 1| 0x01); //asignar dirrección que tendra, pero con general call
	
	// habilitar ACK automático, Atividar modulo I2c, Activar interrupciones
	TWCR = (1 << TWEA)|(1 << TWEN)|(1 << TWIE); 
	
}