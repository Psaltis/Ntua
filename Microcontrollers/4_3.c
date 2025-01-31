#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#define DEL 1000U
float apotelesma1;
unsigned int AD;
unsigned char Monades;
unsigned char Dekata;
unsigned char Ekatosta;
unsigned char Print;
void write_2_nibbles(unsigned char x){
	unsigned char y;
	y=x;
	unsigned char z;
	z=PIND;
	z=z & (0x0F);
	x=x & (0xF0);
	x=x+z;
	PORTD=x;
	PORTD=PORTD|8; //NA TO DOYME
	PORTD=PORTD&(0b11110111);
	x=y;
	x=x<<4;
	x=x & (0xF0);
	x=x+z;    
	PORTD=x;
	PORTD=PORTD|8; //NA TO DOYME
	PORTD=PORTD&(0b11110111);
}
void lcd_data(unsigned char w){
	PORTD=PORTD|4;
	write_2_nibbles(w);
 	_delay_us(100);
}
void lcd_command(unsigned char b){
	PORTD=PORTD & (0b11111011);
	write_2_nibbles(b);
 	_delay_us(100);
}
void lcd_init(void){
	unsigned char m;
	_delay_ms(100);
	PORTD=0x30;
	PORTD=PORTD|8; //NA TO DOYME
	PORTD=PORTD&(0b11110111);
	_delay_us(100);
	PORTD=0x30;
	PORTD=PORTD|8; //NA TO DOYME
	PORTD=PORTD&(0b11110111);
	_delay_us(100);
	PORTD=0x20;
	PORTD=PORTD|8; //NA TO DOYME
	PORTD=PORTD&(0b11110111);
	_delay_us(100);
	m=0x28;
	lcd_command(m);
	m=0x0c;
	lcd_command(m);
	m=0x01;
	lcd_command(m);
	_delay_us(4000);
	m=0x06;
	lcd_command(m);
}

int main(void){
	DDRD=0xFF;
	DDRB=0xFF;
	DDRC=0x00;
	ADMUX=0b01000011;
	ADCSRA=0b10000111;
	lcd_init();
	while(1){
    	lcd_command(0x01);
    	_delay_us(4000);
    	ADCSRA|=(1<<ADSC);
    	while((ADCSRA & 0x40) ==0x40);  	//Wait for the ADSC to become 0
    	AD=ADC;                         	//Read the ADC value
    	apotelesma1=AD*(0.0048828125);  	//Calculate the Vin value
    	//Check which level includes the value calculated
    	if(apotelesma1<1){
        	lcd_data('C');
        	lcd_data('l');
        	lcd_data('e');
        	lcd_data('a');
        	lcd_data('r');
        	_delay_ms(500);
    	}
    	else if(apotelesma1<1.8){
     	 
        	PORTB=0x01;
        	_delay_ms(50);
        	PORTB=0x00;
        	_delay_ms(50);
  	 
        	lcd_data('G');
        	lcd_data('a');
        	lcd_data('s');
        	lcd_data(' ');
        	lcd_data('D');
        	lcd_data('e');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('c');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('d');
        	_delay_ms(500);
    	}
    	else if(apotelesma1<2.6)
    	{
        	PORTB=0x03;
        	_delay_ms(50);
        	PORTB=0x00;
        	_delay_ms(50);
        	lcd_data('G');
        	lcd_data('a');
        	lcd_data('s');
        	lcd_data(' ');
        	lcd_data('D');
        	lcd_data('e');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('c');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('d');
        	_delay_ms(500);
    	}
    	else if(apotelesma1<3.2)
    	{
        	PORTB=0x07;
        	_delay_ms(50);
        	PORTB=0x00;
        	_delay_ms(50);
        	lcd_data('G');
        	lcd_data('a');
        	lcd_data('s');
        	lcd_data(' ');
        	lcd_data('D');
        	lcd_data('e');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('c');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('d');
        	_delay_ms(500);
    	}
    	else if(apotelesma1<4)
    	{
        	PORTB=0x0F;
        	_delay_ms(50);
        	PORTB=0x00;
        	_delay_ms(50);
        	lcd_data('G');
        	lcd_data('a');
        	lcd_data('s');
        	lcd_data(' ');
        	lcd_data('D');
        	lcd_data('e');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('c');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('d');
        	_delay_ms(500);
    	}
    	else if(apotelesma1<4.8)
    	{
        	PORTB=0x1F;
        	_delay_ms(50);
        	PORTB=0x00;
        	_delay_ms(50);
        	lcd_data('G');
        	lcd_data('a');
        	lcd_data('s');
        	lcd_data(' ');
        	lcd_data('D');
        	lcd_data('e');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('c');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('d');
        	_delay_ms(500);
    	}
    	else
    	{
        	PORTB=0x3F;
        	_delay_ms(50);
        	PORTB=0x00;
        	_delay_ms(50);
        	lcd_data('G');
        	lcd_data('a');
        	lcd_data('s');
        	lcd_data(' ');
        	lcd_data('D');
        	lcd_data('e');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('c');
        	lcd_data('t');
        	lcd_data('e');
        	lcd_data('d');
        	_delay_ms(500);
    	}   
	}
}