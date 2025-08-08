/*
 * I2C_LIB.h
 *
 * Created: 7/08/2025 09:35:04
 *  Author: aleja
 */ 


#ifndef I2C_LIB_H_
#define I2C_LIB_H_



#define F_CPU 16000000


#include <avr/io.h>
#include <stdint.h>

//Función para inicializar maestro
void I2C_Master_init(unsigned long SCL_Clock, uint8_t Prescaler);

// Funcion de inicio de comunicacion I2C
 void I2C_Master_Start(void);
 
//Funcion de parada de la comunicacion I2C
void I2C_Master_Stop(void);

/*Funcion de transmición de datos: Maestro -> esclavo
Si devuelve 0 si el esclavo a recibido el dato
*/
uint8_t I2C_Master_Write(uint8_t dato);

//Funcion de recepcion de datos enviados: Esclavo -> maestro
//Lectura de datos en el esclavo

uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack);

//Función de inicialización como Esclavo

void I2C_Slave_init(uint8_t address);


#endif /* I2C_LIB_H_ */