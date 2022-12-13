/***********************************************************************
 * 
 * Blink two LEDs using functions from GPIO and Timer libraries. Do not 
 * use delay library any more.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2018 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Defines -----------------------------------------------------------*/
#define LED_GREEN PB5  // Arduino Uno on-board LED
#define LED_RED PB0    // External active-low LED
#define BUTTON PD3


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <uart.h>
#include <stdlib.h>

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Toggle two LEDs using the internal 8- and 16-bit 
 *           Timer/Counter.
 * Returns:  none
 **********************************************************************/
#define SERVO_PIN 5
#define SERVO2_PIN 4
int main(void)
{
    // Set pins where LEDs are connected as output
    DDRB |= (1<<SERVO_PIN);
    DDRB |= (1<<SERVO2_PIN);
    uart_init(UART_BAUD_SELECT(9600, F_CPU));
    // Configuration of 16-bit Timer/Counter1 for LED blinking
    // Set the overflow prescaler to 262 ms and enable interrupt
    TIM2_overflow_16us();
    TIM2_overflow_interrupt_enable();

    TIM1_overflow_262ms();
    TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();
uart_puts("hello");
    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;
}

uint16_t duty = 63;
uint16_t duty2 = 94;
char string[4];

void set_servo(float angle)
{
  // angle from -90 to 90
  // 1.5ms is 96
  duty = 94 - ((65/2) * (-angle/90));
  //uart_puts("\nsetting duty: ");
  //itoa(duty, string, 10);
  //uart_puts(string);
}

void set_servo2(float angle)
{
  // angle from -90 to 90
  // 1.5ms is 96
  duty2 = 94 - ((65/2) * (-angle/90));
  //uart_puts("\nsetting duty2: ");
  //itoa(duty, string, 10);
  //uart_puts(string);
}

ISR(TIMER1_OVF_vect)
{
  static uint8_t no_of_overflows = 0;
  int8_t values[] = {45, 80, 90, -45};
  if ((no_of_overflows%8) == 0)
  {
    int8_t angle = values[(no_of_overflows/8)%4];
    set_servo(angle);
    
    uart_puts("\nsetting angle: ");
    itoa(angle, string, 10);
    uart_puts(string);
    uart_puts(".");
    int8_t angle2 = values[(no_of_overflows/8)%2];
    set_servo2(angle2);
    uart_puts("\nsetting angle2: ");
    itoa(angle2, string, 10);
    uart_puts(string);
    uart_puts(".");
  }
  if (no_of_overflows > 64)
  {
      no_of_overflows = 0;
  }
  no_of_overflows++;
}

ISR(TIMER2_OVF_vect)
{
  // 63 cycles is 1 ms
  // 125 cycles is 2 ms
  static uint16_t count = 0;
  static uint16_t period = 1250;
  // Period length is 1250 cycles (1250*0.016ms = 20ms)
  if (count<duty){
    //uart_puts("1\n");
    PORTB |= (1<<SERVO_PIN);
  }else{
    //uart_puts("0\n");
    PORTB &= ~(1<<SERVO_PIN);
  }
  if (count<duty2){
    //uart_puts("1\n");
    PORTB |= (1<<SERVO2_PIN);
  }else{
    //uart_puts("0\n");
    PORTB &= ~(1<<SERVO2_PIN);
  }
  count++;
  if (count>period){
    count = 0;
  }
}