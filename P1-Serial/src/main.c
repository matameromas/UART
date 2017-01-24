/*
Author: Luis Israel Rivera Rodriguez , Daniela Becerra, Sergio Flores
Date:   Jan, 2017
Description: Serial a patin 
*/
#include <avr/io.h>
#include <avr/wdt.h>

#define TB 13  
#define HIGH 0x20
#define LOW  0x00      
//TB(tiempo de bit) = 104us that invertionally proportional to 9615 bit/s
//Prescaler 16Mhz/64 = 250khz (4us) 

void UART_init(void);
void UART_send(unsigned char data);
void setup_timer(void);

void UART_init(void)
{
	PORTD=0x20;		//set high pin 6 of PORT D
	DDRD=0x20;		//configure pin 6 as output
}

void UART_send(unsigned char data)
{
	unsigned char volatile cont;			//used to count the send bits
	unsigned int volatile value_OUTCOMP;
	unsigned char volatile OUTCOMP_flag;
	
	cont = 8;
	value_OUTCOMP=0;
	OUTCOMP_flag=0;
	
	value_OUTCOMP=TCNT1+TB;			//time for next send
	PORTD=0x00;						//set low pin 6 of PORT D (START BIT)
	do{
		wdt_reset();
		OUTCOMP_flag = (TCNT1 >= value_OUTCOMP) ? 1:0;
	}while(OUTCOMP_flag==0);
	OUTCOMP_flag=0;
	
	
	do{
		value_OUTCOMP+=TB;					//Update next send time
		PORTD = data & 0x01? HIGH : LOW;	//mask to LSB to send 1 by 1 bit
		data>>=1;							//shift to send next bit
		OUTCOMP_flag=0;						
		do{
			wdt_reset();
			OUTCOMP_flag = (TCNT1 >= value_OUTCOMP) ? 1:0;
		}while(OUTCOMP_flag==0);
		
	}while(--cont);							//secure 8 bits are send
	
	PORTD = HIGH;							//STOP
	value_OUTCOMP += TB;
	do{
		wdt_reset();
		OUTCOMP_flag = (TCNT1 >= value_OUTCOMP) ? 1:0;
	}while(OUTCOMP_flag==0);				
	TCNT1 =0;
	
}
void setup_timer(void)
{
	TCCR1B = (1<<CS11)|(1<<CS10);    //1:64 prescaler (meterlo en timer_init)
}

int main(void)
{
	UART_init();
	setup_timer();
	
	for(;;)
	{
	    //Read timer value and act according with it
		UART_send(0x50);
		UART_send(0x25);
		UART_send(0x41);
		UART_send(0x0A);
		UART_send(0x0D);
    }
	return 0;
}

