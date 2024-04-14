/*******************************************************************
* Project           : OneKnobRadioFS20
* Version           : 1.0
* License           : MIT
* Author            : Albert J. Fdez.
* Date              : 14/APR/2024
*
* Description       : One Knob Radio for FS2020
*
* Dependencies      : See readme.md
*                    
*
* Compilation       : See readme.md
*
* Revision History  :
* Date		    Author 			    Comments
* ------------------------------------------------------------------
* 15/04/2024	Albert J. Fdez	Initial version.
*
*/

#include <LiquidCrystal.h>
#include <CmdMessenger.h>  
#include <EncoderButton.h>
#include <EEPROM.h>

// ------------------ V A R I A B L E S  D E C L A R A T I O N S ------------------------------

bool isReady = false;
// Mode selector: (false) = Khz; (true) = Mhz; --- Default: Khz 
bool freqSelMode = true; 
// System selector:  (1)=COM1; (2)=NAV1; (3)=COM2; (4)=NAV2; (5)=ADF; (6)=XPNDR; --- Default: COM1 
int sysSelect = 1;
// ADF mode selector: (false)=Frequency; (true)=Heading; --- Default: Frequency
bool modeADF = false;
//Frequency fraction selector: (0)=0.1Khz;(1)=1Khz;(2)=10Khz;(3)=100Khz; --- Default: 1Khz
int freqADF = 1;
// IDENT decimal selector: (1)=Unit; (2)=Decimal; (3)=Hundred; (4)=Thousand
int decIDENT = 1;
//Display mode selector
bool modeLCD = false;       // (false)=COM/NAV; (true)=NAV/NAV;
// Configuration mode selector
bool configMode = false;
int configState = 1;        // (1)=Display mode; (2)=Lighting; (3)=Contrast
// LCD Display Lighting; Pin - Value 
int luzPin = 5;
byte iluminacionDef = 150;
byte iluminacion;
// LCD Display Contrast; Pin - Value
int contrastePin = 6;
byte contrasteDef = 105;
byte contraste;
bool pwmset = false;
// Data Containers
float newADFActiveFreq = 123;
int newADFHDG;
float newCOM1StandbyFreq = 0;
float newCOM1ActiveFreq = 0;
float newCOM2StandbyFreq = 0;
float newCOM2ActiveFreq = 0;
float newNAV1StandbyFreq = 0;
float newNAV1ActiveFreq = 0;
float newNAV2StandbyFreq = 00;
float newNAV2ActiveFreq = 0;
int newXpndr = 7000;
bool newIDENT;

// ---------------------- C U S T O M   D I S P L A Y   C H A R A C T E R S --------------------

// C 
byte customCharC[8] = {
	0b00000,
	0b00000,
	0b01110,
	0b01010,
	0b01000,
	0b01010,
	0b01110,
	0b00000
};

// 1
byte customChar1[8] = {
	0b00000,
	0b00000,
	0b00100,
	0b01100,
	0b00100,
	0b00100,
	0b01110,
	0b00000
};

// N
byte customCharN[8] = {
	0b00000,
	0b00000,
	0b01001,
	0b01101,
	0b01111,
	0b01011,
	0b01001,
	0b00000
};

// 2
byte customChar2[8] = {
	0b00000,
	0b00000,
	0b00110,
	0b01001,
	0b00010,
	0b00100,
	0b01111,
	0b00000
};


// ------------------------ L I B R A R I E S  I N I T I A L I T A T I O N ---------------

// ----- LiquidCristal --------
// Pin configuration: RS, E, D4, D5, D6, D7
const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0;
// LiquidCrystal Initialization
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// ----- CmdMessenger --------
CmdMessenger messenger(Serial);

// ----- EncoderButton --------
// EncoderButton Initialization. PinA, PinB, Button
EncoderButton eb1(2, 3, 4);   


// -------------------------------- C O M M A N D S ------------------------------------

