.include "m328PBdef.inc"
.equ FOSC_MHZ=16;clock frequency in MHz
.equ walk_mS=1500;every led stays ON for 1.5s or 1500ms
.equ wait_mS=2000 			;wagon stops for 2s
.equ F1=FOSC_MHZ*walk_mS
.equ F2=FOSC_MHZ*wait_mS
.def wagon=r16 				;labeling the registers
.def counter=r18 			; counter for tracking wagon position
reset:
	ldi r24,low(RAMEND)
	out SPL,r24
	ldi r24,high(RAMEND)
	out SPH,r24
	ldi wagon, 0x80 		; initialize wagon position at PD7
	ser r24
	out DDRD,r24
	out PORTD,wagon
Loop:
	ldi counter,0x07
	set 					; Set T flag for left movement
walkLeft:					;Left movement of the wagon
	ldi r24, low(F1-1) 		; Set up of the delay function for 1.5sec
	ldi r25, high(F1-1)
	rcall delay_mS 			; walk
	lsr wagon
	out PORTD,wagon
	dec counter
	brne walkLeft			; if counter=0 then it has arrived at PD0
	ldi r24, low(F2-1)
	ldi r25, high(F2-1)
	rcall delay_mS 			; wait
	ldi counter,0x07
	clt 					;Clear T flag for right movement
walkRight: 					;Right movement of the wagon
	ldi r24, low(F1-1) 		; Set up of the delay function for 1.5sec
	ldi r25, high(F1-1)
	rcall delay_mS 			; walk
	lsl wagon
	out PORTD,wagon
	dec counter
	brne walkRight 			; if counter=0 then it has arrived at PD0
	ldi r24, low(F2-1) 		; Set up of the delay function for 2sec
	ldi r25, high(F2-1)
	rcall delay_mS 			; wait
	rjmp Loop 				; repeat
delay_mS:
	nop
	nop
	ldi r23,248
	;total group delay 996 cycles
loop0:
	dec r23
	nop
	brne loop0
delay_inner:
	ldi
	r23, 249
loop_inn:
	dec r23
	nop
	brne loop_inn
	sbiw r24 ,1
	brne delay_inner
	ret
