.include "m328PBdef.inc"
;r16 is a register that can be used to set other registers(USE IT)
;r17 is the counter in the TimeInterruptServiceRoutine(MUST NOT CHANGE)
;r18,r19,r20 is the quantum X.XXr20(4)r19(88)r18(28)
;r21,r22,r23 is the number we want to print r21.r22r23
;r24,r25 is used to store the ADC number
;r26,r27,r28 is used for the delays
;r30,r31 are used for the Z for the table 
;r29 is free and we can also use temp
.def temp = r16
.def QLL=r18
.def QL=r19
.def QH=r20
.def Monades=r21
.def Dekata=r22
.def Ekatosta=r23
.def ADC_L=r24
.def ADC_H=r25
.org 0x00
    rjmp reset
.org 0x1A
    rjmp timer_interrupt
.org 0x2A
    rjmp adc_interrupt
reset:
    ldi temp,LOW(RAMEND)   ;Stack initialisation
    out SPL,temp
    ldi temp, HIGH(RAMEND)
    out SPH,temp
    ser temp         
    out DDRD,temp          ;Port D is output for the LCD
    clr temp
    out DDRC,temp          ;Port C is input for the POT2 of AD conversion
    clr Monades
    clr Dekata
    clr Ekatosta
    clr QH               ;QH and QL and QLL will be added with the numbers 4 and 88 and 28
    clr QL		 ;as step for the add 0.0048828 for ADC times. This number 
    clr QLL		 ;will act as floating number aka 0.0048828
    clr counter
    
    ldi temp ,(1<<CS12) | (0<<CS11) | (1<<CS10) ; Initialise the timer1
    sts TCCR1B, temp				;Clock/1024
    ldi temp,HIGH(49910)			;Timer interrupts every 1sec.
    sts TCNT1H,temp
    ldi temp,LOW(49910)
    sts TCNT1L,temp
    ldi temp,0b01000010  ;REFSn[1:0]=01---Vref=5V
			 ;ADLAR=0---------Right adjust
			 ;MUX[4:0]=0010---Select ADC2(PC2)
    sts ADMUX,temp
    ldi temp,0b10001111
    sts ADCSRA,temp      ;ADEN=1----------We enable the AD
			 ;ADCS=0----------We will start the conversion after we sei
			 ;ADATE=0---------We dont use triggering
			 ;ADIF=0----------When the interrupts happens is set 1 and 
			 ;it becomes 0 after the interrupt
			 ;ADIE=1----------We enable the interrup of AD but sei is not yet set
			 ;ADPS[0:2]=111--We use scale of 128
    rcall lcd_init       ;We initialize the lcd  
    ldi r26,low(16*5) 
    ldi r27,high(16*5)
    rcall wait_mS
    sei
    lds temp,ADCSRA
    ori temp, (1<<ADSC)  ;ADSC=1----------The conversion starts and is 0 when it finishes
    sts ADCSRA,temp	 ;so we must set it again in the end of the interrupt
    ldi temp,(1<<TOIE1)
    sts TIMSK1,temp

main:
    sei
    rjmp main

