#include "I2C.h"


// ================= VARIABLES GLOBALES ESCLAVO =================

// Buffer para datos recibidos (vol�til porque se usa en ISR)
static volatile I2C_Buffer_t rx_buffer = {0};

// Buffer para datos a transmitir (vol�til porque se usa en ISR)
static volatile I2C_Buffer_t tx_buffer = {0};

// Estado actual del esclavo (vol�til porque se usa en ISR)
static volatile I2C_SlaveState_t slave_state = I2C_SLAVE_READY;

// Direcci�n actual del esclavo (vol�til porque se usa en ISR)
static volatile uint8_t slave_address = 0;

// ================= FUNCIONES MODO MAESTRO =================

void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler) {
	// Configurar pines SDA (PC4) y SCL (PC5) como entradas con pull-up
	DDRC &= ~((1 << DDC4) | (1 << DDC5));  // 0 para entrada
	PORTC &= ~(1 << PORTC4) | (1 << PORTC5); // Evitar pull-up

	// Configurar prescaler del I2C
	switch (Prescaler) {
		case 1:
		TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // 00 - Prescaler 1
		break;
		case 4:
		TWSR &= ~(1 << TWPS1);                  // 01 - Prescaler 4
		TWSR |= (1 << TWPS0);
		break;
		case 16:
		TWSR &= ~(1 << TWPS0);                  // 10 - Prescaler 16
		TWSR |= (1 << TWPS1);
		break;
		case 64:
		TWSR |= (1 << TWPS1) | (1 << TWPS0);    // 11 - Prescaler 64
		break;
		default:
		TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // Por defecto Prescaler 1
		Prescaler = 1;
		break;
	}
	
	// Calcular y establecer el bit rate (f�rmula del datasheet)
	// TWBR = (F_CPU/SCL_Clock - 16) / (2 * Prescaler)
	TWBR = (((F_CPU) / SCL_Clock) - 16) / (2 * Prescaler);
	
	// Habilitar el m�dulo TWI
	TWCR = (1 << TWEN);
}

uint8_t I2C_Master_Start(uint8_t address, uint8_t read) {
	uint8_t status;
	
	// Enviar condici�n de START
	// TWINT: Clear flag, TWSTA: Generar START, TWEN: Habilitar TWI
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	// Esperar a que se complete la operaci�n (TWINT se pone en 1)
	while (!(TWCR & (1 << TWINT)));
	
	// Verificar estado del bus
	status = TWSR & 0xF8; // M�scara para bits de estado
	if (status != TW_START && status != TW_REP_START) {
		return 0; // Error: No se gener� START correctamente
	}
	
	// Enviar direcci�n + operaci�n (lectura/escritura)
	// address << 1: Correr direcci�n a izquierda (bit 0 es R/W)
	// | read: 0 para escritura, 1 para lectura
	TWDR = (address << 1) | read;
	
	// Iniciar transmisi�n de la direcci�n
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	// Esperar a que se complete la operaci�n
	while (!(TWCR & (1 << TWINT)));
	
	// Verificar si el esclavo respondi� con ACK
	status = TWSR & 0xF8;
	if ((read && status != TW_MR_SLA_ACK) ||      // Para lectura
	(!read && status != TW_MT_SLA_ACK)) {     // Para escritura
		return 0; // Error: Esclavo no respondi�
	}
	
	return 1; // �xito
}

uint8_t I2C_Master_Write(uint8_t data) {
	// Cargar el dato a enviar en el registro de datos
	TWDR = data;
	
	// Iniciar transmisi�n
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	// Esperar a que se complete la operaci�n
	while (!(TWCR & (1 << TWINT)));
	
	// Verificar si el esclavo respondi� con ACK
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
		return 0; // Error: Esclavo no confirm� recepci�n
	}
	
	return 1; // �xito
}

uint8_t I2C_Master_Read(uint8_t ack) {
	if (ack) {
		// Leer y enviar ACK (quiero m�s datos)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		} else {
		// Leer y enviar NACK (�ltimo byte, no quiero m�s)
		TWCR = (1 << TWINT) | (1 << TWEN);
	}
	
	// Esperar a que se complete la operaci�n
	while (!(TWCR & (1 << TWINT)));
	
	// Devolver el dato le�do
	return TWDR;
}

