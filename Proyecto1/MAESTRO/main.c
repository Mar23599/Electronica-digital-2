/*
 * MAESTRO.c
 * 
 * Created: 8/08/2025 12:27:55
 * Author : Luis Sactic
 */ 

#include "LCD/LCD.h"
#include "I2C/I2C.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Definiciones de constantes
#define AHT10_ADDRESS 0x38
#define TSL2561_ADDRESS 0x39
#define SLAVE_ADDRESS 0x40
#define ESP32_I2C_ADDRESS 0x50     // Dirección para el ESP32
#define FILTER_SAMPLES 5

// Comandos para AHT10
#define AHT10_INIT_CMD 0xE1
#define AHT10_TRIGGER_MEASUREMENT_CMD 0xAC
#define AHT10_SOFT_RESET_CMD 0xBA

// Comandos para TSL2561
#define TSL2561_CMD 0x80
#define TSL2561_REG_CONTROL 0x00
#define TSL2561_REG_TIMING 0x01
#define TSL2561_REG_DATA0LOW 0x0C
#define TSL2561_REG_DATA0HIGH 0x0D
#define TSL2561_REG_DATA1LOW 0x0E
#define TSL2561_REG_DATA1HIGH 0x0F
#define TSL2561_POWER_ON 0x03
#define TSL2561_POWER_OFF 0x00

// Banderas de estado
typedef struct {
    uint8_t aht10_connected;
    uint8_t aht10_initialized;
    uint8_t tsl2561_connected;
    uint8_t tsl2561_initialized;
} SystemStatus;

// Variables globales
SystemStatus system_status = {0};
float humidity_history[FILTER_SAMPLES];
uint8_t history_index = 0;

// Prototipos de funciones
void System_Init(void);
void Sensor_Init(void);
void Display_Init(void);
void Main_Loop(void);

// Funciones del sensor AHT10
uint8_t AHT10_Init(void);
uint8_t AHT10_ReadStatus(void);
float AHT10_ReadHumidity(void);
void AHT10_SoftReset(void);
uint8_t CheckI2CConnection(uint8_t address);
float ApplyMedianFilter(float new_value);

// Funciones del sensor TSL2561
uint8_t TSL2561_Init(void);
uint32_t TSL2561_ReadLight(void);
float TSL2561_CalculateLux(uint32_t ch0, uint32_t ch1);

// Funciones de visualización
void DisplayHumidity(float humidity);
void DisplayTemperature(float temp);
void DisplayLight(float lux);

// Funciones de comunicación I2C
float ReadSlaveTemperature(void);
void SendSensorDataToESP(float temp, float hum, float lux);

int main(void) {
    System_Init();
    Main_Loop();
    
    return 0;
}

void System_Init(void) {    
    // Inicializar periféricos básicos
    LCD_Init();
    I2C_Master_Init(100000UL, 1);
    
    // Inicializar subsistemas
    Display_Init();
    Sensor_Init();
}

void Display_Init(void) {
    // Mensajes iniciales
    LCD_SetCursor(0, 0);
    LCD_WriteString("H:");
    LCD_SetCursor(9, 0);
    LCD_WriteString("T:");
    LCD_SetCursor(0, 1);
    LCD_WriteString("L:");
    
    // Inicializar filtro
    for(uint8_t i = 0; i < FILTER_SAMPLES; i++) {
        humidity_history[i] = 0.0;
    }
}

