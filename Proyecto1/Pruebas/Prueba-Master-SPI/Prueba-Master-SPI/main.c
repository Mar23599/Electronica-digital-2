#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>  // Para usar sprintf

#include "I2C_LIB.h"
#include "Ale-lib-UART.h"

#define AHT10_ADDRESS 0x38

float temperatura = 0.0;

void AHT10_Init() {
	_delay_ms(20);  // Espera inicial
}

uint8_t AHT10_ReadTemperature(float* temp) {
	uint8_t data[6];

	// Comando de medición
	I2C_Master_Start();
	I2C_Master_Write((AHT10_ADDRESS << 1) | 0);  // Escribir
	I2C_Master_Write(0xAC);
	I2C_Master_Write(0x33);
	I2C_Master_Write(0x00);
	I2C_Master_Stop();

	_delay_ms(80);  // Esperar medición

	// Lectura de 6 bytes
	I2C_Master_Start();
	I2C_Master_Write((AHT10_ADDRESS << 1) | 1);  // Leer
	for (uint8_t i = 0; i < 6; i++) {
		I2C_Master_Read(&data[i], i < 5 ? 1 : 0);  // ACK hasta el penúltimo
	}
	I2C_Master_Stop();

	// Extraer temperatura
	uint32_t temp_raw = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
	*temp = ((float)temp_raw * 200.0 / 1048576.0) - 50.0;

	return 1;
}

int main(void) {
	char buffer[32];

	I2C_Master_init(100000, 1);  // 100kHz I2C
	UART_init(0);                // UART sin interrupciones
	AHT10_Init();

	UART_SendString("Iniciando sensor AHT10...\r\n");

	while (1) {
		if (AHT10_ReadTemperature(&temperatura)) {
			// Convertir a string
			sprintf(buffer, "Temperatura: %.2f C\r\n", temperatura);
			UART_SendString(buffer);
			} else {
			UART_SendString("Error leyendo temperatura\r\n");
		}

		_delay_ms(1000);
	}
}
