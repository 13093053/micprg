#include <stdio.h>
#include "lcd.h"

int main(void) {
    char buffer[17];
    int i = 30000;
    lcd_init();
    lcd_cursor(false, false); /* cursor uit */

    while (i > 0) {
        lcd_home();
        lcd_puts("0123456789ABCDEF");
        lcd_goto(1, 0);
        snprintf(buffer, sizeof buffer, "i = %12d", i++);
        lcd_puts(buffer);
    }
    
    lcd_cls();
    lcd_puts("Hallo");
    lcd_goto(lcd_get_row() + 1, lcd_get_column() - 1);
    lcd_puts("AVR wereld !");

    while (1);
    return 0;
}
