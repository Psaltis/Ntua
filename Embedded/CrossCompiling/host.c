#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "fcntl.h"
#include "termios.h"
/* for buffer overflow */
#define MAX_SIZE 256
void serial(char *string, const char *port)
{
/* Open serial port */
int fd = open(port, O_RDWR | O_NOCTTY);
if(fd == -1){
perror("Error opening serial port...\n");
return;
}
printf("Changing serial port: %s communication settings to match with ARM
machine...\n", port);
char terminalportc[MAX_SIZE];
snprintf(terminalportc, sizeof(terminalportc), "stty -F %s 9600 cs8
-cstopb -parenb", port);
if(system(terminalportc) == -1){
perror("Error with port...\n");
exit(1);
}
snprintf(terminalportc, sizeof(terminalportc), "stty -F %s", port);
if(system(terminalportc) == -1)
{
perror("Error with port\n");
exit(1);
}
printf("transmitbyteting string of %ld bytes (char*) via UART to ARM
architecture machine...\n", strlen(string));
size_t transmitbyte = write(fd, string, 64);
if(transmitbyte != 64)
{
printf("Not all bytes 64 were sent properly via port... Sent only:
%ld\n", transmitbyte);
close(fd);
return;
}
/* Reading response */
char readguest[MAX_SIZE];
uint32_t count = 0;
int bytesRecv, totalbytes = 0;
do{
/* Do reading per byte... */
bytesRecv = read(fd, readguest + totalbytes, 78);
if(bytesRecv < 0)
{
printf("Error from guest...\n");
close(fd);
return;
}
totalbytes += bytesRecv;
count++;
} while(bytesRecv > 0 && totalbytes < 140); // 64 + 78
char *arm_id = strstr(readguest);
if(arm_id != NULL)
printf("%.76s\n", arm_id);
else
printf("Couldn't find identifier\n");
printf("\n");
/* Verify from guest ARM machine's response the characters that were sent
on string */
/* ... */
printf("Serial Communication checks OK... Closing port now...\n");
close(fd);
}
int main(int argc, char **argv)
{
char inString[64]; // string array to be read
if(argc < 2){
perror("Error. No port specified\n");
exit(2);
}
else
printf("Opening port: %s\n", argv[1]);
/* Prompt message to user */
printf("Provide a string to send via UART: ");
/* Read string from stdin */
if(fgets(inString, sizeof(inString), stdin) == NULL)
{
perror("Error reading string\n");
exit(1);
}
/* Send string over serial port opened with ARM system architecture
emulated*/
serial(inString, argv[1]);
exit(0);
}
