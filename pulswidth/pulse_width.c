#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include "lcd.h"
#include <util/delay.h>


volatile bool lf = true;
volatile uint16_t pw = 1234;
//      ____                       _____
// ____|    |_____________________|     |_____________________|
// LOW  HIGH  LOW
//          | Start counting      | Stop counting
// 

#define ICP PINB0

ISR(TIMER1_CAPT_vect)
{
	pw=ICR1;
	TCNT1 = 0;
	lf = true;
}
//ISR(__vector_default){}

int main()
{
	char buffer[17];					// Normal mode, no prescaler
	TIMSK = 1 << TICIE1;				// Enable input capture interrupts
	TCCR1A = 0x00;
	TCCR1B = ( 1 << ICNC1 ) | (1 << ICES1) | (1 << CS10);
  	TCNT1 = 0x0000;

	lcd_init();							// Initialize lcd
    lcd_cursor(false, false);			// Cursor off

	sei();

	while(1)
	{
		if(lf)
		{
			lcd_home();
	        lcd_puts("Pulsbreedte is:");	// Print pulsewidth
	        lcd_goto(1, 0);
			pw = (uint16_t)(pw / 3.6864);
	        snprintf(buffer, sizeof buffer, "%12d us", pw);
	        lcd_puts(buffer);
			lf = false;					// Clear update flag
			_delay_ms(1000);
		}
	}
}