void Sensor_Init(void) {
    // Verificar conexión con los sensores
    system_status.aht10_connected = CheckI2CConnection(AHT10_ADDRESS);
    system_status.tsl2561_connected = CheckI2CConnection(TSL2561_ADDRESS);
    
    // Inicializar AHT10
    if(system_status.aht10_connected) {
        for(uint8_t i = 0; i < 5; i++) {
            if(AHT10_Init()) {
                system_status.aht10_initialized = 1;
                break;
            }
            _delay_ms(100);
        }
        
        if(!system_status.aht10_initialized) {
            LCD_SetCursor(0, 0);
            LCD_WriteString("Error: Inic. AHT10");
            _delay_ms(2000);
        }
    } else {
        LCD_SetCursor(0, 0);
        LCD_WriteString("No AHT10");
        _delay_ms(2000);
    }
    
    // Inicializar TSL2561
    if(system_status.tsl2561_connected) {
        for(uint8_t i = 0; i < 5; i++) {
            if(TSL2561_Init()) {
                system_status.tsl2561_initialized = 1;
                break;
            }
            _delay_ms(100);
        }
        
        if(!system_status.tsl2561_initialized) {
            LCD_SetCursor(0, 1);
            LCD_WriteString("Error: Inic. TSL2561");
            _delay_ms(2000);
        }
    } else {
        LCD_SetCursor(0, 1);
        LCD_WriteString("No TSL2561");
        _delay_ms(2000);
    }
}

void Main_Loop(void) {
    while(1) {
        float current_hum = -1.0, current_temp = -100.0, current_lux = -1.0;

        // Leer humedad del AHT10
        if(system_status.aht10_connected && system_status.aht10_initialized) {
            current_hum = AHT10_ReadHumidity();
            if(current_hum < 0 || current_hum > 100) {
                LCD_SetCursor(0, 0);
                LCD_WriteString("AHT10 Error");
                current_hum = -1.0;
            }
        } else {
            LCD_SetCursor(0, 0);
            LCD_WriteString("AHT10 Discon");
        }

        // Leer luz del TSL2561
        if(system_status.tsl2561_connected && system_status.tsl2561_initialized) {
            uint32_t light_raw = TSL2561_ReadLight();
            if(light_raw != 0) {
                current_lux = TSL2561_CalculateLux(light_raw >> 16, light_raw & 0xFFFF);
            } else {
                LCD_SetCursor(0, 1);
                LCD_WriteString("TSL2561 Err");
            }
        } else {
            LCD_SetCursor(0, 1);
            LCD_WriteString("TSL2561 Off");
        }

        // Leer temperatura del esclavo
        current_temp = ReadSlaveTemperature();
        if(current_temp < -40 || current_temp > 80) {
            LCD_SetCursor(9, 0);
            LCD_WriteString("TempErr");
            current_temp = -100.0;
        }

        // Mostrar valores si son válidos
        if(current_hum >= 0) DisplayHumidity(current_hum);
        if(current_temp > -40) DisplayTemperature(current_temp);
        if(current_lux >= 0) DisplayLight(current_lux);

        // Enviar datos al ESP32
        if(current_hum >= 0 && current_temp > -40 && current_lux >= 0) {
            SendSensorDataToESP(current_temp, current_hum, current_lux);
        }

        _delay_ms(1000);
    }
}

void I2C_Reset_Bus(void) {
    TWCR = 0; // Deshabilitar I2C
    _delay_ms(1);
    I2C_Master_Init(100000UL, 1); // Reiniciar I2C
}

uint8_t CheckI2CConnection(uint8_t address) {
    I2C_Master_Start();
    uint8_t result = I2C_Master_Write(address << 1);
    I2C_Master_Stop();
    return result;
}

void AHT10_SoftReset(void) {
    I2C_Master_Start();
    I2C_Master_Write(AHT10_ADDRESS << 1);
    I2C_Master_Write(AHT10_SOFT_RESET_CMD);
    I2C_Master_Stop();
    _delay_ms(20);
}

uint8_t AHT10_Init(void) {
    AHT10_SoftReset();
    
    I2C_Master_Start();
    if(!I2C_Master_Write(AHT10_ADDRESS << 1)) {
        I2C_Master_Stop();
        return 0;
    }
    I2C_Master_Write(AHT10_INIT_CMD);
    I2C_Master_Write(0x08);
    I2C_Master_Write(0x00);
    I2C_Master_Stop();
    
    _delay_ms(10);
    
    // Verificar que el sensor esté calibrado
    uint8_t status = AHT10_ReadStatus();
    return (status & 0x68) == 0x08;
}

