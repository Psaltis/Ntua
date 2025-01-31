/*
 * File:   main.c
 * Author: Mpzaltis
 *
 * Created on December 7, 2023, 4:02 PM
 */

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
uint8_t temp;  //used for the lcd
unsigned char digits[10]={'0','1','2','3','4','5','6','7','8','9'};
unsigned char DekadesT,MonadesT,DekataT;
unsigned char DekadesP,MonadesP,DekataP;
int temperature=0;
float decimal_temperature,decimal_AD;//
unsigned char ascii[16]={'*','0','#','D','7','8','9','C','4','5','6','B','1','2','3','A'};
unsigned char pointer;
unsigned char value_new[16];
unsigned char value_old[16];
int emergencybutton;
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

//LCD Screen functions using PORTD expander via PCA9555
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
    temp=temp|8; //NA TO DOYME
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
    temp=temp|8; //NA TO DOYME
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
    temp=temp&(0b11110111);
    PCA9555_0_write(REG_OUTPUT_0,temp);
    _delay_us(100);
}
//Void to print on LCD screen
void lcd_data(unsigned char w){
    temp=temp|4;
    PCA9555_0_write(REG_OUTPUT_0,temp);
    write_2_nibbles(w);
     _delay_us(100);
}
//Void to clear and move cursor on the screen
void lcd_command(unsigned char b){
    temp=temp & (0b11111011);
    PCA9555_0_write(REG_OUTPUT_0,temp);
    write_2_nibbles(b);
     _delay_us(100);
}
//LCD screen initialization
void lcd_init(void){
    uint8_t m;
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
    _delay_ms(1);
    m=0x0c;
    lcd_command(m);
    _delay_ms(1);
    m=0x01;
    lcd_command(m);
    _delay_us(4000);
    m=0x06;
    lcd_command(m);
    _delay_ms(1);
}
//One wire communication functions for the use of the thermometer
unsigned char one_wire_reset(){
    unsigned char x;
    DDRD=DDRD|(0b00010000);
    PORTD=PORTD&(0b11101111);
    _delay_us(480);
    DDRD=DDRD&(0b11101111);
    PORTD=PORTD&(0b11101111);
    _delay_us(100);
    x=PIND;
    _delay_us(380);
    if((x&(0b00010000))==0b00000000){
        x=1;
    }
    else{
        x=0;
    }
    return x;
}

unsigned char one_wire_receive_bit(){
    unsigned char x;
    DDRD=DDRD|(0b00010000);
    PORTD=PORTD&(0b11101111);
    _delay_us(2);
    DDRD=DDRD&(0b11101111);
    PORTD=PORTD&(0b11101111);
    _delay_us(10);
    x=PIND;
    if((x&(0b00010000))==0b00010000){
        x=1;
    }
    else{
        x=0;
    }
    _delay_us(49);
    return x;
}

void one_wire_transmit_bit(unsigned char x){
    DDRD=DDRD|(0b00010000);
    PORTD=PORTD&(0b11101111);
    _delay_us(2);
    x=(x&(0b00000001));
    if(x==0){
        PORTD=PORTD&(0b11101111);
    }
    else{
        PORTD=PORTD|(0b00010000);
    }
    _delay_us(58);
    DDRD=DDRD&(0b11101111);
    PORTD=PORTD&(0b11101111);
    _delay_us(1);
}

unsigned char one_wire_receive_byte(){
    unsigned char x;
    unsigned char y=0;
    for(int i=0; i<8; i++){
        x=one_wire_receive_bit();
        y=y>>1;
        if(x==0){
            y=y|x;
        }
        else{
            y=y|(0x80);
        }
    }
    return y;
}

