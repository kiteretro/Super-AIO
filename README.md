# Super-AIO
Kite's Super AIO

[Sudomod forum thread](http://www.sudomod.com/forum/viewtopic.php?f=3&t=1878)

# QUICK HOW TO
1. Get hardware
2. Solder up Pi to hardware
3. Download pre-made image and write to an SD
4. Boot up and test all features work
5. Make housing/shell
6. Install hardware bit by bit to check working
7. Complete build, test some more
8. Complete!

## PRE-MADE IMAGES
The pre-made images are available on the 'releases' section, and are a standard image + a build script run on it to install the extra bits!

There are now SINGLE images for each platform, and you can change them between NORMAL and TESTER by plugging in the SD to a PC and editing `config-saio.txt` (from within linux this is `/boot/config-saio.txt`) and uncommenting/commenting the right lines :)

When in TESTER mode, the power switch will no longer shutdown the system .. you must hard power it off, or plug in a USB keyboard and press `ctrl+c` and the `sudo poweroff`.

## SAIO MANUAL INSTALL
This github root folder must exist as the root of path `/home/pi/Super-AIO` (unless you change the bindir in the saio-osd.py and all associated config files).
This is done with `cd /home/pi ; git clone https://github.com/geebles/Super-AIO/`

The script will open a serial port to the ATMEGA32u4 and inquire details about battery voltage and config states, this requires the python-serial module. This can be installed with `sudo dpkg -i python-serial_2.6-1.1_all.deb` or `sudo apt-get install python-serial`.

You will probably need to set the binaries to executable:
- `chmod +x Super-AIO/release/saio/osd/osd`
- `chmod +x Super-AIO/release/saio/rfkill/rfkill`
- `chmod +x Super-AIO/release/saio/flash/flash.sh`

The script is launched with `python Super-AIO/release/saio/saio-osd.py`

You can run this as a cronjob with `crontab Super-AIO/release/saio/cronSAIO.txt`

Place the `autostart.sh` in `/opt/r*/configs/all/autostart.sh` and then configure the `/boot/config-saio.txt` file for the boot mode

To enable USB sound, copy the `asound.conf` into `/etc/` (`sudo cp Super-AIO/release/saio/asound.conf /etc/`)

### Flasher
In the path `Super-AIO/release/saio/flash` there exists a set of scripts that allow you to upload the Arduino code to the ATMEGA32u4 from the Pi itself. This requires `avrdude`. This can be done with the following commands:

1. Connect to wifi
2. ssh to pi (pi/raspberry)
3. `sudo apt-get install avrdude`
4. `cd Super-AIO/release/saio/saio/flash`
5. `./flash.sh SAIO_v2a_20170225_1_with_bootloader_leonardo.hex`
6. `sudo reboot`

If it complains about missing COM port, do the following:

1. `ls /dev/tty*`
2. Identify the name of ttyACM0 (or 1?)
3. `vim.tiny flash.sh`
4. Change line "PORT=/dev/ttyACM0" to the actual port
5. Save, and try again

## Tester
The 'tester' folder must exist as the root path of `/home/pi/Super-AIO/release/tester` (unless you change the bindir in the tester.py sctipt).

The script will test for the existence of USB devices (basically just an `lsusb` and some regex), followedby showing a 320x240 test image to screen. With this you can identify which LCD channels are problematic.

You will probably need to set the binaries to executable:
- `chmod +x Super-AIO/release/tester/pngview`

The script is launched with `python Super-AIO/release/saio/tester.py`
