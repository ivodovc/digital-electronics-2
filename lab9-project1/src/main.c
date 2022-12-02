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
#include <lcd.h>   

// rotary encoder
#define ROT_SW 2
#define ROT_CLK 2
#define ROT_DT 3

#define JOY_SW 2

// Global Variables
uint8_t hours=0, seconds=0, minutes=1, tenths=0; // timer
uint8_t set_hours=0, set_seconds=0, set_minutes=1, set_tenths=0; // timer
uint8_t realx=0, realy=0; // curosr
#define STARTED 1
#define STOPPED 0
uint8_t state = STOPPED;

//Initialization of LCD display
void init_lcd()
{
    // Initialize display
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    // Put string(s) on LCD screen
    lcd_gotoxy(3, 0);
    lcd_puts("00:00:00:0");
    lcd_gotoxy(0, 1);
    lcd_puts("START");
    lcd_gotoxy(11, 1);
    lcd_puts("RESET");

    //Timer 2 is for LCD
    TIM2_overflow_16ms();
}

//Initialization of Joystick functions (ADC)
void init_joystick()
{
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

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

    //set up SW button
    DDRC &= ~(1<<JOY_SW);
    PORTC |= (1<<JOY_SW);
}

//Initialization of Rotary encoder
void init_encoder()
{
    // Initialize Rotary Encoder:
    DDRD &= ~(1<<ROT_SW);
    PORTD |= (1<<ROT_SW);

    DDRD &= ~(1<<ROT_CLK);
    PORTD |= (1<<ROT_CLK);

    DDRD &= ~(1<<ROT_DT);
    PORTD |= (1<<ROT_DT);
    
    // external interrupts
    EIMSK = 1<<INT0;
    EICRA = 1<<ISC00;
    EICRA &= ~(1<<ISC01);
}

void cursor_off()
{
    lcd_command(12);
}

void cursor_on()
{
    lcd_command(15);
    lcd_gotoxy(realx, realy);
}

void start_timer()
{
    TIM2_overflow_interrupt_enable();
    TIM1_overflow_interrupt_disable();
    EIMSK &= (~1<<INT0);
    set_hours = hours;
    set_minutes = minutes;
    set_seconds = seconds;
    set_tenths = tenths;
    cursor_off();
}

void stop_timer()
{
    TIM2_overflow_interrupt_disable();
    TIM1_overflow_interrupt_enable();
    EIMSK = 1<<INT0;
    cursor_on();
}

