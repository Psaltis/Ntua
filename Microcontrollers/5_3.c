#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define PCA9555_0_ADDRESS 0x40 //A0=A1=A2=0 by hardware
#define TWI_READ 1 // reading from twi device
#define TWI_WRITE 0 // writing to twi device
#define SCL_CLOCK 100000L // twi clock in Hz
#define DEL 1000U
//Fscl=Fcpu/(16+2*TWBR0_VALUE*PRESCALER_VALUE)
#define TWBR0_VALUE ((F_CPU/SCL_CLOCK)-16)/2
// PCA9555 REGISTERS
typedef enum {
 REG_INPUT_0 = 0,
 REG_INPUT_1 = 1,
 REG_OUTPUT_0 = 2,
 REG_OUTPUT_1 = 3,
 REG_POLARITY_INV_0 = 4,
 REG_POLARITY_INV_1 = 5,
 REG_CONFIGURATION_0 = 6,
 REG_CONFIGURATION_1 = 7
} PCA9555_REGISTERS;
//----------- Master Transmitter/Receiver -------------------
#define TW_START 0x08
#define TW_REP_START 0x10
//---------------- Master Transmitter ----------------------
#define TW_MT_SLA_ACK 0x18
#define TW_MT_SLA_NACK 0x20
#define TW_MT_DATA_ACK 0x28
//---------------- Master Receiver ----------------
#define TW_MR_SLA_ACK 0x40
#define TW_MR_SLA_NACK 0x48
#define TW_MR_DATA_NACK 0x58
#define TW_STATUS_MASK 0b11111000
#define TW_STATUS (TWSR0 & TW_STATUS_MASK)
uint8_t temp;
//initialize TWI clock
void twi_init(void)
{
 TWSR0 = 0; // PRESCALER_VALUE=1
 TWBR0 = TWBR0_VALUE; // SCL_CLOCK 100KHz
}
// Read one byte from the twi device (request more data from device)
unsigned char twi_readAck(void)
{
TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
while(!(TWCR0 & (1<<TWINT)));
 return TWDR0;
}
//Read one byte from the twi device, read is followed by a stop condition
unsigned char twi_readNak(void)
{
TWCR0 = (1<<TWINT) | (1<<TWEN);
while(!(TWCR0 & (1<<TWINT)));
 return TWDR0;
}
// Issues a start condition and sends address and transfer direction.
// return 0 = device accessible, 1= failed to access device
unsigned char twi_start(unsigned char address)
{
 uint8_t twi_status;
// send START condition
TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
// wait until transmission completed
while(!(TWCR0 & (1<<TWINT)));
// check value of TWI Status Register.
twi_status = TW_STATUS & 0xF8;
if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) return 1;
// send device address
TWDR0 = address;
TWCR0 = (1<<TWINT) | (1<<TWEN);
// wail until transmission completed and ACK/NACK has been received
while(!(TWCR0 & (1<<TWINT)));
// check value of TWI Status Register.
twi_status = TW_STATUS & 0xF8;
if ( (twi_status != TW_MT_SLA_ACK) && (twi_status != TW_MR_SLA_ACK) )
 {
 return 1;
 }