void one_wire_transmit_byte(unsigned char x){
    for(int i=0; i<8; i++){
        if((x&(0b00000001))==0){
            one_wire_transmit_bit(0b00000000);
        }
        else{
            one_wire_transmit_bit(0b00000001);
        }
        x=x>>1;
    }
}
//Void to turn binary temperature value into decimal amd print on screen with the oC symbol
void positive(int temperature){
    float x;
    x=temperature*0.0625+15.0;
    decimal_temperature=x;
    DekadesT=(unsigned char)x/10;
    x=x-DekadesT*10.0;
    MonadesT=(unsigned char)x;
    x=x-MonadesT*1.0;
    x=x*10;
    DekataT=(unsigned char)x;
    x=x-DekataT*1.0;
    lcd_command(0x01);
    _delay_us(200);
    lcd_data(digits[DekadesT]);
    lcd_data(digits[MonadesT]);
    lcd_data('.');
    lcd_data(digits[DekataT]);
    lcd_data((char)223);
    lcd_data('C');
}
//Void to read the temperature measurement and 
//check if it is truly positive
void read_temperature(void){
    unsigned char msg1[]="No Device";
    unsigned char reset_bit, low_bytes, high_bytes;
        temperature=0;
        reset_bit=one_wire_reset();
        if(reset_bit==0){
            for(int i=0; msg1[i]!=0; i++){
                lcd_data(msg1[i]);
            }
        }
        else{
            one_wire_transmit_byte(0xCC);
            one_wire_transmit_byte(0x44);
            while((one_wire_receive_bit())==0);
            one_wire_reset();
            one_wire_transmit_byte(0xCC);
            one_wire_transmit_byte(0xBE);
            low_bytes=one_wire_receive_byte();
            high_bytes=one_wire_receive_byte();
            temperature=temperature|high_bytes;
            temperature=temperature<<8;
            temperature=temperature|low_bytes;
            if((temperature&0b111110000000000)==0){
                positive(temperature);
            }
        }
}
//Functions for the usart 
void usart_init(unsigned int ubrr){
UCSR0A=0;
UCSR0B=(1<<RXEN0)|(1<<TXEN0);
UBRR0H=(unsigned char)(ubrr>>8);
UBRR0L=(unsigned char)ubrr;
UCSR0C=(3 << UCSZ00);
return;
}
void usart_transmit(uint8_t data){
while(!(UCSR0A&(1<<UDRE0)));
UDR0=data;
}
uint8_t usart_receive(){
while(!(UCSR0A&(1<<RXC0)));
return UDR0;
}
void usart_flash(void){
    unsigned char dump;
    while((UCSR0A&(1<<RXC0))){
        dump=UDR0;
    }
}
//Void that transmits "ESP:connect" message
void transmit_ESPconnect(void){
    uint8_t ESPconnect[]="ESP:connect\n";
    for(int i=0; ESPconnect[i]!=0; i++){
        usart_transmit(ESPconnect[i]);
    }
    
}
//Void that prints TimeOverflow if no Success message has been received when Success is been sent
//not necessary to use
void try_again(void){
    transmit_ESPconnect();
    usart_flash();
    uint8_t Successmsg1[]="1.Success";
    uint8_t Failmsg1[]="TimeoutError";
    int answer_received;
    answer_received=1;
    uint8_t answer[40];
    uint8_t Answer[]="\"Success\"";
    int i=0;
    answer_received=1;
    while(1){
        answer[i]=usart_receive();
        if(answer[i]=='\n'){
            break;
        }
        i++;
    }
    if(i!=9){
        answer_received=0;
    }
    else{
        for(int y=0; y<9; y++){
            if(answer[y]!=Answer[y]){
                answer_received=0;
            }
        }
    }
    lcd_command(0x01);
    _delay_us(400);
    usart_flash();
    if(answer_received==0){
        for(int i=0; Failmsg1[i]!=0; i++){
            lcd_data(Failmsg1[i]);
        }
        while(1);
    }
    else{
        for(int i=0; Successmsg1[i]!=0; i++){
            lcd_data(Successmsg1[i]);
        }
    }
}
//Starting from 8.1 print on screen the 1.Success/Fail message 
//depending on what it has been received via usart_receive
void receive_ESPconnect(void){
    uint8_t Successmsg1[]="1.Success";
    uint8_t Failmsg1[]="1.Fail";
    int answer_received;
    answer_received=1;
    uint8_t answer[100];
    uint8_t Answer[]="\"Success\"";
    int i=0;
        answer_received=1;
        while(1){
        answer[i]=usart_receive();
        if(answer[i]=='\n'){
            break;
        }
        i++;
        }
        lcd_command(0x01);
        _delay_us(400);
        if(i!=9){
            answer_received=0;
        }
        else{
            for(int y=0; y<9; y++){
                if(answer[y]!=Answer[y]){ 
                    answer_received=0;
                }
            }
        }
        usart_flash();
        if(answer_received==1){
            for(int i=0; Successmsg1[i]!=0; i++){
                lcd_data(Successmsg1[i]);
            }
        }
        else{
            for(int i=0; Failmsg1[i]!=0; i++){
                lcd_data(Failmsg1[i]);
            }
            try_again();
        }   
}
//Void to transmit URL to USART
void transmit_URL(void){
    uint8_t URL[]="ESP:url:\"http://192.168.1.250:5000/data\"\n";
    for(int i=0; URL[i]!=0; i++){
        usart_transmit(URL[i]);
    }
}
//Function to return whether the URL has been successfully received
//and print on screen 2.Success/Fail
int receive_URL(void){
    uint8_t Successmsg2[]="2.Success";
    uint8_t Failmsg2[]="2.Fail"; 
    int answer_received;
    answer_received=1;
    uint8_t answer[50];
    uint8_t Answer[]="\"Success\"";
    int i=0;
    while(1){
      answer[i]=usart_receive();
      if(answer[i]=='\n'){
          break;
      }
      i++;
    }
    if(i!=9){
        answer_received=0;
    }
    else{
        for(int y=0; y<i; y++){
            if(answer[y]!=Answer[y]){
                answer_received=0;
            }
        }
    }
    lcd_command(0x01);
    _delay_us(400);
    if(answer_received==1){
        for(int i=0; Successmsg2[i]!=0; i++){
            lcd_data(Successmsg2[i]);
        }
        return 1;
    }
    else{
       for(int i=0; Failmsg2[i]!=0; i++){
            lcd_data(Failmsg2[i]);
        }
        return 0;
    }    
}
//Functions for the ADC converter
//Turns binary measurement into decimal and prints it on screen
void read_AD(void){
    float apotelesma;
    apotelesma=ADC*5.0/1024;
    apotelesma=apotelesma*4.0;
    decimal_AD=apotelesma;
    DekadesP=(unsigned char)(apotelesma/10);
    apotelesma=apotelesma-(DekadesP*10);  
    MonadesP=(unsigned char) apotelesma;
    apotelesma=apotelesma-MonadesP;
    apotelesma=apotelesma*10;
    DekataP=(unsigned char) apotelesma;
    lcd_data(' ');
    lcd_data(' ');
    lcd_data(digits[DekadesP]);
    lcd_data(digits[MonadesP]);
    lcd_data('.');
    lcd_data(digits[DekataP]);
    lcd_data('m');
    lcd_data('m');
    lcd_data('H');
    lcd_data('g');
}
//Keypad voids
void scan_row(void){                     //We read for every line that is set 0
                                         //if there is a pressed button by reading 
                                         // 0 as input
    unsigned char data;
    data=PCA9555_0_read(REG_INPUT_1);          
    data=data>>4;
    data=data&0x0F;
    for(int i=0; i<4; i++){
        value_new[pointer]=data&0b00000001;     //We store the for bits we read 
                                                //in the table value_new
        data=data>>1;
        pointer=pointer+1;
    }
    
}
void scan_keypad(void){                         //We enable every line of the keyboard
                                                //by setting 0. When this function
                                                //is over we have 16 bits in the array
                                                //value_new
    
    pointer=0;                                  //This variable is used as index 
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
void scan_keypad_rising_edge(void){
    unsigned char X[16];
    unsigned char Y[16];
    scan_keypad();
    for(int i=0; i<16; i++){
        X[i]=value_new[i];
    }
    _delay_ms(20);
    scan_keypad();
    for(int i=0; i<16; i++){
        Y[i]=value_new[i];
    }
    for(int i=0; i<16; i++){
        if(X[i]!=Y[i]){
            Y[i]=1;
        }
    }
    for(int i=0; i<16; i++){
        value_new[i]=Y[i];
    }
    if (value_new[2]==0){
        emergencybutton=0;
    }
    if (value_new[14]==0){
       emergencybutton=1; 
    }     
}
//Void to trasmit with the payloads the digits of the measurements of temperature
//and pressure required
void print(unsigned char number){
    if(number==0) usart_transmit('0');
    if(number==1) usart_transmit('1');
    if(number==2) usart_transmit('2');
    if(number==3) usart_transmit('3');
    if(number==4) usart_transmit('4');
    if(number==5) usart_transmit('5');
    if(number==6) usart_transmit('6');
    if(number==7) usart_transmit('7');
    if(number==8) usart_transmit('8');
    if(number==9) usart_transmit('9');    
}
//Void that transmits the payloads 
void send_status(uint8_t status){
    uint8_t payload_temp[]="\"},{\"name\": \"team\",\"value\": \"63\"},{\"name\": \"status\",\"value\": \"CHECKTEMP\"}]\n";
    uint8_t payload_press[]="\"},{\"name\": \"team\",\"value\": \"63\"},{\"name\": \"status\",\"value\": \"CHECKPRESSURE\"}]\n";
    uint8_t payload_ok[]="\"},{\"name\": \"team\",\"value\": \"63\"},{\"name\": \"status\",\"value\": \"OK\"}]\n";
    uint8_t payload_nurse[]="\"},{\"name\": \"team\",\"value\": \"63\"},{\"name\": \"status\",\"value\": \"NURSECALL\"}]\n";
    uint8_t payload1[]="ESP:payload:[{\"name\": \"temperature\",\"value\": \"";
    uint8_t payload2[]="\"},{\"name\": \"pressure\",\"value\": \"";
    for(int k=0; payload1[k]!=0; k++){
        usart_transmit(payload1[k]);
    }
    print(DekadesT);
    print(MonadesT);
    usart_transmit('.');
    print(DekataT);
    for(int k=0; payload2[k]!=0; k++){
        usart_transmit(payload2[k]);
    }
    print(DekadesP);
    print(MonadesP);
    usart_transmit('.');
    print(DekataP);
    if(status==1){
        for(int i=0; payload_temp[i]!=0; i++){
            usart_transmit(payload_temp[i]);
        }
    }
    else if(status==2){
        for(int i=0; payload_press[i]!=0; i++){
            usart_transmit(payload_press[i]);
        }
    }
    else if(status==3){
        for(int i=0; payload_nurse[i]!=0; i++){
         usart_transmit(payload_nurse[i]);
        }
    }
    else
    {
        for(int i=0; payload_ok[i]!=0; i++){
            usart_transmit(payload_ok[i]);
        }
    }
}
//Void that transmits the "ESP:transmit\n" message
void transmit_ESPtransmit(void){
    uint8_t ESPtransmit[]="ESP:transmit\n";
    for(int i=0; ESPtransmit[i]!=0; i++){
        usart_transmit(ESPtransmit[i]);
    }   
}
//Void that checks if the status has been successfully received and prints
//on screen 3.Success/Fail
void receive_status(void){
    uint8_t Successmsg3[]="3.Success";
    uint8_t Failmsg3[]="3.Fail";
    int answer_received;
    answer_received=1;
    uint8_t answer[100];
    uint8_t Answer[]="\"Success\"";
    int i=0;
        answer_received=1;
        while(1){
        answer[i]=usart_receive();
        if(answer[i]=='\n'){
            break;
        }
        i++;
        }
        lcd_command(0x01);
        _delay_us(400);
        if(i!=9){
            answer_received=0;
        }
        else{
            for(int y=0; y<9; y++){
                if(answer[y]!=Answer[y]){ 
                    answer_received=0;
                }
            }
        }
        usart_flash();
        if(answer_received==1){
            for(int i=0; Successmsg3[i]!=0; i++){
                lcd_data(Successmsg3[i]);
            }
            transmit_ESPtransmit();
        }
        else{
            for(int i=0; Failmsg3[i]!=0; i++){
                lcd_data(Failmsg3[i]);
            } 
        }
}
//Prints on screen the server's answer
void final_result(){
    uint8_t answer[20];
    int i=0;
    while(1){
        answer[i]=usart_receive();
        if(answer[i]=='\n') break;
        i++;
    }
    lcd_command(0x01);
    _delay_us(400);
    lcd_data('4');
    lcd_data('.');
    for(int y=0; y<i; y++){
        lcd_data(answer[y]);
    }
    
}
//Main that calls all of the above functions and voids
int main(void){
    uint8_t CheckPressure[]="CHECK PRESSURE";
    uint8_t CheckTemperature[]="CHECK TEMP";
    uint8_t CallNurse[]="NURSE CALL";
    uint8_t status;
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00);
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0);
    DDRC=0;
    ADMUX=0b01000000;
    ADCSRA=0b10000111;
    lcd_init();
    _delay_us(4000);
    lcd_command(0x01);
    _delay_us(400);
    usart_init(103);

    while(1){
        transmit_ESPconnect();
        receive_ESPconnect();
        _delay_ms(500);
        transmit_URL();
        uint8_t url=receive_URL();
        _delay_ms(500);
        if(url){
            scan_keypad_rising_edge();
            lcd_command(0x01);
            _delay_us(200);
            read_temperature();
            ADCSRA|=(1<<ADSC);
            while((ADCSRA & 0x40) ==0x40);
            read_AD();
            lcd_command(0b11000000);
            _delay_us(200);
            if(decimal_temperature>37.0 || decimal_temperature<34.0){
                status=1;
                emergencybutton=0;
                for(int k=0; CheckTemperature[k]!=0; k++){
                    lcd_data(CheckTemperature[k]);
                }                
            }
            else if(decimal_AD>12 || decimal_AD<4){
                status=2;
                emergencybutton=0;
                for(int k=0; CheckPressure[k]!=0; k++){
                    lcd_data(CheckPressure[k]);
                }               
            }
            else if(emergencybutton==1){
                status=3;
                for(int k=0; CallNurse[k]!=0; k++){
                    lcd_data(CallNurse[k]);
                }
            }
            else{
                status=0;
                lcd_data('O');
                lcd_data('K');
            }
            _delay_ms(500);
            send_status(status);
            receive_status();
            final_result();
            _delay_ms(800);
        }
    }
}




