.text
.align 4
.global strlen
.type strlen, %function
strlen:
mov r4, #0 @counter
mov r5, #0 @null character
count:
ldrb r1, [r0], #1 @load string and go to next character
cmp r1, r5
beq exit
add r0, #1
b count
exit:
mov r0, r4 @return counter
bx lr
