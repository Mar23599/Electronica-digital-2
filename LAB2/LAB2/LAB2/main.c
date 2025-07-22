/*
 * LAB2.c
 *
 * Created: 17/07/2025 22:44:26
 * Author : aleja
 */ 

#include <avr/io.h>
#include "Ale-lib-LCD8.h" // Libreria para pantalla LCD
#include "Ale-Lib-ADC.h" // Libereria para ADC


int main(void)
{
	
	LCD_init8(); // Inicializar la pantalla LCD
	LCD_command_cursor(0,0); // Colocar el cursor en (0,0)
	LCD_PORT_str("LOL O KKK");
	LCD_command_cursor(1,0); // Colocar el cursor en (0,0)
	LCD_PORT_str("SIUUUUU");
	
	
    
    while (1) 
    {
		
    }
}

