#!/usr/bin/env python

# Setup
#############################

import subprocess
import time
import datetime
import os.path

bindir = "/home/pi/tester/"

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
def testUSB():
  lsusb = subprocess.Popen(["/usr/bin/lsusb"], stdout=subprocess.PIPE).communicate()[0]
  print "LSUSB OUTPUT: \n" + str(lsusb) + "\n\n"

  if "Atmel Corp." in lsusb:
    print bcolors.OKGREEN + "USB ARDUINO = [ OK ]" + bcolors.ENDC
  else:
    if "Leonardo" in lsusb:
      print bcolors.OKGREEN + "USB ARDUINO = [ OK ]" + bcolors.ENDC
    else:
      print bcolors.FAIL + "USB ARDUINO = [FAIL]" + bcolors.ENDC

  if "C-Media Electronics" in lsusb:
    print bcolors.OKGREEN + "USB AUDIO   = [ OK ]" + bcolors.ENDC
  else:
    print bcolors.FAIL + "USB AUDIO   = [FAIL]" + bcolors.ENDC

  if "erminus Technology Inc. 4-Port HUB" in lsusb:
    print bcolors.OKGREEN + "USB HUB     = [ OK ]" + bcolors.ENDC
  else:
    print bcolors.FAIL + "USB HUB     = [FAIL]" + bcolors.ENDC

  if "Realtek Semiconductor Corp." in lsusb:
    print bcolors.OKGREEN + "USB WIFI    = [ OK ]" + bcolors.ENDC
  else:
    print bcolors.FAIL + "USB WIFI    = [FAIL]" + bcolors.ENDC
  
  if os.path.exists("/dev/input/js0"):
    print bcolors.OKGREEN + "JOYSTICK    = [ OK ]" + bcolors.ENDC
  else:
    print bcolors.FAIL + "JOYSTICK    = [FAIL]" + bcolors.ENDC
  
  time.sleep(3)

# Test LCD
#############################
def testLCD():
  print bcolors.OKBLUE + "TEST LCD.." + bcolors.ENDC
  pngview_proc = subprocess.Popen([bindir + "pngview", "-l", "99999", bindir + "COLOUR.png", "-x", "0", "-y", "0"])
  time.sleep(1)
  pngview_poll = pngview_proc.poll()
  if (pngview_poll):
    print "ERROR: Failed to start PNGVIEW, got return code [" + str(pngview_poll) + "]\n"
  else:
    time.sleep(3)
    pngview_proc.terminate()

# Test AUDIO
#############################
def testAUDIO():
  print bcolors.OKBLUE + "TEST AUDIO L.." + bcolors.ENDC
  aplay_proc = subprocess.Popen(["aplay", bindir + "audiocheck_l.wav"])
  time.sleep(2)
  print bcolors.OKBLUE + "TEST AUDIO R.." + bcolors.ENDC
  aplay_proc = subprocess.Popen(["aplay", bindir + "audiocheck_r.wav"])
  time.sleep(2)

# LOGIC
#############################
# Main loop
try:
  while 1:
    print "\n\n\n------------------------------"
    print datetime.datetime.now().time()
    print "------------------------------"
    
    testUSB()
    testLCD()
    #testAUDIO()
    
    time.sleep(1);
  
except KeyboardInterrupt:
  print "Quitting.."