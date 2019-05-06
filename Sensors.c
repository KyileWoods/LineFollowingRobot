#include "Sensors.h"

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
		ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0);   //MUX |= 7;
		break;

	case 8:  //ADC11
		ADCSRB |= (1 << MUX5);
		ADMUX |= (1 << MUX1) | (1 << MUX0);
		break;

	case 7:  //ADC10
		ADCSRB |= (1 << MUX5);
		ADMUX |= (1 << MUX1);
		break;

	case 6:  //ADC9
		ADCSRB |= (1 << MUX5);
		ADMUX |= (1 << MUX0);
		break;

	case 5:  //ADC8
		ADCSRB |= (1 << MUX5);
		//No ADMUX bits for ADC8
		break;

	}

	//Start the conversion, wait for it to be done, return the result
	ADCSRA |= (1 << ADSC);
	while (~ADCSRA &(1 << ADIF)) {}
	return ADCH;

}