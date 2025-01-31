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
unsigned char index;
unsigned char value_new[16];
unsigned char value_old[16];
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


void scan_row(void){                     //We read for every line that is set 0
                                         //if there is a pressed button by reading
                                         // 0 as input
    unsigned char data;
    data=PCA9555_0_read(REG_INPUT_1);          
    data=data>>4;
    data=data&0x0F;
    for(int i=0; i<4; i++){
        value_new[index]=data&0b00000001;     //We store the four bits we read
                                                //in the array value_new
        data=data>>1;
        index=index+1;
    }
   
}
void scan_keypad(void){                         //We enable every line of the keyboard
                                                //by setting 0. When this function
                                                //is over we have 16 bits in the array
                                                //value_new
    index=0;                                  //This variable is used as index
                                                //for the table value_new to store
                                                //which button is pressed
    unsigned char temp=0b11111110;
    for(int i=0; i<4; i++){
        PCA9555_0_write(REG_OUTPUT_1, temp);   //We enable all the 4 lines
                                               //by setting as output 0
                                               //and then we check if there is
                                               //a pressed button in this line
        scan_row();
        temp=temp<<1;
        temp=temp|0b00000001;      
    }
 
}
unsigned char keypad_to_ascii(unsigned char p)          //This function finds the character
                                               //of the button that is pressed                                              
{  
   return ascii[p];    
}

void scan_keypad_rising_edge(void){
    unsigned char X[16];
    unsigned char Y[16];
        scan_keypad();                  //We read the keypad and store the 16 bits
                                        //in X array
        for(int i=0; i<16; i++){
            X[i]=value_new[i];
        }
        _delay_ms(20);                  //We cause a delay for arcing problem
        scan_keypad();                  //We read the keypad again and store
                                        //into Y array
        for(int i=0; i<16; i++){
            Y[i]=value_new[i];
        }
        for(int i=0; i<16; i++){        //We compare X and Y and if and element
                                        //is different then this is due to arcing
                                        //and we set this element in Y 1, which means
                                        //the button was not pressed
            if(X[i]!=Y[i]){
                Y[i]=1;
            }
        }
        for(int i=0; i<16; i++){        //In Y we have the 16 bits that are surely
                                        //pressed and we store it in the value_new
            value_new[i]=Y[i];
        }
        for(int i=0; i<16; i++){        //We now compare the old values with the
                                        //new values and we print the first character
                                        //which had an old value of 1(it was not pressed)
                                        //and now has value 0 which means it was pressed.  
            if(value_old[i]==1 && value_new[i]==0){
                keypad_to_ascii(i);
                if(i == 3){ PORTB =0b1000 ;}
                else if(i == 6){PORTB =0b100 ;}
                else if(i == 9){PORTB =0b10 ;}
                else if(i == 12){PORTB =0b1 ;}
                 
                break;
            }
        }  
       
        for(int i=0; i<16; i++){        //We store the new values in the old value
                if(value_old[i]==0 && value_new[i]==1 ){
                    keypad_to_ascii(1); //reset the button
                    PORTB = 0;
                    }
            value_old[i]=value_new[i];

        }
       
}

int main(void)
{
    DDRB=0xFF;
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0);  //We define IO1_0-IO1_3 as outputs
                                                 //and IO1_4-IO1_7 as inputs
    for(int i=0; i<16; i++){
        value_old[i]=1;                          //We initialize the old values as 1
                                                 //because we assume no button is pressed
    }
    while(1)
    {
        scan_keypad_rising_edge();              //We scan the keypad
    }
   
}