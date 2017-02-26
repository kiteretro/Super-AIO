# Super-AIO
Kite's Super AIO

[Sudomod forum thread](http://www.sudomod.com/forum/viewtopic.php?f=3&t=1878)

## SAIO
The 'saio' folder must exist as the root of path `/home/pi/saio` (unless you change the bindir in the saio-osd.py and all associated config files).

The script will open a serial port to the ATMEGA32u4 and inquire details about battery voltage and config states, this requires the python-serial module. This can be installed with `sudo dpkg -i python-serial_2.6-1.1_all.deb` or `sudo apt-get install python-serial`.

You will probably need to set the binaries to executable:
- `chmod +x osd/osd`
- `chmod +x flash/flash.sh`

The script is launched with `python saio-osd.py`

You can run this as a cronjob with `crontab cronSAIO.txt`

To enable USB sound, copy the `asound.conf` into `/etc/` (`sudo cp asound.conf /etc/`)

### Flasher
In the path `saio/flash` there exists a set of scripts that allow you to upload the Arduino code to the ATMEGA32u4 from the Pi itself. This requires `avrdude`. This can be done with the following commands:

1. Connect to wifi
2. ssh to pi (pi/raspberry)
3. `sudo apt-get install avrdude`
4. `cd saio/flash`
5. `./flash.sh SAIO_v2a_20170225_1_with_bootloader_leonardo.hex`
6. `sudo reboot`

If it complains about missing COM port, do the following:

1. `ls /dev/tty*`
2. Identify the name of ttyACM0 (or 1?)
3. `vim.tiny flash.sh`
4. Change line "PORT=/dev/ttyACM0" to the actual port
5. Save, and try again

## Tester
The 'tester' folder must exist as the root path of `/home/pi/tester` (unless you change the bindir in the tester.py sctipt).

The script will test for the existence of USB devices (basically just an `lsusb` and some regex), followedby showing a 320x240 test image to screen. With this you can identify which LCD channels are problematic.

You will probably need to set the binaries to executable:
- `chmod +x pngview`

The script is launched with `python tester.py`