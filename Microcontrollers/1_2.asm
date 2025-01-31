 .include "m328PBdef.inc"
 .equ a=0x45
 .equ b=0x23
 .equ c=0x21
 .equ d=0x01
 .def Ar=r20
 .def Br=r21
 .def Cr=r22
 .def Dr=r23
 .def f0r= r29
 .def f1r=r30
 .def counter = r24
 reset:
	 ldi r24, low(RAMEND)
	 out SPL, r24
	 ldi r24, high(RAMEND)
	 out SPH, r24
	 clr r24
 main:
	 ldi counter, 0x05
	 ldi r25, 0x01
	 ldi r26, 0x02
	 ldi r27, 0x04
	 ldi r28, 0x05
	 ldi Ar,a
	 ldi Br,b
	 ldi Cr,c
	 ldi Dr,d
 loop:
 
	 //F0 CALCULATION
	 mov f0r,Ar
	 or f0r,Br
	 or f0r,Cr
	com Dr
	 and f0r,Dr
	 com Dr
	 
	 //F1 calculation
	 mov f1r,Ar
	 com f1r
	 or f1r,Cr
	 push Br
	 com Br
	 push Dr
	 com Dr
	 or Br,Dr
	 and f1r,Br
	 pop Dr
	 pop Br
	 add Ar,r25
	 add Br,r26
	 add Cr,r27
	 add Dr,r28
	 dec counter
	 brne loop