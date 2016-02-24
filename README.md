# Wireless Brother KH-9xx knitting machine connection.

Isn't your Brother KH-9xx online and blogging yet :smile:?
Come on, stop wiring your room, it's time for IoT and WiFi!
Let's create wireless BrotherLink cable!

Brief
--------
- Assemble [Hardware](#Hardware)
- Flash [Firmware](#Firmware)
- Knit with [DesignaKnit](#Software)

Introduction
------------

You all know that BrotherLink BLx cable are far overpriced. Some of you create 
DIY cables following [Adafruit](https://learn.adafruit.com/electroknit?view=all)
or [Daviworks](http://daviworks.com/knitting/cable_tutorial.html) tutorials.
After having repeated their cable I got an idea to replace the wired connection
with a WiFi one.

After looking around I found that there's a pretty cheap solution out there on
the market – ESP8266 module, which is actually an MCU (micro controller unit)
with built-in SPI flash memory, WiFi, GPIO, UART, etc. periphery. It is very
popular for DIY IoT projects, has a lot of users, who contributed a lot of
libraries helping to develop ESP8266 applications using pure C/C++, Arduino
Wiring, Lua and so on.

Hardware
--------

I'm not an electronic engineer so I decided to use ready-to-use modules to reduce
welding and avoid mistakes. As for the minimum you need:

1. ESP8266 module (or board). I selected [WeMos D1 mini](http://www.wemos.cc/wiki/doku.php?id=en:d1_mini) -
a minimalist ESP8266 board with all necessary resistors, integrated USB-UART 
bridge (you won't need a separate converter to connect the module to PC) and 
a 5V-to-3.3V power converter (not very reliable though).
2. Logic level converter. Brother KH-9xx serial interface operates at 5V level (at least),
while ESP8266 IO expects 3.3V. The simplest logic level conversion can be achieved with
two resistors by I opted to use a factory manufactured module. I bought mine from
[AliExpress](http://www.aliexpress.com/item/5pcs-lot-4-channel-IIC-I2C-Logic-Level-Converter-Bi-Directional-Module-5V-to-3-3V/32308637756.html)
which is times cheaper than the one from [SparkFun](https://www.sparkfun.com/products/12009).
*Caution*: do not! connect ESP8266 RX/TX directly to KH-9xxx port, doing so will 
most likely harm the module.
3. Power. ESP8266 operates at 3.3V (caution!); to coordinate logic levels you'll
also need 5V. For the time writing the article I feed my device with a breadboard
power supply (note separate +5V and +3.3V buses on the diagram), but for a final
setup I'm going to pick up +5V directly from KH-9xx internals in order to get rid
of wires completely. To convert +5V to +3.3V you should consider including a
separate AMS1117-based [power module](http://www.aliexpress.com/item/Free-shipping-5pcs-lot-AMS1117-3-3V-power-supply-module-AMS1117-3-3-power-module-AMS1117/1996263198.html)
(not shown on the diagram). 
4. Optionally, to provide for Interactive Knitting (IK)  mode of the [DesignaKnit](https://www.softbyte.co.uk/designaknit.htm)
application, you might want to add a reed switch and a 10kOm resistor (or, like
did I, use a pre-manufactured [reed switch module](http://www.aliexpress.com/item/Free-Shipping-Reed-sensor-module-magnetron-module-reed-switch-magnetic-switch-for-arduino/1946556259.html)).
You also need a magnet, which you'll put (glue or somehow attach) to the carriage
of the knitting machine (that'll be your BL5 “magnetic arm”). With this option
your device becomes a BrotherLink BL5 wireless cable, while without it – just BL1.

The diagram:
![Diagram](/BrotherLink_bb.png)

Firmware
--------

To make things fly you'll have to upload some firmware to the ESP8266.

The best option for those who are not close to MCU firmware programing
is to download ready-to-flash binary files from the [release](../../releases/download/v1.0/release.zip).

The firmware is based on [esp8266web](https://github.com/pvvx/esp8266web) 
project which already contains almost all the necessary things including: 
WEB Server, TCP-to-UART bridge with a lot of configurable options, device 
and application configuration through web interface.

I had to make some customizations of the original project in order to 
utilize reed switch and provide for Interactive Knitting. Customized 
sources are [here](https://github.com/ZaitsevY/esp8266web/tree/custom) 
and the sources of the application (Eclipse project) are right in this
very repository.
Again, to skip the time consuming learning trail of compiling firmware 
from sources, you might prefer to download a ready-to-flash binary files.

To upload firmware to your module you'll have to install [Python](https://www.python.org/downloads/) 
(choose 2.x version) with [pyserial](https://pythonhosted.org/pyserial/) 
(the simplest way is to issue `pip install pyserial` from command prompt 
after Python installation). [Esptool](https://github.com/themadinventor/esptool) 
is already included in the binary distribution. If you follow me and use 
WeMos D1 mini module, make sure you have installed [Ch340G driver](http://www.wemos.cc/wiki/doku.php?id=en:ch340g) 
(if appropriate for your OS). 

Connect your module to USB, detect the resulting COM-port number.
*Note:* COM4 is assumed for the example commands below.
*Note:* click RESET button of D1 mini just before you run `esptool.py … write_flash ...` command.

First, flash clear settings with the following from the command prompt:
```
esptool.py --port COM4 --baud 460800 write_flash -ff 80m -fm qio -fs 4m 0x79000 clear_eep.bin 0x7C000 esp_init_data_default.bin 0x7E000 blank.bin
```

Second, flash the software portion:
```
esptool.py --port COM4 --baud 460800 write_flash -ff 80m -fm qio -fs 4m 0x00000 .0x00000.bin 0x07000 .0x07000.bin
```

Press RESET on the module again (just to be on the save side), wait a
couple of seconds and now you can use any of your any of your WiFi-enabled 
devices (notebook, smartphone) to:

1. Connect to a WiFi access point named ESP8266 (by default);
2. Point you browser to http://192.168.4.1/fsupload (default username:password 
are ESP8266:0123456789) and upload WEBFiles.bin file;
3. Point you browser to http://192.168.4.1/ to continue device configuration.

### Application configuration:

1. WiFi Settings: set your home WiFi SSID and password, optionally switch off 
ESP8266 access point by selecting STATION_MODE. Mention that right after switching 
AP off you won't be able to access web interface of the module with 192.168.4.1, 
instead reconnect your notebook (or whatever) back to your home network and 
use http://sesp8266/ or, if the latter doesn't work for you, find out the new 
module IP address from your WiFi router. Or just leave WiFi mode setting 
as STATIONAP_MODE.<br>
![WiFi Settings](/Web_WiFi.png)
2. TCP-TO-UART Settings: set baud rate to 9600, enable RX and TX inversion.<br>
![TCP=UART Settings](/Web_TCP-UART.png)
3. System Setup: disable checking RX pin to reset WiFi configuration (*important!* 
When you power your device while it is connected to KH-9xx port this setting, 
if enabled, will reset your customizations from 1.).<br>
![System Settings](/Web_System.png)

Software
--------

DesignaKnit expects knitting machine to be connected through COM interface. 
To make it acquainted with your device you'd install a virtual COM-port driver. 
There are several solutions out there: [HW VSP](http://www.hw-group.com/products/hw_vsp/index_en.html), 
[VSPE](http://www.eterlogic.com/Products.VSPE.html), 
[USR-VCOM](http://www.usriot.com/Download/31.html); 
I didn't try them all so the following screen shots are all about HW VSP.

After installing HW VSP you'd create a virtual COM-port with the following settings:
First, at the Settings tab, click Login to acquire administrative access, 
then configure as on the following screen shot:<br>
![VSP Settings](/VSP_Settings.png)

Now go to the Virtual Serial Port tab to select your COM-port name; put “sesp8266” for 
the IP-address (or take the IP of your device from its web interface WiFi Settings 
page, IP from WiFi Station column); set port to 12345:<br>
![VSP Virtual Port](/VSP_Port.png)

### Finally it's time to DesignaKnit!

YouTube video pending....
?????????????? see http://stackoverflow.com/questions/11804820/embed-a-you-tube-video
[Wireless BrotherLink]()

Acknowlegement
--------------

- Great knitting machine interface analysis by Grakham Wideman at 
https://grahamwideman.wikispaces.com/Knitting+machine+interfaces