uint8_t AHT10_ReadStatus(void) {
    uint8_t status = 0;
    I2C_Master_Start();
    if(!I2C_Master_Write((AHT10_ADDRESS << 1) | 0x01)) {
        I2C_Master_Stop();
        return 0;
    }
    I2C_Master_Read(&status, 0);
    I2C_Master_Stop();
    return status;
}

float AHT10_ReadHumidity(void) {
    uint8_t data[6] = {0};
    
    // Iniciar medición
    I2C_Master_Start();
    if(!I2C_Master_Write(AHT10_ADDRESS << 1)) {
        I2C_Master_Stop();
        return -1.0;
    }
    I2C_Master_Write(AHT10_TRIGGER_MEASUREMENT_CMD);
    I2C_Master_Write(0x33);
    I2C_Master_Write(0x00);
    I2C_Master_Stop();
    
    // Esperar hasta que esté listo (máximo 80ms)
    uint8_t timeout = 20;
    uint8_t status;
    do {
        _delay_ms(5);
        status = AHT10_ReadStatus();
    } while(!(status & 0x80) && --timeout);
    
    if(timeout == 0) return -1.0;
    
    // Leer datos
    I2C_Master_Start();
    if(!I2C_Master_Write((AHT10_ADDRESS << 1) | 0x01)) {
        I2C_Master_Stop();
        return -1.0;
    }
    
    for(uint8_t i = 0; i < 6; i++) {
        I2C_Master_Read(&data[i], (i < 5) ? 1 : 0);
    }
    I2C_Master_Stop();
    
    // Procesar humedad (los primeros 20 bits)
    uint32_t hum_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    float humidity = ((float)hum_raw / 1048576.0) * 100.0;
    
    return humidity;
}

uint8_t TSL2561_Init(void) {
    // Encender el sensor
    I2C_Master_Start();
    if(!I2C_Master_Write(TSL2561_ADDRESS << 1)) {
        I2C_Master_Stop();
        return 0;
    }
    I2C_Master_Write(TSL2561_CMD | TSL2561_REG_CONTROL);
    I2C_Master_Write(TSL2561_POWER_ON);
    I2C_Master_Stop();
    
    // Configurar tiempo de integración (402ms)
    I2C_Master_Start();
    if(!I2C_Master_Write(TSL2561_ADDRESS << 1)) {
        I2C_Master_Stop();
        return 0;
    }
    I2C_Master_Write(TSL2561_CMD | TSL2561_REG_TIMING);
    I2C_Master_Write(0x02); // 402ms, low gain
    I2C_Master_Stop();
    
    _delay_ms(10);
    return 1;
}

uint32_t TSL2561_ReadLight(void) {
    uint8_t data[4];
    uint16_t ch0, ch1;
    
    // Leer canales 0 y 1
    I2C_Master_Start();
    if(!I2C_Master_Write(TSL2561_ADDRESS << 1)) {
        I2C_Master_Stop();
        return 0;
    }
    I2C_Master_Write(TSL2561_CMD | TSL2561_REG_DATA0LOW);
    I2C_Master_Stop();
    
    I2C_Master_Start();
    if(!I2C_Master_Write((TSL2561_ADDRESS << 1) | 0x01)) {
        I2C_Master_Stop();
        return 0;
    }
    
    for(uint8_t i = 0; i < 4; i++) {
        I2C_Master_Read(&data[i], (i < 3) ? 1 : 0);
    }
    I2C_Master_Stop();
    
    ch0 = (data[1] << 8) | data[0];
    ch1 = (data[3] << 8) | data[2];
    
    return ((uint32_t)ch0 << 16) | ch1;
}