enum
{
  kRequest = 0,             // Request from SPAD.neXt
  kCommand = 1,             // Command to SPAD.neXt
  kEvent = 2,               // Events from SPAD.neXt
  kDebug = 3,               // Debug strings to SPAD.neXt Logfile
  kSimCommand = 4,          // Send Event to Simulation
  kADFActiveFreq = 10,      // Receive ADF Active Frequency
  kCOM1ActiveFreq = 11,     // Receive COM1 Active Frequency
  kCOM1StandbyFreq = 12,    // Receive COM1 Standby Frequency
  kNAV1ActiveFreq = 13,     // Receive NAV1 Active Frequency
  kNAV1StandbyFreq = 14,    // Receive NAV1 Standby Frequency
  kCOM2ActiveFreq = 15,     // Receive COM2 Active Frequency
  kCOM2StandbyFreq = 16,    // Receive COM2 Standby Frequency
  kNAV2ActiveFreq = 17,     // Receive NAV2 Active Frequency
  kNAV2StandbyFreq = 18,    // Receive NAV2 Standby Frequency
  kADFHDG = 19,             // Receive ADF Heading
  kXpndr = 20,              // Receive Xpndr Code
  kIDENT = 21               // Receive IDENT
};

// -------------------------------- F U N C T I O N S ----------------------------------

