/*
 * NombreProgra.c
 *
 * Created: 
 * Author: 
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)

#define  F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

#include "I2C.h"
#include "LCD.h"
#include "Ale-lib-UART.h"


//--------------AHT10---------------------------

//Comandos

#define AHT10_INIT_CMD 0xE1 // Comando de inicializacion
#define AHT10_TRIGGER_MEASUREMENT_CMD 0xAC // Comando de inicio de proceso de medicion
#define AHT10_SOFT_RESET 0xBA // Comando para resetear el sensor suavemente. Limpia registros internos

// Dirreccion
#define AHT10_ADDRESS 0x38 // Dirección del dispositivo AHT10

// Estados del sensor
#define AHT10_STATUS_BUSY 0x80 // bit de busy (1 = ocupado, 0 = listo)
#define AHT10_STATUS_CALIB 0x08 // bit de calibracion (1 = calibrado, 0 = no calibrado)

//tiempo
#define AHT10_100_ms 100 // timeout para mediciones
#define AHT10_POWER_ON_DELAY 40 // tiempo despues de enceder le dispositivo

//Variables del AHT10

uint16_t current_temp_scaled = 0;
uint16_t current_hum_scaled = 0;


//--------------TSL2561---------------------------

// Dirección I2C del TSL2561
#define TSL2561_ADDRESS 0x39

// Registros del TSL2561
#define TSL2561_CMD          0x80
#define TSL2561_CMD_CLEAR    0x40
#define TSL2561_REG_CONTROL  0x00
#define TSL2561_REG_TIMING   0x01
#define TSL2561_REG_THRESHLOW 0x02
#define TSL2561_REG_THRESHHIGH 0x04
#define TSL2561_REG_INTERRUPT 0x06
#define TSL2561_REG_DATA0LOW  0x0C
#define TSL2561_REG_DATA0HIGH 0x0D
#define TSL2561_REG_DATA1LOW  0x0E
#define TSL2561_REG_DATA1HIGH 0x0F
#define TSL2561_REG_ID       0x0A

// Modos de ganancia e integración
#define TSL2561_GAIN_1X      0x00
#define TSL2561_GAIN_16X     0x10
#define TSL2561_INTEGRATION_13MS 0x00
#define TSL2561_INTEGRATION_101MS 0x01
#define TSL2561_INTEGRATION_402MS 0x02

// Variables globales
float lux_value = 0.0;
uint16_t ch0_value = 0;  // Canal 0 (full spectrum)
uint16_t ch1_value = 0;  // Canal 1 (IR)



/****************************************/
// Function prototypes

void setup();


//Funciones de AHT10
uint8_t AHT10_Init(void);
uint8_t AHT10_ReadRawData(uint32_t *raw_temp, uint32_t *raw_hum);
uint8_t AHT10_ReadMeasurements(uint16_t *temperature_scaled, uint16_t *humidity_scaled);
uint8_t AHT10_SoftReset(void);
uint8_t AHT10_GetStatus(uint8_t *status);
uint8_t AHT10_ReadStatusByte(uint8_t *status); 
void AHT10_Debug_ReadRaw(void);
void DisplayMeasurements(uint16_t temp_scaled, uint16_t hum_scaled);
void AHT10_OutString(void);

//Funciones de utilidad
void uint16_to_str(uint16_t value, char* buffer, uint8_t decimal_places);
void send_hex_byte(uint8_t byte);
void send_decimal(uint16_t value, uint8_t decimal_places);
void I2C_Scanner(void);
// Función auxiliar para mostrar 24 bits en hex
void send_hex_24bit(uint32_t value);

void debug_raw_values(void) {
	uint32_t raw_temp, raw_hum;
	
	if (AHT10_ReadRawData(&raw_temp, &raw_hum)) {
		UART_SendString("DEBUG - Raw Humidity: ");
		// Función para enviar número decimal de 32 bits
		send_uint32(raw_hum);
		UART_SendString("\n");
		
		UART_SendString("DEBUG - Raw Temperature: ");
		send_uint32(raw_temp);
		UART_SendString("\n");
		
		UART_SendString("DEBUG - Hex Humidity: 0x");
		send_hex_24bit(raw_hum);
		UART_SendString("\n");
		
		UART_SendString("DEBUG - Hex Temperature: 0x");
		send_hex_24bit(raw_temp);
		UART_SendString("\n");
	}
}

