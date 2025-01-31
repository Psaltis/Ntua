.text
.global main
.syntax unified
main:
push {ip,lr}
loop:
mov r4, #0 @ counter
mov r5, #10 @ newline character
mov r0, #1 @ stdout
ldr r1, =text
mov r2, #len @ length of text
mov r7, #4 @ write system call
swi 0
mov r0, #0 @ stdin
ldr r1, =inp_str @ location of inp_str in memory
mov r2, #33 @ read 32 characters + \n from input
mov r7, #3 @ read system call
swi 0
cmp r0, #2 @ Is it start of text?
bne convert
ldrb r3, [r1]
cmp r3, #81 @ Q?
beq exit
cmp r3, #113 @ q?
beq exit
convert:
ldrb r3, [r1]
add r4, #1
cmp r3, #10 @ Is the character '\n'?
beq print
cmp r3, #97 @ >= a?
blt letter
cmp r3, #122 @ <= z?
ble capital
letter:
cmp r3, #65 @ >= A?
blt numeric
cmp r3, #90 @ <= Z?
ble lower
numeric:
cmp r3, #48 @ >= 0?
blt next
cmp r3, #57 @ <= 9?
ble conv_number
next:
strb r3, [r1], #1
final:
cmp r4, #33
beq print
b convert
capital:
sub r3, r3, #32
strb r3, [r1], #1
b final
lower:
add r3, r3, #32
strb r3, [r1], #1
b final
conv_number:
cmp r3, #53
addlt r3, #5
subge r3, #5
strb r3, [r1], #1
b final
print:
cmp r4, #33
strbeq r5, [r1, #-1] @if 33rd character != '\n' make it '\n'
mov r0, #1 @ stdout
ldr r1, =inp_str @ location of string in memory
mov r2, r4 @ length string
mov r7, #4 @ write system call
swi 0
cmp r4, #33
beq flush
b loop
flush:
mov r7, #3 @ read system call
mov r0, #2
ldr r1,=inp_str
mov r2, #(1 << 30) @ flush
swi 0
b loop
exit:
pop {ip,pc}
mov r7, #1 @ exit system call
swi 0
.data
text: .ascii "Input a string : "
len = . - text @ length of text
inp_str: .space 32
