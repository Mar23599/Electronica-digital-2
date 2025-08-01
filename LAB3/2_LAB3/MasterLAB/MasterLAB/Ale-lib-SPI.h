/*
 * Ale_lib_SPI.h
 *
 * Created: 28/07/2025 21:18:13
 *  Author: aleja
 */ 


#ifndef ALE-LIB-SPI_H_
#define ALE-LIB-SPI_H_

#include <avr/io.h>
#include <stdint.h>


typedef enum {
	
	MS_OSC_DIV2 = 0b01010000,
	MS_OSC_DIV4 = 0b01010001,
	MS_OSC_DIV8 = 0b01010010,
	MS_OSC_DIV16 = 0b01010011,
	MS_OSC_DIV32 = 0b01010100,
	MS_OSC_DIV64 = 0b01010101,
	MS_OSC_DIV128 = 0b01010110,
	SLAVE_SS = 0b00000000
	
	} SPI_type ;
	
typedef enum {
	
	DATA_ORDER_MSB = 0b00000000,
	DATA_ORDER_LSB = 0b00100000
	
	} SPI_data_order;
	
typedef enum {
	
	CLK_HIGH = 0b00001000,
	CLK_LOW = 0b00000000
	
	} SPI_clock_pol ;
	
typedef enum {
	
	CLK_FIRST_EDGE = 0b00000000,
	CLK_LAST_EDGE = 0b00000100
	
	} SPI_clock_phase ;
	


	

void SPI_init (SPI_type, SPI_data_order, SPI_clock_pol, SPI_clock_phase);
void SPI_Write (uint8_t data);
uint8_t SPI_Read(void);

void SPI_SS_control(uint8_t controlador); 



#endif /* ALE-LIB-SPI_H_ */