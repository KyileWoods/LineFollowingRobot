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

void MotorControlSetup() {
	DDRB |= (1 << 7); //set pin B7 as output
	DDRB |= (1 << 5); //set pin B5 as output
	timer1_init();
	timer0_init();
}


/*Timers work like this: There is a certain register (TCNT0->TimerCouNTer0)(8/16 bits) which increases automatically, but can also be written to (and begins couting up again)
A second register (OutputCompareRegisterA) holds a value which is constantly compared against the timer. If the timer exceeds the value, the timer can be reset to zero.

*/


void timer0_init() {


	//set up timer0 to output fast PWM, 256 prescaler, clear on compare match, set on top

	DDRB |= (1 << 7);
	DDRD |= (1 << 0);
	TCCR0A |= (1 << 7) | (1 << 5) | (1 << 1) | 1;
	TIMSK0 |= (1 << 0); //enable overflow interrupt
	OCR0A = 0;
	OCR0B = 0;
	TCCR0B |= (1 << 2); // xxxxxnnn->prescale by: 1=1;2=64,3=256 {100=3}

}
//ISR(TIMER0_OVF_vect) { }//interrupt called when timer0 overflows


void timer1_init() {

	//set up timer1 to output fast PWM (8-bit), 256 prescaler, clear on compare match, set on top

	DDRB |= (1 << 6) | (1 << 5);
	TCCR1A |= (1 << 7) | (1 << 5) | 1;
	TCCR1B |= (1 << 3);
	TIMSK1 |= (1 << 0); ////enable overflow interrupt
	OCR1A = 0;
	OCR1B = 0;
	TCCR1B |= (1 << 2);

}
//ISR(TIMER1_OVF_vect) {} //interrupt called when timer1 overflows

int read_LED(LED_number) {

	ADMUX = 0x00;
	ADCSRB = 0x00;
	ADMUX |= (1 << REFS1) | (1 << REFS0) | (1 << ADLAR);
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	switch (LED_number) {
	case 1:  //ADC4
		ADMUX |= (1 << ADMUX2); //Could also be written as:		  ADMUX |= 4;
		break;
	case 2:  //ADC5
		ADMUX |= (1 << ADMUX2) | (1 << ADMUX0);				    //ADMUX |= 5;
		break;
	case 3:  //ADC6
		ADMUX |= (1 << ADMUX2) | (1 << ADMUX1);					//ADMUX |= 6;
		break;
	case 4:  //ADC7
		ADMUX |= (1 << ADMUX2) | (1 << ADMUX1) | (1 << ADMUX0); //ADMUX |= 7;
		break;

	case 5:  //ADC11
		ADCSRB |= (1 << ADMUX5);
		ADMUX |= (1 << ADMUX1) | (1 << ADMUX0);
		break;

	case 6:  //ADC10
		ADCSRB |= (1 << ADMUX5);
		ADMUX |= (1 << ADMUX1);
		break;

	case 7:  //ADC9
		ADCSRB |= (1 << ADMUX5);
		ADMUX |= (1 << ADMUX0);
		break;

	case 8:  //ADC8
		ADCSRB |= (1 << ADMUX5);
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