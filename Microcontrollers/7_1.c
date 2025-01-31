#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <xc.h>

unsigned char one_wire_reset() {
	DDRD |= 0b00010000;
	PORTD &= 0b11101111;
	_delay_us(480);
	DDRD &= 0b11101111;
	PORTD &= 0b11101111;
	_delay_us(100);
	unsigned char temp = PIND;
	_delay_us(380);
	if((temp & 0b00010000) == 0){
    	return 1;
	}
	else{
    	return 0;
	}
}

unsigned char one_wire_receive_bit() {
	DDRD |= 0b00010000;
	PORTD &= 0b11101111;
	_delay_us(2);
	DDRD &= 0b11101111;
	PORTD &= 0b11101111;
	_delay_us(10);
	unsigned char temp = PIND;
	unsigned char res = 1;
	if((temp & 0b00010000) == 0){
    	res = 0;
	}
	_delay_us(49);
	return res;
}
void one_wire_transmit_bit(unsigned char x) {
	DDRD |= 0b00010000;
	PORTD &= 0b11101111;
	_delay_us(2);
	if((x & 0b1) == 1){
    	PORTD |= 0b00010000;
	}
	else {
    	PORTD &= 0b11101111;
	}
	_delay_us(58);
	DDRD &= 0b11101111;
	PORTD &= 0b11101111;
	_delay_us(1);
}
unsigned char one_wire_receive_byte() {
	unsigned char temp;
	unsigned char res = 0;
	for(int i = 0 ; i < 8 ; i++){
    	res = res >> 1 ;
    	temp = one_wire_receive_bit();
    	if(temp != 0){
        	res |= 0x80;
    	}
	}
	return res;
}
void one_wire_transmit_byte(unsigned char x) {
	for(int i = 0 ; i < 8 ; i++){
    	if((x & 0b1) != 0){
        	one_wire_transmit_bit(0b00000001);
    	}
    	else{
        	one_wire_transmit_bit(0b00000000);
    	}
    	x = x >> 1 ;
	}
}
int read_temp(){
	int temprature = 0;
	unsigned char high;
	unsigned char low;
	if(one_wire_reset() == 1){
    	one_wire_transmit_byte(0xCC);
    	one_wire_transmit_byte(0x44);
    	while(one_wire_receive_bit() == 0);
    	one_wire_reset();
    	one_wire_transmit_byte(0xCC);
    	one_wire_transmit_byte(0xBE);
    	_delay_ms(750);
    	low = one_wire_receive_byte();
    	high = one_wire_receive_byte();
    	temprature |= high ;
    	temprature << 8 ;
    	temprature |= low ;
    	if((temprature&0b1111100000000000)==0b1111100000000000){
            	temprature=~temprature+1;
    	}
    	return temprature;
	}
	else {
    	return 0x8000;
	}
}
void main(void) {
	DDRB = 0xFF;
	DDRC = 0xFF;
	int temp;
	unsigned char b;
	unsigned char c;
	while(1){
    	temp = read_temp();
    	c = (unsigned char) temp;
    	temp = temp >> 6;
    	b = (unsigned char) temp;
    	PORTB = b;
    	PORTC = c;
      }
	return;
}