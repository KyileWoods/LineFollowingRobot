#ifndef MOTORS_H
#define MOTORS_H

#include <avr/io.h>

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

void MotorControlSetup() {
	DDRB |= (1 << 7); //set pin B7 as output
	DDRB |= (1 << 5); //set pin B5 as output
	timer1_init();
	timer0_init();
}

#endif