float TSL2561_CalculateLux(uint32_t ch0, uint32_t ch1) {
    if(ch0 == 0) return 0.0;
    
    float ratio = (float)ch1 / (float)ch0;
    float lux = 0.0;
    
    if(ratio <= 0.5) {
        lux = (0.0304 * ch0) - (0.062 * ch0 * pow(ratio, 1.4));
    } else if(ratio <= 0.61) {
        lux = (0.0224 * ch0) - (0.031 * ch1);
    } else if(ratio <= 0.80) {
        lux = (0.0128 * ch0) - (0.0153 * ch1);
    } else if(ratio <= 1.30) {
        lux = (0.00146 * ch0) - (0.00112 * ch1);
    } else {
        lux = 0.0;
    }
    
    return lux;
}

float ApplyMedianFilter(float new_value) {
    // Almacenar nuevo valor
    humidity_history[history_index] = new_value;
    history_index = (history_index + 1) % FILTER_SAMPLES;
    
    // Calcular promedio
    float sum = 0;
    for(uint8_t i = 0; i < FILTER_SAMPLES; i++) {
        sum += humidity_history[i];
    }
    
    return sum / FILTER_SAMPLES;
}

void DisplayHumidity(float humidity) {
    char buffer[8];
    LCD_SetCursor(2, 0);
    
    if(humidity < 0.0) {
        LCD_WriteString("NoSen ");
    } else if(humidity > 100.0) {
        LCD_WriteString("Error ");
    } else {
        dtostrf(humidity, 5, 1, buffer);
        LCD_WriteString(buffer);
        LCD_WriteString("%");
    }
}

void DisplayTemperature(float temp) {
    LCD_SetCursor(11, 0);
    
    if(temp <= -99.0) {
        LCD_WriteString("NoSen");
    } else if(temp <= -40.0) {
        LCD_WriteString("Error");
    } else {
        char temp_str[6];
        dtostrf(temp, 5, 1, temp_str);
        LCD_WriteString(temp_str);
    }
}

void DisplayLight(float lux) {
    LCD_SetCursor(2, 1);
    
    if(lux < 0.0) {
        LCD_WriteString("NoSen   ");
    } else {
        char lux_str[8];
        if(lux < 1000.0) {
            dtostrf(lux, 5, 1, lux_str);
            LCD_WriteString(lux_str);
            LCD_WriteString(" lx");
        } else if(lux < 10000.0) {
            dtostrf(lux/1000.0, 4, 2, lux_str);
            LCD_WriteString(lux_str);
            LCD_WriteString(" klx");
        } else {
            dtostrf(lux/1000.0, 5, 1, lux_str);
            LCD_WriteString(lux_str);
            LCD_WriteString("klx");
        }
    }
}

float ReadSlaveTemperature(void) {
    uint8_t temp_raw;
    
    // Intento de lectura con timeout
    for(uint8_t attempt = 0; attempt < 3; attempt++) {
        I2C_Master_Start();
        if(!I2C_Master_Write((SLAVE_ADDRESS << 1) | 0x01)) {
            I2C_Master_Stop();
            _delay_ms(10);
            continue;
        }
        
        if(I2C_Master_Read(&temp_raw, 0)) {
            I2C_Master_Stop();
            
            // Convertir a temperatura con signo
            int8_t signed_temp = (int8_t)temp_raw;
            
            // Validar rango razonable
            if(signed_temp >= -40 && signed_temp <= 80) {
                return (float)signed_temp;
            }
        }
        
        I2C_Master_Stop();
        _delay_ms(10);
    }
    
    return -100.0; // Valor de error
}

void SendSensorDataToESP(float temp, float hum, float lux) {
    // Estructura para enviar datos al ESP32
    typedef struct {
        float temperature;
        float humidity;
        float light;
    } SensorData;
    
    SensorData data = {temp, hum, lux};
    
    // Enviar datos al ESP32
    I2C_Master_Start();
    if(I2C_Master_Write(ESP32_I2C_ADDRESS << 1)) {
        uint8_t* bytes = (uint8_t*)&data;
        for(uint8_t i = 0; i < sizeof(data); i++) {
            I2C_Master_Write(bytes[i]);
        }
    }
    I2C_Master_Stop();
}