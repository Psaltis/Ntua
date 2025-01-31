.include "m328PBdef.inc"
.def DC_VALUE = r16
.org 0x00
	rjmp reset

reset:
	ldi r24 , low(RAMEND)   	;Stack initialization
	out SPL , r24
	ldi r24, high(RAMEND)
	out SPH , r24
	ser r26
	out DDRB,r26   	 	;Set PortB as output
	clr r26
	out DDRD,r26   	 	;Set PortD as input
	ldi r24,(1 << WGM10)| (1<<COM1A1)
	sts TCCR1A, r24
	ldi r24,(1<<WGM12)|(1<<CS11)
	sts TCCR1B, r24
	ldi ZH,HIGH(2*ocr1a_array)  	;2byte word organization
	ldi ZL,LOW(2*ocr1a_array)   	 
	adiw ZL, 6   	 	; access the 0x80 value Low Byte of 3rd 16bit value
	lpm
	sts OCR1AL, r0
	ldi r25, HIGH(16*200)
	ldi r24, LOW(16*200)
	rcall delay_mS   	   ;arcing

main:
	in r18, PIND
	andi r18, 0b00000110         	;read PD1 and PD2
	cpi r18, 0b00000010   	 	;check if PD1 was pressed
	breq subtraction
	cpi r18, 0b00000100   	 	;check if PD2 was pressed
	breq addition
	rjmp main

addition:
	lpm DC_VALUE, Z   		 	;cant use immediate instuctions with r0-r15
	cpi DC_VALUE, 0xFA   		 
	breq END
	adiw ZL,1
	lpm DC_VALUE, Z
	sts OCR1AL, DC_VALUE
	ldi r25, HIGH(16*200)
	ldi r24, LOW(16*200)
	rcall delay_mS
	rjmp main
    
subtraction:
	lpm DC_VALUE, Z   		 	;cant use immediate instuctions with r0-r15
	cpi DC_VALUE, 0x05   		 
	breq END
	sbiw ZL,1
	lpm DC_VALUE, Z
	sts OCR1AL, DC_VALUE
	ldi r25, HIGH(16*200)
	ldi r24, LOW(16*200)
	rcall delay_mS
	rjmp main
    
END:
	rjmp main
    
delay_mS:                     	;It causes a delay of 1000*x+3
	ldi r23, 249
	loop_inn:
 	dec r23
 	nop
 	brne loop_inn
 	sbiw r24,1
 	brne delay_mS
ret

    
.cseg
ocr1a_array: .dw 0x1A05, 0x422E, 0x6B57, 0x9480, 0xBDA8, 0xE6D1, 0xFA