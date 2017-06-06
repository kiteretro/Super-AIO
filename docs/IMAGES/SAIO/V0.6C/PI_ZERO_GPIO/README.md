# PI_ZERO_GPIO
How to solder up the through hole GPIOs.

## Images at this level
Prepare the Pi by:
* Cutting the corner out
* Trimming the USB pads with a knife
* Soldering the USB pads
* Soldering the SD pads highlighted
* Placing kapton tape in order to cover the USB GROUND pins

![Prepare Pi](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/1.jpg)

Break the through hole pins into 2x 2pin rows, and 2x 18pin rows:

![Prepare pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/2.jpg)

Place the 2x pin rows into the Pi, and place on top of a hard metal object (pictured are a pair of pliers):

![Prepare pins push1](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/3.jpg)

Push the Pi down to slide the black plastic of the pin header down right to the bottom:

![Prepare pins push2](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/4.jpg)

The end result will look like the 'After' picture on the right:

![Compare pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/5.jpg)

Do the same for ALL pins. The 18x pin row will be hardest, you need to do it gently and put pressure on either side of the Pi:

![Do all pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/6.jpg)

Line up the Pi on top of the SAIO board:

![Place Pi](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/7.jpg)

You should notice that the USB and SD solder lumps will automatically align up the Pi to the board. Double check but pinching between your thumb and finger and looking down the soldes. You should see the bright shine of the solder we placed earlier. The through hole pins need to be in line too. If you have an M2 but and bolt you can use this to clamp the Pi down. I personally find holding it on one hand as pictured to work best:

![Align Pi](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/8.jpg)

With a soldering iron and pointed tip, make sure it is nice and hot. Then push the iron into the hole and make contact with the Pi on the other side. The iron tip should melt the solder that we already placed on the Pi and if you move your iron in a gently circle you should be able to get it to stick to the through holes. DO NOT apply force, if you twist and push hard you WILL break the solder holes:

![Solder USB](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/9.jpg)

Repeat for the SD pins. These holes are slightly larger so should be easier to see. Don't worry about making it perfect, we will add some solder later on to make it perfect:

![Solder SD](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/10.jpg)

Now that all the solder lumps have been tried, go back and using a sharp angle on the iron, place it in the holes. Then feed the solder into the iron from the opposite angle slowly. You should see it melt and wick down the hole and make a great connection (see the 'DAT2' solder dome just below the iron tip) :

![Solder SD 2](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/11.jpg)

Once complete, it should look like the following:

![Solder done](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/12.jpg)

Using kapton tape (selotape, or electrical tape is NOT recommended as it will melt..) place on the BACK side of the board over ALL the holes:

![Kapton tape](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/13.jpg)

Using the 2pin headers, place them gently on both ends of the Pi GPIO connector so that they touch the kapton tape on the other side (but don't go through any further) :

![Align pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/14.jpg)

By using the iron on the side, you need to solder the 2pin header to the Pi, this should look like the image below:

![Solder pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/15.jpg)

Using side cutters, cut the header off as low as possible (without scratching the Pi PCB) :

![Cut pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/16.jpg)

Flip the board over and peel back the kapton tape a little bit, you should be able to see the tips of the 2pin headers in the middle of the through holes:

![View other side pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/17.jpg)

Solder these. You will find them fairly difficult as some are GROUND pins and they require a lot of heat to be able to melt the solder. Turn the iron up slighly if needed:

![Solder other side pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/18.jpg)

Repeat the same for the other end of the board. At this end also solder the '3.3V' solder jumper. Ignore the '5V' jumper it is not needed:

![Solder jumper](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/19.jpg)

TEST that the board powers up and on. By placing an SD in (with SAIO image set to TESTER mode), adding power to the SAIO (DO NOT POWER THE PI BY ITS USB PORTS!!), connecting HDMI (making sure to make the config.txt changes to enable HDMI), and then using some tweezers in the power switch (or installing the back board), you can turn the Pi on.

Brief troubleshooting:
* No HDMI - Did you edit the config.txt on the SD?
* Status LED on Pi is NOT flashing - Did you burn the image to the SD? Re-solder the SD pins on the back.
* No power LEDs - Have you got the power switch installed? Try shorting the 'power enable' as pictured below.

![Test connection 1](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/20.jpg)

If all goes well it will boot into the TESTER mode and show some information on the screen. Make sure the USB HUB and ARDUINO are 'OK' as this will indicate that your USB soldering is good. If these are 'FAIL' you need to re-work the USB pins on the back. If your HDMI screen is black, make sure you have added to config.txt the HDMI settings.

![Test connection 2](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/21.jpg)

Now we are ready to solder the remaining pins. Place ONE row of the 18pin header into the top of the Pi (don't forget the kapton tape behind) :

![Place many pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/22.jpg)

Solder all the pins:

![Solder many pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/23.jpg)

Cut off all the pins:

![Cut many pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/24.jpg)

Repeat for the second row:

![Finished all pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/25.jpg)

Flip the board over and remove the kapton tape:

![View all pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/26.jpg)

Apply solder to all pins for perfect connections:

![Solder all pins](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/27.jpg)

Edit the config.txt and put back into DPI mode, connect the LCD, and boot the Pi:

![Test boot LCD 1](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/28.jpg)

Tester mode should be shown and be the same as the HDMI results:

![Test boot LCD 2](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/29.jpg)

Check that the colour image looks good. Any bad colours will be because the LCD isn't seated properly, or one (or more) of the solder pins are bad (re-work and re-solder them by adding more solder) :

![Test boot LCD 3](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/30.jpg)

You can now put it into NORMAL mode, connect the back board and speaker, and power on fully. You should be able to test out the remaining functionality here:

![Test setup](https://github.com/geebles/Super-AIO/raw/master/docs/IMAGES/SAIO/V0.6V/PI_ZERO_GPIO/31.jpg)