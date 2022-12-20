/***********************************************************************
 * 
 * Control 2 servomotors using 2 axis joystick and switch to invert controls
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *  
 * Based on code from Digital Electronics 2 Course
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

// Joystick Switch is connected to bit 2 on PortC register (A2)
#define JOY_SW 2
// Servo PWM pins are at bit 2 and 3 in Port B register
#define SERVO_PIN 3
#define SERVO2_PIN 2

//Initialization of Joystick functions (ADC)
void init_joystick()
{
    // Timer1 is for starting ADC conversion
    TIM0_overflow_16ms();
    TIM0_overflow_interrupt_enable();

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

//Initialization of AVR internal PWM generator
void init_pwm()
{
  // Set PB1 and PB2 as outputs for PWM
  DDRB |= (1 << 1);
  DDRB |= (1 << 2);
  // Set OC1A/OC1B on compare match, clear OC1A/OC1B at BOTTOM (inverting mode)
  TCCR1A = (1 << COM1A1) | (1 << COM1B1);
  // Set Waveform Generation Mode to 10 -> PWM, phase correct
  TCCR1A |= (1 << WGM11);
  TCCR1B = (1 << WGM13);
  // Set prescaler clkI/O / 8
  TCCR1B |= (1 << CS11);
  // Set PWM period
  ICR1 = 20000;
  
  // Set PWM duty cycle to default values
  OCR1A = 1500;
  OCR1B = 1500;
}

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Main function,
 * initalize joystick and AVR internal pwm generators
 * logic flow is controlled through interrupts
 **********************************************************************/
int main(void)
{
    // Set pins where Servos are connected as output
    DDRB |= (1<<SERVO_PIN);
    DDRB |= (1<<SERVO2_PIN);

    init_joystick();
    init_pwm();

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


/**********************************************************************
 * TIMER1 interrupt controls ADC conversion start,
 * which is designed to start every 100ms (6x16ms~100ms)
 **********************************************************************/
ISR(TIMER0_OVF_vect)
{
  static uint8_t no_of_overflows = 0;
  no_of_overflows++;

    if (no_of_overflows >= 6)
    {
        no_of_overflows = 0;  
        ADCSRA |= (1<<ADSC);
    }
}

/**********************************************************************
 * ADC conversion
 **********************************************************************/
// joy_sw_state is used to prevent unintended doubleclick 
// (e.g. if button is pressed for more than 100ms, it would look like it was pressed 2 times)
uint8_t joy_sw_state;
uint8_t invert_controls=0;
ISR(ADC_vect)
{
    uint8_t joy_sw = (PINC & (1<<JOY_SW)) >> JOY_SW;
    // Start timer
    if (joy_sw==0)
    {
        //do it only once per click
        if (joy_sw_state==1){
            //invret controls for each axis on joystick
            invert_controls=!invert_controls;
        }
    }
    joy_sw_state=joy_sw;
    // Read converted value
    float adc_value = ADC;
    // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
    if ((ADMUX & 7) == 0) {
        //X AXIS ADC

        if (invert_controls) {
          OCR1B = 1000.0f + ((adc_value / 1024.0f) * 1000.0f);
        } else {
          OCR1A = 1000.0f + ((adc_value / 1024.0f) * 1000.0f);
        }
      
        // start reading y joystick position
        // Select input channel ADC1 (Y joystick)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1));
        ADMUX |= (1<<MUX0);
        // start conversion
        ADCSRA |= (1<<ADSC);
    }else if  ((ADMUX & 7)  == 1){
        // Y AXIS ADC

         if (invert_controls) {
          OCR1A = 1000.0f + ((adc_value / 1024.0f) * 1000.0f);
        } else {
          OCR1B = 1000.0f + ((adc_value / 1024.0f) * 1000.0f);
        }
        // select channel back to x input (channel 0)
        ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
    }
}