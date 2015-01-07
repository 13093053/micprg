/* Beep beep - Morse code generator
 *
 * Reads characters from zero terminated ascii string.
 * Case insensitive. Ignores special characters.
 * Program stops at the end of the string.
 *
 * Authors: Daniel Martoredjo & Koen van Vliet
 * Version: 0.1. v1 pre-release - Early access - Alpha
 * 
 *
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <ctype.h>

#define tick(xx) _delay_ms(100 * xx)

void buffer_on();
void buffer_off();
uint8_t get_morse_letter( char c );

void main( void ){
	volatile char c;					// Character
	volatile uint8_t cc;				// Character count
	volatile uint8_t mc, ml;			// Morse count & morse length
	volatile uint8_t b;					// Buffer for morse letter

	const char ascii_string[] = "Hello World";
	
	DDRD |= (1 << 5); 					// Set speaker as output

	TCCR1B |= 1 << WGM12;				// Timer mode CTC
	TCCR1A |= 1 << COM1A0;				// Output compare match toggle mode.	
	OCR1A = 1842;
	TCCR1B |= (1 << CS12)| (1 << CS10); // Start timer at 2000Hz with f/1024 prescaler


	while(1)
	{	
		c = ascii_string[cc ++];		// Read character from string and increment character counter
		
		if( c == ' ' )
		{
			tick(7);
		}
		else if( c != 0 )
		{
			b = get_morse_letter(c);	// Look up morse letter corresponding to the character
			if(b)
			{
				ml = b & 0x07;
				for( mc = 0; mc < ml; mc ++ )
				{
					buzzer_on();		// Enable buzzer
					if( b & (1<<7) )	// 1: Dash
					{
						tick(3);
					}
					else				// 0: Dot
					{
						tick(1);
					}
					buzzer_off();		// Disable buzzer
					tick(1);
				}
				tick(2);				// Delay in between characters (2+1).
			}
		}
		else
		{
			break;
		}
	}
	while(1)							// End (loop forever)
	{}
}

void buzzer_on()
{
	TCCR1A |= 1 << COM1A0;				// Enable timer1 output
}

void buzzer_off()
{
	TCCR1A |= 1 << COM1A0;				// Disable timer1 output
	PORTB |= 1 << 5; 					// Set pin high to prevent clicking sound
}

uint8_t get_morse_letter( char c )
{
	volatile uint8_t ml;
	uint8_t morseLetters[] = {    		// Morse codes from A - Z 
		0x42, 0x84, 0xA4, 0x83, 0x01, 0x24, 0xC3, 0x04, 0x02, 0x74, 0xA3, 0x44, 0xC2,
		0x82, 0xE3, 0x64, 0xD4, 0x43, 0x03, 0x81, 0x23, 0x14, 0x63, 0x94, 0xB4, 0xC4
	};

	uint8_t morseDigits[] = {    		// Morse codes from 0 - 9 
		0xFD, 0x7D, 0x3D, 0x1D, 0x0D, 0x05, 0x85, 0xC5, 0xE5, 0xF5
	};
	
	if( isalnum(c) )
	{
		if ( isalpha(c) )
		{
			c = toupper(c);				// It is a letter. Make it uppercase.
			ml = morseLetters[ c - 'A' ];
		}
		else if ( isdigit(c) )
		{
			ml = morseDigits[ c - '0' ];
		}
	}
	else
	{
		ml = 0;
	}

	return ml;
}
