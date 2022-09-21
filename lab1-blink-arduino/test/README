# Lab 1: IVO DOVICAK

### Morse code

1. Listing of C code which repeats one "dot" and one "comma" (BTW, in Morse code it is letter `A`) on a LED. Always use syntax highlighting, meaningful comments, and follow C guidelines:

```c
#define SHORT_DELAY 250 // Delay in milliseconds
#define LONG_DELAY 750 // Dash is three times Dot delay
#define LED_GREEN PB5
#define PB5 13  

int main(void)
{
    // Set pin where on-board LED is connected as output
    pinMode(LED_GREEN, OUTPUT);

    // Infinite loop
    while (1)
    {
        //display dot blink
        digitalWrite(LED_GREEN, HIGH);
        _delay_ms(SHORT_DELAY);
        digitalWrite(LED_GREEN, LOW);
        _delay_ms(SHORT_DELAY);

        //display comma blink
        digitalWrite(LED_GREEN, HIGH);
        _delay_ms(LONG_DELAY);
        digitalWrite(LED_GREEN, LOW);

        // delay between 2 letters is same as dash delay
        _delay_ms(LONG_DELAY);

        // Turn ON/OFF on-board LED
        digitalWrite(LED_GREEN, led_value);
    }

    // Will never reach this
    return 0;
}
```

2. Scheme of Morse code application, i.e. connection of AVR device, LED, resistor, and supply voltage. The image can be drawn on a computer or by hand. Always name all components and their values!

   ![scheme_lab1_de2](lab1_schema_de2.png)
