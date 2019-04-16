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



int delayer = 0;
int which_way = 1;
int LEDS[8];
int diffs[4];
int white_threshold = 240;
int base_speed = 50; //duty cycle /255


int main(){

	DDRB |= (1 << 3);

	PORTB |= (1 << 3); //Turn on the IR-emittors
	
	PortDasBinary(0);
	
	shutter(30,20);
	_delay_ms(2000); //For safety: flash and wait for 2 seconds

	MotorControlSetup();
	
	while(1){
		//-----Get the values of all the LED's
		for (int i = 0; i <= 7; i++) {
			LEDS[i] = read_LED(i+1); //i+1 , beause the 1st LED is assigned to 0th array location, (i-1) and so on.
		}
		
		for (int i=0;i<=7; i++){
		    
		}
		
		for(int i=1;i<=4;i++){ //Diffs may not be necessary to be found, but this is how they'd be foud, anyway.
			diffs[i-1] = LEDS[8-i]-LEDS[i]; //Close in from the edges. Positive values mean  left reading is stronger than right
		}
		
		
		
		//------Assign the motor speed to the motors/timers		
		OCR0A = base_speed+3*diffs[3]+2*diffs[2]+diffs[1]; //Set duty cycle for pin B7 x/255 motor B
		OCR1A = base_speed-3*diffs[3]-2*diffs[2]-diffs[1]; //set duty cycle for pin B5 x/255 motor A

	}

}