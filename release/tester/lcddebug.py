#!/usr/bin/env python

# 
# This file originates from Kite's Super AIO control board project.
# Author: Kite (Giles Burgess)
# 
# THIS HEADER MUST REMAIN WITH THIS FILE AT ALL TIMES
#
# This firmware is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This firmware is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this repo. If not, see <http://www.gnu.org/licenses/>.
#

# Setup
#############################

import RPi.GPIO as GPIO 
import subprocess
import time
import datetime
import os.path

bindir = "/home/pi/Super-AIO/release/tester/"

# Software variables
bipontime  = 0.3
bipofftime = 0.3

# Hardware variables
pi_pclk  = 0
pi_den   = 1
pi_vsync = 2
pi_hsync = 3

pins = [ pi_pclk, pi_den, pi_vsync, pi_hsync ]

# Init GPIO pins
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

i = 0
for i in xrange (0, len(pins)):
  GPIO.setup(pins[i], GPIO.OUT)
  GPIO.output(pins[i], GPIO.LOW)

class bcolors:
  HEADER = '\033[95m'
  OKBLUE = '\033[94m'
  OKGREEN = '\033[92m'
  WARNING = '\033[93m'
  FAIL = '\033[91m'
  ENDC = '\033[0m'
  BOLD = '\033[1m'
  UNDERLINE = '\033[4m'

print "INFO: Started.\n"

# Test for USB devices
#############################
def debugLCD():
  print "DEBUG STARTED"
  
  print "\nGPIO 0 = PCLK  = 1 FLASH"
  print "GPIO 1 = DEN   = 2 FLASH"
  print "GPIO 2 = VSYNC = 3 FLASH"
  print "GPIO 3 = HSYNC = 4 FLASH"
  print "Running.."

  # One by one
  while 1:
    for i in xrange (0, len(pins)):
      for x in xrange(0, pins[i] + 1):
        GPIO.output(pins[i], GPIO.HIGH)
        time.sleep(bipontime)
        GPIO.output(pins[i], GPIO.LOW)
        time.sleep(bipofftime)

# LOGIC
#############################
# Main loop
try:
  while 1:
    print "\n\n\n------------------------------"
    print datetime.datetime.now().time()
    print "------------------------------"

    time.sleep(3)
    
    debugLCD()
  
except KeyboardInterrupt:
  print "Quitting.."