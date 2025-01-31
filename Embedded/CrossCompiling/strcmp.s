.text
.align 4
.global strcmp
.type strcmp %function
strcmp:
mov r4, #0 @return value
cmp:
ldrb r5, [r0], #1 @load character from string1
ldrb r6, [r1], #1 @load character from string2
cmp r5, r6
bne notequal
cmp r5, #0 @end of string1 and string2
beq exit
b cmp
notequal:
movlt r4, #-1 @str1 < str2 return -1
movgt r4, #1 @str1 > str2 return 1
exit:
mov r0, r4
bx lr
