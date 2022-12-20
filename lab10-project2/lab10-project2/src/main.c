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

#define JOY_SW 2

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


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Toggle two LEDs using the internal 8- and 16-bit 
 *           Timer/Counter.
 * Returns:  none
 **********************************************************************/
#define SERVO_PIN 3
#define SERVO2_PIN 2
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

    init_joystick();

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

uint16_t duty = 63;
uint16_t duty2 = 94;
char string[4];

void set_servo(float angle)
{
  // angle from -90 to 90
  // 1.5ms is 96
  duty = 94 - ((60) * (-angle/90));
}

void set_servo2(float angle)
{
  // angle from -90 to 90
  // 1.5ms is 96
  duty2 = 94 - ((60) * (-angle/90));
}


ISR(TIMER1_OVF_vect)
{
   static uint8_t no_of_overflows = 0;
  no_of_overflows++;

    if (no_of_overflows >= 3)
    {
        no_of_overflows = 0;  
        ADCSRA |= (1<<ADSC);
    }
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

uint8_t joy_sw_state;
uint8_t button_press;
ISR(ADC_vect)
{

    // Read converted value
    uint8_t joy_sw = (PINC & (1<<JOY_SW)) >> JOY_SW;
    // Start timer
    if (joy_sw==0)
    {
        //do it only once per click
        if (joy_sw_state==1){
            //otherwise start the clock
            button_press++;
        }
    }
    joy_sw_state=joy_sw;
    // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
    float value = ADC;
    if ((ADMUX & 7) == 0) {
        //X AXIS ADC

        if (button_press%2){
          set_servo((value-511)/1024 * 180);
        }else{
          set_servo2((value-511)/1024 * 180);
        }
      
        // start reading y joystick position
        // Select input channel ADC1 (Y joystick)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1));
        ADMUX |= (1<<MUX0);
        // start conversion
        ADCSRA |= (1<<ADSC);
    }else if  ((ADMUX & 7)  == 1){
        // Y AXIS ADC
        //uart_puts("\nsetting value22: ");
        //itoa(value, string, 10);
        //uart_puts(string);
        if (button_press%2){
          set_servo2((value-511)/1024 * 180);
        }else{
          set_servo((value-511)/1024 * 180);
        }
        // select channel back to x input (channel 0)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
    }
}