adc_interrupt:
    clr Monades         ;We initialize all the variables used to get the correct digits
    clr Dekata		;The AD conversion starts with 0.0048828
    clr Ekatosta	;In QLL we add the number 28
    ldi QH,0		;In QL we add the number 88
    ldi QL,0		;In QH we add the number 4
    ldi QLL,0		;In every loop that follows we add this number and
    lds ADC_L,ADCL	;increase Ekatosta, Dekata, Monades
    lds ADC_H,ADCH
    adiw ADC_L,1         ;We increase the ADC by 1 to run the loop N times
    
    loop:
	sbiw ADC_L,1
	breq NEXT	 ;When we loop for N times we use the LCD
	ldi temp,28     ;We increase the QLL in every loop by 28 and if it is
	add QLL,temp	 ;over 100 we substract it and then increase by 1 QL
	cpi QLL,100	 ;If it is lower than 100 we continue in SKIP0
	brlo SKIP0
	subi QLL,100
	inc QL
	    
    SKIP0:
	ldi temp,88	 ;We increase the QL in every loop by 88 and if it is
	add QL,temp	 ;over 100 we substract it and then increase by 1 QH
	cpi QL,100	 ;If it is lower than 100 we continue in SKIP1
	brlo SKIP1
	subi QL,100
	inc QH
		
    SKIP1:
	ldi temp,4	 ;We increase the QH in every loop by 4 and if it is
	add QH,temp	 ;over 10 we substract it and then increase by 1 Ekatosta
	cpi QH,10       ;If it is lower then we dont increase Ekatosta 
	brlo loop	 ;and we dont need to check the other digits beacuse they wont increase
	subi QH,10
	inc Ekatosta    ;If we increase Ekatosta by 1 then we need to compare it with 10
	cpi Ekatosta,10 ;and if it is lower than 10 then Dekata cannot be increased
	brlo loop	 ;so we dont need to check it and branch again on loop
	subi Ekatosta,10;But if it is over 10 then we substract it and increase Dekata by 1
	inc Dekata      ;Similarly if Dekata is increased by 1 becuse of Ekatosta
	cpi Dekata,10	 ;we check if it is lower than 10 and if so we dont need to check 
	brlo loop	 ;Monades and branch on loop again
	subi Dekata,10  ;If they are over 10 we sub 10 from Dekata and increase Monades
	inc Monades	 
	rjmp loop
	 ;After the loop we will have Ekatosta(max 9),Dekata(max 9) and Monades(max 4
	 ;because max(ADC)=1023 and [1023*5/1024]=[4.99]=4
    NEXT:;We need to find the characters of these numbers and print them on LCD
	ldi r29,0x01        ;We clear the diplay
	rcall lcd_command
	ldi r26,low(4000)
	ldi r27,high(4000)
	rcall wait_usec    
	ldi zl,low(characters*2)
	ldi zh,high(characters*2)
	add zl,monades
	lpm r29,z
	rcall lcd_data
	ldi r29,','
	rcall lcd_data
	ldi zl,low(characters*2)
	ldi zh,high(characters*2)
	add zl,dekata
	lpm r29,z 
	rcall lcd_data
	ldi zl,low(characters*2)
	ldi zh,high(characters*2)
	add zl,ekatosta
	lpm r29,z  
	rcall lcd_data
	sei
	ldi r26,low(500)
	ldi r27,high(500)
	rcall wait_mS 
	reti
timer_interrupt:
    push temp
    in temp,SREG
    push temp
    ldi temp,HIGH(49910)
    sts TCNT1H,temp
    ldi temp,LOW(49910)
    sts TCNT1L,temp
    pop temp
    out SREG,temp
    pop temp
    sei
    lds temp,ADCSRA
    ori temp, (1<<ADSC)  ;ADSC=1----------The conversion starts again after the Interrupt
    sts ADCSRA,temp
    reti
write_2_nibbles:
    push r29
    in temp,PIND
    andi temp,0x0F
    andi r29, 0xF0
    add r29,temp
    out PORTD,r29
    sbi PORTD,3
    nop
    nop
    cbi PORTD,3
    pop r29
    swap r29
    andi r29, 0xF0
    add r29,temp
    out PORTD,r29
    sbi PORTD,3
    nop
    nop
    cbi PORTD,3
    ret
lcd_data:		;The data is in register r29
    sbi PORTD,2
    rcall write_2_nibbles
    ldi r26,100
    ldi r27,0
    rcall wait_usec
    ret
lcd_command:		;The command is in register r29
    cbi PORTD,2
    rcall write_2_nibbles
    ldi r26,100
    ldi r27,0
    rcall wait_usec
    ret
lcd_init:
    ldi r26,low(16*100)
    ldi r27,high(16*100)
    rcall wait_mS
    ldi r29,0x30
    out PORTD,r29
    sbi PORTD,3
    nop
    nop
    cbi PORTD,3
    ldi r26,100
    ldi r27,0
    rcall wait_usec
    ldi r29,0x30
    out PORTD,r29
    sbi PORTD,3
    nop
    nop
    cbi PORTD,3
    ldi r26,100
    ldi r27,0
    rcall wait_usec
    ldi r29,0x20
    out PORTD,r29
    sbi PORTD,3
    nop
    nop
    cbi PORTD,3
    ldi r26,100
    ldi r27,0
    rcall wait_usec
    ldi r29,0x28
    rcall lcd_command
    ldi r29,0x0C
    rcall lcd_command
    ldi r29,0x01
    rcall lcd_command
    ldi r26,low(4000)
    ldi r27,high(4000)
    rcall wait_usec
    ldi r29,0x06
    rcall lcd_command
    
    ret
;The wait_mS creates a delay of 1000*x+3 cycles and the delay is (1000x+3)/16MHz
;and is almost 1/16KHz. So if we want a delay of Yms we need to rcall wait_mS(16*Y)
wait_mS:
    ldi r28, 249
    loop_inn:
    dec r28
    nop
    brne loop_inn
    sbiw r26,1
    brne wait_mS     
    ret 
;The wait_usec creates a delay of 16*x+3 cycles which are almost (16*x+3)/16MHz=xusec
;and x is the number in r26,r27 so when we use rcall wait_usec(200) we have almost
;200 usec delay
wait_usec:
    sbiw r26,1
    nop
    nop
    nop    
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    brne wait_usec
    ret    
 
.cseg    
characters: .DB "0123456789"