void I2C_Master_Stop(void) {
	// Enviar condici�n de STOP
	// TWSTO: Generar STOP, TWINT: Clear flag, TWEN: Habilitar TWI
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	
	// Esperar a que se complete la condici�n de STOP
	// (TWSTO se limpia autom�ticamente cuando termina)
	while (TWCR & (1 << TWSTO));
}

// ================= FUNCIONES DE ALTO NIVEL =================

uint8_t I2C_Write_Register(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
	// Iniciar comunicaci�n en modo escritura
	if (!I2C_Master_Start(dev_addr, 0)) return 0;
	
	// Enviar direcci�n del registro a escribir
	if (!I2C_Master_Write(reg_addr)) return 0;
	
	// Enviar dato a escribir
	if (!I2C_Master_Write(data)) return 0;
	
	// Finalizar comunicaci�n
	I2C_Master_Stop();
	return 1; // �xito
}

uint8_t I2C_Read_Register(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data) {
	// Primero: modo escritura para especificar el registro a leer
	if (!I2C_Master_Start(dev_addr, 0)) return 0;
	
	// Enviar direcci�n del registro a leer
	if (!I2C_Master_Write(reg_addr)) return 0;
	
	// Segundo: reiniciar comunicaci�n en modo lectura
	if (!I2C_Master_Start(dev_addr, 1)) return 0;
	
	// Leer el dato (NACK porque es el �nico byte a leer)
	*data = I2C_Master_Read(0);
	
	// Finalizar comunicaci�n
	I2C_Master_Stop();
	return 1; // �xito
}

uint8_t I2C_Read_Multiple(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t length) {
	// Si no hay bytes que leer, retornar �xito
	if (length == 0) return 1;
	
	// Primero: modo escritura para especificar el registro inicial
	if (!I2C_Master_Start(dev_addr, 0)) return 0;
	
	// Enviar direcci�n del primer registro a leer
	if (!I2C_Master_Write(reg_addr)) return 0;
	
	// Segundo: reiniciar comunicaci�n en modo lectura
	if (!I2C_Master_Start(dev_addr, 1)) return 0;
	
	// Leer m�ltiples bytes
	for (uint8_t i = 0; i < length; i++) {
		if (i == length - 1) {
			// �ltimo byte: leer con NACK
			data[i] = I2C_Master_Read(0);
			} else {
			// Bytes intermedios: leer con ACK
			data[i] = I2C_Master_Read(1);
		}
	}
	
	// Finalizar comunicaci�n
	I2C_Master_Stop();
	return 1; // �xito
}

// ================= FUNCIONES MODO ESCLAVO =================

void I2C_Slave_Init(uint8_t address) {
	// Configurar pines SDA (PC4) y SCL (PC5) como entradas con pull-up
	DDRC &= ~((1 << DDC4) | (1 << DDC5));  // 0 para entrada
	PORTC |= (1 << PORTC4) | (1 << PORTC5); // 1 para pull-up
	
	// Establecer direcci�n y habilitar reconocimiento de llamada general
	// address << 1: Direcci�n en los 7 bits superiores
	// | 0x01: Habilitar respuesta a llamada general (0x00)
	TWAR = (address << 1) | 0x01;
	slave_address = address;
	
	// Inicializar buffers
	rx_buffer.index = 0;   // �ndice de lectura en 0
	rx_buffer.length = 0;  // Buffer vac�o
	tx_buffer.index = 0;   // �ndice de lectura en 0
	tx_buffer.length = 0;  // Buffer vac�o
	slave_state = I2C_SLAVE_READY; // Estado inicial
	
	// Habilitar TWI con interrupciones:
	// TWIE: Habilitar interrupci�n TWI
	// TWEA: Habilitar ACK
	// TWEN: Habilitar TWI
	// TWINT: Clear flag de interrupci�n
	TWCR = (1 << TWIE) | (1 << TWEA) | (1 << TWEN) | (1 << TWINT);
}

void I2C_Slave_SetAddress(uint8_t address) {
	slave_address = address;
	// Actualizar registro de direcci�n
	TWAR = (address << 1) | 0x01;
}

uint8_t I2C_Slave_Read(uint8_t *data) {
	// Verificar si hay datos disponibles
	if (rx_buffer.index >= rx_buffer.length) {
		return 0; // No hay datos
	}
	
	// Leer dato y avanzar �ndice
	*data = rx_buffer.data[rx_buffer.index++];
	return 1; // �xito
}

