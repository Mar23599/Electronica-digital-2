#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>      // Incluye definiciones de registros del AVR
#include <stdint.h>      // Para tipos de datos est�ndar (uint8_t, etc.)
#include <util/twi.h>    // Para constantes de estados TWI

#ifndef F_CPU 
#define F_CPU 16000000
#endif

// Definir tama�o del buffer si no est� definido (32 bytes por defecto)
#ifndef TWI_BUFFER_SIZE
#define TWI_BUFFER_SIZE 32
#endif


// Estados posibles para el modo esclavo
typedef enum {
	I2C_SLAVE_READY,    // Esclavo listo, sin comunicaci�n activa
	I2C_SLAVE_RX,       // Esclavo en modo recepci�n (recibiendo datos)
	I2C_SLAVE_TX        // Esclavo en modo transmisi�n (enviando datos)
} I2C_SlaveState_t;

// Estructura para el buffer circular de I2C
typedef struct {
	uint8_t data[TWI_BUFFER_SIZE];  // Almacenamiento de datos
	uint8_t index;                  // �ndice actual de lectura
	uint8_t length;                 // Cantidad de datos v�lidos en el buffer
} I2C_Buffer_t;

// ================= FUNCIONES MODO MAESTRO =================

// Inicializa el I2C en modo maestro con frecuencia espec�fica
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler);

// Inicia comunicaci�n con un dispositivo esclavo
// address: direcci�n del esclavo (7 bits)
// read: 0 para escritura, 1 para lectura
uint8_t I2C_Master_Start(uint8_t address, uint8_t read);

// Escribe un byte de datos al esclavo
uint8_t I2C_Master_Write(uint8_t data);

// Lee un byte del esclavo
// ack: 1 para enviar ACK (quiere m�s datos), 0 para NACK (�ltimo byte)
uint8_t I2C_Master_Read(uint8_t ack);

// Finaliza la comunicaci�n I2C
void I2C_Master_Stop(void);

// ================= FUNCIONES DE ALTO NIVEL =================

// Escribe un valor en un registro espec�fico de un dispositivo
uint8_t I2C_Write_Register(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

// Lee un registro espec�fico de un dispositivo
uint8_t I2C_Read_Register(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data);

// Lee m�ltiples registros consecutivos de un dispositivo
uint8_t I2C_Read_Multiple(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t length);

// ================= FUNCIONES MODO ESCLAVO =================

// Inicializa el I2C en modo esclavo con direcci�n espec�fica
void I2C_Slave_Init(uint8_t address);

// Cambia la direcci�n del esclavo en tiempo de ejecuci�n
void I2C_Slave_SetAddress(uint8_t address);

// Lee un byte del buffer de recepci�n
uint8_t I2C_Slave_Read(uint8_t *data);

// Escribe un byte al buffer de transmisi�n
uint8_t I2C_Slave_Write(uint8_t data);

// Retorna cu�ntos bytes hay disponibles para leer
uint8_t I2C_Slave_Available(void);

// Limpia los buffers de recepci�n y transmisi�n
void I2C_Slave_FlushBuffer(void);

// Maneja las interrupciones I2C (debe llamarse desde ISR(TWI_vect))
void I2C_Slave_IRQHandler(void);

#endif