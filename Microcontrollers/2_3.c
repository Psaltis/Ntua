#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
int check = 0;
ISR (INT1_vect) {
	if(check == 0) {
		PORTB = 0x01;
		check = 1;
		sei();
		_delay_ms(3000);
		PORTB = 0;
		check = 0;
	}
	else {
		sei();
		PORTB = 0xFF;
		_delay_ms(500);
		PORTB = 0x01;
		_delay_ms(2500);
	}
}
int main() {
	EICRA = (1 << ISC11) | (1 << ISC10);
	EIMSK = (1 << INT1);
	sei();
	DDRB = 0xFF; /*arxikopoiisi tou portB*/
	while(1) {
	}
}