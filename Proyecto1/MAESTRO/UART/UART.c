/*
 * UART.c
 *
 * Created: 21/08/2025 07:21:30
 *  Author: aleja
 */ 
#include "UART.h"

/**
 * @brief Inicializa el m�dulo UART a 9600 baudios
 * @param interrupts: 1 para habilitar interrupciones de recepci�n, 0 para deshabilitar
 * 
 * Configuraci�n:
 * - Baud rate: 9600
 * - Data bits: 8
 * - Stop bits: 1
 * - Parity: None
 * - Modo: As�ncrono
 * 
 * C�lculo de UBRR:
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
    
    // Configurar interrupciones de recepci�n si se solicitan
    if (interrupts) {
        UCSR0B |= (1 << RXCIE0);  // Habilitar interrupci�n de recepci�n
        
    } else {
        UCSR0B &= ~(1 << RXCIE0); // Deshabilitar interrupci�n de recepci�n
    }
}

/**
 * @brief Env�a un car�cter por UART (modo polling)
 * @param data: Car�cter a enviar
 * 
 * Espera a que el buffer de transmisi�n est� vac�o
 * antes de enviar el nuevo car�cter
 */
void UART_SendChar(char data) {
    // Esperar hasta que el buffer de transmisi�n est� vac�o
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Colocar el dato en el buffer de transmisi�n
    UDR0 = data;
}

/**
 * @brief Env�a una cadena de texto por UART
 * @param str: Puntero a la cadena de texto (debe terminar en '\0')
 * 
 * Itera a trav�s de cada car�cter de la cadena
 * y lo env�a usando UART_SendChar()
 */
void UART_SendString(const char *str) {
    // Iterar a trav�s de cada car�cter hasta encontrar el null terminator
    while (*str) {
        UART_SendChar(*str);
        str++; // Avanzar al siguiente car�cter
    }
}

/**
 * @brief Lee un car�cter desde UART (modo polling)
 * @return Car�cter recibido
 * 
 * Espera a que haya datos disponibles en el buffer de recepci�n
 * antes de leer el car�cter
 */
char UART_ReceiveChar(void) {
    // Esperar hasta que haya datos disponibles
    while (!(UCSR0A & (1 << RXC0)));
    
    // Leer y retornar el dato recibido
    return UDR0;
}

/**
 * @brief Verifica si hay datos disponibles en el buffer de recepci�n
 * @return 1 si hay datos disponibles, 0 si no
 * 
 * �til para evitar bloqueos al leer datos
 */
uint8_t UART_DataAvailable(void) {
    // Verificar el flag de recepci�n completa
    return (UCSR0A & (1 << RXC0));
}