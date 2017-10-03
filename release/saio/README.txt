FROM A BLANK RETROPIE:

1.  git clone https://github.com/geebles/Super-AIO/
2.  cd Super-AIO/release/saio
3.  sudo dpkg -i python-serial_2.6-1.1_all.deb
4.  sudo chmod +x ../tester/pngview
5.  sudo chmod +x osd/saio-osd
6.  sudo chmod +x rfkill/rfkill
7.  sudo chmod +x flash/flash.sh
8.  python saio-osd.py #(to test)
9.  crontab cronSAIO.txt
10.  sudo cp asound.conf /etc/
11. sudo cp config.txt /boot/config.txt
12. sudo cp config-saio.txt /boot/config-saio.txt
13. sudo mv /opt/retropie/configs/all/autostart.sh /opt/retropie/configs/all/autostart_OLD.sh
14. sudo cp autostart.sh /opt/retropie/configs/all/autostart.sh
15. sudo reboot
