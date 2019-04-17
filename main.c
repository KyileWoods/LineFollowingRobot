//Main

#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

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



void BPin_On(char pin) {
	PORTB |= (1 << pin);
}
void BPin_Off(char pin) {
	PORTB &= ~(1 << pin);
}
void BPin_Toggle(char pin) {
	PORTB ^= (1 << pin);
}




int LEDS[8];
int diffs[4];
int base_speed = 20; //duty cycle /255


int16_t LED_values[8];
int16_t sum_LED_values;
int16_t weighted_sum_LED_values;
int16_t error;
int16_t steering_correction;
int main() {
	 
	 //Turn on the IR-emittors
	DDRB |= (1 << 3);
	PORTB |= (1 << 3);
	
	PortDasBinary(0);
	
	//For safety: flash and wait for 2 seconds
	shutter(30,20);
	_delay_ms(2000); 

	MotorControlSetup();

	while (1) {
		sum_LED_values = 0;
		for (int i = 1; i <= 8; i++) {
			LED_values[i - 1] = read_LED(i);
			sum_LED_values = sum_LED_values + LED_values[i - 1];
		}

		for (int i = 0; i < 8; i++) {
			weighted_sum_LED_values = (i)*LED_values[i];
		}
			/*=[  1000*(0<7)-(7/2) ] */
		error = 1000*(weighted_sum_LED_values / sum_LED_values)-3500;
		steering_correction = error/10; //normalise to 255
		steering_correction = steering_correction/10; //normalise to 255
	OCR0A = base_speed+error; //Not sure yet which one should gain the error, which subtracts.
	OCR1A = base_speed-error;	
	}
	return 0;
}