/****************************************/
// Main Function

int main(void)
{
	
	setup();
	debug_raw_values();
	
	
	while (1)
	{
		

		AHT10_OutString(); //LEctura de la humedad en formato Hxx.xH
        
	    
			
			
			_delay_ms(2000);
		
		   
	}
	
}
	

/****************************************/
// NON-Interrupt subroutines

//----------------------FUNCION SETUP--------------------------------
void setup(){
	
	cli();
	UART_init(1); // Inicializar comunicacion serial a 9600 baundrys con interrupciones de recepcion habilitadas
	UART_SendString("Comunicacion serial inicializada correctamente \n");
	
	I2C_Master_Init(100000, 1); // Inicializar I2C
	UART_SendString("Comunicacion I2C inicializada \n");
	

	// Revisando dispositivos I2C
	I2C_Scanner();
	
	//Inicializacion de AHT10
	if (AHT10_Init() == 1)
	{
		UART_SendString("Sensor AHT10 inicializado correctamente \n");
	} else {
		UART_SendString("Sensor AHT10: ERROR \n");
	}
	
	
	
	
	
	

	sei();
	
}

//--------------------------FUNCIONES TSL2561------------------------------------




//--------------------------FUNCIONES AHT10--------------------------------------

void AHT10_OutString(void){
	
	//Desplegar lectura de humedad
	if (AHT10_ReadMeasurements(&current_temp_scaled, &current_hum_scaled))
	{
		DisplayMeasurements(current_temp_scaled, current_hum_scaled);
	}
}

uint8_t AHT10_Init(void)
{
	uint8_t status;
	
	UART_SendString("INIT: Verificando estado inicial...\n");
	
	// PRIMERO: Leer el estado sin hacer reset
	if (!AHT10_GetStatus(&status)) {
		UART_SendString("INIT: ERROR LEYENDO ESTADO INICIAL\n");
		return 0;
	}
	
	UART_SendString("INIT: Estado inicial: 0x");
	send_hex_byte(status);
	UART_SendString("\n");
	
	// Si ya está calibrado, no necesitamos inicializar
	if (status & AHT10_STATUS_CALIB) {
		UART_SendString("INIT: SENSOR YA CALIBRADO - Saltando inicializacion\n");
		return 1;
	}
	
	UART_SendString("INIT: Sensor no calibrado, iniciando soft reset...\n");
	if (!AHT10_SoftReset())
	{
		UART_SendString("INIT: ERROR DE RESET\n");
		return 0;
	}
	
	_delay_ms(100); // Espera después del reset
	
	// Verificar si el reset solucionó el problema
	if (!AHT10_GetStatus(&status)) {
		UART_SendString("INIT: ERROR LEYENDO ESTADO POST-RESET\n");
		return 0;
	}
	
	UART_SendString("INIT: Estado post-reset: 0x");
	send_hex_byte(status);
	UART_SendString("\n");
	
	// Si ya está calibrado después del reset, perfecto
	if (status & AHT10_STATUS_CALIB) {
		UART_SendString("INIT: SENSOR CALIBRADO DESPUES DE RESET\n");
		return 1;
	}
	
	UART_SendString("INIT: Intentando comando de calibracion alternativo...\n");
	
	// ALTERNATIVA: Usar método directo de registro
	if (!I2C_Write_Register(AHT10_ADDRESS, 0xE1, 0x08)) {
		UART_SendString("INIT: ERROR ESCRIBIENDO REGISTRO 0xE1\n");
		return 0;
	}
	
	_delay_ms(10);
	
	// Segundo byte del comando
	if (!I2C_Write_Register(AHT10_ADDRESS, 0xE1, 0x00)) {
		UART_SendString("INIT: ERROR ESCRIBIENDO SEGUNDO BYTE\n");
		return 0;
	}
	
	UART_SendString("INIT: Comando de calibracion enviado, esperando...\n");
	_delay_ms(300); // Espera para calibración
	
	// Verificar calibración
	uint16_t timeout = 0;
	do {
		_delay_ms(10);
		timeout++;
		
		if (timeout > 100) {
			UART_SendString("INIT: TIMEOUT EN CALIBRACION\n");
			return 0;
		}
		
		if (!AHT10_GetStatus(&status)) {
			UART_SendString("INIT: ERROR LEYENDO ESTADO\n");
			return 0;
		}
		
	} while (status & AHT10_STATUS_BUSY);
	
	UART_SendString("INIT: Estado final: 0x");
	send_hex_byte(status);
	UART_SendString("\n");
	
	if ((status & AHT10_STATUS_CALIB) == 0) {
		UART_SendString("INIT: SENSOR NO SE PUDO CALIBRAR\n");
		
		// ÚLTIMO INTENTO: Leer medición directamente (SIN FLOATS)
		UART_SendString("INIT: Intentando lectura directa...\n");
		uint16_t temp_scaled, hum_scaled;
		if (AHT10_ReadMeasurements(&temp_scaled, &hum_scaled)) {
			UART_SendString("INIT: LECTURA DIRECTA EXITOSA - Sensor funciona!\n");
			
			// Mostrar valores leídos para debug
			UART_SendString("INIT: Temp: ");
			send_decimal(temp_scaled, 2);
			UART_SendString("C, Hum: ");
			send_decimal(hum_scaled, 2);
			UART_SendString("%\n");
			
			return 1;
		}
		
		return 0;
	}
	
	UART_SendString("INIT: CALIBRACION EXITOSA!\n");
	return 1;
}

