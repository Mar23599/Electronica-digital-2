/*
 * UART.h
 *
 * Created: 21/08/2025 07:21:39
 *  Author: aleja
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#include <stdint.h>

/**
 * @brief Inicializa el m�dulo UART
 * @param interrupts: 1 para habilitar interrupciones de recepci�n, 0 para deshabilitar
 */
void UART_init(uint8_t interrupts);

/**
 * @brief Env�a un car�cter por UART
 * @param data: Car�cter a enviar
 */
void UART_SendChar(char data);

/**
 * @brief Env�a una cadena de texto por UART
 * @param str: Puntero a la cadena de texto (debe terminar en '\0')
 */
void UART_SendString(const char *str);

/**
 * @brief Lee un car�cter desde UART (modo polling)
 * @return Car�cter recibido
 */
char UART_ReceiveChar(void);

/**
 * @brief Verifica si hay datos disponibles en el buffer de recepci�n
 * @return 1 si hay datos disponibles, 0 si no
 */
uint8_t UART_DataAvailable(void);



#endif /* UART_H_ */