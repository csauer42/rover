/*
 * SerialPwmTest.c
 *
 * Created: 1/13/2017 9:01:47 PM
 * Author : csauer
 */ 

#define F_CPU 8000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
#define CLENGTH 6

#include <avr/io.h>
#include <util/delay.h>

void USART_init();
void USART_send(unsigned char);
unsigned char USART_receive();
void PWMInit();
void setPWM(unsigned char[]);
void clearCommand(unsigned char[]);

int main(void)
{
	unsigned char command[CLENGTH];
	int cbuff;
	unsigned short retval = 0;
	int i = 0;
	clearCommand(command);
    PWMInit();
	setPWM(command);
	USART_init();
	
    while (1) 
    {
		cbuff = USART_receive();
		command[i] = cbuff;
		retval += cbuff;
		i++;
		if (i == 5) {
			setPWM(command);
			_delay_ms(20);
			USART_send(retval >> 8);
			USART_send(retval & 0xFF);
			clearCommand(command);
			retval = 0;
			i = 0;
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
	OCR0A = c[0];
	OCR0B = c[1];
	OCR1A = c[2];
	OCR1B = c[3];
	OCR2A = c[4];
	OCR2B = c[5];
}
