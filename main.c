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


int32_t LED_values[8];
uint32_t LED1;
uint32_t LED2;
uint32_t LED3;
uint32_t LED4;
uint32_t LED5;
uint32_t LED6;
uint32_t LED7;
uint32_t LED8;

int32_t sum_LED_values=0;
int32_t weighted_sum_LED_values=0;
int32_t Q = (5000-4000);

int32_t error=0;
int32_t kp=10;
int32_t proportional=0;
int32_t minimum_read = 180;

int base_speed = 25;
int motor1;
int motor2;

int main() {

	 	//For safety: flash and wait for some amount of seconds
	shutter(30,20);
	_delay_ms(2000); 
	 
	 
	//Turn on the IR-emittors
	DDRB |= (1 << 3) |(1 << 1);
	PORTB |= (1 << 3);
	
	PortDasBinary(0);
			


	MotorControlSetup();

	while (1) {
		sum_LED_values = 0;
		weighted_sum_LED_values=0;

		LED1=0;
		LED2=0;
		LED3=0;
		LED4=0;
		LED5=0;
		LED6=0;
		LED7=0;
		LED8=0;

		
		LED1 = read_LED(1)-minimum_read;
		LED2 = read_LED(2)-minimum_read;
		LED3 = read_LED(3)-minimum_read;
		LED4 = read_LED(4)-minimum_read;
		LED5 = read_LED(5)-minimum_read;
		LED6 = read_LED(6)-minimum_read;
		LED7 = read_LED(7)-minimum_read;
		LED8 = read_LED(8)-minimum_read;


	

		weighted_sum_LED_values = (/*(1*LED2)+(2*LED3)+*/(3*LED4)/*+(4*LED5)*/+(5*LED6)/*+(6*LED7)+(7*LED8)*/)*1000; //MAX= 385*1000
		sum_LED_values = /*LED1+LED2+LED3+*/LED4+/*LED5*/+LED6;//+LED7+LED8;				//MAX=2040
																					//Highest Q=255
		

	error = (weighted_sum_LED_values /(sum_LED_values))-1500;//MAX==3500 //The Thousands are there to maintain decimal places during division! [ 12/2=6 ];[ 1.2/2=0 ]
				 			 	                    				    // error-> 0+/-3500
	

	proportional = kp*(error/1500)*(base_speed/2); //+/-~35
	//PORTB ^= (1<<1);


	motor1 = 1+ base_speed - proportional; //1+base_speed - proportional;//OCR0A shoud increase as the error becomes positive. We should #define these as 'rightMotor" /'leftMotor"
	motor2 = 1+ base_speed + proportional; //1+base_speed + proportional;

	if(motor1<=0 |motor2 <=0){shutter(20,20);}

	//if(proportional<=0){shutter(10,10);}
	
/*	if (motor1 > 255){
		motor1 = 255;
	}
	else if (motor1 < 0){
		motor1 = 0;
	}

	if (motor2 > 255){
		motor2 = 255;
	}
	else if (motor2 < 0){
		motor2 = 0;
	}*/
	OCR0A = motor1;
	OCR1A = motor2;
	
/*int sensor_out = LED5;
if(sensor_out >= 0 & sensor_out < 17){
		PORTE |=(1<<6);
		PORTB &= ~(1<<0);
		PORTB &= ~(1<<1);
		PORTB &= ~(1<<2);
	}
	else if(sensor_out >= 17 & sensor_out < 34){
		PORTE &= ~(1<<6);
		PORTB |= (1<<0);
		PORTB &= ~(1<<1);
		PORTB &= ~(1<<2);
	}
	else if(sensor_out >= 34 & sensor_out < 51){
		PORTE |= (1<<6);
		PORTB |= (1<<0);
		PORTB &= ~(1<<1);
		PORTB &= ~(1<<2);
	}
	else if(sensor_out >= 51 & sensor_out < 68){
		PORTE &= ~(1<<6);
		PORTB &= ~(1<<0);
		PORTB |= (1<<1);
		PORTB &= ~(1<<2);
	}
	else if(sensor_out >= 68 & sensor_out < 85){
		PORTE |= (1<<6);
		PORTB &= ~(1<<0);
		PORTB |= (1<<1);
		PORTB &= ~(1<<2);
	}
	else if(sensor_out >= 85 & sensor_out < 102){
		PORTE &= ~(1<<6);
		PORTB |= (1<<0);
		PORTB |= (1<<1);
		PORTB &= ~(1<<2);
	}
	else if(sensor_out >= 102 & sensor_out < 119){
		PORTE |=(1<<6);
		PORTB |= (1<<0);
		PORTB |= (1<<1);
		PORTB &= ~(1<<2);
	}
	else if(sensor_out >= 119 & sensor_out < 136){
		PORTE &= ~(1<<6);
		PORTB &= ~(1<<0);
		PORTB &= ~(1<<1);
		PORTB |= (1<<2);
	}
	else if(sensor_out >= 136 & sensor_out < 153){
		PORTE |=(1<<6);
		PORTB &= ~(1<<0);
		PORTB &= ~(1<<1);
		PORTB |= (1<<2);
	}
	else if(sensor_out >= 153 & sensor_out < 170){
		PORTE &= ~(1<<6);
		PORTB |= (1<<0);
		PORTB &= ~(1<<1);
		PORTB |= (1<<2);
	}
	else if(sensor_out >= 170 & sensor_out < 187){
		PORTE |= (1<<6);
		PORTB |= (1<<0);
		PORTB &= ~(1<<1);
		PORTB |= (1<<2);

	}
	else if(sensor_out >= 187 & sensor_out < 204){
		PORTE &= ~(1<<6);
		PORTB &= ~(1<<0);
		PORTB |= (1<<1);
		PORTB |= (1<<2);

	}
	else if(sensor_out >= 204 & sensor_out < 221){
		PORTE |= (1<<6);
		PORTB &= ~(1<<0);
		PORTB |= (1<<1);
		PORTB |= (1<<2);
		
	}
	else if(sensor_out >= 221 & sensor_out < 238){
		PORTE &= ~(1<<6);
		PORTB |= (1<<0);
		PORTB |= (1<<1);
		PORTB |= (1<<2);
		
	}
	else if(sensor_out >= 238 & sensor_out < 255){
		PORTE |= (1<<6);
		PORTB |= (1<<0);
		PORTB |= (1<<1);
		PORTB |= (1<<2);
		
	}
	*/
}
	return 0;
}
	