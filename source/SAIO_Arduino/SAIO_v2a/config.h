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

// Options
//#define DEBUG //Print out a load of debug messages. YOU NEED THE SERIAL MONITOR ALWAYS OPEN OTHERWISE THE BUFFER FILLS UP
//#define NOMODE //Disable the use of the 'mode' button completely
//#define A0MODECHECK //Read voltage on A0 to determine board power check
#define VBATMODECHECKHIGH //Read the VBAT voltage to determine board power check, HIGH = OFF
//#define VBATMODECHECKLOW //Read the VBAT voltage to determine board power check, LOW = OFF
//#define NOJOY //Disable the joystick

// Hardware bits
#define PIN_LCD_CS 5
#define PIN_LCD_RESET 13
#define PIN_VOLT A2
#define PIN_CURR A3
#define PIN_LOWBATT A4
#define PIN_BTN_MODE A5
#define PIN_JOY_X A6 //D4
#define PIN_INT 7 //TCA9555 interrupt
#define PIN_LED1 6
#define PIN_LED2 8
#define PIN_WIFI 9
#define PIN_BL 10
#define PIN_AUD 11
#define PIN_JOY_Y A11 //D12
#define PIN_JOY2_X A0
#define PIN_JOY2_Y A1

// Joystick settings
#define CALIBTIME 10      // Time in seconds to calibrate the joystick
#define DEADZONE 50       // ADC value for 'deadzone' of joystick
//#define INVERT_X        // Invert the X axis
#define INVERT_Y          // Invert the Y axis

// Backlight settings
#define BL_MIN 0          // 0 = FULL ON
#define BL_MAX 235        // High val = LOW
#define BL_INC 10
