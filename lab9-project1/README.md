
# Project 1

Časovač s LCD rozhraním založený na kontroléry ATMEGA328p. 

### Členovia týmu

**Dominik Caban** <br>
- kód displej, enkóder, návrh shieldu, dokumentácia

**Ivo Dovičák** <br>
- kód AD prevodu, správa Timer-ov, podporné funkcie, flowchart, preslov u videa 

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
- vytvoriť časovač s užívateľským tozhraním,
- využiť analógový joistick (2 kanály ADC, 1 tlačidlo) a enkóder pre ovládanie užívateľského rozhrania,
- využiť vývojovú dosku Arduino UNO,
- vytvoriť užívateľské rozhranie pomocou LCD modulu Digilent PmodCLP,
- vytvoriť kód a využiť pri tom knižnice vytvorené počas laboratórnych cvičení,
- vytoriť schému a návrh DPS v prostredí Altia (PCB Design Software).

<a name="hardware"></a>

## Popis Hardwaru
### Arduino UNO, Enkóder, Joystick 
V rámci vývojovej sady bola použitá vývojová doska Arduino UNO obsahujúca 14 digitálnych vstupných/výstupných pinov (z ktorých 6 môže byť použitých ako PWM výstupy) a 6 analógových vstupov. Ďalej bol použitý rotačný enkóder typu KY-040, ktorému na jedno 360° otočenie pripadá 20 pulzov. Zároveň bol využitý aj joystick, ktorý sníma v 2 osiach (X a Y) do akej miery je vychýlený (má analógový výstup pre každú z osí). Joystick má aj zabudované tlačidlo (pin SW). Ako posledný bol použitý 16×2 znakový LCD modul (Digilent Pmod CLP), ktorý využíva 8-bitové paralelné dátové rozhranie a umožňuje zobrazovať až 32 rôznych znakov z viac ako 200 možných.

### Shield
- Do nášho študentského projektu sme zakomponovali shield, ktorý by bolo možné umiestniť nad vývojovú dosku Arduino UNO a bolo by tak možné mať hotový finálny produkt bez dodatočných prepojov a kontaktného poľa. Súčasťou shieldu je LCD display, Enkóder, Joistick a Piezoelektrický menič so zabudovaným generátorom. (V simulácii a aj počas laboratórneho cvičenia bol piezoelektrický menič nahradený LED diódou.) 
- Návrh shieldu je realizovaný pomocou aplikácie Altium Designer, viď. obrázky nižšie.


