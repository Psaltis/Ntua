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
    	transmit_ESPconnect();//we transmit the command ESP:connect\n
    	receive_ESPconnect();//we receive from usart the success/fail message
    	_delay_ms(500);
    	transmit_URL();//we transmit the url http://192.168.1.250:5000/data\ through the ESP to connect to the server
    	uint8_t url=receive_URL();//receive success or fail message for the transmit url command
    	_delay_ms(500);
    	if(url){
//url = 1 if the url was succesfully transmitted, therefore we can move on to the measurements
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
        	send_status(status);//we transmit the appropriate payload
        	receive_status();//we receive the success/error in message reception from server
        	final_result();//we print that message on to the LCD
        	_delay_ms(800);
    	}
	}
}