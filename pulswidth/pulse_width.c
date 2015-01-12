#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include "lcd.h"

volatile bool lf = false;
volatile uint16_t pw = 1234;
//      ____                       _____
// ____|    |_____________________|     |_____________________|
// LOW  HIGH  LOW
//          | Start counting      | Stop counting
// 

#define ICP PINB0

ISR(TIMER1_COMPA_vect)
{
	pw=ICR1;
	TCNT1 = 0;
}
ISR(__vector_default){}

int main()
{
	char buffer[17];					// Normal mode, no prescaler
	TIMSK = 1 << TICIE1;				// Enable input capture interrupts
	TCCR1B = (1 << ICES1) | (1 << CS10);

	lcd_init();							// Initialize lcd
    lcd_cursor(false, false);			// Cursor off

	sei();

	label:
	if(lf)
	{
		lcd_home();
        lcd_puts("Pulsbreedte is:");	// Print pulsewidth
        lcd_goto(1, 0);
        snprintf(buffer, sizeof buffer, "%12d us", pw);
        lcd_puts(buffer);
		lf = false;					// Clear update flag
	}
	goto label;
}
