#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
//Table with OCR1A values
int ocr1a_values[13]={5,26,46,67,87,108,128,148,169,189,210,230,251};

int main() {
	int print;
	int counter=6;
	TCCR1A = (1<<WGM10) | (1<<COM1A1);  	//Set TMR1A in fast PWM 8 bit mode with non inverted output
	TCCR1B = (1<<WGM12) | (1<<CS11);    	//and prescale 8                   	//
	DDRD=0xFF;
	DDRB = 0b111;                       	//set the 3 lsb for output. we only care about PB1 and we will use the rest for input to change its brightness
	// we will use the ADC1 ADMUX[3:0]= 0001
	// AVCC with external capacitor at AREF pin REFSn[1:0]=01 and left adjust
	ADMUX = 0b01100001;
	//enable ADC
	ADCSRA = 0b10000000;
	OCR1AL = ocr1a_values[counter];
	print = ADCH; //since it's left adjusted, the 8 most significant bits are in ADCH
	PORTD = print;
	while(1){
  	 
    	if((PINB & 0b011000) == 0b001000){
        	//increase brightness
         	;
        	ADCSRA = 0b11000000;	//start conversion
        	if(counter != 12){
            	counter++;
            	OCR1AL = ocr1a_values[counter];
        	}
            while((ADCSRA & 0b00010000)!= 0b0001000){};
        	print = ADCH;
        	if (print < 31.875) {
            	PORTD = 0b1;
        	}
        	else if(print >= 31.875 && print < 63.75){
            	PORTD = 0b10;
        	}
        	else if(print >= 63.75 && print < 95.625){
            	PORTD = 0b100;
        	}
        	else if(print >= 95.625 && print < 127.5){
            	PORTD = 0b1000;
        	}
        	else if(print >= 127.5 && print < 159.375){
            	PORTD = 0b10000;
        	}
        	else if(print >= 159.375 && print < 191.25){
            	PORTD = 0b100000;
        	}
        	else if(print >= 191.25 && print < 223.125){
            	PORTD = 0b1000000;
        	}
        	else if(print >= 223.125 && print <= 255){
            	PORTD = 0b10000000;
        	}
        	ADCSRA = 0b10000000;
        	_delay_ms(100);
    	}
    	else if((PINB & 0b011000) == 0b010000){
        	//decrease brightness
    
        	ADCSRA = 0b11000000; //start conversion
        	if(counter != 0){
            	counter--;
            	OCR1AL = ocr1a_values[counter];
        	}
            while((ADCSRA & 0b00010000)!= 0b0001000){};
       	print = ADCH;
        	if (print < 31.875) {
            	PORTD = 0b1;
        	}
        	else if(print >= 31.875 && print < 63.75){
            	PORTD = 0b10;
        	}
        	else if(print >= 63.75 && print < 95.625){
            	PORTD = 0b100;
        	}
        	else if(print >= 95.625 && print < 127.5){
            	PORTD = 0b1000;
        	}
        	else if(print >= 127.5 && print < 159.375){
            	PORTD = 0b10000;
        	}
        	else if(print >= 159.375 && print < 191.25){
            	PORTD = 0b100000;
        	}
        	else if(print >= 191.25 && print < 223.125){
            	PORTD = 0b1000000;
        	}
        	else if(print >= 223.125 && print <= 255){
            	PORTD = 0b10000000;
        	}
        	ADCSRA = 0b10000000;
        	_delay_ms(100);
    	}
	}
    
    
}