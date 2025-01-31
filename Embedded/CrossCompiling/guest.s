.global main
.equ O_RDWR, 2
.equ MAX_SIZE, 64 @ max. size of bytes to be read
.equ INIT, 70 @ 'F' char
.equ EMPTY, 32 @ space char
/* Seperate the ASCII code into 2 seperate values eg. r0, #58 into r0,
#8 and r1, #5 */
ascii_to_dec:
cmp r0, #58 @ >ASCII(9)
bxlt lr @if true, skip
mov r1, #49 @ '1' character
find10s:
sub r0, #10 @ Substract '10' every iteration
cmp r0, #58 @ >ASCII(9)
bxlt lr
add r1, #1 @ Increment 10's by 1 value each time
b find10s
main:
@ wait for host
mov r0, #1 @syscall
ldr r1, =waitmsg
mov r2, #11
mov r7, #4 @sys write
swi 0
@ open port
ldr r0, =path @ string that sets serial port to be opened
mov r1, #O_RDWR @ request read-write privilenges
mov r7, #5 @sys open
swi 0
mov r5, r0 @save fd
ldr r1, =buffer
mov r2, #MAX_SIZE
mov r7, #3
swi 0
/* Display data being read from serial port from host to STDOUT */
mov r7, #4 @sys write
mov r0, #1
ldr r1, =msg
mov r2, #18
swi 0
mov r0, #1
ldr r1, =buffer @ print read message
mov r2, #MAX_SIZE
swi 0
ldr r0, =input @ input mem address
mov r2, #INIT
mov r3, #255
/*fill buffer with INIT*/
inputb:
strb r2, [r0, r3]
subs r3, #1
bne inputb
ldr r1, =buffer
write_input:
@ r1: contains buffer[] base memory address
ldrb r2, [r1], #1 @ Get input: buffer[] string characters
cmp r2, #0
beq freq @ Branch if NULL char is found
@ Ignore ' ' empty characters
cmp r2, #EMPTY
beq write_input
@ r2: buffer[i] byte element
ldrb r3, [r0, r2] @ if bigger than 1 byte
add r3, #1 @ input[buffer[i]]++;
strb r3, [r0, r2]
b write_input
/* finding frequency */
freq:
mov r4, #0 @ local index of loop: 0-256 in ASCII code
mov r1, #0 @ r1: how many times the most freq. char appeared
mov r2, #0 @ r2: most frequent character from 0 to 256 in
ASCII
freqloop:
ldrb r3, [r0], #1
@ Increment ASCII char being checked and compare local freq. with
local max.freq.
add r4, r4, #1
cmp r4, #256
beq freq_char
cmp r3, r1
movgt r1, r3 @update frequency
@ update most freq
movgt r2, r4
freq_char:
@save most freq char along with frequency
ldr r0, =input
sub r1, #22
sub r2, #1
strb r1, [r0, #2] @ frequent char = input[2]
strb r2, [r0, #3] @ frequency = input[3]
mov r7, #4 @sys write
mov r0, #1
ldr r1, =dbgmsg
mov r2, #21
swi 0
@if freq>9
ldr r1, =input
ldrb r3, [r1, #3] @ save most frequent character in r4.
ldrb r0, [r1, #2]
mov r1, #EMPTY
bl ascii_to_dec
ldr r2, =output
strb r3, [r2, #47] @ replace the space in '' with most
frequent character
strb r1, [r2, #66] @ freq
strb r0, [r2, #67] @ freq
mov r0, r5 @fd
ldr r1, =output
mov r2, #78
swi 0
b exit_program
exit_program:
mov r0, #1
ldr r1, =exit
mov r2, #12
mov r7, #4
swi 0
mov r0, #1
mov r7, #1
swi 0
.section .data
buffer: .skip MAX_SIZE @ Reserve memory
addresses for reading input characters
input: .skip 256 @ Save space of 256 bytes
for manipulating all ASCII's found on input string
path: .asciz "/dev/ttyAMA0" @ Serial port used
msg: .asciz "Serial port read: "
waitmsg: .asciz "Waiting...\n"
dbgmsg: .asciz "Sending data now...\n"
exit: .asciz "Exiting...\n"
output: .asciz "The most frequent character is: ' 'and it appeared times...\n"