// ------------------ LCD Print ----------------
void printLCD(){

  lcd.cursor();
  lcd.clear();
  if (configMode == 0) {       
    if (modeLCD == 0) {          // COM/NAV
      // --- COM1 o NAV1 ---
      if (sysSelect == 1 || sysSelect == 2) {
        lcd.setCursor(0,0);
        lcd.print(newCOM1ActiveFreq,3);
        lcd.setCursor(7,0);
        lcd.write(byte(1));
        lcd.setCursor(8,0);
        lcd.write(byte(2));
        lcd.setCursor(9,0);
        lcd.print(newCOM1StandbyFreq,3);    
        lcd.setCursor(0,1);
        lcd.print(newNAV1ActiveFreq,3);
        lcd.setCursor(7,1);
        lcd.write(byte(3));
        lcd.setCursor(8,1);
        lcd.write(byte(2));
        lcd.setCursor(9,1);
        lcd.print(newNAV1StandbyFreq,3);
      }
      // --- COM2 o NAV2 ---
      if (sysSelect == 3 || sysSelect == 4) {
        lcd.setCursor(0,0);
        lcd.print(newCOM2ActiveFreq,3);
        lcd.setCursor(7,0);
        lcd.write(byte(1));
        lcd.setCursor(8,0);
        lcd.write(byte(4));
        lcd.setCursor(9,0);
        lcd.setCursor(9,0);
        lcd.print(newCOM2StandbyFreq,3);    
        lcd.setCursor(0,1);
        lcd.print(newNAV2ActiveFreq,3);
        lcd.setCursor(7,1);
        lcd.write(byte(3));
        lcd.setCursor(8,1);
        lcd.write(byte(4));
        lcd.setCursor(9,1);
        lcd.print(newNAV2StandbyFreq,3);
      }
    }
if (modeLCD == 1) {          // COM/COM
      // --- COM1 o COM2 ---
      if (sysSelect == 1 || sysSelect == 3) {
        lcd.setCursor(0,0);
        lcd.print(newCOM1ActiveFreq,3);
        lcd.setCursor(7,0);
        lcd.write(byte(1));
        lcd.setCursor(8,0);
        lcd.write(byte(2));
        lcd.setCursor(9,0);
        lcd.print(newCOM1StandbyFreq,3);    
        lcd.setCursor(0,1);
        lcd.print(newCOM2ActiveFreq,3);
        lcd.setCursor(7,1);
        lcd.write(byte(1));
        lcd.setCursor(8,1);
        lcd.write(byte(4));
        lcd.setCursor(9,1);
        lcd.print(newCOM2StandbyFreq,3);
      }
      // --- NAV1 o NAV2 ---
      if (sysSelect == 2 || sysSelect == 4) {
        lcd.setCursor(0,0);
        lcd.print(newNAV1ActiveFreq,3);
        lcd.setCursor(7,0);
        lcd.write(byte(3));
        lcd.setCursor(8,0);
        lcd.write(byte(2));
        lcd.setCursor(9,0);
        lcd.setCursor(9,0);
        lcd.print(newNAV1StandbyFreq,3);    
        lcd.setCursor(0,1);
        lcd.print(newNAV2ActiveFreq,3);
        lcd.setCursor(7,1);
        lcd.write(byte(3));
        lcd.setCursor(8,1);
        lcd.write(byte(4));
        lcd.setCursor(9,1);
        lcd.print(newNAV2StandbyFreq,3);
      }
    }
    // --- ADF ---
    if (sysSelect == 5) {
      lcd.setCursor(0,0);
      lcd.print(F("HDG"));
      lcd.setCursor(6,0);
      lcd.print(F("ADF"));
      lcd.setCursor(12,0);
      lcd.print(F("FREQ"));
      lcd.setCursor(0,1);
      lcd.print(newADFHDG,1);
      if (newADFActiveFreq <= 999){
        lcd.setCursor(11,1);
        lcd.print(newADFActiveFreq,1);
      } 
      if (newADFActiveFreq >= 1000){
        lcd.setCursor(10,1);
        lcd.print(newADFActiveFreq,1);
      }
    } 
    // --- XPNDR ---
    if (sysSelect == 6) {
      if(newIDENT == 0){
        lcd.setCursor(5,0);
        lcd.print(F("IDENT:"));
        lcd.setCursor(7,1);
        lcd.print(newXpndr);
      }
      if(newIDENT == 1) {
        lcd.setCursor(1,0);
        lcd.print(F("*** IDENT: ***"));
        lcd.setCursor(7,1);
        lcd.print(newXpndr);
      }
    }

    // --- Cursor ---

    // --- Modo COM1/2 ---
    // ------ Khz ------
    if ((sysSelect == 1 ||sysSelect == 3) && freqSelMode == 0) {   
      lcd.setCursor(15,0);
    }
    // ------ Mhz ------
    if ((sysSelect == 1 ||sysSelect == 3) && freqSelMode == 1) {   
      lcd.setCursor(11,0);
    }
    // --- NAV1/2 ---
    // ------ Khz ------
    if ((sysSelect == 2 || sysSelect == 4) && freqSelMode == 0) {   
      lcd.setCursor(15,1);
    }
    // ------ Mhz ------
    if ((sysSelect == 2 || sysSelect == 4) && freqSelMode == 1) {   
      lcd.setCursor(11,1);
    }
    // --- ADF ---
    if (sysSelect == 5) {
      if (modeADF == 0){          // --- ADF Frecuency
        if (freqADF == 0){        // --- 0.1Khz
          lcd.setCursor(15,1);
        }
        if (freqADF == 1){        // --- 1Khz
          lcd.setCursor(13,1);
        }
        if (freqADF == 2){        // --- 10Khz
          lcd.setCursor(12,1);
        }
        if (freqADF == 3){        // --- 100Khz
          lcd.setCursor(11,1);
        }
      }
      if (modeADF == 1){          // --- ADF HDG
        if (newADFHDG <=360){
          lcd.setCursor(2,1);
        }
        if (newADFHDG <=99){
          lcd.setCursor(1,1);
        }
        if (newADFHDG <=9){
          lcd.setCursor(0,1);
        }
      }
    } 
    // --- XPNDR ---
    if(sysSelect == 6) {
      // Selector decimal IDENT
      if (decIDENT == 1) {    // First
        lcd.setCursor(10,1);
      }
      if (decIDENT == 2) {    // Second
        lcd.setCursor(9,1);
      }
      if (decIDENT == 3) {    // Third
        lcd.setCursor(8,1);
      }
      if (decIDENT == 4) {    // Fourth
        lcd.setCursor(7,1);
      }
    }
  }
// --- Configuration ---
  if (configMode == 1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Mode:"));
    lcd.setCursor(6, 0);
    if (modeLCD == 0) {
      lcd.print(F("COM/NAV"));
    }
    if (modeLCD == 1) {
      lcd.print(F("COM/COM"));
    }
    if (pwmset == false){
      iluminacion = iluminacionDef;
      contraste = contrasteDef;
    }
    lcd.setCursor(0, 1);
    lcd.print(F("LCD:"));
    lcd.setCursor(4, 1);
    lcd.print(iluminacion);
    lcd.setCursor(8, 1);
    lcd.print(F("Cont:"));
    lcd.setCursor(13, 1);
    lcd.print(contraste);

// --- Cursor ---
    // CONFIG
    // (1)=Mode; (2)=Brightness; (3)=Contrast
    if (configState == 1){
      lcd.setCursor(12,0);
    }
    if (configState == 2){
      lcd.setCursor(6,1);
    }
    if (configState == 3){
      lcd.setCursor(15,1);
    }
  }
  
}

