//Main.

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



int16_t LED_values[8];
int LED1;
int LED2;
int LED3;
int LED4;
int LED5;
int LED6;
int LED7;
int LED8;

int16_t sum_LED_values=0;
int16_t weighted_sum_LED_values=0;

int32_t error=0;
int16_t kp=1;
int16_t proportional=0;

int16_t base_speed = 40;
//base_speed = (3500*kp)/10000; //duty cycle /255


int16_t sumcheck =1;

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
		sum_LED_values = 0;
		weighted_sum_LED_values=0;

		
		LED1 = read_LED(1);
		LED2 = read_LED(2);
		LED3 = read_LED(3);
		LED4 = read_LED(4);
		LED5 = read_LED(5);
		LED6 = read_LED(6);
		LED7 = read_LED(7);
		LED8 = read_LED(8);

		sum_LED_values = LED1+LED2+LED3+LED4+LED5+LED6+LED7+LED8;
		weighted_sum_LED_values = LED2+2*LED3+3*LED4+4*LED5+5*LED6+6*LED7+7*LED8;

	error = ((1000*weighted_sum_LED_values) / sum_LED_values)-3500; //The Thousands are there to maintain decimal places during division! [ 12/2=6 ];[ 1.2/2=0 ]
																	// error-> 0+/-3500
	proportional = (kp*error)/10000; //~35


	OCR0A = base_speed-proportional; //Not sure yet which one should gain the error, which subtracts. We should #define these as 'rightMotor" /'leftMotor"
	OCR1A = base_speed+proportional;
	}
	return 0;
}
	