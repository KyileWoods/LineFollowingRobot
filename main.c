/*#define F_CPU 16000000UL
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
*/

#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "Motors.h"
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

void shutter(int time, int flashes) {
	//simply to indicate the passage through a certain point of code

	for (int i = 0; i < flashes; i++) {
		PORTB = 0B11111111;
		PORTC = 0B11111111;
		PORTD = 0B11111111;
		PORTE = 0B11111111;
		_delay_ms(time);
		PORTB = 0B00000000;
		PORTC = 0B00000000;
		PORTD = 0B00000000;
		PORTE = 0B00000000;
		_delay_ms(time);
	}
}


int read_LED(int LED_number) {

	ADMUX = 0x00;
	ADCSRB = 0x00;
	ADMUX |= (1 << REFS1) | (1 << REFS0) | (1 << ADLAR);
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	switch (LED_number) {
	case 1:  //ADC4
		ADMUX |= (1 << MUX2); //Could also be written as:	//MUX |= 4;
		break;
	case 2:  //ADC5
		ADMUX |= (1 << MUX2) | (1 << MUX0);				    //MUX |= 5;
		break;
	case 3:  //ADC6
		ADMUX |= (1 << MUX2) | (1 << MUX1);					//MUX |= 6;
		break;
	case 4:  //ADC7
		ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0); //MUX |= 7;
		break;

	case 5:  //ADC11
		ADCSRB |= (1 << MUX5);
		ADMUX |= (1 << MUX1) | (1 << MUX0);
		break;

	case 6:  //ADC10
		ADCSRB |= (1 << MUX5);
		ADMUX |= (1 << MUX1);
		break;

	case 7:  //ADC9
		ADCSRB |= (1 << MUX5);
		ADMUX |= (1 << MUX0);
		break;

	case 8:  //ADC8
		ADCSRB |= (1 << MUX5);
		//No ADMUX bits for ADC8
		break;

	}

	//Start the conversion, wait for it to be done, return the result
	ADCSRA |= (1 << ADSC);
	while (~ADCSRA &(1 << ADIF)) {}
	return ADCH;

}

int delayer = 0;
int which_way = 1;
int LEDS[8];
int diffs[4];
int white_threshold = 240;
int base_speed = 50; //duty cycle /255

int main(){

	DDRB |= (1 << 3);
	PORTB |= (1 << 3); //Turn on the detector emittors

	DDRD |= 0XFF; //Only if PORTD is used for the LED-board, displaying values you assign (in binary)

	shutter(30,20);
	_delay_ms(2000); //For safety: flash and wait for 2 seconds

	MotorControlSetup();
	// // //sei(); //<<~~This function is MESSING with the entire program. It causes While(1) loops to be avoided entirely. Very unpredictable behaviour
	while(1){
		//-----Get the values of all the LED's
		for (int i = 0; i <= 7; i++) {
			LEDS[i] = read_LED(i+1); //i+1 , beause the 1st LED is assigned to the array location 0, (i-1) and so on.
		}
		
		for(int i=1;i<=4;i++){
			diffs[i-1] = LEDS[i+4]-LEDS[i];
		}
		
		
		//------Assign the motor speed to the motors/timers		
		OCR0A = base_speed+3*diffs[3]+2*diffs[2]+diffs[1]; //Set duty cycle for pin B7 x/255 motor B
		OCR1A = base_speed-3*diffs[3]-2*diffs[2]-diffs[1]; //set duty cycle for pin B5 x/255 motor A
		PORTD = base_speed+balancer;

	}

}