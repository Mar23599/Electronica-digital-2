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

float current_temp = 0.0;
float current_hum = 0.0;

float temperatura_read = 0.0;
float humedad_read = 0.0;


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
uint8_t AHT10_ReadMeasurements(float *temperature, float *humidity);
uint8_t AHT10_SoftReset(void);
uint8_t AHT10_GetStatus(uint8_t *status);
uint8_t AHT10_ReadStatusByte(uint8_t *status); 
void AHT10_Debug_ReadRaw(void);
void DisplayMeasurements(float temp, float hum);
void AHT10_OutString(void);




void I2C_Scanner(void);

/****************************************/
// Main Function

int main(void)
{
	
	setup();
	
	
	
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
	
        // Primero debug: leer datos crudos
        //UART_SendString("Leyendo datos crudos...\n");
        // AHT10_Debug_ReadRaw();
        
        // Luego intentar medición normal
        if (AHT10_ReadMeasurements(&current_temp, &current_hum))
        {
	        // Mostrar mediciones en UART
	        DisplayMeasurements(current_temp, current_hum);
	        
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
	char buf[10];
	sprintf(buf, "%02X\n", status);
	UART_SendString(buf);
	
	// Si ya está calibrado, no necesitamos inicializar
	if (status & AHT10_STATUS_CALIB) {
		UART_SendString("INIT: SENSOR YA CALIBRADO - Saltando inicialización\n");
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
	sprintf(buf, "%02X\n", status);
	UART_SendString(buf);
	
	// Si ya está calibrado después del reset, perfecto
	if (status & AHT10_STATUS_CALIB) {
		UART_SendString("INIT: SENSOR CALIBRADO DESPUÉS DE RESET\n");
		return 1;
	}
	
	UART_SendString("INIT: Intentando comando de calibración alternativo...\n");
	
	// ALTERNATIVA: Usar método directo de registro
	// Algunos AHT10 prefieren este enfoque
	
	// Escribir al registro de configuración (0xE1 no es el único comando)
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
	
	UART_SendString("INIT: Comando de calibración enviado, esperando...\n");
	_delay_ms(300); // Espera para calibración
	
	// Verificar calibración
	uint16_t timeout = 0;
	do {
		_delay_ms(10);
		timeout++;
		
		if (timeout > 100) {
			UART_SendString("INIT: TIMEOUT EN CALIBRACIÓN\n");
			return 0;
		}
		
		if (!AHT10_GetStatus(&status)) {
			UART_SendString("INIT: ERROR LEYENDO ESTADO\n");
			return 0;
		}
		
	} while (status & AHT10_STATUS_BUSY);
	
	UART_SendString("INIT: Estado final: 0x");
	sprintf(buf, "%02X\n", status);
	UART_SendString(buf);
	
	if ((status & AHT10_STATUS_CALIB) == 0) {
		UART_SendString("INIT: SENSOR NO SE PUDO CALIBRAR\n");
		
		// ÚLTIMO INTENTO: Leer medición directamente
		UART_SendString("INIT: Intentando lectura directa...\n");
		float temp, hum;
		if (AHT10_ReadMeasurements(&temp, &hum)) {
			UART_SendString("INIT: LECTURA DIRECTA EXITOSA - Sensor funciona!\n");
			return 1;
		}
		
		return 0;
	}
	
	UART_SendString("INIT: CALIBRACIÓN EXITOSA!\n");
	return 1;
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
		//	UART_SendString("Error: I2C Write failed\n");
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

uint8_t AHT10_ReadMeasurements(float *temperature, float *humidity) {
	uint32_t raw_temp, raw_hum;  // Variables para datos crudos del sensor
	
	// Leer datos crudos del sensor AHT10
	if (!AHT10_ReadRawData(&raw_temp, &raw_hum)) {
		return 0;  // Retorna 0 si hay error en la lectura
	}
	
	// CONVERSIÓN DE HUMEDAD (según datasheet AHT10)
	// Fórmula: Humedad = (raw_hum / 2^20) * 100%
	// Se usa aritmética entera primero para mayor precisión
	uint32_t hum_calc = (raw_hum * 10000UL) / 1048576UL;  // 1048576 = 2^20
	*humidity = hum_calc / 100.0f;  // Convertir a float con un decimal
	
	// GUARDAR EN VARIABLE GLOBAL - Esto permite acceso desde cualquier función
	humedad_read = *humidity;  // humedad_read ahora contiene el último valor medido
	
	// CONVERSIÓN DE TEMPERATURA (según datasheet AHT10)
	// Fórmula: Temperatura = (raw_temp / 2^20) * 200 - 50
	// Se hace en float para evitar problemas de overflow
	float temp_calc_float = ((float)raw_temp / 1048576.0f) * 200.0f - 50.0f;
	*temperature = temp_calc_float;
	
	// GUARDAR EN VARIABLE GLOBAL
	temperatura_read = *temperature;  // temperatura_read contiene el último valor medido
	
	return 1;  // Retorna 1 indicando lectura exitosa
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

//Funcion para lectura de estado del AHT10
uint8_t AHT10_ReadStatusByte(uint8_t *status) {
	if (!I2C_Master_Start(AHT10_ADDRESS, 1)) { // 1 para lectura
		return 0; // Error
	}
	*status = I2C_Master_Read(0); // 0 para NACK
	I2C_Master_Stop();
	return 1; // Éxito
}

void AHT10_Debug_ReadRaw(void)
{
	uint32_t raw_temp, raw_hum;
	
	if (AHT10_ReadRawData(&raw_temp, &raw_hum)) {
		char debug_msg[40];
		
		// Mostrar datos crudos
		//sprintf(debug_msg, "Datos crudos - Hum: %lu (%06lX), Temp: %lu (%06lX)\n",
		//raw_hum, raw_hum, raw_temp, raw_temp);
		//UART_SendString(debug_msg);
		
		// Calcular valores manualmente
		float hum_calc = ((float)raw_hum / 1048576.0f) * 100.0f;
		float temp_calc = ((float)raw_temp / 1048576.0f) * 200.0f - 50.0f;
		
		// Mostrar cálculos con aritmética entera para evitar problemas de sprintf
		//UART_SendString("Calculo manual - Hum: ");
		
		int hum_int = (int)hum_calc;
		int hum_frac = (int)((hum_calc - hum_int) * 100);
		itoa(hum_int, debug_msg, 10);
		UART_SendString(debug_msg);
		UART_SendString(".");
		if (hum_frac < 10) UART_SendString("0");
		itoa(hum_frac, debug_msg, 10);
		UART_SendString(debug_msg);
		UART_SendString("%");
		
		UART_SendString(", Temp: ");
		
		int temp_int = (int)temp_calc;
		int temp_frac = (int)((temp_calc - temp_int) * 100);
		itoa(temp_int, debug_msg, 10);
		UART_SendString(debug_msg);
		UART_SendString(".");
		if (temp_frac < 10) UART_SendString("0");
		itoa(temp_frac, debug_msg, 10);
		UART_SendString(debug_msg);
		UART_SendString("C\n");
		
		} else {
		UART_SendString("Error leyendo datos crudos\n");
	}
}

void DisplayMeasurements(float temp, float hum) {
	char buffer[20];  // Buffer para conversión de números a texto
	
	// FORMATO: "Hxx.xH" donde:
	// - Primera 'H': delimitador inicial
	// - xx: dos dígitos enteros (con leading zero si es necesario)
	// - .x: punto y un dígito decimal
	// - Última 'H': delimitador final
	
	UART_SendString("H");  // Enviar delimitador inicial
	
	// CONVERSIÓN DE PARTE ENTERA DE HUMEDAD
	int hum_int = (int)humedad_read;  // Obtener parte entera de la variable global
	int hum_frac = (int)((humedad_read - hum_int) * 10 + 0.5);  // Parte decimal redondeada
	
	// AGREGAR LEADING ZERO si la parte entera es menor a 10
	// Ejemplo: 5.3% se convierte en "05.3"
	if (hum_int < 10) {
		UART_SendString("0");  // Agregar cero a la izquierda
	}
	
	// CONVERTIR Y ENVIAR PARTE ENTERA
	itoa(hum_int, buffer, 10);  // Convertir entero a string (base 10)
	UART_SendString(buffer);
	
	// ENVIAR PUNTO DECIMAL
	UART_SendString(".");
	
	// CONVERTIR Y ENVIAR PARTE DECIMAL
	itoa(hum_frac, buffer, 10);  // Convertir decimal a string
	UART_SendString(buffer);
	
	// ENVIAR DELIMITADOR FINAL Y NUEVA LÍNEA
	UART_SendString("H\n");
	
	// La temperatura se recibe como parámetro pero no se muestra en este formato
	// Sin embargo, está almacenada en temperatura_read por si se necesita después
}


//----------------------------------------------------------------------------------------

/****************************************/
// Interrupt routines