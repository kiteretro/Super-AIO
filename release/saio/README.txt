FROM A BLANK RETROPIE:

1.  git clone https://github.com/geebles/Super-AIO/
2.  cd Super-AIO/release/saio
3.  sudo dpkg -i python-serial_2.6-1.1_all.deb
3.  sudo dpkg -i rfkill_0.5-1_armhf.deb
4.  chmod +x ../tester/pngview
5.  chmod +x osd/osd
6.  chmod +x flash/flash.sh
7.  python saio-osd.py #(to test)
8.  crontab cronSAIO.txt
9.  sudo cp asound.conf /etc/
10. sudo cp config.txt /boot/config.txt
11. sudo cp config-saio.txt /boot/config-saio.txt
12. mv /opt/retropie/configs/all/autostart.sh /opt/retropie/configs/all/autostart_OLD.sh
13. cp autostart.sh /opt/retropie/configs/all/autostart.sh
14. sudo reboot