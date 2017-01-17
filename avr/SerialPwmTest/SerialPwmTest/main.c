/*
 * SerialPwmTest.c
 *
 * Created: 1/13/2017 9:01:47 PM
 * Author : csauer
 */ 

#define F_CPU 8000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
#define CLENGTH 9
#define START 0xFF

#include <avr/io.h>
#include <util/delay.h>

void USART_init();
void USART_send(unsigned char);
unsigned char USART_receive();
void PWMInit();
void setPWM(unsigned char[]);
void clearCommand(unsigned char[]);
int CRCcheck(unsigned char[]);

int main(void)
{
	unsigned char command[CLENGTH];
	int cbuff;
	clearCommand(command);
    PWMInit();
	setPWM(command);
	USART_init();
	
    while (1) 
    {
		cbuff = USART_receive();
		if (cbuff == START) {
			command[0] = START;
			int count = 1;
			int sum = 0;
			do {
				cbuff = USART_receive();
				command[count] = cbuff;
				sum += cbuff;
				count++;
				if (count == 8) {
					if (CRCcheck(command) == 1) {
						setPWM(command);
						for (int i = 0; i < CLENGTH; i++) {
							USART_send(command[i]);
						}
						clearCommand(command);
						//USART_send(sum >> 8);
						//USART_send(sum & 0xFF);
						break;
					} else {
						for (int i = 0; i < CLENGTH; i++) {
							USART_send(0xFF);
							//USART_send(0xFF);
						}
						break;
					}
				}
			} while ((count < 8) && (cbuff != START));
		}
	}
}

void USART_init() {
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));
}

void USART_send(unsigned char data) {
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

unsigned char USART_receive() {
	// change to receive all
	// block until data
	// read until not data
	// read into buffer
	// read buffer for start bytes into secondary command buffer
	// execute commands
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void PWMInit() {
	PORTB = 0;
	PORTD = 0;
	
	// set all 3 timer counters to clear OC* when up-counting, phase-correct pwm,
	// no prescaling
	TCCR0A |= ((1<<COM0A1) | (1<<COM0B1) | (1<<WGM00));
	TCCR0B |= ((1<<WGM00) | (1<<CS00));

	// use 8-bit pwm on timer1
	TCCR1A |= ((1<<COM1A1) | (1<<COM1B1) | (1<<WGM10));
	TCCR1B |= ((1<<WGM10) | (1<<CS10));

	TCCR2A |= ((1 << COM2A1) | (1<<COM2B1) | (1<<WGM20));
	TCCR2B |= ((1<<WGM20) | (1<<CS20));
	
	// set all pins to output
	DDRB |= ((1<<DDB1) | (1<<DDB2) | (1<<DDB3));
	DDRD |= ((1<<DDD3) | (1<<DDD5) | (1<<DDD6));
}

void clearCommand(unsigned char c[]) {
    int i;
	for (i=0; i<CLENGTH; i++) {
		c[i] = 0;
	}
}

void setPWM(unsigned char c[]) {
	OCR0A = c[1];
	OCR0B = c[2];
	OCR1A = c[3];
	OCR1B = c[4];
	OCR2A = c[5];
	OCR2B = c[6];
}

int CRCcheck(unsigned char c[]) {
	uint16_t crc = 0x0000;
	// this is a fake crc
	for (int i = 1; i < (CLENGTH-2); i++) {
		//crc ^= (c[i] << i);
		crc += c[i];
	}
	//if (((crc >> 8) == c[7]) && ((crc & 0xFF) == c[8])) {
	return 1;
	//}
    //return 0;
}
