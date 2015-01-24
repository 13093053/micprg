#include <avr/io.h>
#include <stdio.h>

int main(void) {
    char c;
	float adcval;
    // 9600-7-E-1
    UCSRA = 0x00;
    UCSRB = 0x18;
    UCSRC = 0xA4;
    UBRRH = 0;
    UBRRL = 23;
    // division factor fclk / fadc = 3686 / 200 = 18.4 Kies 32
    // fadc = 3686 / 32 = 115 KHz.
    ADMUX = 0;
    ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS0;

    while (1) {
        while (~UCSRA & 1<<RXC);
        if (UCSRA & (1<<FE | 1<<DOR | 1<<PE)) {
            c = UDR;
            while (~UCSRA & 1<<UDRE);
            UDR = '?';
        }
        else {
            c = UDR;
            if (c == 'S') {
                char s[50];
                int i;
		ADCSRA |= 1<<ADSC | 1<<ADIF;
      		while (~ADCSRA & 1<<ADIF);
        
       		// 1 stapje = 5 / 1024 = 0.0048828125 V = 4.8828125 mV
       		// fout = +/- 2.44 mv. Geef antwoord met 2 cijfers achter de punt
		adcval = 5.0 * (ADC + 0.5) / 1024;
		snprintf(s, sizeof(s), "%d.%dVQ", (int)(adcval),(int)((adcval - (int)(adcval))*100));
                for (i = 0; s[i] != '\0'; i++) {
                    while (~UCSRA & 1<<UDRE);
                    UDR = s[i];
                }
            }
        }
    }
    return 0;
}
