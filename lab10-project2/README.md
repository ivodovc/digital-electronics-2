
# Project 2

Ovládanie dvoch servomotorov SG90 pomocou joysticku (2 ADC kanály, 1 tlačidlo).  

### Členovia týmu

**Dominik Caban** <br>
- kód PWM, dokumentácia, simulácia

**Ivo Dovičák** <br>
- kód AD prevodu, dokumentácia, flowchart

### Obsah

* [Ciele projektu](#objectives)
* [Popis Hardwaru](#hardware)
* [Popis Software](#software)
* [Knižnice](#lbr)
* [Zdrojové súbory](#sourcefiles)
* [Video](#video)
* [Referencie](#references)

<a name="objectives"></a>

## Ciele projektu
- vytvoriť ovládanie dvoch servomotorov (v osiach X a Y) pomocou joysticku, 
- využiť analógový joistick (2 kanály ADC, 1 tlačidlo),
- využiť 2 servomotory (Tower Pro Micro Servo SG90),
- využiť vývojovú dosku Arduino UNO,
- vytvoriť kód a využiť pri tom knižnice vytvorené počas laboratórnych cvičení,

<a name="hardware"></a>

## Popis Hardwaru
### Arduino UNO, Servomotor, Joystick 
V rámci vývojovej sady bola použitá vývojová doska Arduino UNO obsahujúca 14 digitálnych vstupných/výstupných pinov (z ktorých 6 môže byť použitých ako PWM výstupy) a 6 analógových vstupov. Zároveň boli použité dva servomotory, ktorých funkčnosť je popísaná v osobitnom odstavci. Ďalej bol použitý joystick, ktorý sníma v 2 osiach (X a Y) do akej miery je vychýlený (má analógový výstup pre každú z osí). Joystick má aj zabudované tlačidlo (pin SW) - ktoré používame na prepínanie (výmenu) osí ovládania servomotorov. 

![arduinojoy](https://user-images.githubusercontent.com/99599292/208713827-a7a78a99-fe45-401a-af97-4ff0a893b400.png)

### Servomotor
Servomotory sú ovládané prostredníctvom pulzne šírkovej modulácie (PWM) cez riadiaci vodič. PWM modulácia určuje polohu hriadeľa. Na základe šírky impulzov (stridy) vysielaných s presne definovanou periódou 20ms (odpovedajúcou frekvencii 50Hz) sa rotor otočí do požadovanej polohy. Napríklad pri striede trvajúcej 1,5 ms sa rotor otočí do polohy 90°. V prípade privedenia impulzu so striedou rovnej 1 ms sa rotor otočí do polohy 0°. Pokiaľ by bola privedená strieda rovná 2 ms nastane otočenie servomotora v smere hodinových ručičiek do polohy 180°. Interný princíp zapojenia servomotora je možné vyrozumieť na základe blokovej schémy nižšie (Riadiaci systém servomotora). Ako Feedback Encoder sa v našom prípade používa potenciometer zabudovaný v servomotore viď. nižšie. 

<p align="center">
  <img src="https://user-images.githubusercontent.com/99599292/208722110-f47903c4-e4eb-47e4-aa9d-fa025c04faae.png"/>
</p>

<fig caption> <p align="center"> *Znázornenie vplyvu PWM na natočenie hriadeľa.*

![Control](https://user-images.githubusercontent.com/99599292/208719620-c128765a-d21f-4443-ab53-5469835295a5.jpg)
<fig caption> <p align="center"> *Riadiaci systém servomotora.*

![Servo0](https://user-images.githubusercontent.com/99599292/208720097-2d1bf3c9-5ffb-447d-b417-f0eb535ed862.png)
<fig caption> <p align="center"> *Interné komponenty servomotora.*

### Schéma zapojenia modulov 
Funkčnosť zapojenia bola demonštrovaná na základe schémy uvedenej nižšie.
![Schema](https://user-images.githubusercontent.com/99599292/208721050-2430577b-5e9e-4c89-8bf0-10707ec813ed.PNG)
<fig caption> <p align="center"> *Schéma zapojenia modulov.*

<a name="software"></a>

## Popis Softwaru
Projekt je kvôli jednoduchosti umiestnený len do 1 súboru `main.c`

### INICIALIZÁCIA:
  - Piny (ktoré sú pripojené na interný PWM generátora) sú inicializované ako digitálny output.
  - Joystick je pripojený na ADC piny, ktoré sú inicializované vo funkcii `init_joystick()`
  - interný PWM generátor je inicializovaný vo funkcií `init_pwm()`, kde je zároveň nastavená perióda PWM signálu na `20ms` a základná strieda (duty) na 1.5ms pomocou premenných `OCR1A` a `OCR1B`

Kód využíva len jeden časovač (`TIMER0`), ktorý spúšťa ADC prevod, v ktorm následne prebieha logia aplikácie. Podobne ako v projekte 1 je použitá premenná `joy_sw_state`, ktorá kontroluje predošlý stav vstupu aby sa predišlo nechcenému tzv. dvojkliku.
Stlačením joysticku sa invertuje ovládanie joystickov, pomocou premennej invert_controls.

Interný generátor je ovládaný len zmenou hodnôt OCR1A a OCR1B, 
použitý vzorec na výpočet striedy je:  `OCR = 1000.0f + ((adc_value / 1024.0f) * 1000.0f)`
jedná sa o priamu úmeru, ku ktorej je pripočítaná konštanta 1000 (adc_value nadobúda hodnôt od `0` po `1024`, a hodnota striedy `OCR` môže byť od `1000` po `2000` pre správne fungovanie servomotorov)

Pri užívateľských vstupoch sú použité premenné ako napríklad `joy_sw_state` (predošlý stav), doraz, ktoré kontrolujú predošlý stav vstupu aby sa predišlo nechcenému tzv. dvojkliku.


### Vývojový diagram
<p align="center">
<img width="913" alt="Flowchart1" src="https://user-images.githubusercontent.com/99599292/208776328-2dd1f8ba-cf47-44c9-93fa-08435d026652.png">
</p>

### Simulácia - východzí stav
- Na nasledujúcej ukážke je možné vidieť predvolenú polohu hriadeľov jednotlivých servomotorov.
<p align="center">
  <img src="https://user-images.githubusercontent.com/99599292/208726454-87e2dec1-c50a-496a-80e7-141ed631db9d.PNG"/>
</p>

<fig caption> <p align="center"> *Predvolená poloha hriadeľov servomotorov.*

### Simulácia - ovládanie servomotorov  
- Na nasledujúcej ukážke je možné vidieť ovládanie servomotora pomocou joysticku v ose x. Zároveň je znázornená aj požadovaná strieda.

<p align="center">
  <img src="https://user-images.githubusercontent.com/99599292/208726426-b30c32a5-50ed-40d5-a153-2d8ab4aeccc4.PNG"/>
</p>

<fig caption> <p align="center"> *Ovládanie v osy x.*

### Simulácia - ovládanie servomotorov  
- Na nasledujúcej ukážke je možné vidieť ovládanie servomotora pomocou joysticku v ose y. Opäť je znázornená aj požadovaná strieda.

<p align="center">
  <img src="https://user-images.githubusercontent.com/99599292/208735674-56a62d6e-0073-4baf-8c8c-ba22f57b84f2.PNG"/>
"/>
  
<fig caption> <p align="center"> *Ovládanie v osy y.*

### Simulácia - ovládanie servomotorov  
- Na nasledujúcej ukážke je možné vidieť ovládanie servomotora pomocou joysticku v ose x - po zatlačení encodera. Opäť je znázornená aj požadovaná strieda.

<p align="center">
  <img src="https://user-images.githubusercontent.com/99599292/208726441-3bc25c28-90b7-4db1-bab6-e358a7bfde16.PNG"/>

<fig caption> <p align="center"> *Ovládanie po zatlačení joysticku.*

<a name="lbr"></a>

### Knižnice
#### Popis
1. ***TIMER*** používa sa na ovládanie timerov mikrokontrola AVR.
2. ***AVR*** používa sa na volanie timerov mikrokontrola AVR.

<a name="sourcefiles"></a>

#### Zdrojové súbory
1. TIMER
   1. [timer.h](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/10-project2/project2/include/timer.h)
2. UART
   1. [uart.h](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/10-project2/project2/lib/uart/uart.h)
   2. [uart.c](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/10-project2/project2/lib/uart/uart.c)
3. SOURCE
   1. [main.c](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/10-project2/project2/src/main.c)

## GitHub štruktúra úložiska

   ```c
   ├── include         // Included files
       └── timer.h  
   ├── lib             // Libraries
       └── gpio
           └── gpio.c
           └── gpio.h
       └── uart        // for debugging (not in final code)
           └── uart.h
           └── uart.c
   ├── src             // Source file(s)
   │   └── main.c
   ├── test            // No need this
   ├── platformio.ini  // Project Configuration File
   └── README.md       // Report of this project
   ```

## Video 
Na nasledujúcej ukážke je k dispozícií video funkčnosti našej aplikácie joysticku a servomotorov. 

<p align="center">
   <img src="https://media.giphy.com/media/cJJM415Cc96GmwGFOp/giphy-downsized-large.gif" />

<a name="references"></a>

## Referencie

1. [ATMEGA 328p Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
2. [Servomotor](https://www.jameco.com/Jameco/workshop/Howitworks/how-servo-motors-work.html#:~:text=Servos%20are%20controlled%20by%20sending,total%20of%20180%C2%B0%20movement.)
3. [ServoController](https://www.thomasnet.com/articles/instruments-controls/servo-motor-controllers/)
4. [Encoder](https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/?fbclid=IwAR2GDmzOCwF2mUCt-pVNGLNIA0n9qdLGAsA48_TlhPRhTdYTlosFNacai3k)
5. [Digital Electronics 2 course at BUT Brno](https://github.com/tomas-fryza/digital-electronics-2)

