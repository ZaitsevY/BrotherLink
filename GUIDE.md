# Wireless BrotherLink

## Package contents

- Wireless BrotherLink device
- Piece of double sided tape
- Magnet

## Power

The device is powered with 5V (USB-plug), you can use any modern smartphone wall charger.
Also any PC/notebook USB-port can be used (though not recommended).

## Knitting machine connection.

The device has a plug for connection to the knitting machine port.
There's a mark (a perforation) on top of the plug for proper orientation.

## WiFi configuration

Power the device on. After initialization (5..10 seconds) a new WiFi Access Point
named "ESP8266" by default is established. Now you can use any WiFi-capable device 
(notebook, tablet, smartphone) to:
- connect to ESP8622 network;
- point you browser to http://192.168.4.1
- revise or change device settings, default login and password are "ESP8266":"0123456789";

You might wish to configure device to connect to your home WiFi (if available).
Visit "WiFi Settings" page and under "WiFi Station" section fill "SSID" and "Password", 
check "AutoConnect" and press "Set Config". After a while (30..60 secods) reload
the page and make sure that "Status" has changed to "GOT_IP" indicating successful
connection of the device to your home WiFi network. Optionally you can now switch
ESP8266 Access Point off by selecting "WiFi Mode" STATION_MODE at "WiFi SoftAP" section.
Or you can leave this setting STATIONAP_MODE. 
After device is connected to your home WiFi, your notebook/smartphone/etc can now 
leave ESP8266 and return to your home WiFi. The address of the device in your
home network becomes "sesp8266" (http://sesp8266). If the latter doesn't work
for you, you'll have to get the device's IP-address from your WiFi router.
![WiFi Settings](/Web_WiFi.png)

## DesignaKnit configuration

DesignaKnit expects knitting machine to be connected through COM interface. 
To make it acquainted with your device you'd install a virtual COM-port driver. 
There are several solutions out there: [HW VSP](http://www.hw-group.com/products/hw_vsp/index_en.html), 
[VSPE](http://www.eterlogic.com/Products.VSPE.html), 
[USR-VCOM](http://www.usriot.com/Download/31.html); 
I didn't try them all so the following screen shots are all about HW VSP.

After installing HW VSP you'd create a virtual COM-port with the following settings:

First, at the Settings tab, click Login to acquire administrative access 
(default password is "admin"), then configure as on the following screen shot:<br>
![VSP Settings](/VSP_Settings.png)

Next, go to the Virtual Serial Port tab to select your COM-port name (COM3 in the example);
set “sesp8266” for the IP-address (or take the IP of your device from its web interface 
WiFi Settings page, IP from WiFi Station column); set port to 12345:<br>
![VSP Virtual Port](/VSP_Port.png)

It might take 10..30 seconds after you click "Create COM" until VSP creates and mounts the port.
After that it's high time to launch DesignaKnit and use it to download or upload patterns,
just don't forget to set COM-port number in the settings.

*Note:* if you power your device off, HW VSP looses connection to your device
and to enable it again you'll have to delete and create your virtual port again.
Looks like "Use NOP to Keep Connection" and "Renew Automatically" settings of
HW VSP might help but they are by some reason disabled.

## Interactive knitting

Put a magnet from the package contents (or any other one) onto the arm of the 
knitting machine carriage and fix it with scotch tape.
Fix the device on top of the machine (you may use the provided piece of double 
sided tape to fix it) so, that the arm with the magnet will pass the reed switch
at a distance of 3..5 mm. There's a led which will flash when the reed switch 
detects the magnet pass. Now you're ready for interactive mode.

![Magnet position](/WBL_Magnet.png)

*Note:* when knitting with patterns, the arm with a magnet should not be in
lower position because the magnet can interfere with the knitting machine
build-in sensors.