/*ADC  Channels
    Analog0 -> VRx
    Analog1 -> VRy
*/
int main(void)
{
    // Initialize USART to asynchronous, 8N1, 9600
    uart_init(UART_BAUD_SELECT(9600, F_CPU));
    uart_puts("Start of project...\n");

    init_lcd();
    init_joystick();
    init_encoder();

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

ISR(INT0_vect)
{  
    char string[1];
    uint8_t sw, clk, dt;
    sw = (PIND & (1<<ROT_SW));
    clk = (PIND & (1<<ROT_CLK))>>ROT_CLK;
    dt = (PIND & (1<<ROT_DT))>>ROT_DT;
    //SIMULIDE
    if (clk!=dt)
    {
        uart_puts("LEFT");
        switch (realx)
        {
            case 4:
                if(hours>0) hours--;
                break;
            
            case 7:
               if(minutes>0)  minutes--;
                break;

            case 10:
                if(seconds>0)  seconds--;
                break;

            case 12:
               if(tenths>0)  tenths--;
                break;
        }
    }else if (clk==dt)
    {
        uart_puts("RIGHT");
        switch (realx)
        {
            case 4:
                if(hours<99) hours++;
                break;
            
            case 7:
                if(minutes<59) minutes++;
                break;

            case 10:
                if(seconds<59) seconds++;
                break;

            case 12:
                if(tenths<9) tenths++;
                break;
        }
    }
    //REAL
    /*if (clk==0 && dt==1)
    {
        uart_puts("RIGHT");
    }else if (clk==1 && dt==0)
    {
        uart_puts("LEFT");
    }*/
    /*uart_puts("CLK read: ");
    itoa(clk, string, 10);
    uart_puts(string);
    uart_puts(", ");
    uart_puts("DT read: ");
    itoa(dt, string, 10);
    uart_puts(string);
    uart_puts("\n ");*/
    refresh_lcdtime();
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

    uint8_t joy_sw = (PINC & (1<<JOY_SW));
    if (joy_sw==0)
    {
        uart_puts("START");
        start_timer();
    }

    if (no_of_overflows >= 3)
    {
        no_of_overflows = 0;  
        ADCSRA |= (1<<ADSC);
    }
}

ISR(TIMER2_OVF_vect)
{
    static uint8_t no_of_overflows = 0;
    char string[2];// String for converted numbers by itoa()

    uint8_t rot_sw = (PINB & (1<<ROT_SW));
    if (rot_sw==0)
    {
        uart_puts("STOP");
        stop_timer();
    }
    no_of_overflows++;
    if (no_of_overflows >= 6)
    {
        // Do this every 6 x 16 ms = 100 ms
        // Count tenth of seconds 0, 1, ..., 9, 0, 1, ...
        no_of_overflows = 0;
        tenths--;
        if (tenths > 250)
        {
            tenths = 9;
            seconds--;
                if (seconds  > 250)
                {
                    seconds = 59;
                    minutes--;
                    if (minutes  > 250)
                    {
                        minutes = 59;
                        hours--;
                        if (hours  > 250){
                            // timer done
                            tenths = 0;  // Tenths of a second
                            seconds = 0; // Seconds of a second
                            minutes = 0; // Minutes of a second
                            hours = 0;

                        }
                    }
                }
        }
    }
    refresh_lcdtime();
}

void refresh_lcdtime()
{
    char string[2];
    cursor_off();
        {
            lcd_gotoxy(12, 0);

            itoa(tenths, string, 10); // Convert decimal value to string
            lcd_puts(string);
        }

        {
            lcd_gotoxy(9, 0);
            if (seconds < 10)
            {
                lcd_puts("0");
            }
            itoa(seconds, string, 10); // Convert decimal value to string
            lcd_puts(string);
        }
        {
            lcd_gotoxy(6, 0);
            if (minutes < 10)
            {
                lcd_puts("0");
            }
            itoa(minutes, string, 10); // Convert decimal value to string
            lcd_puts(string);
        }

    lcd_gotoxy(3, 0);
    if (hours < 10)
    {
        lcd_puts("0");
    }
    itoa(hours, string, 10); // Convert decimal value to string
    lcd_puts(string);
    cursor_on();
}

void cursor_change(int8_t x_direction,int8_t y_direction)
{
    char string[10];
    static uint8_t vcursor_x=0;
    static uint8_t vcursor_y=0;
    if (x_direction == 1 && vcursor_x<3){
        vcursor_x++;
    }
    if (x_direction == -1 && vcursor_x>0){
        vcursor_x--;
    }
    uint8_t positions[] = {4,7,10,12};
    realx = positions[vcursor_x];

    uart_puts("realx ");
    itoa(realx, string, 10);
    uart_puts(string);
    uart_puts("\n ");
    lcd_gotoxy(realx, 0);
}
/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display converted value on LCD screen.
 **********************************************************************/
ISR(ADC_vect)
{
    uint16_t value;
    char string[10];  // String for converted numbers by itoa()
    static uint8_t cursorx=0;
    static uint8_t cursory=0;
    // slovenska premenna pretoze ma nic ine nenapada, a dlho som nepisal po slovensky, ale mal by som zacat pisat bakalarku
    static uint8_t doraz = 0; //0 neni doraz, 1 je doraz

    // Read converted value
    // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
    
    value = ADC;
    if ((ADMUX & 7) == 0) {
        // X AXIS ADC
        if (value > 1000)
        {
            if (doraz==0)
            cursor_change(1,0);
            doraz = 1;
        }else if (value < 30){
            if (doraz==0)
            cursor_change(-1,0);
            doraz = 1;
        }else{
            // nulujem doraz
            doraz = 0;
        }
        // start reading y joystick position
        // Select input channel ADC1 (Y joystick)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1));
        ADMUX |= (1<<MUX0);
        // start conversion
        ADCSRA |= (1<<ADSC);
    }else if  ((ADMUX & 7)  == 1){
        // Y AXIS ADC
        // if y value is being read
        if (value > 1010)
        {
            cursory=1;
        }else if (value < 5){
            cursory = 0;
        }
        // select channel back to x input (channel 0)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
    }
    //lcd_gotoxy(cursorx, cursory);
}
// Else do nothing and exit the ISR