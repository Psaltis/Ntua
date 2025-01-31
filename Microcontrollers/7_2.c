#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
unsigned char Ekatontades, Dekades, Monades, Dekata, Ekatosta, Xiliosta;
unsigned char digits[10]={'0','1','2','3','4','5','6','7','8','9'};
void write_2_nibbles(unsigned char x) {
	unsigned char y;
	y = x;
	unsigned char z;
	z = PIND;
	z = z & (0x0F);
	x = x & (0xF0);
	x = x + z;
	PORTD = x;
	PORTD = PORTD | 8;
	PORTD = PORTD & (0b11110111);
	x = y;
	x = x << 4;
	x = x & (0xF0);
	x = x + z;
	PORTD = x;
	PORTD = PORTD | 8;
	PORTD = PORTD & (0b11110111);
}

void lcd_data(unsigned char w) {
	PORTD = PORTD | 4;
	write_2_nibbles(w);
	_delay_us(100);
}

void lcd_command(unsigned char b) {
	PORTD = PORTD & (0b11111011);
	write_2_nibbles(b);
	_delay_us(100);
}

void lcd_init() {
	unsigned char m;
	_delay_ms(100);
	PORTD = 0x30;
	PORTD = PORTD | 8;
	PORTD = PORTD & (0b11110111);
	_delay_us(100);
	PORTD = 0x30;
	PORTD = PORTD | 8;
	PORTD = PORTD & (0b11110111);
	_delay_us(100);
	PORTD = 0x20;
	PORTD = PORTD | 8;
	PORTD = PORTD & (0b11110111);
	_delay_us(100);
	m = 0x28;
	lcd_command(m);
	m = 0x0c;
	lcd_command(m);
	m = 0x01;
	lcd_command(m);
	_delay_us(4000);
	m = 0x06;
	lcd_command(m);
}

unsigned char one_wire_reset() {
	DDRD |= 0b00010000;
	PORTD &= 0b11101111;
	_delay_us(480);
	DDRD &= 0b11101111;
	PORTD &= 0b11101111;
	_delay_us(100);
	unsigned char temp = PIND;
	_delay_us(380);
	if ((temp & 0b00010000) == 0) {
    	return 1;
	} else {
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
	if ((temp & 0b00010000) == 0) {
    	res = 0;
	}
	_delay_us(49);
	return res;
}

void one_wire_transmit_bit(unsigned char x) {
	DDRD |= 0b00010000;
	PORTD &= 0b11101111;
	_delay_us(2);
	if ((x & 0b1) == 1) {
    	PORTD |= 0b00010000;
	} else {
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
	for (int i = 0; i < 8; i++) {
    	res = res >> 1;
    	temp = one_wire_receive_bit();
    	if (temp != 0) {
        	res |= 0x80;
    	}
	}
	return res;
}

void one_wire_transmit_byte(unsigned char x) {
	for (int i = 0; i < 8; i++) {
    	if ((x & 0b1) != 0) {
        	one_wire_transmit_bit(0b00000001);
    	} else {
        	one_wire_transmit_bit(0b00000000);
    	}
    	x = x >> 1;
	}
}

void read_temp() {
	float x = 0;
	int temprature = 0;
	unsigned char high;
	unsigned char low;
	if (one_wire_reset() == 1) {
    	one_wire_transmit_byte(0xCC);
    	one_wire_transmit_byte(0x44);
    	while (one_wire_receive_bit() == 0);
    	one_wire_reset();
    	one_wire_transmit_byte(0xCC);
    	one_wire_transmit_byte(0xBE);
    	_delay_ms(750);
    	low = one_wire_receive_byte();
    	high = one_wire_receive_byte();
    	temprature |= high;
    	temprature = temprature << 8;
    	temprature |= low;
    	if ((temprature & 0b1111100000000000) == 0b1111100000000000) {
        	//arnitiki thermokrasia
        	temprature = ~temprature + 1;
        	DDRD = 0xFF;
        	lcd_init();
        	x = 0.0625 * temprature;
        	lcd_data('-');
        	Ekatontades = (unsigned char) x / 100;  
        	x = x - Ekatontades * 100.0;
        	Dekades = (unsigned char) x / 10;
        	x = x - Dekades * 10.0;
        	Monades = (unsigned char) x;
        	x = x - Monades * 1.0;
        	x = x * 10;
        	Dekata = (unsigned char) x;
        	x = x - Dekata * 1.0;
        	x = x * 10;
        	Ekatosta = (unsigned char) x;
        	x = x - Ekatosta * 1.0;
        	x = x * 10;
        	Xiliosta = (unsigned char) x;
        	lcd_data(digits[Ekatontades]);
        	lcd_data(digits[Dekades]);
        	lcd_data(digits[Monades]);
        	lcd_data('.');
        	lcd_data(digits[Dekata]);
        	lcd_data(digits[Ekatosta]);
        	lcd_data(digits[Xiliosta]);
    	} else {
        	//thetiki thermokrasia
        	DDRD = 0xFF;
        	lcd_init();
        	x = 0.0625 * temprature;
        	lcd_data('+');
        	Ekatontades = (unsigned char) x / 100;  
        	x = x - Ekatontades * 100.0;
        	Dekades = (unsigned char) x / 10;
        	x = x - Dekades * 10.0;
        	Monades = (unsigned char) x;
        	x = x - Monades * 1.0;
        	x = x * 10;
        	Dekata = (unsigned char) x;
        	x = x - Dekata * 1.0;
        	x = x * 10;
        	Ekatosta = (unsigned char) x;
        	x = x - Ekatosta * 1.0;
        	x = x * 10;
        	Xiliosta = (unsigned char) x;
        	lcd_data(digits[Ekatontades]);
        	lcd_data(digits[Dekades]);
        	lcd_data(digits[Monades]);
        	lcd_data('.');
        	lcd_data(digits[Dekata]);
        	lcd_data(digits[Ekatosta]);
        	lcd_data(digits[Xiliosta]);
    	}
	} else {
   	 
	}
}

int main(void) {
	unsigned char msg1[] = "NO Device\0";
	unsigned char msg2[] = "what\0";
	int idx;
	int temp = 0;
	DDRB = 0xFF;
	PORTB = 0xFF;
	while (1) {
    	temp = one_wire_reset();
    	if (temp == 0) {
        	idx = 0;
        	DDRD = 0xFF;
        	lcd_init();
        	while (msg1[idx] != '\0') {
            	lcd_data(msg1[idx]);
            	idx++;
        	}
        	PORTB = 0;
        	_delay_ms(75);
    	} else {
        	PORTB = 0xFF;
        	//o_aisthitiras_einai_syndedemenos
        	//to sensitivity einai 0.0625C / bit
        	//ara diavazoume posa bit exoume kai metatrepoume se temp
        	read_temp();

    	}
	}
}