/*
Author: Luis Israel Rivera Rodriguez , Daniela Becerra, Sergio Flores
Date:   Jan, 2017
Description: Software made UART
*/

/************* Includes *************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

//TB(tiempo de bit) = 104us that invertionally proportional to 9615 bit/s
//Prescaler 16Mhz/64 = 250khz (4us)
#define TB 13  
#define HIGH 0x20
#define LOW  0x00
#define DATA_LENGTH 10 
#define STOP_BIT	1
typedef unsigned char u8;     


/************* Function Prototypes *************/
void UART_init(void);
void UART_send(unsigned char data);
void TIMER_init(void);

/*******   Global variables ****************/
u8 THR;							//Transmit holding register
u8 volatile cont;				//used to count the bits we'll send


void UART_init(void)
{
	PORTD=0x20;		//set high pin 6 of PORT D
	DDRD=0x20;		//configure pin 6 as output
}

ISR(TIMER1_COMPA_vect){						//This is our timer interrupt service routine
	// the output compare flag is automatically cleared when the interrupt is executed
	OCR1A+=TB;
	cont--;
	if((cont<=DATA_LENGTH) && (cont!=STOP_BIT))
	{
		PORTD = THR & 0x01 ? HIGH : LOW;		//mask to LSB to send 1 by 1 bit
		THR>>=1;								//shift to send next bit
	}
	if(cont==STOP_BIT)
	{
		PORTD = HIGH;
	}
	if(cont==0)
	{
		TIMSK1 = (0<<OCIE1A);						//Disable timer interrupts, we're done
	}
	wdt_reset();									//reset watchdog
}

void UART_send(unsigned char data)
{
	cont=DATA_LENGTH;								//8 bits plus start and stop bit
	OCR1A=TCNT1+TB;									//time for next send
	PORTD=0x00;										//set low pin 6 of PORT D (START BIT)
	THR=data;										//put the data in the global variable THR
	wdt_reset();									//reset watchdog
	TIMSK1 = (1<<OCIE1A);							//Enable timer interrupts	
}

void TIMER_init(void)
{												// CTC mode allows for automatic OutComp
	TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10);    //Enable CTC mode, and set prescaler to 1:64
}

int main(void)
{
	UART_init();
	TIMER_init();
	sei();										//Enable global interrupts
	
	for(;;)
	{
	    //Read timer value and act according with it
		UART_send(0x45);
		UART_send(0x72);
		UART_send(0x4C);
		UART_send(0x6f);
		UART_send(0x0A);
		UART_send(0x0D);
    }
	return 0;
}

