1. Connect to wifi
2. ssh to pi (pi/raspberry)
3. sudo apt-get install avrdude
4. cd Super-AIO/release/saio/flash
5. ./flash.sh SAIO_v2a_20170225_1_with_bootloader_leonardo.hex
6. sudo reboot

NOTES:
- If it complains about missing COM port, do:
  - ls /dev/tty*
  - Identify the name of ttyACM0 (or 1?)
  - vim.tiny flash.sh
  - Change line "PORT=/dev/ttyACM0" to the actual port
  - Save, and try again