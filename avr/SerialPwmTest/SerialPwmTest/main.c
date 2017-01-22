/*
 * SerialPwmTest.c
 *
 * Created: 1/13/2017 9:01:47 PM
 * Author : csauer
 */ 

#define F_CPU 8000000UL
#define BAUDRATE 38400
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
#define CLENGTH 11
#define START 0xFF

#include <avr/io.h>
#include <util/delay.h>
#include <util/crc16.h>

void USART_init();
void USART_send(uint8_t);
uint8_t USART_receive();
void PWMInit();
void setPWM(uint8_t[]);
void clearCommand(uint8_t[]);
void zeroAll();
int CRCcheck(uint8_t[]);

int main(void)
{
	uint8_t command[CLENGTH];
	int i;
	uint16_t voltRef = 0;
	clearCommand(command);
    PWMInit();
	zeroAll();
	USART_init();
	
	while (1) {
		for (i=0; i < CLENGTH; i++) {             // fragile, switch to UART interrupts
			command[i] = USART_receive();         // to enable smoother acceleration/motion
			if (i > 0 && command[i] == 0xFF) {    // track current vs target motor values,
				command[0] = 0xFF;                // change gradually in main loop
				i = 0;
			}
		}
		if (CRCcheck(command) == 0) {
			setPWM(command);
			voltRef = 730; // dummy adc read
		} else {
			voltRef = 0;
		}
		USART_send(voltRef >> 8);
		USART_send(voltRef & 0xFF);
		clearCommand(command);
	}
}

void USART_init() {
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));
}

void USART_send(uint8_t data) {
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

uint8_t USART_receive() {
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

	// 16-bit, 50 hz, use 10-bit values for input
	ICR1 = 19999;
	TCCR1A |= ((1<<COM1A1) | (1<<COM1B1) | (1<<WGM11));
	TCCR1B |= ((1<<WGM13) | (1<<WGM12) | (1<<CS11));

	TCCR2A |= ((1 << COM2A1) | (1<<COM2B1) | (1<<WGM20));
	TCCR2B |= ((1<<WGM20) | (1<<CS20));
	
	// set all pwm pins to output
	DDRB |= ((1<<DDB1) | (1<<DDB2) | (1<<DDB3));
	DDRD |= ((1<<DDD3) | (1<<DDD5) | (1<<DDD6));
}

void clearCommand(uint8_t c[]) {
    int i;
	for (i=0; i<CLENGTH; i++) {
		c[i] = 0;
	}
}

void setPWM(uint8_t c[]) {
	// motor forward l/r
	OCR0A = c[1];
	OCR0B = c[2];

    // servo vert/horiz	
	uint16_t look_vertical = (c[5] << 8) | c[6];
	uint16_t look_horizontal = (c[7] << 8) | c[8];
	OCR1A = ICR1 * (1.0 + (look_vertical/1023.0)) / 20;
	OCR1B = ICR1 * (1.0 + (look_horizontal/1023.0)) / 20;
	//OCR1A = c[6] << 8; // ICR1 * 2 / 20; // 2 ms pulse  /1ms = left/up, 2ms = right/down? 1.5ms = center
	//OCR1B = c[9] << 8; // ICR1 * 1.5 / 20; // 1.5 ms pulse
	
	// motor backward l/r
	OCR2A = c[3];
	OCR2B = c[4];
}

void zeroAll() {
	OCR0A = 0;
	OCR0B = 0;
	
	OCR1A = ICR1 * 1.5 / 20;
	OCR1B = ICR1 * 1.5 / 20;
	
	OCR2A = 0;
	OCR2A = 0;
}

int CRCcheck(uint8_t c[]) {
	uint16_t crc = 0xFFFF;
	for (int i = 0; i < CLENGTH; i++) {
		crc = _crc16_update(crc, c[i]);
	}
	return crc;
}