// --------------------------- Apply Configuration ---------------------------------

void applyConfig(){
  if (pwmset == false){
    analogWrite(luzPin, iluminacion);
    analogWrite(contrastePin, contraste);
    EEPROM.write(0, iluminacion);
    EEPROM.write(1, contraste);
    pwmset = true;
    return;    
  } else {
    analogWrite(luzPin, iluminacion);
    analogWrite(contrastePin, contraste);
    EEPROM.write(0, iluminacion);
    EEPROM.write(1, contraste);
    pwmset = true;
    return;
  }
}


// ------------------  C A L L B A C K S  F U N C T I O N S -----------------------

void onADFActiveFreq(){
  newADFActiveFreq = messenger.readFloatArg();
  printLCD();
  return;  
}

void onnewADFHDG(){
  newADFHDG = messenger.readInt16Arg();
  printLCD();
  return;
}

void onCOM1ActiveFreq(){
  newCOM1ActiveFreq = messenger.readFloatArg();
  printLCD();
  return;
}

void onCOM1StandbyFreq(){
  newCOM1StandbyFreq = messenger.readFloatArg();
  printLCD();
  return;
}

void onNAV1ActiveFreq(){
  newNAV1ActiveFreq = messenger.readFloatArg();
  printLCD();
  return;
}

void onNAV1StandbyFreq(){
  newNAV1StandbyFreq = messenger.readFloatArg();
  printLCD();
  return;
}

void onCOM2ActiveFreq(){
  newCOM2ActiveFreq = messenger.readFloatArg();
  printLCD();
  return;
}

void onCOM2StandbyFreq(){
  newCOM2StandbyFreq = messenger.readFloatArg();
  printLCD();
  return;
}

void onNAV2ActiveFreq(){
  newNAV2ActiveFreq = messenger.readFloatArg();
  printLCD();
  return;
}

void onNAV2StandbyFreq(){
  newNAV2StandbyFreq = messenger.readFloatArg();
  printLCD();
  return;
}

void onXpndr(){
  newXpndr = messenger.readInt16Arg();
  printLCD();
  return;
}

void onIDENT(){
  newIDENT = messenger.readBoolArg();
  printLCD();
  return;
}


// ---------------------- E N C O D E R B U T T O N  C A L L B A C K S --------------------------

// -------------------------------------- One Short Click ---------------------------------------
void onEb1Clicked(EncoderButton& eb) {
// --- ADF Mode---
  if (sysSelect == 5){
// --- ADF Frequency ---
    if (modeADF == 0){ 
      freqADF++;
      if (freqADF == 4){
        freqADF = 0;
      }
      printLCD();
      return;
    }
// --- ADF Heading ---
    if (modeADF == 1){
      return;
    }
  }
// --- XPNDR Mode ---
  if (sysSelect == 6) {
    decIDENT++;
    if (decIDENT == 5){
      decIDENT = 1;
    }
    printLCD();
    return;
  }
  if (configMode == true) {
    configState++;
    printLCD();
    if (configState == 4){
     configState = 1;
     printLCD();
    }
  }
// --- Else ---
  else {
  freqSelMode = !freqSelMode;
  printLCD();
  return;
  }
}