return 0;
}
// Send start condition, address, transfer direction.
// Use ack polling to wait until device is ready
void twi_start_wait(unsigned char address)
{
 uint8_t twi_status;
 while ( 1 )
 {
// send START condition
TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

 // wait until transmission completed
 while(!(TWCR0 & (1<<TWINT)));

 // check value of TWI Status Register.
 twi_status = TW_STATUS & 0xF8;
 if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) continue;

 // send device address
 TWDR0 = address;
 TWCR0 = (1<<TWINT) | (1<<TWEN);

 // wail until transmission completed
 while(!(TWCR0 & (1<<TWINT)));

 // check value of TWI Status Register.
 twi_status = TW_STATUS & 0xF8;
 if ( (twi_status == TW_MT_SLA_NACK )||(twi_status ==TW_MR_DATA_NACK) )
 {
 /* device busy, send stop condition to terminate write operation */
 TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

 // wait until stop condition is executed and bus released
 while(TWCR0 & (1<<TWSTO));

 continue;
 }
break;
 }
}
// Send one byte to twi device, Return 0 if write successful or 1 if write failed
unsigned char twi_write( unsigned char data )
{
// send data to the previously addressed device
TWDR0 = data;
TWCR0 = (1<<TWINT) | (1<<TWEN);
// wait until transmission completed
while(!(TWCR0 & (1<<TWINT)));
if( (TW_STATUS & 0xF8) != TW_MT_DATA_ACK) return 1;
return 0;
}
// Send repeated start condition, address, transfer direction
//Return: 0 device accessible
// 1 failed to access device
unsigned char twi_rep_start(unsigned char address)
{
 return twi_start( address );
}
// Terminates the data transfer and releases the twi bus
void twi_stop(void)
{
 // send stop condition
TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
// wait until stop condition is executed and bus released
while(TWCR0 & (1<<TWSTO));
}
void PCA9555_0_write(PCA9555_REGISTERS reg, uint8_t value)
{
 twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
 twi_write(reg);
 twi_write(value);
 twi_stop();
}
uint8_t PCA9555_0_read(PCA9555_REGISTERS reg)
{
uint8_t ret_val;

 twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
 twi_write(reg);
 twi_rep_start(PCA9555_0_ADDRESS + TWI_READ);
 ret_val = twi_readNak();
 twi_stop();
 return ret_val;
}

void write_2_nibbles(unsigned char x){
    unsigned char y;
    y=x;
    unsigned char z;
    z=temp;
    z=z & (0x0F);
    x=x & (0xF0);
    x=x+z;
    temp=x;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp|8;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp&(0b11110111);
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    x=y;
    x=x<<4;
    x=x & (0xF0);
    x=x+z;    
    temp=x;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp|8;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp&(0b11110111);
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
}
void lcd_data(unsigned char w){
    temp=temp|4;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    write_2_nibbles(w);
     _delay_us(100);
}
void lcd_command(unsigned char b){
    temp=temp & (0b11111011);
    PCA9555_0_write(REG_OUTPUT_0,temp);
    write_2_nibbles(b);
     _delay_us(100);
}
void lcd_init(void){
    unsigned char m;
    _delay_ms(100);
    temp=0x30;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp|8;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp&(0b11110111);
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=0x30;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp|8;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp&(0b11110111);
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=0x20;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp|8; 
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp&(0b11110111);
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    m=0x28;
    lcd_command(m);
    m=0x0c;
    lcd_command(m);
    m=0x01;
    lcd_command(m);
    _delay_us(4000);
    m=0x06;
    lcd_command(m);
}


int main(void) {
    unsigned char name1[]="Panagiotis\0";
    unsigned char lastname1[]="Prountzos\0";
    unsigned char name2[]="Alexandros\0";
    unsigned char lastname2[]="Psaltis\0";
    
    twi_init();
    
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
     lcd_init();
    _delay_us(4000);
    lcd_command(0x01);
    _delay_us(400);
    while(1)
    {   for(int i=0; name1[i]!='\0'; i++){
            lcd_data(name1[i]);
        }
        lcd_command(0b11000000);
    _delay_us(100);
    for(int i=0; lastname1[i]!='\0'; i++){
       lcd_data(lastname1[i]);
    }
    _delay_ms(5000);
    lcd_command(0x01);
    _delay_us(400);  
   for(int i=0; name2[i]!='\0'; i++){
       lcd_data(name2[i]);
      }
    lcd_command(0b11000000);
    _delay_us(100);
    for(int i=0; lastname2[i]!='\0'; i++){
       lcd_data(lastname2[i]);       
     }
    _delay_ms(5000);
    lcd_command(0x01);  
    _delay_us(400);
    }
    
}