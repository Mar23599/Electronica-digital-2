/*
 * Ale_lib_SPI.c
 *
 * Created: 28/07/2025 21:18:00
 *  Author: aleja
 */ 

#include "Ale-lib-SPI.h"

void SPI_init (SPI_type TYPE, SPI_data_order DATA_ORDER, SPI_clock_pol CLOCK_POL, SPI_clock_phase CLOCK_PHASE){
	
	/* PINOUT:
	PB2 -> SS
	PB3 -> MOSI
	PB4 -> MISO
	PB5 -> SCK
	*/
	
	if (	TYPE & (1 << MSTR)	)
	{
		DDRB |= (1 << PORTB3)|(1 << PORTB5)|(1 << PORTB2); // MOSI, SCK, SS_Negado
		DDRB &= ~(1 << PORTB4); // MISO
		SPCR |= (1 << MSTR); // MAster
		
		
		uint8_t temp = TYPE & 0b00000111; 
		
		switch (temp)
		{
			
		case 0: //DIV2
			SPCR &= ~(1 << SPR0);
			SPCR &= ~(1 << SPR1);
			SPSR |= (1 << SPI2X);
		break;
		
		case 1: //DIV4
			SPCR &= ~(1 << SPR0);
			SPCR &= ~(1 << SPR1);
			SPSR &= ~(1 << SPI2X);
		break;
		
		case 2: //DIV8
			SPCR |= (1 << SPR0);
			SPCR &= ~(1 << SPR1);
			SPSR |= (1 << SPI2X);
		break;
		
		case 3: //DIV16
			SPCR |= (1 << SPR0);
			SPCR &= ~(1 << SPR1);
			SPSR &= ~(1 << SPI2X);
		break;
		
		case 4: //DIV32
			SPCR &=~(1 << SPR0);
			SPCR |= (1 << SPR1);
			SPSR |= (1 << SPI2X);
		break;
		
		case 5: //DIV64
			SPCR &= ~(1 << SPR0);
			SPCR |= (1 << SPR1);
			SPSR &= ~(1 << SPI2X);
		break;	
		
		case 6: //DIV12
			SPCR |= (1 << SPR0);
			SPCR |= (1 << SPR1);
			SPSR &= ~(1 << SPI2X);
		break;
		
			
		}
		
		
	} else {
		
		//SLAVE MODE
		
		DDRB |= (1 << PORTB4); // MISO
		DDRB &= ~(	(1 << PORTB3)|(1 << PORTB5)|(1 << PORTB2)	); // MOSI, SCK, SS
		
	}
	
	
	SPCR |= (1 << SPE)|DATA_ORDER|CLOCK_POL|CLOCK_PHASE;
	
}



void SPI_Write (uint8_t data){
	
	SPDR = data;
	
}

uint8_t SPI_Read(void){
	
	while (	!(SPSR & (1 << SPIF))	);
	return (SPDR);
}


void SPI_SS_control(uint8_t controlador){
	
	
	switch (controlador){
		
	case 0: PORTB &= ~(1 << PORTB2); // Encender SS
	break;
	
	case 1: PORTB |= (1 << PORTB2); // Apagar SS
	break;
	
	default: PORTB &= ~(1 << PORTB2); // Por default encender
	break;
		
	}
	
	
}