// --------------------------------- One Long Click | ACTIVE SWAP ------------------------------------------
void onEb1LongClick(EncoderButton& eb) {
// --- COM1 ---
  if (sysSelect == 1) {
    messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM1_RADIO_SWAP")));
    messenger.sendCmdEnd();
    printLCD();
    return;
  }
// --- NAV1 ---
  if (sysSelect == 2) {
    messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV1_RADIO_SWAP")));
    messenger.sendCmdEnd();
    printLCD();
    return;
  }
// --- COM2 ---
  if (sysSelect == 3) {
    messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM2_RADIO_SWAP")));
    messenger.sendCmdEnd();
    printLCD();
    return;
  }
// --- NAV2 ---
  if (sysSelect == 4) {
    messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV2_RADIO_SWAP")));
    messenger.sendCmdEnd();
    printLCD();
    return;
  }
  // --- ADF ---  
  if (sysSelect == 5) {
    modeADF = !modeADF;
    printLCD();
    return;
  }
  // --- If in XPNDR, send IDENT ---  
  if (sysSelect == 6) {
    messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_IDENT_ON")));
    messenger.sendCmdEnd();
    printLCD();
    return;
  }
}

// ----------------------------------------- Double Click | Switch Systems -----------------------------------------
void onEb1DoubleClick(EncoderButton& eb) {
  sysSelect = sysSelect + 1;
  if (sysSelect == 7) {
    sysSelect = 1;
  }
  printLCD();
  return;
}

// ------------------------------------------ Triple Click | Config Mode ------------------------------------------
void onEb1TripleClick(EncoderButton& eb) {
  configMode = !configMode;
  printLCD();
  return;
}

