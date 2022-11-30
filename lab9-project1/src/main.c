/***********************************************************************
 * 
 * Use Analog-to-digital conversion to read push buttons on LCD keypad
 * shield and display it on LCD screen.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2018 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <stdlib.h>         // C library. Needed for number conversions
#include <uart.h> 

#define SW 0
#define CLK 1
#define DT 2

/*ADC TEST
    Analog0 -> VRx
    Analog1 -> VRy
*/
int main(void)
{
    // Initialize USART to asynchronous, 8N1, 9600
    uart_init(UART_BAUD_SELECT(9600, F_CPU));
    uart_puts("Start of project...\n");

    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    ADMUX |= (1<<REFS0); //1
    ADMUX &= ~(1<<REFS1); //0
    // Select input channel ADC0 (voltage divider pin)
    ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0)); //0
    // Enable ADC module
    ADCSRA |= (1<<ADEN);
    // Enable conversion complete interrupt
    ADCSRA |= (1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 33 ms and enable overflow interrupt
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

    // Initialize Rotary Encoder:
    DDRB &= ~(1<<SW);
    PORTB |= (1<<SW);
    DDRB &= ~(1<<CLK);
    PORTB  &= ~(1<<CLK);
    DDRB &= ~(1<<DT);
    PORTB  &= ~(1<<DT);

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion every 100 ms.
 **********************************************************************/


ISR(TIMER1_OVF_vect)
{
    static uint8_t no_of_overflows = 0;
    char string[1]; 
    no_of_overflows++;
    uint8_t sw, clk, dt;
    if (no_of_overflows >= 3)
    {
        no_of_overflows = 0;  
        ADCSRA |= (1<<ADSC);
        
        sw = (PINB & (1<<SW));
        clk = (PINB & (1<<CLK))>>CLK;
        dt = (PINB & (1<<DT))>>DT;
        //var1 = GPIO_read(&PORTB, 1);
        //var2 = GPIO_read(&PORTB, 2);
        uart_puts("SW read: ");
        itoa(sw, string, 10);
        uart_puts(string);
        uart_puts(", ");
        uart_puts("CLK read: ");
        itoa(clk, string, 10);
        uart_puts(string);
        uart_puts(", ");
        uart_puts("DT read: ");
        itoa(dt, string, 10);
        uart_puts(string);
        uart_puts("\n ");

        /*uart_puts("PortB1 read: ");
        itoa(var1, string, 10);
        uart_puts(string);
        uart_puts(", ");

        uart_puts("PortB2 read: ");
        itoa(var2, string, 10);
        uart_puts(string);
        uart_puts("\n");*/
    }

    
}

/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display converted value on LCD screen.
 **********************************************************************/
ISR(ADC_vect)
{
    uint16_t value;
    char string[10];  // String for converted numbers by itoa()
    static int x_value = 0;
    static int y_value = 0;

    // Read converted value
    // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
    
    value = ADC;
    if ((ADMUX & 7) == 0) {
        // if x value was read
        x_value = value;
        // start reading y joystick position
        // Select input channel ADC1 (Y joystick)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1));
        ADMUX |= (1<<MUX0);
        // start conversion
        ADCSRA |= (1<<ADSC);   
        /*uart_puts("X Value read: ");
        itoa(value, string, 10);
        uart_puts(string);*/
    }else if  ((ADMUX & 7)  == 1){
        // if y value is being read
        y_value = value;
        // select channel back to x input (channel 0)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
        /*uart_puts("Y Value read: ");
        itoa(value, string, 10);
        uart_puts(string);
        uart_puts("\n");*/
    }
}