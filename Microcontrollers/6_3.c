#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define PCA9555_0_ADDRESS 0x40 //A0=A1=A2=0 by hardware
#define TWI_READ 1 // reading from twi device
#define TWI_WRITE 0 // writing to twi device
#define SCL_CLOCK 100000L // twi clock in Hz
//Fscl=Fcpu/(16+2*TWBR0_VALUE*PRESCALER_VALUE)
#define TWBR0_VALUE ((F_CPU/SCL_CLOCK)-16)/2
// PCA9555 REGISTERS
unsigned char ascii[16]={'*','0','#','D','7','8','9','C','4','5','6','B','1','2','3','A'};
unsigned char index, reset;
unsigned char value1[16];
unsigned char value2[16];
unsigned char code[2];
unsigned char counter=0;
typedef enum {
 REG_INPUT_0 = 0,
 REG_INPUT_1 = 1,
 REG_OUTPUT_0 = 2,
 REG_OUTPUT_1 = 3,
 REG_POLARITY_INV_0 = 4,
 REG_POLARITY_INV_1 = 5,
 REG_CONFIGURATION_0 = 6,
 REG_CONFIGURATION_1 = 7,
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
//initialize TWI clock

unsigned char twi_readNak(void)
{
TWCR0 = (1<<TWINT) | (1<<TWEN);
while(!(TWCR0 & (1<<TWINT)));
 return TWDR0;
}
void twi_init(void)
{
 TWSR0 = 0; // PRESCALER_VALUE=1
 TWBR0 = TWBR0_VALUE; // SCL_CLOCK 100KHz
}
// Read one byte from the twi device ( request more data from device)
unsigned char twi_readAck(void)
{
TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
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
    z=PIND;
    z=z & (0x0F);
    x=x & (0xF0);
    x=x+z;
    PORTD=x;
    PORTD=PORTD|8;
    PORTD=PORTD&(0b11110111);
    x=y;
    x=x<<4;
    x=x & (0xF0);
    x=x+z;    
    PORTD=x;
    PORTD=PORTD|8;
    PORTD=PORTD&(0b11110111);
}
void lcd_data(unsigned char w){
    PORTD=PORTD|4;
    write_2_nibbles(w);
     _delay_us(100);
}
void lcd_command(unsigned char b){
    PORTD=PORTD & (0b11111011);
    write_2_nibbles(b);
     _delay_us(100);
}
void lcd_init(void){
    _delay_ms(100);
    PORTD=0x30;
    PORTD=PORTD|8;
    PORTD=PORTD&(0b11110111);
    _delay_us(100);
    PORTD=0x30;
    PORTD=PORTD|8;
    PORTD=PORTD&(0b11110111);
    _delay_us(100);
    PORTD=0x20;               //command to switch to 4 bit mode
    PORTD=PORTD|8;
    PORTD=PORTD&(0b11110111);
    _delay_us(100);
    lcd_command(0x28);                  //5x8 dots, 2 lines
    lcd_command(0x0c);                   //display on, cursor off
    lcd_command(0x01);      
    _delay_us(4000);
    lcd_command(0x06);                      // Increase address, no display shift
}
unsigned char keypad_to_ascii(unsigned char p)          //This function finds the character
                                               //of the button that is pressed                                              
{  
   return ascii[p];    
}

void scan_row(void){                     //We read for every line that is set 0
                                         //if there is a pressed button by reading
                                         // 0 as input
    unsigned char data;
    data=PCA9555_0_read(REG_INPUT_1);
    data=data>>4;
    data=data&0x0F;
    for(int i=0; i<4; i++){
        value1[index]=data&0b00000001;    //We store the four bits we read
                                                //in the array value1
        data=data>>1;
        index=index+1;
    }
   
}

void scan_keypad(void){
    index=0;
    unsigned char temp=0b11111110;
    for(int i=0; i<4; i++){
        PCA9555_0_write(REG_OUTPUT_1, temp);
        scan_row();
        temp=temp<<1;
        temp=temp|0b00000001;        
    }
}

void scan_keypad_rising_edge(void){
        scan_keypad();

        for(int i=0; i<16; i++){
            value2[i]=value1[i];
        }
        _delay_ms(20);
        scan_keypad();
        for(int i=0; i<16; i++){
            if(value2[i]==1 && value1[i]==0)
            {
                code[counter]=i;
                counter=counter+1;
                    lcd_data(keypad_to_ascii((unsigned char)i));
                    _delay_ms(100);
                while(value1[i]==0){        //We wait for the button to be released
                    scan_keypad();          //and don't print on the screen
                }
                break;
            }
        }    
}

int main(void)
{  
    unsigned char correct[]="Access Granted\0";
    unsigned char wrong[]="Access Denied\0";
    DDRB=0xFF;
    PORTB=0;
    DDRD=0xFF;
    twi_init();
    lcd_init();
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0);
    while(1)
    {
        counter=0;
        while(counter<2){
        scan_keypad_rising_edge();
    }
        _delay_ms(500);
        lcd_command(0x01);
        _delay_ms(100);
        if(code[0]==10 && code[1]==14){
            PORTB=0xFF;
            for(int i=0; correct[i]!='\0'; i++){
       lcd_data(correct[i]);
    }    
            _delay_ms(4000);
        }
        else{
            for(int i=0; wrong[i]!='\0'; i++){
       lcd_data(wrong[i]);
      }
            for(int i=0; i<10; i++){  
                PORTB=0;
                _delay_ms(250);
                PORTB=0xFF;
                _delay_ms(250);
            }
        }
        lcd_command(0x01);
        PORTB=0;
    }
}