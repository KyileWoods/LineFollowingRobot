
MAIN = main

FLASH.bin : $(MAIN).elf
	avr-objcopy -O binary $(MAIN).elf FLASH.bin

$(MAIN).elf : $(MAIN).o Motors.o Sensors.o MiscFuncs.o
	avr-gcc -mmcu=atmega32u4 $(MAIN).o Motors.o Sensors.o MiscFuncs.o -o $(MAIN).elf

$(MAIN).o : $(MAIN).c Motors.h makefile
	avr-gcc -O1 -mmcu=atmega32u4 -c $(MAIN).c -o $(MAIN).o -std=c99

Motors.o : Motors.c
	avr-gcc -O1 -mmcu=atmega32u4 -c Motors.c -o Motors.o -std=c99

Sensors.o : Sensors.c
	avr-gcc -O1 -mmcu=atmega32u4 -c Sensors.c -o Sensors.o	-std=c99

MiscFuncs.o : MiscFuncs.c
	avr-gcc -O1 -mmcu=atmega32u4 -c MiscFuncs.c -o MiscFuncs.o  -std=c99