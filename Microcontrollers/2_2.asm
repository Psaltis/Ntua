.include "m328PBdef.inc"
.equ FOSC_MHZ=16
.equ DEL_MS=1000
.equ DEL_NU=FOSC_MHZ*DEL_MS
.org 0x00
	rjmp reset
.org 0x04
	rjmp ISR0
reset:
	ldi r24 , low(RAMEND)
	out SPL , r24
	ldi r24, high(RAMEND)
	out SPH , r24
	ser r26
	out DDRC,r26							;In PORTC we show the counter of the main programm
	ldi r24,(1 << ISC01)| (1<<ISC00)		;Interrupt 1 may occur in the rise of PD3 signal
	sts EICRA, r24
	ldi r24,(1<<INT0)						;We enable the mask of interrupt0
	out EIMSK,r24
	sei										;We enable the global register of interrupts
loop1:
	clr r26 								;r26 is used as counter for the main programm
loop2:
	out PORTC, r26
	ldi r24, low(DEL_NU)
	ldi r25, high(DEL_NU)
	nop
	rcall delay_mS
	inc r26
	cpi r26,32
	brne loop2
	rjmp loop1
delay_mS:
	ldi r23, 249
loop_inn:
	dec r23
	nop
	brne loop_inn
	sbiw r24,1
	brne delay_mS
	ret
ISR0:
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	in r21,SREG
	push r21
	clr r21
	in r21, PINB
	com r21
	ldi r23, 6
	clr r20
loop3:
	ror r21
	brcc NEXT
	rol r20
NEXT:
	dec r23
	brne loop3
	out PORTC, r20
	ldi r24,low(DEL_NU)
	ldi r25,high(DEL_NU)
	rcall delay_ms
	pop r21
	out SREG,r21
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	sei
	reti