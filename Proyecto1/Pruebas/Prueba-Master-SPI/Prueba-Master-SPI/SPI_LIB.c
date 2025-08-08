/*
 * SPI_LIB.c
 *
 * Created: 6/08/2025 22:02:27
 *  Author: aleja
 */ 

#include "SPI_LIB.h"

void SPI_init(Spi_Type sType, Spi_Data_Order sDataOrder, Spi_Clock_Polarity sClockPolarity, Spi_Clock_Phase sClockPhase){
	
	
	/* PINOUT
	
	PB2 -> SS
	PB3 -> MOSI
	PB4 -> MISO
	PB5 -> SCK
	
	*/
	
	if (sType & (1 << MSTR)) //If master mode
	{
		DDRB |= (1 << PB3)|(1 << PB5)|(1 << PB2); //MOSI, SCK, NOT_SS
		DDRB &= ~(1 << PB4); //MISO
		SPCR |= (1 << MSTR); // Master EN
		
		uint8_t temp = sType & 0b00000111;
		
		switch (temp){
			
			case 0: //DIV2
			SPCR &= ~(1 << SPR0);
			SPCR &= ~(1 << SPR1);
			SPSR |= (1 << SPI2X);
			break;
			
			case 1: // DIV4
			SPCR &= ~(1 << SPR0);
			SPCR &= ~(1 << SPR1);
			SPSR &= ~(1 << SPI2X);
			break;
			
			case 2: //DIV8
			SPCR |= (1 << SPR0);
			SPCR &= ~(1 << SPR1);
			SPSR |= (1 << SPI2X);
			break;
			
			case 3: // DIV16
			SPCR |= (1 << SPR0);
			SPCR &= ~(1 << SPR1);
			SPSR &= ~(1 << SPI2X);
			break;
			
			case 4: //DIV32
			SPCR &= ~(1 << SPR0);
			SPCR |= (1 << SPR1);
			SPSR |= (1 << SPI2X);
			break;
			
			case 5: //DIV64
			SPCR &= ~(1 << SPR0);
			SPCR |= (1 << SPR1);
			SPSR &= ~(1 << SPI2X);
			break;
			
			case 6: //DIV128
			SPCR |= (1 << SPR0);
			SPCR |= (1 << SPR1);
			SPSR &= ~(1 << SPI2X);
			break;
			
					 }
		
		
	} else {
		
		//If slave mode
		
		DDRB |= (1 << PB4); //MISO
		DDRB &= ~(	(1 <<  PB3)|(1 << PB5)|(1 << PB2)	); //MOSI, SCK, SS
		SPCR &= ~(1 << MSTR); // Slave
		
	}
	
	//Enable SPI, DataOrder, CLK POL, CLK PHA
	
	SPCR |= (1 << SPE)|sDataOrder|sClockPolarity|sClockPhase;
	
	
}

static void SPI_read_wait(){
	
	while (	!(SPSR & (1 << SPIF))	); //Wait for Data Recive complete
	
}

void SPI_write(uint8_t dato){
	
	SPDR = dato; //Write data to SPI bus
	
}

unsigned SPI_DataReady(){
	
	//Check for data to be ready to read
	
	if(SPSR & (1 << SPIF))
	return 1;
	else
	return 0;
	
}

uint8_t SPI_read(void){
	
	
	while (!(SPSR & (1 << SPIF))); //Wait for Data receive complete
	return(SPDR); // Read the received data from buffer
	
}