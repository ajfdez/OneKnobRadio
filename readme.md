# One Knob Radio for Microsoft Flight Simulator 2020

##### Version: 1.0
##### License: MIT
##### Author: Albert J. Fernandez | 04-2024
##### https://github.com/ajfdez/OneKnobRadio


## DESCRIPTION

This is a code for a radio panel hardware controller for Microsoft Flight Simulator 2020 using SPAD.neXt and an Arduino UNO or Leonardo like board.

<img src="https://github.com/ajfdez/OneKnobRadio/blob/main/img/OneKnobRadio_bbd.png" width="597" height="819">


This project has focused on controlling the entire radio device stack of an airplane with a single encoder. Offering simple and intuitive control of multiple systems with a single device, with the minimum number of components.

With this code, you can control ```COM 1```, ```COM 2```, ```NAV 1```, ```NAV 2```, ```ADF``` & ```XPNDR CODE``` with one single knob.

Even the most el cheapo encoder found on Amazon will perform much more better than those used in the Saitek/Logitech hardware controllers. 
Look for **rotary encoder** on Amazon or your electronics parts supplier to find yours.
This code uses hardware interrupts for not loosing a single rotary step, Allowing for better simulation inmersion and accuracy with a tactile feedback control.
 
The Arduino board is connected to the PC via USB and the SPAD.neXt software is used to communicate with the Arduino board.

SPAD.neXt is a software that allows you to create custom controls for flight simulators. You can download it from https://www.spadnext.com/. 
A license for running SPAD.neXt is needed, and charges may apply.

This project is not endorsed or afiliated in any way with SPAD.neXt.

Please take a look in https://github.com/ajfdez/TrimWheel to see another related project.


## CODE COMPILATION          

#### Visual Studio Code & PlatformIO
This code is compiled using Visual Studio Code and PlatformIO Extension Core 6.1.14 Home 3.4.4 versions.<br>
If you are using other than an UNO board, remember to change the ```board``` build option to the one you are using in the ```platformio.ini``` file.

#### Arduino IDE
It can also be compiled using the Arduino IDE changing the extension from ```src/main.cpp``` to ```main.ino```, or copy/pasting the code into the Arduino IDE.


## HARDWARE

1   Arduino Uno / Leonardo / Mega board.

1   Rotary encoder with a push button. Look for **rotary encoder** on Amazon or your electronics parts supplier. 
    Just about any will work, for best results look for one with a PCB and a knob. The PCB marked pins usually are:

|   PIN    | Connection                                                 |
|----------|------------------------------------------------------------|
|    GND   | Ground connection to Arduino GND pin.                      |
|     +    | 5V power supply. **See NOTE 1.**                           |
|    SW    | Push button.                                               |
|    DT    | Encoder pin B to Arduino Pin 2. **See NOTE 2.**            |
|    CLK   | Encoder pin A to Arduino Pin 3. **See NOTE 2.**            |


1   HD44780 1602 LCD Display with 2 Lines 16 Characters.


#### NOTE 1: + or 5V pin of the encoder PCB is used for the included pull up resistors for the encoder pins A and B, since we are using the pull up resistors embedded in the Arduino chip, this pin is not needed, and may left unconnected. But if you are experiencing "lost ticks", or using long cables from Arduino to encoder boards, this pin may need be used for better rotary detection.

#### NOTE 2: The Encoder pins A and B can be reversed to Arduino Pin 3 and 2 to follow your CW/CCW encoder setup.



## HARDWARE SETUP          

Arduino UNO or Leonardo like board, and a rotary encoder.

The rotary encoder pins ```DT``` and ```CLK``` are connected to Pin 2 and Pin 3 of the Arduino board. The ```GND``` Pin goes to any Arduino ```GND``` pin.
The ```SW``` pin goes to pin 4.

The LCD is connected to the Arduino board using the following pin configuration:

`````` rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; ``````

Since I like to use as few components as possible, in this project the backlighting and contrast of the LCD panel are controlled by PWM. This way, the typical potentiometers for controlling lighting and contrast are no longer necessary. Additionally, this system allows these parameters to be operated within the program without having to access the electronic circuit. 

In future versions, it will be possible to implement a link between the airplane panel lighting value and the LCD lighting.

The backlight pin goes to Arduino pin 5, the Contrast pin goes to Arduino pin 6.

## WIRING

Take a look in the ```/img``` folder for the images of the circuit.

<img src="https://github.com/ajfdez/OneKnobRadio/blob/main/img/OneKnobRadio_sch.png" width="800" height="500">

## USAGE

Encoder rotation changes the active value. A double click cycles between systems.

The first screen shows the ```COM1``` and ```NAV1``` settings, the second shows ```COM2``` and ```NAV2``` settings. The third screen show the ```ADF``` frequency. The fourth screen shows the ```XPNDR``` code.

One triple click enters configuration mode.

#### Encoder switch settings.

|      Clicks       |        |
|-------------------|---------------------------------------------------------------------------------------------------------------|
|  Short click      | Switches between numeric parameters.<br> For example, in ```COM/NAV``` modes, switches between decimal and integers. In ```XPNDR``` mode switches between digits.  |
|  Long click       | Switches between Standby and Active frecuency. In ```XPNDR``` mode sends the IDENT |  
|  Double click     | Switches between radio systems. |
|  Triple click     | Enter or exits configuration mode. |

#### Configuration mode.

|  Setting  |                |
|-----------|----------------|
|   MODE    | Switches between ```COM1/NAV1 - COM2/NAV2``` and ```COM1/COM2 - NAV1/NAV2``` screen setting. **IN TESTING**         |
|   LCD     | LCD Backlight setting. Increase or decrease the LCD brightness.  |
|   CONT    | LCD Contrast setting. Increase or decrease the LCD contrast.     |

## SPAD.neXt

The SPAD.neXt configuration is very simple, just create a new device in the settings page, then add a ```Serial Device``` and select the Arduino board COM port from the list.

The config is ```DTR``` OFF, ```32 bit``` ON. 

SPAD.neXt needs to be restarted after this settings.


## CREDITS

SPAD.neXt  https://www.spadnext.com/

This code uses the CmdMessenger and EncoderButton libraries.
                                  
- CmdMessenger by Thijse. https://github.com/thijse/Arduino-CmdMessenger
                  
- EncoderButton by Stutchbury. https://github.com/Stutchbury/EncoderButton

It also uses the Arduino IDE included libraries LiquidCrystal and EEPROM


You can install using the PlatformIO Library Manager or the Arduino Library Manager.
