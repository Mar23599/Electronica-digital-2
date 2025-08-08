/*
 * LAB 4 (MAESTRO).c
 *
 * 
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "LCD/LCD.h"
#include "I2C/I2C.h"

#define SLAVE_COUNTER_ADDR 0x20
#define SLAVE_POT_ADDR     0x21

void displayValues(uint8_t counter_val, uint16_t pot_val) {
    char buffer[16];
    
    // Primera línea: Contador (0-15)
    LCD_SetCursor(0, 0);
    snprintf(buffer, sizeof(buffer), "Contador: %2u ", counter_val);
    LCD_WriteString(buffer);
    
    // Segunda línea: Potenciómetro (0-1023)
    LCD_SetCursor(0, 1);
    
    // Ajustar valor si está fuera de rango (0-1023)
    if(pot_val > 1023) pot_val = 1023;
    
    // Mostrar valor y porcentaje
    uint8_t percentage = (pot_val * 100) / 1023;
    snprintf(buffer, sizeof(buffer), "Pot: %4u ", pot_val, percentage);
    LCD_WriteString(buffer);
}

uint8_t readCounter() {
    uint8_t value = 0;
    
    I2C_Master_Start();
    if(I2C_Master_Write((SLAVE_COUNTER_ADDR << 1) | 0x01) == 1) {
        I2C_Master_Read(&value, 0); // Leer con NACK
    }
    I2C_Master_Stop();
    
    return value & 0x0F; // Asegurar rango 0-15
}

uint16_t readPotentiometer() {
    uint8_t high_byte, low_byte;
    uint16_t value = 0;
    
    I2C_Master_Start();
    if(I2C_Master_Write((SLAVE_POT_ADDR << 1) | 0x01) == 1) {
        I2C_Master_Read(&high_byte, 1);   // Leer high byte con ACK
        I2C_Master_Read(&low_byte, 0);    // Leer low byte con NACK
        value = (high_byte << 8) | low_byte;
    }
    I2C_Master_Stop();
    
    return value;
}

int main(void) {
    LCD_Init();
    LCD_Clear();
    I2C_Master_Init(100000, 1);
    
    // Mensaje inicial
    LCD_SetCursor(0, 0);
    LCD_WriteString("Sistema I2C");
    LCD_SetCursor(0, 1);
    LCD_WriteString("Inicializando...");
    _delay_ms(1000);
    LCD_Clear();
    
    while(1) {
        uint8_t counter_val = readCounter();
        uint16_t pot_val = readPotentiometer();
        
        displayValues(counter_val, pot_val);
        _delay_ms(200);
    }
}