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

#include "config.h"
#include "defs.h"

bool bootcheck = false;

//--------------------------------------------------------------------------------------
void led(uint8_t state) {
  if (state == LED_ON) {
    digitalWrite(PIN_LED1, HIGH);
  } else if (state == LED_OFF) {
    digitalWrite(PIN_LED1, LOW);
  } else {
    return;
  }
}

//--------------------------------------------------------------------------------------
// Just booted check
bool isJustBooted() {
  bool ret = false;

  if (isBooted()) {
    if (!bootcheck) {
      bootcheck = true;
      ret = true;
    }
  } else {
    if (bootcheck) {
      bootcheck = false;
    }
  }

#ifdef DEBUG
  Serial.print("JB[");
  Serial.print(ret);
  Serial.println("]");
#endif

  return ret;
}

// Boot status
bool isBooted() {
  bool ret = false;
#if MODECHECK == MODECHECK_BOOTPIN
  ret = digitalRead(PIN_BOOTCHECK);
  
#elif MODECHECK == MODECHECK_VBATLOW
  if (voltVal > 2) {
    ret = true;
  }
  
#elif MODECHECK == MODECHECK_VBATHIGH
  if (voltVal < 600) {
    ret = true;
  }
  
#elif MODECHECK == MODECHECK_A0
  if (analogRead(A0) > 1000) {
    ret = true;
  }
  
#else
  ret = false;
  
#endif

#ifdef DEBUG
  Serial.print("BO[");
  Serial.print(ret);
  Serial.println("]");
#endif

  return ret;
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
void setWifi(bool val) {
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
void setAud(bool val) {
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
void setInfo(bool val) {
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

