#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define DEL 1000U
float apotelesma1;
unsigned int AD;
unsigned char Monades;
unsigned char Dekata;
unsigned char Ekatosta;
unsigned char digits[10]={'0','1','2','3','4','5','6','7','8','9'};
void write_2_nibbles(unsigned char x){
	unsigned char y;
	y=x;
	unsigned char z;
	z=PIND;
	z=z & (0x0F);
	x=x & (0xF0);
	x=x+z;
	PORTD=x;
	PORTD=PORTD|8;
	PORTD=PORTD&(0b11110111);
	x=y;
	x=x<<4;
	x=x & (0xF0);
	x=x+z;    
	PORTD=x;
	PORTD=PORTD|8;
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
	PORTD=PORTD|8;
	PORTD=PORTD&(0b11110111);
	_delay_us(100);
	PORTD=0x30;
	PORTD=PORTD|8;
	PORTD=PORTD&(0b11110111);
	_delay_us(100);
	PORTD=0x20;
	PORTD=PORTD|8;
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
ISR(TIMER1_OVF_vect){
	ADCSRA|=(1<<ADSC);                  	//Set ADSC flag of
	while((ADCSRA & 0x40) == 0x40);      	//Wait until ADCSRA becomes 0
	AD=ADC;                             	//Read ADC value
	apotelesma1=AD*0.0048828;           	//We calculate Vin using the given formula:Vin=ADC*5/1024         	 
	Monades=(unsigned char) apotelesma1;	//By casting we keep the Units and get rid of the decimals    
	apotelesma1=apotelesma1-Monades;    	//We substract the Units from the initial Vin result
	apotelesma1=apotelesma1*10;         	//We multiply by 10 to bring the 1st decimal to the Units' place
	Dekata=(unsigned char) apotelesma1; 	//Using casting again we keep in Dekades the new Units' value
	apotelesma1=apotelesma1-Dekata;     	//Repeating the same to isolate 2nd Decimal
	apotelesma1=apotelesma1*10;
	Ekatosta=(unsigned char) apotelesma1;
	Monades=digits[Monades];            	//We correspond the values to the characters in the Table
	Dekata=digits[Dekata];
	Ekatosta=digits[Ekatosta];
	lcd_command(0x01);                  	//clear screen           	 
	_delay_us(4000);
	lcd_data(Monades);
	Monades='.';
	lcd_data(Monades);
	lcd_data(Dekata);
	lcd_data(Ekatosta);
	_delay_ms(50);
	TCNT1=49910;
	sei();
}
int main(void){                             	//Initialize LCD Screen
	DDRD=0xFF;                              	//Set PORTD as output
	ADMUX=0b01000010;                       	//Initialize ADMUX as before (see assembly code)
	ADCSRA=0b10000111;                      	//Initialize ADCSRA as before (see assembly code)
	TCCR1A=(0<<WGM10) | (1<<COM1A1);
	TCCR1B=(0<<WGM12) | (1<<CS12) | (1<<CS10);
	TIMSK1=(1<<TOIE1);
	TCNT1=49910;
	lcd_init();
	sei();
	while(1);
}