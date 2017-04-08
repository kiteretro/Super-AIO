/*
 * This file originats from Kite's Super AIO control board project.
 * Author: Kite (Giles Burgess)
 * 
 * THIS HEADER MUST REMAIN WITH THIS FILE AT ALL TIMES
 *
 * This firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this repo. If not, see <http://www.gnu.org/licenses/>.
 */

 /*
  *  -------------
  * | USAGE NOTES |
  *  -------------
  *  
  * This firmware does the following:
  * - Reads an EEPROM configuration of saved data on boot
  *   - Any changes to configuration are saved automatically
  * - Flashes the RED and GREEN LEDs 4 times each in alternating repaid succession (BOOT OK)
  * - If the 'START' button is held on boot, a joystick calibration is started for 10 seconds (default)
  *   - During these 10 seconds, the LED will flash ORANGE
  *   - During these 10 seconds, slowly rotate the joystick around the full edge of its limits
  *     - You should get about 3-4 rotations done
  *   - NOTE: The joystick WILL NOT FUNCTION UNLESS IT HAS BEEN CALIBRATED!
  * - A loop is started that then processes the following data, and repeats forever:
  *   - @ 62.5Hz
  *     - Read the buttons and joystick
  *     - Write them over USB (as a gamepad)
  *     - If the 'mode' button is pressed with a button combo, action these (this will stall the loop to 1-5Hz)
  *   - @ 5Hz
  *     - Check to see if the 'mode' button is pressed
  *     - Process any incoming serial data (see the function processSerial())
  *   - @ 1Hz
  *     - Read the battery voltage
  * 
  *  -------------
  * | MODE BUTTON |
  *  -------------
  * 
  * By holding down the 'MODE' button while the board is powered on (and the main power switch is on),
  * the following can be done:
  * 
  * MODE + BTN | Result
  * ---------------------------
  * UP         | Brightness of LCD increases
  * DOWN       | Brightness of LCD decreases
  * B          | WIFI OFF
  * A          | WIFI ON
  * X          | AUDIO AMP OFF
  * Y          | AUDIO AMP ON
  * START      | INFO MODE ON
  * SELECT     | INFO MODE OFF
  * 
  */

#include "config.h"
#include "defs.h"

#include "Wire.h"
#include "HID-Project.h" //https://github.com/NicoHood/HID
#include <EEPROM.h>

// Configuration data (loaded from EEPROM)
struct Config {
  // Version info
  uint8_t version  = EEPROM_VERSION;
  // Hardware settings
  uint8_t bl_val   = BL_MIN;
  uint8_t wifi_val = 1;
  uint8_t aud_val  = 1;
  uint8_t info_val = 0;
  // Joystick settings
  int16_t xmid = 0;
  int16_t ymid = 0;
  int16_t xmin  = 0;
  int16_t ymin  = 0;
  int16_t xmax  = 0;
  int16_t ymax  = 0;
  int16_t dz    = DEADZONE;
  bool iscalib  = 0;
} cfg;

