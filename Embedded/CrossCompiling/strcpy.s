.text
.align 4
.global strcpy
.type strcpy %function
strcpy:
mov r5, r0 @pointer to destination string address
mov r6, #0
copy:
ldrb r5, [r1], #1 @get the character from source string
cmp r5, r6
beq exit
strb r5, [r0], #1 @store character to destination string
b copy
exit:
strb r6, [r0] #add a null character at the end of string
mov r0, r5 #return destination string pointer
bx lr