// Función auxiliar para enviar números decimales
void send_decimal(uint16_t value_scaled, uint8_t decimal_places) {
	char buffer[6];
	
	// Parte entera
	uint16_t integer_part = value_scaled / 100;
	uint16_t decimal_part = value_scaled % 100;
	
	// Convertir parte entera
	uint16_to_str(integer_part, buffer, 0);
	UART_SendString(buffer);
	
	UART_SendString(".");
	
	// Convertir parte decimal con leading zeros
	if (decimal_places == 2) {
		if (decimal_part < 10) {
			UART_SendString("0");
		}
		uint16_to_str(decimal_part, buffer, 0);
		UART_SendString(buffer);
		} else if (decimal_places == 1) {
		// Solo un decimal
		uint8_t first_decimal = decimal_part / 10;
		buffer[0] = '0' + first_decimal;
		buffer[1] = '\0';
		UART_SendString(buffer);
	}
}

uint8_t AHT10_ReadRawData(uint32_t *raw_temp, uint32_t *raw_hum)
{
	uint8_t data[6];
	uint8_t trigger_cmd[3] = {AHT10_TRIGGER_MEASUREMENT_CMD, 0x33, 0x00};
	
	// Enviar comando para disparar medición
	if (!I2C_Master_Start(AHT10_ADDRESS, 0)) {
		//UART_SendString("Error: I2C Start failed\n");
		return 0;
	}
	
	// Enviar comando de trigger (3 bytes)
	for (uint8_t i = 0; i < 3; i++) {
		if (!I2C_Master_Write(trigger_cmd[i])) {
			UART_SendString("Error: I2C Write failed\n");
			I2C_Master_Stop();
			return 0;
		}
	}
	
	I2C_Master_Stop();
	
	// Esperar a que la medición se complete
	uint16_t timeout = 0;
	uint8_t status;
	
	do {
		_delay_ms(10);
		timeout++;
		
		if (timeout > 100) {
			//UART_SendString("Error: Timeout waiting for measurement\n");
			return 0;
		}
		
		// Leer estado
		if (!AHT10_GetStatus(&status)) {
			//UART_SendString("Error: Failed to read status\n");
			return 0;
		}
		
	} while (status & AHT10_STATUS_BUSY);
	
	// Leer los 6 bytes de datos
	if (!I2C_Read_Multiple(AHT10_ADDRESS, 0, data, 6)) {
		//UART_SendString("Error: Failed to read data bytes\n");
		return 0;
	}
	
	// DEBUG: Mostrar bytes crudos recibidos
	//char debug_buf[50];
	//sprintf(debug_buf, "Bytes crudos: %02X %02X %02X %02X %02X %02X\n",
	//data[0], data[1], data[2], data[3], data[4], data[5]);
	//UART_SendString(debug_buf);
	
	// Extraer datos - CORREGIDO según datasheet AHT10
	// Humedad: 20 bits en data[1], data[2], data[3] (primeros 4 bits)
	*raw_hum = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | ((uint32_t)data[3] >> 4);
	
	// Temperatura: 20 bits en data[3] (últimos 4 bits), data[4], data[5]
	*raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
	
	return 1;
}