3D Model DPS shieldu <br> (Vrchný pohľad) | 3D Model DPS shieldu <br> (Spodný pohľad)
:-------------------------:|:-------------------------:
![TOP](https://user-images.githubusercontent.com/99599292/206024184-630b0892-5c0f-4bee-af78-f6b76cede5ef.PNG)|![BOT](https://user-images.githubusercontent.com/99599292/206024167-6daa4043-3563-473c-9e71-3126e7d38f11.PNG)

Layoutový pohľad DPS shieldu <br> (Vrchný pohľad) |3D Model DPS shieldu <br> (Bočný pohľad)
:-------------------------:|:-------------------------:
![2D](https://user-images.githubusercontent.com/99599292/206024144-bae264bb-660f-47e1-a58b-0a8ea56003ed.PNG)|![LR_VIEW](https://user-images.githubusercontent.com/99599292/206024231-a2c6670b-b2bc-4459-8b94-76b9149c3647.PNG)

![SchemaTimer](https://user-images.githubusercontent.com/99599292/206024214-85624a24-66cd-4574-841a-bfdcb702cbe3.PNG)
<fig caption> <p align="center"> Schéma DPS shliedu

### Schéma zapojenia modulov 
Funkčnosť zapojenia bola demonštrovaná na základe schémy uvedenej nižšie.
![SchemaZapojenia](https://user-images.githubusercontent.com/99599292/206024222-d09645c6-d962-47e9-bce4-604072a9a58a.PNG)
|:--:| 
|*Schéma zapojenia modulov*|

<a name="software"></a>

## Popis Softwaru

Kód je kvôli jednoduchosti sústredený celý do súboru main.c a pre lepšiu prehľadnosť je rozdelený do viacerých funkcií podľa požadovanej operácie.
Pomyselné stavy aplikácie sú presne 2:
- Stav1: Časovač je zastavený a pomocou joysticku je sa možné pohybovať po jednotlivých poliach a otočného enkóderu môžeme upravovať jednotlivé hodnoty.

- Stav2: Časovač je spustený a nie je možné ho upravovať, 
            pozastaviť časovač je možné len stlačením joystick-u.

Program rozlišuje a prepína jednotlivé stavy využitím 2 AVR Timer-ov, z ktorých môže v jeden okamžik bežať len jeden. Stav čítania otočného enkodéra sa mení pomocou bitovej masky pre externé prerušenia.

Timer1 slúži na spúšťanie AD prevodníka a preto beží len v Stave1, zároveň sú počas sledu operácií pre Timer1 zakomponované podmienky, ktoré sledujú stav stlačenia tlačítok a podľa toho vykonajú požadovanú operáciu (enkóder-reset, joystick-spustenie časovača). 

Timer2 slúži pre samotný časovač a pri spustení odčítava stotiny sekúnd a pomocou toho upravuje všetky časové hodnoty. Zároveň sleduje stav stlačenia tlačítka, pre prípad že je požadované pozastavenie časovača. Po uplynutí nastaveného času sa rozsvieti LED na pine 13 a časovač sa dostane opätovne do stavu1.

Prechod medzi týmito 2 stavmi je realizované pomocou funkcií start_timer() a stop_timer(), ktoré sa starajú o pustenie a zastavenie jednotlivých časovačov a otočného enkodéra.

Pri užívateľských vstupoch sú použité premenné ako napríklad joy_sw_state (predošlý stav), doraz, ktoré kontrolujú predošlý stav vstupu aby sa predišlo nechcenému tzv. dvojkliku.

### Vývojový diagram

![flowchart](https://user-images.githubusercontent.com/99599292/206269500-3c2908ca-0a0e-4aec-9eab-47fa1b842cbb.png)

### Nastavenie časovača
- Požadovaná pozícia na obrazovke sa mení joystickom, hodnota na tejto pozícií sa upravuje otočným enkóderom.

![START](https://user-images.githubusercontent.com/99599292/206030181-46f40f93-0de3-45e5-882b-ca47cf94fb09.PNG)
|:--:| 
|*Nastavenia časovača*|

### Spustenie časovača
- Časovač sa spúšta stlačením joysticku.

![STARTED](https://user-images.githubusercontent.com/99599292/206030185-5deaf6cc-0997-4de3-b73b-8f522007021e.PNG)
|:--:| 
|*Spustenie časovača*|

### Zastavenie časovača
-	Časovač je možné zastaviť stlačením joysticku, časovač je v tomto momente v pozastavenom stave. Pri stlačení enkóderu sa hodnota resetuje na hodnotu pôvodnú, pri otočení enkóderu sa začne upravovať súčasná hodnota.

![STOPPEDV2](https://user-images.githubusercontent.com/99599292/206186754-dede505b-bb65-4eca-8034-8bc2f57c1720.PNG)
|:--:| 
|*Zastavenie časovača*|

### Ukončenie časovača
- Pri ukončení sa spustí funkcia timer_runout, rozsvieti sa LED a časovač sa vráti späť do stavu jedna. Po stlačení enkodéra sa časovač znova resetuje a LED zhasne.

![DONE](https://user-images.githubusercontent.com/99599292/206249848-c34aa907-828e-4d09-aad5-29bd030db8af.PNG)
|:--:| 
|*Ukončenie časovača*|

<a name="lbr"></a>

### Knižnice
#### Popis
1. ***LCD*** využíva knižnicu od Petra Fleuryho pre základné funkcie LCD displayu.
2. ***TIMER*** používa sa na ovládanie timerov mikrokontrola AVR.
3. ***AVR*** používa sa na volanie timerov mikrokontrola AVR.

<a name="sourcefiles"></a>

#### Zdrojové súbory
1. LCD
   1. [lcd.h](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/09-project1/project1/lib/lcd/lcd.h)
   2. [lcd.c](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/09-project1/project1/lib/lcd/lcd.c)
   3. [lcd_definitions.h](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/09-project1/project1/lib/lcd/lcd_definitions.h)
2. TIMER
   1. [timer.h](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/09-project1/project1/include/timer.h)
3. UART
   1. [uart.h](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/09-project1/project1/lib/uart/uart.h)
   2. [uart.c](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/09-project1/project1/lib/uart/uart.c)
4. SOURCE
   1. [main.c](https://github.com/DominikCaban/digital-electronics-2/blob/main/labs/09-project1/project1/src/main.c)

## GitHub štruktúra úložiska

   ```c
   ├── include         // Included files
       └── timer.h  
   ├── lib             // Libraries
       └── gpio
           └── gpio.c
           └── gpio.h
       └── lcd
           └── ldc_definitions.h
           └── lcd.h
           └── lcd.c
       └── uart        // For debugging
           └── uart.h
           └── uart.c
   ├── src             // Source file(s)
   │   └── main.c
   ├── test            // No need this
   ├── platformio.ini  // Project Configuration File
   └── README.md       // Report of this project
   ```

## Video 
Na nasledujúcej adrese je k dispozícií video s vysvetlením kódu a s ukážkou funkčnosti na konci videa.  
https://www.youtube.com/watch?v=uNGBPMvmf-I

<a name="references"></a>

## Referencie

1. [ATMEGA 328p Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
2. [LCD display](https://digilent.com/reference/pmod/pmodclp/start)
3. [Encoder](https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/?fbclid=IwAR2GDmzOCwF2mUCt-pVNGLNIA0n9qdLGAsA48_TlhPRhTdYTlosFNacai3k)
4. [Digital Electronics 2 course at BUT Brno](https://github.com/tomas-fryza/digital-electronics-2)
5. [Shield schematic documents + gerbers](https://1drv.ms/u/s!ApTT7Dyt-1k9hZRq7iB1lt6IxRTp-Q?e=aoRU1v)
