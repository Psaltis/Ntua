.include "m328PBdef.inc"
.equ FOSC_MHZ=16
.equ DEL_MS=3000
.equ DEL_NU=FOSC_MHZ*DEL_MS

.org 0x00
	rjmp reset
.org 0x04
	rjmp ISR1

reset:
	ldi r24 , low(RAMEND)
	out SPL , r24
	ldi r24, high(RAMEND)
	out SPH , r24
	ser r26
	out DDRB,r26
	ldi r24,(1 << ISC11)| (1<<ISC10)
	sts EICRA, r24
	ldi r24,(1<<INT1)
	out EIMSK,r24
	sei
loop1:
	clr r20
	rjmp loop1
delay_mS:
	ldi r23, 249
loop_inn:
	cpi r20, 0
	dec r23
	brne loop_inn
	sbiw r24,1
	brne delay_mS
	ret
ISR1:
	cpi r20, 0
	breq interrupt
refresh:
	sei
	ser r21
	out PORTB, r21
	ldi r24, low(16*500)
	ldi r25, high(16*500)
	rcall delay_mS
	ldi r21, 1

	out PORTB, r21
	ldi r24, low(16*2500)
	ldi r25, high(16*2500)
	rcall delay_mS
	clr r20
	clr r21
	out PORTB, r21
	reti
interrupt:
	ser r20
	sei
	ldi r21, 1
	out PORTB, r21
	ldi r24, low(DEL_NU)
	ldi r25, high(DEL_NU)
	rcall delay_mS
	clr r21
	out PORTB, r21
	clr r20
	reti