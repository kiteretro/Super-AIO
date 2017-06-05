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
  uint16_t btnVal = analogRead(PIN_BTN_MODE);

#ifdef DEBUG
  Serial.print("BV[");
  Serial.print(btnVal);
  Serial.println("]");
#endif

#ifndef NOMODE
  if (btnVal < 5) { //Mode button pressed?
    if (isBooted()) {
      mode = 1;
    } else {
      mode = 0;
    }
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

  if (btns[BTN_A1]) {
    Gamepad.press(11);
  } else {
    Gamepad.release(11);
  }

  if (btns[BTN_A2]) {
    Gamepad.press(12);
  } else {
    Gamepad.release(12);
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
  state = HIGH;
}

//--------------------------------------------------------------------------------------
// CALIBRATE JOYSTICK
void calibrateJoystick()
{
  if (!btns[BTN_START]) {
    return;
  }
  
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
    
    led(LED_ON);
    
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
    led(LED_OFF);
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

