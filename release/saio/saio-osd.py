#!/usr/bin/env python
#sudo apt-get install python-serial

import RPi.GPIO as GPIO 
import time
import os,signal,sys
import serial
import subprocess
import logging
import logging.handlers
try:
  from configparser import ConfigParser
except ImportError:
  from ConfigParser import ConfigParser  # ver. < 3.0

# Config variables
bin_dir         = '/home/pi/Super-AIO/release/saio/osd/'
ini_data_file   = bin_dir + 'data.ini'
ini_config_file = bin_dir + 'config.ini'
osd_path        = bin_dir + 'osd'

# Hardware variables
pi_lowb = 23
pi_shdn = 27
pi_overtemp = 26
serport = '/dev/ttyACM0'

# Setup
logging.basicConfig(level=logging.DEBUG)
logging.info("Program Started")

# Init GPIO pins
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(pi_lowb, GPIO.IN)
GPIO.setup(pi_shdn, GPIO.IN)
GPIO.setup(pi_overtemp, GPIO.OUT)

# Batt variables
voltscale = 203.5 #ADJUST THIS
currscale = 640.0
resdivmul = 4.0
resdivval = 1000.0
dacres = 33.0
dacmax = 1023.0

batt_threshold = 4
batt_low = 330
batt_shdn = 320
batt_islow = False

temperature_max = 60.0
temperature_threshold = 5.0
temperature_isover = False;

# Set up a port
try:
  ser = serial.Serial(
    port=serport,
    baudrate = 115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
  )
except Exception as e:
  logging.exception("ERROR: Failed to open serial port");
  sys.exit(1);
  
# Set up config file
config = ConfigParser()
config.add_section('protocol')
config.set('protocol', 'version', 1)
config.add_section('data')
config.set('data', 'voltage', '-.--')
config.set('data', 'temperature', '--.-')
config.set('data', 'showdebug', 1)
config.set('data', 'showwifi', 0)
config.set('data', 'showmute', 0)

try:
  with open(ini_data_file, 'w') as configfile:
    config.write(configfile)
except Expection as e:
  logging.exception("ERROR: Failed to create config file");
  sys.exit(1);
    
# Set up OSD service
try:
  osd_proc = subprocess.Popen([osd_path, "-d", ini_data_file, "-c", ini_config_file])
  time.sleep(1)
  osd_poll = osd_proc.poll()
  if (osd_poll):
    logging.error("ERROR: Failed to start OSD, got return code [" + str(osd_poll) + "]\n")
    sys.exit(1)
except Exception as e:
  logging.exception("ERROR: Failed start OSD binary");
  sys.exit(1);

# Check for shutdown state
def checkShdn():
  state = not GPIO.input(pi_shdn)
  if (state):
    logging.info("SHUTDOWN")
    doShutdown()

# Check for lowb state
def checkLowb():
  state = GPIO.input(pi_lowb)

# Read voltage
def readVoltage():
  ser.write('V')
  voltVal = int(ser.readline().rstrip('\r\n'))
  volt = int((( voltVal * voltscale * dacres + ( dacmax * 5 ) ) / (( dacres * resdivval ) / resdivmul)))
  
  logging.info("VoltVal [" + str(voltVal) + "]")
  logging.info("Volt    [" + str(volt) + "]V")
  
  global batt_islow
  
  if (batt_islow):
    if (volt > batt_low + batt_threshold):
      batt_islow = False
      logging.info("BATT OK")
    if (volt < batt_shdn):
      logging.info("VERY LOW BATT")
      #doShutdown()
      
  else:
    if (volt < batt_low):
      batt_islow = True
      logging.info("LOW BATT")
  
  return volt

# Get voltage percent
def getVoltagepercent(volt):
  return clamp(int( float(volt - batt_shdn)/float(420 - batt_shdn)*100 ), 0, 100)

# Read current
def readCurrent():
  ser.write('C')
  currVal = int(ser.readline().rstrip('\r\n'))
  curr = int((currVal * (dacres / (dacmax*10)) * currscale))
  
  logging.info("CurrVal [" + str(currVal) + "]")
  logging.info("Curr    [" + str(curr) + "]mA")
  return curr

# Read mode
def readModeDebug():
  ser.write('i')
  debugVal = int(ser.readline().rstrip('\r\n'))
  logging.info("Info    [" + str(debugVal) + "]")
  return debugVal

# Read wifi
def readModeWifi():
  ser.write('w')
  wifiVal = int(ser.readline().rstrip('\r\n'))
  logging.info("Wifi    [" + str(wifiVal) + "]")
  return wifiVal

# Read mute
def readModeMute():
  ser.write('a')
  audVal = int(ser.readline().rstrip('\r\n'))
  logging.info("Audio   [" + str(audVal) + "]")
  if (audVal):
    return 0
  else:
    return 1

# Read CPU temp
def getCPUtemperature():
  res = os.popen('vcgencmd measure_temp').readline()
  return float(res.replace("temp=","").replace("'C\n",""))

# Check temp
def checkTemperature():
  temp = getCPUtemperature()
  
  global temperature_isover
  
  if (temperature_isover):
    if (temp < temperature_max - temperature_threshold):
      temperature_isover = False
      GPIO.output(pi_overtemp, GPIO.LOW)
      logging.info("TEMP OK")
  else:
    if (temp > temperature_max):
      temperature_isover = True
      GPIO.output(pi_overtemp, GPIO.HIGH)
      logging.info("OVERTEMP")
  return temp

# Do a shutdown
def doShutdown():
  os.system("sudo shutdown -h now")
  try:
    sys.stdout.close()
  except:
    pass
  try:
    sys.stderr.close()
  except:
    pass
  sys.exit(0)

# Create ini config
def createINI(volt, curr, temp, debug, wifi, mute, file):
  #config.set('data', 'voltage', '{0:.2f}'.format(volt/100.00))
  config.set('data', 'voltage', volt)
  config.set('data', 'temperature', temp)
  config.set('data', 'showdebug', debug)
  config.set('data', 'showwifi', wifi)
  config.set('data', 'showmute', mute)

  with open(ini_data_file, 'w') as configfile:
    config.write(configfile)
  
  osd_proc.send_signal(signal.SIGUSR1)

# Show MP4 overlay
def doVidOverlay(overlay):
  os.system("/usr/bin/omxplayer --no-osd --layer 999999 " + overlay + " --alpha 160;");

# Show PNG overlay
def doPngOverlay(overlay):
  try:
    os.system("kill -s 9 `pidof pngview`");
  except:
    pass
  try:
    os.system("./pngview -b 0 -l 999999 " + overlay + "&");
  except:
    pass

# Misc functions
def clamp(n, minn, maxn):
  return max(min(maxn, n), minn)

# Main loop
try:
  print "STARTED!"
  while 1:
    volt = readVoltage()
    checkShdn()
    temp = checkTemperature()
    debug = readModeDebug()
    wifi = readModeWifi()
    mute = readModeMute()    

    createINI(volt, 0, temp, debug, wifi, mute, ini_data_file)
    
    time.sleep(3);
  
except KeyboardInterrupt:
  GPIO.cleanup
  osd_proc.terminate()