// Función para leer mediciones convertidas a valores reales
// Función alternativa que evita el uso de floats


// Función para leer mediciones convertidas a valores reales

uint8_t AHT10_ReadMeasurements(uint16_t *temperature_scaled, uint16_t *humidity_scaled) {
	uint32_t raw_temp, raw_hum;
	
	if (!AHT10_ReadRawData(&raw_temp, &raw_hum)) {
		return 0;
	}
	
	// CONVERSIÓN SIN FLOATS - usando aritmética entera
	// Humedad: (raw_hum * 10000) / 1048576 (escalado x100)
	*humidity_scaled = (raw_hum * 10000UL) / 1048576UL;
	
	// Temperatura: ((raw_temp * 20000) / 1048576) - 5000 (escalado x100)
	// Cálculo en dos pasos para evitar overflow
	uint32_t temp_calc = (raw_temp * 20000UL) / 1048576UL;
	if (temp_calc >= 5000) {
		*temperature_scaled = temp_calc - 5000;
		} else {
		// Manejo de valores negativos (raro pero posible)
		*temperature_scaled = 0;
	}
	
	return 1;
}



uint8_t AHT10_SoftReset(void)
{
	UART_SendString("SOFTRESET: Iniciando...\n");
	
	// Método alternativo de reset
	// Algunos AHT10 responden mejor a este enfoque
	
	if (!I2C_Master_Start(AHT10_ADDRESS, 0)) {
		UART_SendString("SOFTRESET: ERROR DE START\n");
		return 0;
	}
	
	// Enviar comando de reset
	TWDR = 0xBA;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	// Esperar con timeout
	uint16_t timeout = 0;
	while (!(TWCR & (1 << TWINT)) && timeout < 500) {
		_delay_us(10);
		timeout++;
	}
	
	if (timeout >= 500) {
		UART_SendString("SOFTRESET: TIMEOUT\n");
		I2C_Master_Stop();
		return 0;
	}
	
	// Verificar si el sensor aceptó el reset
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
		UART_SendString("SOFTRESET: SENSOR NO ACK - Estado: 0x");
		char status_buf[10];
		sprintf(status_buf, "%02X\n", TWSR & 0xF8);
		UART_SendString(status_buf);
		I2C_Master_Stop();
		return 0;
	}
	
	I2C_Master_Stop();
	UART_SendString("SOFTRESET: EXITOSO\n");
	return 1;
}

uint8_t AHT10_GetStatus(uint8_t *status) {
	// Usar la nueva función específica para leer el byte de estado del AHT10
	if (!AHT10_ReadStatusByte(status)) {
		return 0; // Error al leer estado
	}
	return 1; // Lectura exitosa
}


//Funcion para lectura de estado del AHT10
uint8_t AHT10_ReadStatusByte(uint8_t *status) {
	if (!I2C_Master_Start(AHT10_ADDRESS, 1)) { // 1 para lectura
		return 0; // Error
	}
	*status = I2C_Master_Read(0); // 0 para NACK
	I2C_Master_Stop();
	return 1; // Éxito
}