// ------------------------------------------ Encoder rotation ------------------------------------------
void onEb1Encoder(EncoderButton& eb) {
// freqSelMode Mode selector: (0) = Khz; (1) = Mhz; --- Default: Khz ---
// sysSelect System Selector: (1)=COM1; (2)=NAV1; (3)=COM2; (4)=NAV2;(5)=ADF; (6)=XPNDR;  --- Default: COM1 ---
//
// --- Increase ---
  if (eb.increment() == 1) {
    if (sysSelect == 5) {          // --- ADF
      if (modeADF == 0) {          // --- ADF Frecuency Mode
        if (freqADF == 0) {        // --- 0.1Khz 
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_FRACT_INC_CARRY")));
          messenger.sendCmdEnd();
        }
        if (freqADF == 1){        // --- 1Khz
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_1_INC")));
          messenger.sendCmdEnd();
        }
        if (freqADF == 2){        // --- 10Khz
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_10_INC")));
          messenger.sendCmdEnd();
        }
        if (freqADF == 3){        // --- 100Khz
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_100_INC")));
          messenger.sendCmdEnd();
        }
      }
      if (modeADF == 1){          // --- Modo ADF HDG
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_CARD_INC")));
          messenger.sendCmdEnd();
      }
    }
    // COM1 Khz    
    if (freqSelMode == 0 && sysSelect == 1) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM_RADIO_FRACT_INC_CARRY")));
      messenger.sendCmdEnd();
    }
    // COM1 Mhz
    if (freqSelMode == 1 && sysSelect == 1) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM_RADIO_WHOLE_INC")));
      messenger.sendCmdEnd();
    }
    // NAV1 Khz
    if (freqSelMode == 0 && sysSelect == 2) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV1_RADIO_FRACT_INC")));
      messenger.sendCmdEnd();
    }
    // NAV1 Mhz
    if (freqSelMode == 1 && sysSelect == 2) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV1_RADIO_WHOLE_INC")));
      messenger.sendCmdEnd();
    }
    // COM2 Khz
    if (freqSelMode == 0 && sysSelect == 3) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM2_RADIO_FRACT_INC_CARRY")));
      messenger.sendCmdEnd();
    }
    // COM2 Mhz
    if (freqSelMode == 1 && sysSelect == 3) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM2_RADIO_WHOLE_INC")));
      messenger.sendCmdEnd();
    }
    // NAV2 Khz
    if (freqSelMode == 0 && sysSelect == 4) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV2_RADIO_FRACT_INC")));
      messenger.sendCmdEnd();
    }
    // NAV2 Mhz
    if (freqSelMode == 1 && sysSelect == 4) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV2_RADIO_WHOLE_INC")));
      messenger.sendCmdEnd();
    }
    // XPNDR
    if (sysSelect == 6) {
     // ----  XPNDR code Increase -----
     // XPNDR position Selector
      if (decIDENT == 1) {    // First
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_1_INC")));
        messenger.sendCmdEnd();        
      }
      if (decIDENT == 2) {    // Second
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_10_INC")));
        messenger.sendCmdEnd();
      }
      if (decIDENT == 3) {    // Third
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_100_INC")));
        messenger.sendCmdEnd();
      }
      if (decIDENT == 4) {    // Fourth
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_1000_INC")));
        messenger.sendCmdEnd();        
      }
    }
    // CONFIG
    if(configMode == 1) {
      // (1)=Screen Mode; (2)=Brightness; (3)=Contrast
      if (configState == 1){
        modeLCD = !modeLCD;
        printLCD();
      }
      if (configState == 2){
        iluminacion++;
        applyConfig();
        printLCD();
      }
      if (configState == 3){
        contraste++;
        applyConfig();
        printLCD();
      }
    }
  }

  // --- Decrease ---
  if (eb.increment() == -1) {
    if (sysSelect == 5) {         // --- ADF
      if (modeADF == 0) {         // --- ADF Frecuency Mode
        if (freqADF == 0){        // --- 0.1Khz
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_FRACT_DEC_CARRY")));
          messenger.sendCmdEnd();
        }
        if (freqADF == 1){        // --- 1Khz
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_1_DEC")));
          messenger.sendCmdEnd();
        }
        if (freqADF == 2){        // --- 10Khz
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_10_DEC")));
          messenger.sendCmdEnd();
        }
        if (freqADF == 3){        // --- 100Khz
          messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_100_DEC")));
          messenger.sendCmdEnd();
        }
      }
      if (modeADF == 1){          // --- ADF HDG
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:ADF_CARD_DEC")));
        messenger.sendCmdEnd();
      }
    }    
    // COM1 Khz
    if (freqSelMode == 0 && sysSelect == 1) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM_RADIO_FRACT_DEC_CARRY")));
      messenger.sendCmdEnd();
    }
    // COM1 Mhz
    if (freqSelMode == 1 && sysSelect == 1) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM_RADIO_WHOLE_DEC")));
      messenger.sendCmdEnd();
    }
    // NAV1 Khz
    if (freqSelMode == 0 && sysSelect == 2) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV1_RADIO_FRACT_DEC")));
      messenger.sendCmdEnd();
    }
    // NAV1 Mhz
    if (freqSelMode == 1 && sysSelect == 2) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV1_RADIO_WHOLE_DEC")));
      messenger.sendCmdEnd();
    }
    // COM2 Khz
    if (freqSelMode == 0 && sysSelect == 3) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM2_RADIO_FRACT_DEC_CARRY")));
      messenger.sendCmdEnd();
    }
    // COM2 Mhz
    if (freqSelMode == 1 && sysSelect == 3) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:COM2_RADIO_WHOLE_DEC")));
      messenger.sendCmdEnd();
    }
    // NAV2 Khz
    if (freqSelMode == 0 && sysSelect == 4) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV2_RADIO_FRACT_DEC")));
      messenger.sendCmdEnd();
    }
    // NAV2 Mhz
    if (freqSelMode == 1 && sysSelect == 4) {
      messenger.sendCmd(kSimCommand,(F("SIMCONNECT:NAV2_RADIO_WHOLE_DEC")));
      messenger.sendCmdEnd();
    }
    // XPNDR
    if (sysSelect == 6){
      // ----  XPNDR code decrease -----
      // XPNDR position Selector
      if (decIDENT == 1) {    // First
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_1_DEC")));
        messenger.sendCmdEnd();        
      }
      if (decIDENT == 2) {    // Second
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_10_DEC")));
        messenger.sendCmdEnd();
      }
      if (decIDENT == 3) {    // Third
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_100_DEC")));
        messenger.sendCmdEnd();
      }
      if (decIDENT == 4) {    // Fourth
        messenger.sendCmd(kSimCommand,(F("SIMCONNECT:XPNDR_1000_DEC")));
        messenger.sendCmdEnd();        
      }
    }
    // CONFIG
    if(configMode == 1) {
      // (1)=Screen Mode; (2)=Brightness; (3)=Contrast
      if (configState == 1){
        modeLCD = !modeLCD;
        printLCD();
      }
      if (configState == 2){
        iluminacion--;
        applyConfig();
        printLCD();
      }
      if (configState == 3){
        contraste--;
        applyConfig();
        printLCD();
      }
    }
  }
}