uint8_t I2C_Slave_Write(uint8_t data) {
	// Verificar si hay espacio en el buffer
	if (tx_buffer.length >= TWI_BUFFER_SIZE) {
		return 0; // Buffer lleno
	}
	
	// Escribir dato y aumentar contador
	tx_buffer.data[tx_buffer.length++] = data;
	return 1; // �xito
}

uint8_t I2C_Slave_Available(void) {
	// Calcular bytes disponibles para leer
	return rx_buffer.length - rx_buffer.index;
}

void I2C_Slave_FlushBuffer(void) {
	// Reiniciar buffers
	rx_buffer.index = 0;
	rx_buffer.length = 0;
	tx_buffer.index = 0;
	tx_buffer.length = 0;
}

// ================= MANEJADOR DE INTERRUPCIONES =================

void I2C_Slave_IRQHandler(void) {
	// Leer estado actual del TWI (bits 7:3 de TWSR)
	uint8_t status = TWSR & 0xF8;
	
	switch (status) {
		case TW_SR_SLA_ACK:           // Direcci�n propia recibida, modo escritura
		case TW_SR_GCALL_ACK:         // Llamada general recibida
		slave_state = I2C_SLAVE_RX; // Cambiar a modo recepci�n
		rx_buffer.index = 0;        // Reiniciar �ndice de lectura
		rx_buffer.length = 0;       // Reiniciar contador de datos
		// Confirmar interrupci�n y habilitar ACK para siguiente byte
		TWCR |= (1 << TWINT) | (1 << TWEA);
		break;
		
		case TW_SR_DATA_ACK:          // Datos recibidos, ACK enviado
		case TW_SR_GCALL_DATA_ACK:    // Datos de llamada general recibidos, ACK enviado
		// Si hay espacio en buffer, guardar dato
		if (rx_buffer.length < TWI_BUFFER_SIZE) {
			rx_buffer.data[rx_buffer.length++] = TWDR;
		}
		// Confirmar interrupci�n y habilitar ACK
		TWCR |= (1 << TWINT) | (1 << TWEA);
		break;
		
		case TW_SR_DATA_NACK:         // Datos recibidos, NACK enviado
		case TW_SR_GCALL_DATA_NACK:   // Datos de llamada general recibidos, NACK enviado
		// Confirmar interrupci�n y habilitar ACK
		TWCR |= (1 << TWINT) | (1 << TWEA);
		break;
		
		case TW_SR_STOP:              // Condici�n STOP o REPEATED START recibida
		// Confirmar interrupci�n y habilitar ACK
		TWCR |= (1 << TWINT) | (1 << TWEA);
		slave_state = I2C_SLAVE_READY; // Volver a estado listo
		break;
		
		case TW_ST_SLA_ACK:           // Direcci�n propia recibida, modo lectura
		slave_state = I2C_SLAVE_TX; // Cambiar a modo transmisi�n
		tx_buffer.index = 0;        // Reiniciar �ndice de transmisi�n
		// Caer a trav�s para enviar el primer byte inmediatamente
		
		case TW_ST_DATA_ACK:          // Datos transmitidos, ACK recibido
		// Si hay datos en buffer, enviar siguiente byte
		if (tx_buffer.index < tx_buffer.length) {
			TWDR = tx_buffer.data[tx_buffer.index++];
			} else {
			// Si no hay datos, enviar 0xFF
			TWDR = 0xFF;
		}
		// Confirmar interrupci�n y habilitar ACK
		TWCR |= (1 << TWINT) | (1 << TWEA);
		break;
		
		case TW_ST_DATA_NACK:         // Datos transmitidos, NACK recibido
		case TW_ST_LAST_DATA:         // �ltimo byte transmitido, ACK recibido
		// Confirmar interrupci�n y habilitar ACK
		TWCR |= (1 << TWINT) | (1 << TWEA);
		slave_state = I2C_SLAVE_READY; // Volver a estado listo
		break;
		
		case TW_BUS_ERROR:            // Error de bus
		// Recuperarse del error: resetear control register
		TWCR = 0;
		// Reconfigurar TWI con interrupciones
		TWCR = (1 << TWIE) | (1 << TWEA) | (1 << TWEN) | (1 << TWINT);
		slave_state = I2C_SLAVE_READY; // Volver a estado listo
		break;
		
		default:
		// Para cualquier otro estado no manejado espec�ficamente
		// Confirmar interrupci�n y habilitar ACK
		TWCR |= (1 << TWINT) | (1 << TWEA);
		break;
	}
}