void AHT10_Debug_ReadRaw(void) {
	uint32_t raw_temp, raw_hum;
	
	if (AHT10_ReadRawData(&raw_temp, &raw_hum)) {
		// Mostrar datos crudos en hex
		UART_SendString("Humedad cruda: 0x");
		send_hex_byte((raw_hum >> 16) & 0xFF);
		send_hex_byte((raw_hum >> 8) & 0xFF);
		send_hex_byte(raw_hum & 0xFF);
		UART_SendString("\n");
		
		UART_SendString("Temperatura cruda: 0x");
		send_hex_byte((raw_temp >> 16) & 0xFF);
		send_hex_byte((raw_temp >> 8) & 0xFF);
		send_hex_byte(raw_temp & 0xFF);
		UART_SendString("\n");
		} else {
		UART_SendString("Error leyendo datos crudos\n");
	}
}

void DisplayMeasurements(uint16_t temp_scaled, uint16_t hum_scaled) {
	char buffer[6];
	
	// Mostrar humedad en formato Hxx.xH
	UART_SendString("H");
	
	// Parte entera de humedad
	uint8_t hum_int = hum_scaled / 100;
	uint8_t hum_dec = hum_scaled % 100;
	
	// Leading zero si es necesario
	if (hum_int < 10) {
		UART_SendString("0");
	}
	uint16_to_str(hum_int, buffer, 0);
	UART_SendString(buffer);
	
	UART_SendString(".");
	
	// Parte decimal (solo primer dígito)
	uint8_t first_decimal = hum_dec / 10;
	buffer[0] = '0' + first_decimal;
	buffer[1] = '\0';
	UART_SendString(buffer);
	
	UART_SendString("H\n");
	
	// Opcional: mostrar temperatura también
	UART_SendString("T");
	uint8_t temp_int = temp_scaled / 100;
	uint8_t temp_dec = temp_scaled % 100;
	
	if (temp_int < 10 && temp_int >= 0) {
		UART_SendString("0");
	}
	uint16_to_str(temp_int, buffer, 0);
	UART_SendString(buffer);
	
	UART_SendString(".");
	first_decimal = temp_dec / 10;
	buffer[0] = '0' + first_decimal;
	buffer[1] = '\0';
	UART_SendString(buffer);
	
	UART_SendString("C\n");
}



//----------------------------------------------------------------------------------------
//-------------FUNCIONES AUXILEARES------------------------------------------------------

// Función auxiliar para convertir uint16_t a string
void uint16_to_str(uint16_t value, char* buffer, uint8_t decimal_places) {
	uint8_t i = 0;
	uint16_t temp = value;
	
	// Calcular longitud
	uint8_t digits = 1;
	uint16_t divisor = 1;
	while (temp >= 10) {
		temp /= 10;
		digits++;
		divisor *= 10;
	}
	
	// Convertir dígito por dígito
	temp = value;
	for (uint8_t j = 0; j < digits; j++) {
		uint8_t digit = temp / divisor;
		buffer[i++] = '0' + digit;
		temp %= divisor;
		divisor /= 10;
	}
	
	buffer[i] = '\0';
}

void I2C_Scanner(void) {
	
	UART_SendString("Escaneando dispositivos I2C...\n");
	char buffer[10]; // Suficiente para "0xXX\n\0"
	for (uint8_t address = 1; address < 127; address++) {
		// Intentar iniciar comunicacin en modo escritura para ver si el dispositivo responde
		if (I2C_Master_Start(address, 0)) {
			UART_SendString("Dispositivo encontrado en: 0x");
			// Convertir la direccin a hexadecimal y enviarla por UART
			sprintf(buffer, "%02X\n", address); // Formatea a 2 dgitos hexadecimales
			UART_SendString(buffer);
			I2C_Master_Stop(); // Detener la comunicacin
		}
	}
	UART_SendString("Escaneo completado\n");
}

// Enviar byte en formato hexadecimal
void send_hex_byte(uint8_t byte) {
	char hex_chars[] = "0123456789ABCDEF";
	UART_SendChar(hex_chars[(byte >> 4) & 0x0F]);
	UART_SendChar(hex_chars[byte & 0x0F]);
}

// Función auxiliar para mostrar 24 bits en hex
void send_hex_24bit(uint32_t value) {
	send_hex_byte((value >> 16) & 0xFF);
	send_hex_byte((value >> 8) & 0xFF);
	send_hex_byte(value & 0xFF);
}


/****************************************/
// Interrupt routines