// ----------------------------- SPAD.neXt connection UP / DOWN events -----------------------

void onEvent()
{
  char *szRequest = messenger.readStringArg();
// ------ Begin transmission ------
  if (strcmp(szRequest, "START") == 0) {
    lcd.clear();
    return;
  }
// ------- End Transmission --------
   if (strcmp(szRequest, "END") == 0) {
    lcd.clear();
    return;
  }
  // ------- Provider Event --------
  if (strcmp(szRequest, "PROVIDER") == 0) {
    // Aquí manejas el evento PROVIDER
    // Por ejemplo, puedes leer los argumentos adicionales:
    String arg1 = messenger.readStringArg();
    String arg2 = messenger.readStringArg();

    if (arg1 == "MSFS") {
      if (arg2 == "1") {
        // MSFS is connected
        isReady = true;
      }
    }
    return;
  }
}

// --------------------------------------- Unknown Command ------------------------------------

void onUnknownCommand()
{
  messenger.sendCmd(kDebug,"UNKNOWN COMMAND"); 
}

// ------------------------------------- SPAD.neXt Init --------------------------------------

void onIdentifyRequest()
{
  char *szRequest = messenger.readStringArg();
  if (strcmp(szRequest, "INIT") == 0) {
    messenger.sendCmdStart(kRequest);
    messenger.sendCmdArg(F("SPAD"));
    messenger.sendCmdArg(F("{9d6440d1-3d36-4f2c-884d-1d4bc2cde171}"));
    messenger.sendCmdArg(F("One Knob Radio_FS20 v1.0"));
    messenger.sendCmdEnd();
    return;
  }

// --------------------------------- SPAD.neXt Ping ----------------------------------

  if (strcmp(szRequest, "PING") == 0) {
    messenger.sendCmdStart(kRequest);
    messenger.sendCmdArg(F("PONG"));
    messenger.sendCmdArg(messenger.readInt32Arg());
    messenger.sendCmdEnd();
    return;
  }

  // ------------------------------- SPAD.neXt Subscriptions ------------------------------

  if (strcmp(szRequest, "CONFIG") == 0) {

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kADFActiveFreq);
    messenger.sendCmdArg(F("SIMCONNECT:ADF ACTIVE FREQUENCY:1"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kCOM1ActiveFreq);
    messenger.sendCmdArg(F("SIMCONNECT:COM ACTIVE FREQUENCY:1"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kCOM1StandbyFreq);
    messenger.sendCmdArg(F("SIMCONNECT:COM STANDBY FREQUENCY:1"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kNAV1ActiveFreq);
    messenger.sendCmdArg(F("SIMCONNECT:NAV ACTIVE FREQUENCY:1"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kNAV1StandbyFreq);
    messenger.sendCmdArg(F("SIMCONNECT:NAV STANDBY FREQUENCY:1"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kCOM2ActiveFreq);
    messenger.sendCmdArg(F("SIMCONNECT:COM ACTIVE FREQUENCY:2"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kCOM2StandbyFreq);
    messenger.sendCmdArg(F("SIMCONNECT:COM STANDBY FREQUENCY:2"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kNAV2ActiveFreq);
    messenger.sendCmdArg(F("SIMCONNECT:NAV ACTIVE FREQUENCY:2"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kNAV2StandbyFreq);
    messenger.sendCmdArg(F("SIMCONNECT:NAV STANDBY FREQUENCY:2"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kADFHDG);
    messenger.sendCmdArg(F("SIMCONNECT:ADF CARD"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kXpndr);
    messenger.sendCmdArg(F("SIMCONNECT:TRANSPONDER CODE:1"));
    messenger.sendCmdEnd();

    messenger.sendCmdStart(kCommand);
    messenger.sendCmdArg(F("SUBSCRIBE"));
    messenger.sendCmdArg(kIDENT);
    messenger.sendCmdArg(F("SIMCONNECT:TRANSPONDER IDENT"));
    messenger.sendCmdEnd();

    // --- End of Subscriptions ---
    messenger.sendCmd(kRequest, F("CONFIG"));
    messenger.sendCmdEnd();
    isReady = true;
    return;
  }
}

// --------------- C M D M E S S E N G E R  C A L L B A C K S  D E C L A R A T I O N S ------------

void attachCommandCallbacks()
{
  messenger.attach(onUnknownCommand);
  messenger.attach(kRequest , onIdentifyRequest);
  messenger.attach(kEvent, onEvent);
  messenger.attach(kADFActiveFreq, onADFActiveFreq);
  messenger.attach(kADFHDG, onnewADFHDG);
  messenger.attach(kCOM1ActiveFreq, onCOM1ActiveFreq);
  messenger.attach(kCOM1StandbyFreq, onCOM1StandbyFreq);
  messenger.attach(kNAV1ActiveFreq, onNAV1ActiveFreq);  
  messenger.attach(kNAV1StandbyFreq, onNAV1StandbyFreq);
  messenger.attach(kCOM2ActiveFreq, onCOM2ActiveFreq);
  messenger.attach(kCOM2StandbyFreq, onCOM2StandbyFreq);
  messenger.attach(kNAV2ActiveFreq, onNAV2ActiveFreq);  
  messenger.attach(kNAV2StandbyFreq, onNAV2StandbyFreq);
  messenger.attach(kXpndr, onXpndr);
  messenger.attach(kIDENT, onIDENT);
}



// ----------------------------------- S E T U P ---------------------------------------

void setup() {

// PWM Brightness control LCD   
  pinMode(luzPin, OUTPUT);
  pinMode(contrastePin, OUTPUT);
  if (pwmset == true){
    iluminacion = EEPROM.read(0);
    contraste = EEPROM.read(1);
    analogWrite(luzPin, iluminacion);
    analogWrite(contrastePin, contraste);
  } else {
    analogWrite(luzPin, iluminacionDef);
    analogWrite(contrastePin, contrasteDef);  
  }
    
// LiquidCrystal Initialization
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print(F("One Knob Radio"));
  lcd.setCursor(12,1);
  lcd.print(F("v1.0"));

// Load LCD Custom Characters 
  lcd.createChar(1,customCharC);
  lcd.createChar(2,customChar1);
  lcd.createChar(3,customCharN);
  lcd.createChar(4,customChar2);

// Serial Port Initialization
  Serial.begin(115200);

// User callback initialization
  attachCommandCallbacks();

// Encoder & button callback initialization
  eb1.setLongClickDuration(700);
  eb1.setClickHandler(onEb1Clicked);
  eb1.setEncoderHandler(onEb1Encoder);
  eb1.setLongPressHandler(onEb1LongClick);
  eb1.setDoubleClickHandler(onEb1DoubleClick);
  eb1.setTripleClickHandler(onEb1TripleClick);
}

// ------------------------------------ L O O P --------------------------------------

void loop() {
// CmdMessenger start
  messenger.feedinSerialData();

// EncoderButton start
  eb1.update();  
}
