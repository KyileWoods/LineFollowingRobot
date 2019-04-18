#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/portpins.h>
//#include <adc.h>
void timer0_init(){
	//set up timer0 to output fast PWM, 256 prescaler, clear on compare match, set on top
	DDRB |= (1<<7);
	DDRD |= (1<<0);
	TCCR0A |= (1<<7)|(1<<5)|(1<<1)|1;
	TIMSK0 |= (1<<0); //enable overflow interrupt
	OCR0B = 0;
	TCCR0B |= (1<<2);
	
}

void timer1_init(){
	//set up timer1 to output fast PWM (8-bit), 256 prescaler, clear on compare match, set on top
	DDRB |= (1<<6)|(1<<5);
	TCCR1A |= (1<<7)|(1<<5)|1;
	TCCR1B |= (1<<3);
	TIMSK1 |= (1<<0); ////enable overflow interrupt
	OCR1B = 0;
	TCCR1B |= (1<<2);
}

void ADC_trim_init(){
    ADMUX = (0xE0); //Sets the reference to the internal 2.56V and left adjusts the results
    ADCSRA = (0x87); // Enables the ADC conversion and sets the prescaler to 128
	DDRB |= (1<<3); //This and the next line turns on the IR LED of the sensor board.
	PORTB |= (1<<3);
}

// Reads ADC4 which is connected to sensor 1 on sensor board
uint16_t adc_read1 (){
    ADCSRB =(ADCSRB & 0b11011111); //clears MUX5
    ADMUX = (ADMUX & 0xF8); //clears the botom 3 bits of the ADMUX register before oring the channel to be converted
    ADMUX |=(0b00000100);
    ADCSRA |= (1<<6);   // sets a flag to start a single conversion of the value on ch ADC channel
    while (ADCSRA  & (1<<6)){}
    //_delay_ms(100);
    return ADCH;
}

// Reads ADC5 which is connected to sensor 2 on sensor board
uint16_t adc_read2 (){
    ADCSRB =(ADCSRB & 0b11011111); //clears MUX5
    ADMUX = (ADMUX & 0xF8); //clears the botom 3 bits of the ADMUX register before oring the channel to be converted
    ADMUX |=(0b00000101);
    ADCSRA |= (1<<6);   // sets a flag to start a single conversion of the value on ch ADC channel
    while (ADCSRA  & (1<<6)){}
    //_delay_ms(100);
    return ADCH;
}

// Reads ADC6 which is connected to sensor 3 on sensor board
uint16_t adc_read3 (){
    ADCSRB =(ADCSRB & 0b11011111); //clears MUX5
    ADMUX = (ADMUX & 0xF8); //clears the botom 3 bits of the ADMUX register before oring the channel to be converted
    ADMUX |=(0b00000110);
    ADCSRA |= (1<<6);   // sets a flag to start a single conversion of the value on ch ADC channel
    while (ADCSRA  & (1<<6)){}
    return ADCH;
}

// Reads ADC7 which is connected to sensor 4 on sensor board
uint16_t adc_read4 (){
    ADCSRB =(ADCSRB & 0b11011111); //clears MUX5
    ADMUX = (ADMUX & 0xF8); //clears the botom 3 bits of the ADMUX register before oring the channel to be converted
    ADMUX |=(0b00000111);
    ADCSRA |= (1<<6);   // sets a flag to start a single conversion of the value on ch ADC channel
    while (ADCSRA  & (1<<6)){}
    return ADCH;
}

// Reads ADC11 which is connected to sensor 5 on sensor board
uint16_t adc_read5 (){
    ADCSRB |=(0b00100000); //Sets MUX5
    ADMUX = (ADMUX & 0xF8); //clears the botom 3 bits of the ADMUX register before oring the channel to be converted
    ADMUX |=(0b00000011);
    ADCSRA |= (1<<6);   // sets a flag to start a single conversion of the value on ch ADC channel
    while (ADCSRA  & (1<<6)){}
    return ADCH;
}

// Reads ADC10 which is connected to sensor 6 on sensor board
uint16_t adc_read6 (){
    ADMUX = (ADMUX & 0xF8); //clears the botom 3 bits of the ADMUX register before oring the channel to be converted
    ADMUX |=(0b00000010);
    ADCSRB |=(0b00100000); //Sets MUX5
    ADCSRA |= (1<<6);   // sets a flag to start a single conversion of the value on ch ADC channel
    while (ADCSRA  & (1<<6)){}
    return ADCH;
}


// Reads ADC9 which is connected to sensor 7 on sensor board
uint16_t adc_read7 (){
    ADMUX = (ADMUX & 0xF8); //clears the botom 3 bits of the ADMUX register before oring the channel to be converted
    ADMUX |=(0b00000001);
    ADCSRB |=(0b00100000);
    ADCSRA |= (1<<6);   // sets a flag to start a single conversion of the value on ch ADC channel
    while (ADCSRA  & (1<<6)){}
    return ADCH;
}

// Reads ADC8 which is connected to sensor 8 on sensor board
uint16_t adc_read8 (){
    ADMUX = (ADMUX & 0xF8); //clears the botom 3 bits of the ADMUX register before oring the channel to be converted
    ADMUX |=(0b00000000); // Not really necessary
    ADCSRB |=(0b00100000); //Sets MUX5
    ADCSRA |= (1<<6);   // sets a flag to start a single conversion of the value on ch ADC channel
    while (ADCSRA  & (1<<6)){}
    return ADCH;
}

// Pin C7 sw0
//Pin C6 SW1

uint16_t centre_left;
uint16_t centre_right;

void button_setup(void){
    DDRC = (0<<6); //Sets pin C6 as an input connected to SW1
    DDRB |= (1<<1); //Sets Pin 1 on PORTB as output. 
}


void centre_normalise(void){
    int y=0;
    while(PINC == 0x00){ //Waits for one of the buttons to be pushed
    }
    PORTB |=(1<<1);     //Turns on the LED
    centre_right = 0; //Zeros the 
    centre_left = 0;
    int x=0;
        for(x = 0; x < 5 ; x++){
            centre_right += adc_read2();
            centre_left += adc_read7();
            }
    centre_left = centre_left/5;
    centre_right = centre_right/5;
    _delay_ms(2000);
    PORTB = (PORTB & 0b11111101);
}

void setup(){
    timer0_init();
    timer1_init();
    ADC_trim_init();
    button_setup();
}



void main(){
    setup();
    centre_normalise();
    _delay_ms(5000);
    
    while(1){
        int MAX_SPEED = 35;
        int CORRECT_SPEED = 35;
        //int current2 = (adc_read2() - white2);
        //int current7 = (adc_read7()-white7);
        //int diff = current2-current7;
        int left_adc = adc_read7();
        int right_adc = adc_read2();
        int left_speed = MAX_SPEED+((left_adc-centre_left)/3);
        int right_speed = MAX_SPEED+((right_adc-centre_right)/3);

            OCR0A = (left_speed); //this drives motor 2 or the left wheel
            OCR1A =(right_speed); //this drives motor 1 or the right wheel
        }
}