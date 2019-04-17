#define F_CPU 16000000UL 
#include <util/delay.h>
#include "MiscFuncs.h"



void shutter(int time, int flashes) {
	//simply to indicate the passage through a certain point of code

	for (int i = 0; i < flashes; i++) {
		PORTB ^= 0B11111111;
		PORTC ^= 0B11111111;
		PORTD ^= 0B11111111;
		PORTE ^= 0B11111111;
		_delay_ms(time);
		PORTB ^= 0B00000000;
		PORTC ^= 0B00000000;
		PORTD ^= 0B00000000;
		PORTE ^= 0B00000000;
		_delay_ms(time);
	}
}

void PortDasBinary(int OnOrOff){
	if(OnOrOff >=1){
		DDRD = 0XFF;
	}
	else{DDRD |=0;}
}

void counterD(int time){
	for(int i=0;i<8;i++){
		PORTD =(1<<i);
		_delay_ms(time);
	}
}