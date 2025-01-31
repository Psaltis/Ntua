.include "m328PBdef.inc"
.equ FOSC_MHZ=16
.equ DEL_MS=500
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
	out DDRC,r26
										;In PORTC we show the number of interrupts
	out DDRB,r26
										;In PORTB we show the counter of the mainp rogramm
	ldi r24,(1 << ISC11)| (1<<ISC10)	;Interrupt 1 may occur in the rise of PD3 signal
	sts EICRA, r24
	ldi r24,(1<<INT1)					;We enable the mask of interrupt1
	out EIMSK,r24
	sei									;We enable the global register of interrupts
loop1:
	clr r26								;r26 is used as counter for the main programm
loop2:
	out PORTB, r26
	ldi r24, low(DEL_NU)
	ldi r25, high(DEL_NU)
	nop
	rcall delay_mS
	inc r26
	cpi r26,16
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
ISR1:
	push r21							;We save all the registers that are affected in this
	push r22							;interrupt because an interrupt may occur.
	push r23							;We save the status register for the main programm
	push r24
	push r25
	in r21,SREG
	push r21
loop3:
	ldi r24,low(5*16)					;We set a delay for the arcing problem
	ldi r25,high(5*16)					;We set the register EIFR(1)=0 (inverted logic)
	ldi r22,(1<<INTF1)					;to check if it will be 1 after a delay
	out EIFR,r22
	rcall delay_mS
	in r22,EIFR	
	andi r22,2							;We use a mask to check the EIFR(1) bit
	brne loop3							;If it is 0 then there is no arcing problem	and we continue,
										;but if it is not we repeat this until its 0
										
										
	in r19,PIND							;We need to check if PD6 is pressed
	andi r19,64							;We use a mask to isolate the 6th bit
	breq END							;If it is not zero then we press PD7 and the counetr of interupts does not increase



	inc r20
	cpi r20,32
	brne NEXT
	clr r20
NEXT:
	out PORTC,r20
END:
	pop r21
	out SREG,r21
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	sei
	reti
	 
	 
 