.text
.align 4
.global strcat
.type strcat %function
strcat:
mov r5, r0 @pointer to destination string
mov r6, #0
while1:
ldrb r4, [r0], #1 @get the character from destination string
cmp r4, r6 @is it null character?
beq while2
b while1
while2:
sub r0, #1
ldrb r3, [r1], #1 @get the character from source string
cmp r3, r6
beq exit
strb r3, [r0], #1 @store character to destination string
b while2
exit:
strb r6, [r0] @add a null character at the end of string
mov r0,
