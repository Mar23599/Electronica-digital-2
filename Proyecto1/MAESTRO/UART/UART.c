/*
 * UART.c
 *
 * Created: 21/08/2025 07:21:30
 *  Author: aleja
 */ 
#include "UART.h"

/**
 * @brief Inicializa el módulo UART a 9600 baudios
 * @param interrupts: 1 para habilitar interrupciones de recepción, 0 para deshabilitar
 * 
 * Configuración:
 * - Baud rate: 9600
 * - Data bits: 8
 * - Stop bits: 1
 * - Parity: None
 * - Modo: Asíncrono
 * 
 * Cálculo de UBRR:
 * UBRR = (F_CPU / (16 * BAUD)) - 1
 * Para 16MHz y 9600 baudios: (16000000 / (16 * 9600)) - 1 = 103.166 ? 103
 */
void UART_init(uint8_t interrupts) {
    // Configurar Baud Rate para 9600
    uint16_t ubrr = 103; // Valor para 16MHz y 9600 baudios
    UBRR0H = (uint8_t)(ubrr >> 8);  // High byte de UBRR
    UBRR0L = (uint8_t)ubrr;         // Low byte de UBRR
    
    // Habilitar transmitter y receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    
    // Configurar formato de frame: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data
    
    // Configurar interrupciones de recepción si se solicitan
    if (interrupts) {
        UCSR0B |= (1 << RXCIE0);  // Habilitar interrupción de recepción
        
    } else {
        UCSR0B &= ~(1 << RXCIE0); // Deshabilitar interrupción de recepción
    }
}

/**
 * @brief Envía un carácter por UART (modo polling)
 * @param data: Carácter a enviar
 * 
 * Espera a que el buffer de transmisión esté vacío
 * antes de enviar el nuevo carácter
 */
void UART_SendChar(char data) {
    // Esperar hasta que el buffer de transmisión esté vacío
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Colocar el dato en el buffer de transmisión
    UDR0 = data;
}

/**
 * @brief Envía una cadena de texto por UART
 * @param str: Puntero a la cadena de texto (debe terminar en '\0')
 * 
 * Itera a través de cada carácter de la cadena
 * y lo envía usando UART_SendChar()
 */
void UART_SendString(const char *str) {
    // Iterar a través de cada carácter hasta encontrar el null terminator
    while (*str) {
        UART_SendChar(*str);
        str++; // Avanzar al siguiente carácter
    }
}

/**
 * @brief Lee un carácter desde UART (modo polling)
 * @return Carácter recibido
 * 
 * Espera a que haya datos disponibles en el buffer de recepción
 * antes de leer el carácter
 */
char UART_ReceiveChar(void) {
    // Esperar hasta que haya datos disponibles
    while (!(UCSR0A & (1 << RXC0)));
    
    // Leer y retornar el dato recibido
    return UDR0;
}

/**
 * @brief Verifica si hay datos disponibles en el buffer de recepción
 * @return 1 si hay datos disponibles, 0 si no
 * 
 * Útil para evitar bloqueos al leer datos
 */
uint8_t UART_DataAvailable(void) {
    // Verificar el flag de recepción completa
    return (UCSR0A & (1 << RXC0));
}