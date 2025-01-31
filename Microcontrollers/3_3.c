#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//Table with OCR1A values
int ocr1a_values[13]={5,26,46,67,87,108,128,148,169,189,210,230,251};
int mode = 1;   //int for switching modes
int DC_VALUE;   // we will save the values in here
int main(){
    int counter=6;
    TCCR1A = (1<<WGM10) | (1<<COM1A1);      //Set TMR1A in fast PWM 8 bit mode with non inverted output 
    TCCR1B = (1<<WGM12) | (1<<CS11);        //and prescale 8
    DDRB=0b00111111;                        //Set PORTB as output
    DDRD=0b00111001;                                 //Set PD7, PD6, PD2, PD1 as inputs, we don't care about the rest
    ADMUX = 0b01100000;    // we will use the ADC0 ADMUX[3:0]= 0000
    // AVCC with external capacitor at AREF pin REFSn[1:0]=01 and left adjust
    //enable ADC
    ADCSRA = 0b10000000;
    OCR1AL=ocr1a_values[counter];                  //Initial Value of OCR1A is the 6th-50% Duty Cycle (128)
    while(1){
        if((PIND & 0b11000000) == 0b10000000){
            mode = 1;
        }
        else if((PIND & 0b11000000) == 0b01000000){
            mode = 2;
        }
        if(mode == 1){
            if((PIND & 0b00000110) == 0b100){
                //increase
                if(counter != 12){
                    counter++;
                    DC_VALUE = ocr1a_values[counter];
                    OCR1AL = DC_VALUE;
                    _delay_ms(100);
                }
            }
            else if((PIND & 0b00000110) == 0b10){
                //decrease
                if(counter != 0){
                    counter--;
                    DC_VALUE = ocr1a_values[counter];
                    OCR1AL = DC_VALUE;
                    _delay_ms(100);
                }
            }
        }
        else{
            //mode2
            ADCSRA = 0b11000000;
           while((ADCSRA & 0b00010000)!= 0b0001000){};
            DC_VALUE = ADCH;
            OCR1AL = DC_VALUE;
            _delay_ms(100);
        }
    }
}