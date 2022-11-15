# Lab 7: Ivo Dovičák

### Arduino Uno pinout

1. In the picture of the Arduino Uno board, mark the pins that can be used for the following functions/operations:
   * PWM generators from Timer0, Timer1, Timer2
   * analog channels for ADC
   * UART pins
   * I2C pins
   * SPI pins
   * external interrupt pins INT0, INT1

   ![arduino_uno_pinout.png](arduino_uno_pinout.png)
   
   | ** Functionality** | **Pins** |
   | :-: | :-: |
   | PWM generators | 3,5,6,9,10,11|
   | ADC | 14,15,16,17,18,19 |
   | UART | 0 (RX), 1(TX) |
   | I2C | 18 (SDA), 19 (SCL) |
   | SPI | 10 (SS), 11 (MISO), 12 (MOSI), 13 (SCK) |
   | external interrupt pins INT0, INT1 | 2 (INT0),3 (INT1) |

### I2C communication

2. Draw a timing diagram of I2C signals when calling function `rtc_read_years()`. Let this function reads one byte-value from RTC DS3231 address `06h` (see RTC datasheet) in the range `00` to `99`. Specify when the SDA line is controlled by the Master device and when by the Slave device. Draw the whole request/receive process, from Start to Stop condition. The image can be drawn on a computer (by [WaveDrom](https://wavedrom.com/) for example) or by hand. Name all parts of timing diagram.

   ![your figure]()

### Meteo station

Consider an application for temperature and humidity measurements. Use sensor DHT12, real time clock DS3231, LCD, and one LED. Every minute, the temperature, humidity, and time is requested from Slave devices and values are displayed on LCD screen. When the temperature is above the threshold, turn on the LED.

3. Draw a flowchart of `TIMER1_OVF_vect` (which overflows every 1&nbsp;sec) for such Meteo station. The image can be drawn on a computer or by hand. Use clear description of individual algorithm steps.

   ![lab7_3](lab7_3.png)
