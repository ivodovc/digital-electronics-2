/***********************************************************************
 * 
 * LCD Timer controlled by 2Axis Joystick and rotary encoder
 * Digital Electronics 2 Team Project 1
 * 5.12.2022
 * 
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *  
 * 
 * Based on code from Digital Electronics 2 Course
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

// joystick switch pin
#define JOY_SW 2

// Global Variables
uint8_t hours=0, seconds=0, minutes=1, tenths=0; // timer
uint8_t set_hours=0, set_seconds=0, set_minutes=1, set_tenths=0; // timer
uint8_t realx=0, realy=0; // real x,y cursor position on lcd
uint8_t rot_sw_state=1, joy_sw_state=1; // previos state to prevent multiple actions on single click
#define STARTED 1
#define STOPPED 0
uint8_t state = STOPPED;

#define LED_GREEN 5

//Initialization of LCD display
void init_lcd()
{
    // Initialize display
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    // Put string(s) on LCD screen
    lcd_gotoxy(3, 0);
    lcd_puts("00:00:00:0");
    lcd_gotoxy(5, 1);
    lcd_puts("STOPPED");

    //Timer 2 is for LCD
    TIM2_overflow_16ms();
}

//Initialization of Joystick functions (ADC)
void init_joystick()
{
    // Timer1 is for starting ADC conversion
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
    lcd_gotoxy(realx, 0);
}

// refresh lcd with new values
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

// Start Clock Timer on LCD
void start_timer()
{
    TIM2_overflow_interrupt_enable();
    TIM1_overflow_interrupt_disable();
    //disable rotary encoder interrupts
    EIMSK &= (~1<<INT0);
    //save set values to reset back to
    set_hours = hours;
    set_minutes = minutes;
    set_seconds = seconds;
    set_tenths = tenths;
    lcd_gotoxy(5, 1);
    lcd_puts("STARTED");
    cursor_off();
}

// Stop Clock Timer on LCD
void stop_timer()
{
    TIM2_overflow_interrupt_disable();
    TIM1_overflow_interrupt_enable();
    //enable rotary encoder interrupts
    EIMSK = 1<<INT0;
    lcd_gotoxy(5, 1);
    lcd_puts("STOPPED");
    cursor_on();
}


void timer_runout(){
    //if timer ran out blink LED or buzzer
     PORTB |= (1<<LED_GREEN);
}

void reset_timer(){
    hours = set_hours;
    minutes= set_minutes;
    seconds = set_seconds;
    tenths = set_tenths;
    refresh_lcdtime();
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

    // Call all initialization functions
    init_lcd();
    init_joystick();
    init_encoder();

    // init LED/buzzer
    DDRB |= (1<<LED_GREEN);

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
    uint8_t sw, clk, dt;
    sw = (PIND & (1<<ROT_SW));
    clk = (PIND & (1<<ROT_CLK))>>ROT_CLK;
    dt = (PIND & (1<<ROT_DT))>>ROT_DT;
    // SIMULIDE
    // In reality check if clk>dt or clk<dt
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
    refresh_lcdtime();
}

// Interrupt Vector for Timer1 overflow
// Used for starting ADC conversion of joystick position
ISR(TIMER1_OVF_vect)
{
    static uint8_t no_of_overflows = 0;
    no_of_overflows++;

    uint8_t joy_sw = (PINC & (1<<JOY_SW)) >> JOY_SW;
    uint8_t rot_sw = (PINB & (1<<ROT_SW)) >> ROT_SW;
    // Start timer
    if (joy_sw==0)
    {
        //do it only once per click
        if (joy_sw_state==1){
            //otherwise start the clock
            uart_puts("START");
            start_timer();
        }
    }
    joy_sw_state=joy_sw;

    // Reset timer back to set time
    if (rot_sw==0)
    {
        //do it only once per click
        if (rot_sw_state==1){
            //otherwise start the clock
            uart_puts("RESET");
            reset_timer();
        }
    }
    rot_sw_state=rot_sw;

    if (no_of_overflows >= 3)
    {
        no_of_overflows = 0;  
        ADCSRA |= (1<<ADSC);
    }
}

// Interrupt Vector for Timer2 overflow
// Used for LCD Timer
ISR(TIMER2_OVF_vect)
{
    static uint8_t no_of_overflows = 0;

    uint8_t joy_sw = (PINC & (1<<JOY_SW)) >> JOY_SW;
    if (joy_sw==0)
    {
        //do it only once per click
        if (joy_sw_state==1){
            //otherwise start the clock
            uart_puts("STOP");
            stop_timer();
        }
    }
    joy_sw_state=joy_sw;

    /*uint8_t rot_sw = (PINB & (1<<ROT_SW));
    if (rot_sw==0)
    {
        uart_puts("STOP");
        stop_timer();
    }*/
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
                            stop_timer();
                            timer_runout();
                        }
                    }
                }
        }
    }
    refresh_lcdtime();
}

void cursor_change(int8_t x_direction,int8_t y_direction)
{
    char string[10];
    static uint8_t vcursor_x=0;
    //static uint8_t vcursor_y=0; not needed for 2 lines, useful for multiline display
    if (x_direction == 1 && vcursor_x<3){
        vcursor_x++;
    }
    if (x_direction == -1 && vcursor_x>0){
        vcursor_x--;
    }
    if (y_direction == 1){
        realy=1;
    }else if (y_direction==-1) {
        realy=0;
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
            cursor_change(0,1);
        }else if (value < 5){
            cursor_change(0,-1);
        }
        // select channel back to x input (channel 0)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
    }
}