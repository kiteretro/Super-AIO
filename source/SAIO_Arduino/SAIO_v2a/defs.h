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

// I2C Bits
#define ADDR 0x24

#define PORT_0 0
#define PORT_1 1

#define DIR_OUTPUT 0
#define DIR_INPUT  1

#define POLARITY_NORMAL 0
#define POLARITY_INV    1

#define CR_IN0  0x0
#define CR_IN1  0x1
#define CR_OUT0 0x2
#define CR_OUT1 0x3
#define CR_INV0 0x4
#define CR_INV1 0x5
#define CR_CFG0 0x6
#define CR_CFG1 0x7

// Button mappings
#define BTN_UP     2
#define BTN_DOWN   4
#define BTN_LEFT   3
#define BTN_RIGHT  5
#define BTN_A      9
#define BTN_B      8
#define BTN_X      11
#define BTN_Y      10
#define BTN_START  7
#define BTN_SELECT 6
#define BTN_L1     1
#define BTN_L2     0
#define BTN_R1     13
#define BTN_R2     12
#define BTN_A1     14
#define BTN_A2     15

// BACKLIGHT
#define BL_UP   2
#define BL_DOWN 1

// EEPROM
#define EEPROM_VERSION 5
#define EEPROM_START   0

// MISC
#define LED_OFF 0
#define LED_ON  1

#define HARDWARE_05E1 1
#define HARDWARE_06C  2

#define MODECHECK_A0       1 //Read voltage on A0 to determine board power check
#define MODECHECK_VBATHIGH 2 //Read the VBAT voltage to determine board power check, HIGH = OFF
#define MODECHECK_VBATLOW  3 //Read the VBAT voltage to determine board power check, LOW = OFF
#define MODECHECK_BOOTPIN  4 //Read a pin to check if booted or not

#define LCD_SUCCESS "LS"
#define FAILED_TIMEOUT_NO_END "F1"
#define FAILED_NOT_EVEN_ARGS  "F2"

#define STATUS_MODE 0
#define STATUS_WIFI 1
#define STATUS_AUD  2

