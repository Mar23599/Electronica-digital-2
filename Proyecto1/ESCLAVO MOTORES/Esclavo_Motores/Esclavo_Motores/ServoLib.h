/*
 * ServoLib.h
 *
 * Created: 26/08/2025 12:18:26
 *  Author: aleja
 */ 


#ifndef SERVOLIB_H_
#define SERVOLIB_H_


#include <avr/io.h>
#include <util/delay.h>

void servo_init(void);
void servo_0_grados(void);
void servo_175_grados(void);


#endif /* SERVOLIB_H_ */