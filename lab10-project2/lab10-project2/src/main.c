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


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Toggle two LEDs using the internal 8- and 16-bit 
 *           Timer/Counter.
 * Returns:  none
 **********************************************************************/
#define SERVO_PIN 5
int main(void)
{
    // Set pins where LEDs are connected as output
    DDRB |= (1<<SERVO_PIN);
    // Configuration of 16-bit Timer/Counter1 for LED blinking
    // Set the overflow prescaler to 262 ms and enable interrupt
    TIM2_overflow_128us();
    TIM2_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Toggle on-board LED.
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
  static uint16_t count = 0;
  static uint16_t pause = 4000;
  static uint8_t duty = 10;
  static uint8_t enabled = 1;
  // Period length is 20 (20*0.5ms = 10ms)
  uint8_t actual = count%80;
  if (actual<duty){
    //uart_puts("1\n");
    PORTB |= (1<<SERVO_PIN);
  }else{
    //uart_puts("0\n");
    PORTB &= ~(1<<SERVO_PIN);
  }
  count++;
  if (count>pause){
    count=0;
    if (duty==16){
      duty = 32;
    }else{
      duty = 16;
    }
  }
}