// Button array
volatile bool btns[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile uint8_t btns_char[2] = {0,0};
volatile bool mode = 0;

//Analog
volatile uint16_t voltVal = 0;
volatile uint16_t currVal = 0;

// Button update state
volatile byte state = LOW;

//--------------------------------------------------------------------------------------
// MAIN SETUP
void setup()
{
  // Startup delay
  delay(1000);
  
  // Set up SERIAL
  Serial.begin(115200);

  // Set up STATUS LED
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  for (uint8_t i=0; i<4; i++) {
    digitalWrite(PIN_LED1, HIGH);
    delay(50);
    digitalWrite(PIN_LED1, LOW);
    delay(50);
    digitalWrite(PIN_LED2, HIGH);
    delay(50);
    digitalWrite(PIN_LED2, LOW);
    delay(50);
  }

  // Set up BACKLIGHT
  pinMode(PIN_BL, OUTPUT);

  // Set up LCD
  pinMode(PIN_LCD_RESET, OUTPUT);
  delay(10); // Hold low for at least 1ms
  digitalWrite(PIN_LCD_RESET, HIGH);
  delay(10);
  pinMode(PIN_LCD_RESET, INPUT_PULLUP); // Low power pullup

  // Analogs
  pinMode(PIN_BTN_MODE, INPUT);
  pinMode(PIN_JOY_X, INPUT);
  pinMode(PIN_JOY_Y, INPUT);
  pinMode(PIN_CURR, INPUT);
  // Volt pin is special, we might use this to determine if we are OFF or ON..
  // When OFF, we expect this pin to be at max (1023)
  // When ON, we expect this pin to be the battery voltage (100/150k voltage divider)
  pinMode(PIN_VOLT, INPUT);
  

  // Read from EEPROM
  eepromCheck();

  // Set defaults
  setBl(cfg.bl_val);
  setWifi(cfg.wifi_val);
  setAud(cfg.aud_val);
  setInfo(cfg.info_val);

  // Set up I2C
  Wire.begin(); // wake up I2C bus
  //attachInterrupt(digitalPinToInterrupt(PIN_INT), change, FALLING);

  // Reset pointer
  Wire.beginTransmission(ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
  
  // Get button initial states
  readButtons();

  // Calibrate joystick?
  if (btns[BTN_START]) {
    calibrateJoystick();
  }
  
  // Sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

//--------------------------------------------------------------------------------------
// MAIN LOOP
void loop()
{
  
  // Button loop (62.5Hz)
  static uint32_t tnow = 0;
  if (millis() - tnow > 16) {

    digitalWrite(PIN_LED1, HIGH);
    
    // Read Buttons
    readButtons();

    // Set gamepad buttons (USB)
    setGamepad();

    // Set modes and aux things
    setModes();

    digitalWrite(PIN_LED1, LOW);
    
    tnow = millis();
  }

  // Misc functions (5Hz)
  static uint32_t tnow2 = 0;
  if (millis() - tnow2 > 200) {

    // See if mode presses
    readMode();

    // Check serial console
    processSerial();
    
    tnow2 = millis();
  }

  // Misc functions (1Hz)
  static uint32_t tnow3 = 0;
  if (millis() - tnow3 > 1000) {

    // Read some analog values
    readAnalogData();
    
    tnow3 = millis();
  }
  
}

//--------------------------------------------------------------------------------------
// READ BUTTONS
void readButtons()
{
  // Read
  uint8_t pos = 0;
  Wire.requestFrom(ADDR, 2); // request 2 bytes from slave device
  while(Wire.available()) // slave may send less than requested
  {
    uint8_t c = Wire.read(); // receive a byte as character
    
    if (pos) {
      btns_char[0] = c;
      for (uint8_t i=0; i<8; i++) {
        btns[i+8] = !bitRead(c, i);
      }
    } else {
      btns_char[1] = c;
      for (uint8_t i=0; i<8; i++) {
        btns[i] = !bitRead(c, i);
      }
    }
    pos++;
  }
}

//--------------------------------------------------------------------------------------
// READ MODE
void readMode()
{
  //uint16_t btnVal = analogRead(PIN_BTN_MODE);
  uint16_t btnVal = analogRead(PIN_BTN_MODE);

#ifdef DEBUG
  Serial.print("BV[");
  Serial.print(btnVal);
  Serial.println("]");
#endif

#ifndef NOMODE
  if (btnVal < 5) { //Mode button pressed?
#ifdef VBATMODECHECKLOW
    if (voltVal > 2) {
      mode = 1;
    } else {
      mode = 0;
    }
#endif
#ifdef VBATMODECHECKHIGH
    if (voltVal < 600) {
      mode = 1;
    } else {
      mode = 0;
    }
#endif
#ifdef A0MODECHECK
    if (analogRead(A0) > 1000) {
      mode = 1;
    } else {
      mode = 0;
    }
#endif
  } else {
    mode = 0;
  }
#endif
}

//--------------------------------------------------------------------------------------
// SET GAMEPAD
void setGamepad()
{
  //DPAD
  Gamepad.dPad1(GAMEPAD_DPAD_CENTERED);

  if (btns[BTN_DOWN]) {
    Gamepad.dPad1(GAMEPAD_DPAD_DOWN);
    if (btns[BTN_LEFT]) {
      Gamepad.dPad1(GAMEPAD_DPAD_DOWN_LEFT);
    }
    if (btns[BTN_RIGHT]) {
      Gamepad.dPad1(GAMEPAD_DPAD_DOWN_RIGHT);
    }
  } else if(btns[BTN_UP]) {
    Gamepad.dPad1(GAMEPAD_DPAD_UP);
    if (btns[BTN_LEFT]) {
      Gamepad.dPad1(GAMEPAD_DPAD_UP_LEFT);
    }
    if (btns[BTN_RIGHT]) {
      Gamepad.dPad1(GAMEPAD_DPAD_UP_RIGHT);
    }
  } else if (btns[BTN_LEFT]) {
    Gamepad.dPad1(GAMEPAD_DPAD_LEFT);
  } else if (btns[BTN_RIGHT]) {
    Gamepad.dPad1(GAMEPAD_DPAD_RIGHT);
  }
  
  //BUTTON
  if (btns[BTN_A]) {
    Gamepad.press(1);
  } else {
    Gamepad.release(1);
  }

  if (btns[BTN_B]) {
    Gamepad.press(2);
  } else {
    Gamepad.release(2);
  }

  if (btns[BTN_X]) {
    Gamepad.press(3);
  } else {
    Gamepad.release(3);
  }

  if (btns[BTN_Y]) {
    Gamepad.press(4);
  } else {
    Gamepad.release(4);
  }

  if (btns[BTN_START]) {
    Gamepad.press(5);
  } else {
    Gamepad.release(5);
  }

  if (btns[BTN_SELECT]) {
    Gamepad.press(6);
  } else {
    Gamepad.release(6);
  }

  if (btns[BTN_L1]) {
    Gamepad.press(7);
  } else {
    Gamepad.release(7);
  }

  if (btns[BTN_L2]) {
    Gamepad.press(8);
  } else {
    Gamepad.release(8);
  }

  if (btns[BTN_R1]) {
    Gamepad.press(9);
  } else {
    Gamepad.release(9);
  }

  if (btns[BTN_R2]) {
    Gamepad.press(10);
  } else {
    Gamepad.release(10);
  }

  // SPEEDHACK!
  if (!mode) {
    Gamepad.write();
  }

  // JOYSTICK
  if (cfg.iscalib) {
    uint16_t x = analogRead(PIN_JOY_X);
    uint16_t y = analogRead(PIN_JOY_Y);
    
    int16_t xx = x-cfg.xmid;
    int16_t yy = y-cfg.ymid;

    if (xx > cfg.dz) {
      xx -= cfg.dz;
    } else if (xx<(cfg.dz-cfg.dz-cfg.dz)) {
      xx += cfg.dz;
    } else {
      xx = 0;
    }

#ifdef INVERT_X
    xx = constrain(map(xx, cfg.xmin+50, cfg.xmax-50, 32678, -32678), -32678, 32678);
#else
    xx = constrain(map(xx, cfg.xmin+50, cfg.xmax-50, -32678, 32678), -32678, 32678);
#endif

    if (yy > cfg.dz) {
      yy -= cfg.dz;
    } else if (yy<(cfg.dz-cfg.dz-cfg.dz)) {
      yy += cfg.dz;
    } else {
      yy = 0;
    }

#ifdef INVERT_Y
    yy = constrain(map(yy, cfg.ymin+50, cfg.ymax-50, 32678, -32678), -32678, 32678);
#else
    yy = constrain(map(yy, cfg.ymin+50, cfg.ymax-50, -32678, 32678), -32678, 32678);
#endif

    Gamepad.xAxis(xx);
    Gamepad.yAxis(yy);
  } else {
    Gamepad.xAxis(0);
    Gamepad.yAxis(0);
  }
#ifndef NOJOY
  if (!mode) {
    Gamepad.write();
  }
#endif
}

//--------------------------------------------------------------------------------------
// SET MODES
void setModes()
{
  // WRITE
  if (mode) {
    if (btns[BTN_UP]) {
      setBl(BL_UP);
    }
    if (btns[BTN_DOWN]) {
      setBl(BL_DOWN);
    }
    if (btns[BTN_A]) {
      setWifi(1);
    }
    if (btns[BTN_B]) {
      setWifi(0);
    }
    if (btns[BTN_Y]) {
      setAud(1);
    }
    if (btns[BTN_X]) {
      setAud(0);
    }
    if (btns[BTN_START]) {
      setInfo(1);
    }
    if (btns[BTN_SELECT]) {
      setInfo(0);
    }

#ifdef DEBUG
    Serial.print("B[");
    Serial.print(cfg.bl_val);
    Serial.println("]");
#endif
    
    delay(200);
  }
}

//--------------------------------------------------------------------------------------
// PROCESS SERIAL
void processSerial()
{
  if (Serial.available() > 0) {
    // read the incoming byte:
    char in = Serial.read();

    switch (in) {
      case 'V':
        Serial.print(voltVal);
        break;
      case 'C':
        Serial.print(currVal);
        break;
      case 'i':
        Serial.print(cfg.info_val);
        break;
      case 'w':
        Serial.print(cfg.wifi_val);
        break;
      case 'a':
        Serial.print(cfg.aud_val);
        break;
      case 'b':
        Serial.write(btns_char[0]);
        Serial.write(btns_char[1]);
        break;
      default: 
        Serial.print('?');
      break;
    }
  }
}

//--------------------------------------------------------------------------------------
// ANALOG DATA
void readAnalogData()
{
  voltVal = analogRead(PIN_VOLT);
  delay(1);
  currVal = analogRead(PIN_CURR);
  delay(1);

#ifdef DEBUG
  Serial.print("V[");
  Serial.print(voltVal);
  Serial.print("]C[");
  Serial.print(currVal);
  Serial.println("]");
#endif
}

//--------------------------------------------------------------------------------------
// INTERRUPT FOR I2C
void change()
{
  digitalWrite(PIN_LED1, HIGH);
  state = HIGH;
}

//--------------------------------------------------------------------------------------
// BACKLIGHT FUNCTION
void setBl(uint8_t t) {

  if (t == BL_DOWN) {
    if (cfg.bl_val < BL_MAX) {
      if (cfg.bl_val + BL_INC < BL_MAX) {
        cfg.bl_val += BL_INC;
      } else {
        cfg.bl_val = BL_MAX;
      }
    }
  } else if (t == BL_UP) {
    if (cfg.bl_val > BL_MIN) {
      if (cfg.bl_val - BL_INC > BL_MIN) {
        cfg.bl_val -= BL_INC;
      } else {
        cfg.bl_val = BL_MIN;
      }
    }
  }
  
  analogWrite(PIN_BL, cfg.bl_val);
  eepromWrite();
}

//--------------------------------------------------------------------------------------
// WIFI
void setWifi(bool val)
{
  if (val) {
    cfg.wifi_val = 1;
    pinMode(PIN_WIFI, INPUT);
#ifdef DEBUG
    Serial.println("W ON");
#endif
  } else {
    cfg.wifi_val = 0;
    pinMode(PIN_WIFI, OUTPUT);
    digitalWrite(PIN_WIFI, HIGH);
#ifdef DEBUG
    Serial.println("W OFF");
#endif
    delay(500);
  }

  eepromWrite();
}

//--------------------------------------------------------------------------------------
// AUDIO
void setAud(bool val)
{
  if (val) {
    cfg.aud_val = 1;
    pinMode(PIN_AUD, INPUT);
#ifdef DEBUG
    Serial.println("A ON");
#endif
  } else {
    cfg.aud_val = 0;
    pinMode(PIN_AUD, OUTPUT);
#ifdef DEBUG
    Serial.println("A OFF");
#endif
  }

  eepromWrite();
}

//--------------------------------------------------------------------------------------
// INFO
void setInfo(bool val)
{
  if (val) {
    cfg.info_val = 1;
#ifdef DEBUG
    Serial.println("INFO ON");
#endif
  } else {
    cfg.info_val = 0;
#ifdef DEBUG
    Serial.println("INFO OFF");
#endif
  }

  eepromWrite();
}

//--------------------------------------------------------------------------------------
// CALIBRATE JOYSTICK
void calibrateJoystick()
{
#ifdef DEBUG
  //while (!Serial) {}
  Serial.println("CALIB");
#endif
  
  // Set up the vars
  uint16_t xmid = 0;
  uint16_t ymid = 0;
  int16_t xmin = 0;
  int16_t ymin = 0;
  int16_t xmax = 0;
  int16_t ymax = 0;

  uint16_t x = analogRead(PIN_JOY_X);
  xmin = x;
  xmax = x;
  xmid = x;
  uint16_t y = analogRead(PIN_JOY_Y);
  ymin = y;
  ymax = y;
  ymid = y;

  uint32_t tnow = millis();
  // Calibrate mid, min, and max for X secs
  //while(millis() < CALIBTIME * 1000) {
  while(millis() - tnow < CALIBTIME * 1000) {
    
    digitalWrite(PIN_LED1, HIGH);
    digitalWrite(PIN_LED2, HIGH);
    
    uint16_t x = analogRead(PIN_JOY_X); delay(1);
    uint16_t y = analogRead(PIN_JOY_Y);

    xmid = (xmin + xmax)/2;
    ymid = (ymin + ymax)/2;
    
    int16_t nx = x - xmid;
    int16_t ny = y - ymid;

    if (x < xmin)
      xmin = x;
    if (y < ymin)
      ymin = y;

    if (x > xmax)
      xmax = x;
    if (y > ymax)
      ymax = y;

#ifdef DEBUG
    Serial.print("XM:");
    Serial.print(xmid);
    Serial.print(" YM:");
    Serial.print(ymid);
    Serial.print(" X:");
    Serial.print(x);
    Serial.print(" Y:");
    Serial.print(y);
    Serial.print(" NX:");
    Serial.print(nx);
    Serial.print(" NY:");
    Serial.print(ny);
    Serial.print(" XMI:");
    Serial.print(xmin);
    Serial.print(" YMI:");
    Serial.print(ymin);
    Serial.print(" XMA:");
    Serial.print(xmax);
    Serial.print(" YMA:");
    Serial.print(ymax);
    Serial.println();
#endif
    
    delay(100);
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, LOW);
    delay(100);
  }

  xmin = xmin - xmid;
  ymin = ymin - ymid;
  xmax = xmax - xmid;
  ymax = ymax - ymid;

#ifdef DEBUG
  Serial.println("CALIB COMPLETE");
  Serial.print("XM:");
  Serial.print(xmid);
  Serial.print(" YM:");
  Serial.print(ymid);
  Serial.print(" XMI:");
  Serial.print(xmin);
  Serial.print(" YMI:");
  Serial.print(ymin);
  Serial.print(" XMA:");
  Serial.print(xmax);
  Serial.print(" YMA:");
  Serial.print(ymax);
  Serial.println();
#endif

  // Save values
  cfg.xmin = xmin;
  cfg.ymin = ymin;
  cfg.xmax = xmax;
  cfg.ymax = ymax;
  cfg.xmid = xmid;
  cfg.ymid = ymid;
  cfg.dz = DEADZONE;
  cfg.iscalib = 1;
  eepromWrite();
}

//--------------------------------------------------------------------------------------
// Check EEPROM is ok and read contents
void eepromCheck() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(EEPROM_START + 0) == EEPROM_VERSION) {
    eepromRead();
  } else {
    eepromWrite();
    eepromRead();
  }
}

//--------------------------------------------------------------------------------------
// Write eeprom
void eepromWrite() {
  for (unsigned int t=0; t<sizeof(cfg); t++) {
    EEPROM.write(EEPROM_START + t, *((char*)&cfg + t));
  }
}

//--------------------------------------------------------------------------------------
// Read eeprom
void eepromRead() {
  for (unsigned int t=0; t<sizeof(cfg); t++) {
    *((char*)&cfg + t) = EEPROM.read(EEPROM_START + t);
  }
}

