//Main.

#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "Motors.h"
#include "Sensors.h"
#include "MiscFuncs.h"
/*The following definitions allow you to compare two sets of bit (any size)
and bit-wise operate on specified bits, and the result is returned.
Eg: if p=10011010, m=2, then bit_set(p,m)returns 10011110
p=port...m = pinNumber*/
#define bit_get(p,m) ((p) & (1<<m))
#define bit_set(p,m) ((p) |= (1<<m))
#define bit_clear(p,m) ((p) &= ~(1<<m))
#define bit_flip(p,m) ((p) ^= (1<<m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))



//Variables
int balancer = 0;
int left_advance = 0;
int right_advance = 0;

//Constants
int wheel_width_mm = 140;
int wheel_circumference_mm = 115;



int16_t LED_values[2];
int16_t sum_LED_values=0;
int16_t weighted_sum_LED_values=0;
int16_t error=0;
int16_t proportional=0;
int16_t kp=500;
int16_t steering_correction=0;

int base_speed = 45; //duty cycle /255

int main() {
	 //For safety: flash and wait for some amount of seconds
	shutter(30,20);
	_delay_ms(2000); 
	 
	 
	 //Turn on the IR-emittors
	DDRB |= (1 << 3);
	PORTB |= (1 << 3);
	
	PortDasBinary(0);
			


	MotorControlSetup();

	while (1) {
			for (int i = 0; i <= 1; i++) {
				LED_values[i] = read_LED(i+4);
			}
		proportional =abs( kp*(LED_values[0]-LED_values[1]));
		
		if(LED_values[0]>LED_values[1]){
			OCR1A = 45-(proportional)/1000;
			OCR0A = 45+(proportional)/1000;
		}
		else{
			OCR1A = 45+(proportional)/1000;
			OCR0A = 45-(proportional)/1000;
		}
	}
	return 0;
}