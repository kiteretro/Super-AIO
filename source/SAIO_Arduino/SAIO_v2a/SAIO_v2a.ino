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
  for (uint8_t i=0; i<4; i++) {
    led(LED_ON);
    delay(50);
    led(LED_OFF);
    delay(50);
  }

  // Set up BACKLIGHT
  pinMode(PIN_BL, OUTPUT);

  // Set up LCD pins
  lcdPreInit();
  lcdInit();

  // Analogs
  pinMode(PIN_BTN_MODE, INPUT);
  pinMode(PIN_JOY_X, INPUT);
  pinMode(PIN_JOY_Y, INPUT);
  pinMode(PIN_CURR, INPUT);
  // Volt pin is special, we might use this to determine if we are OFF or ON..
  // When OFF, we expect this pin to be at max (1023)
  // When ON, we expect this pin to be the battery voltage (100/150k voltage divider)
  pinMode(PIN_VOLT, INPUT);

#ifdef CONFIG_WIFI_DEFAULT_OFF
  cfg.wifi_val = 0;
#endif

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
  calibrateJoystick();
  
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

    // Led ON for debug
    led(LED_ON);
    
    // Read Buttons
    readButtons();

    // Set gamepad buttons (USB)
    setGamepad();

    // Set modes and aux things
    setModes();

    // Led OFF for debug
    led(LED_OFF);
    
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

    // Init sequence if we have just booted
    if (isJustBooted()) {
      lcdInit();      
    }
    
    tnow3 = millis();
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
      case 'L':
        if (lcdSerial()) {
          Serial.print(LCD_SUCCESS);
        }
        break;
      case 'l':
        lcdInit();
        Serial.print(1);
        break;
      default:
        Serial.print('?');
      break;
    }
  }
}

