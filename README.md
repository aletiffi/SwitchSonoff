**Sonoff management as a switch**
=======================
By installing this firmware you can control a Sonoff as a relay both from the wall switch and from your smartphone. This Sonoff can communicate via MQTT protocol with a broker like mosquitto, and in stand-alone mode by accessing the device homepage. Once connected to your wifi network it will be accessible in localhost or at 192.168.4.1 by connecting to its wifi network.

<p align="center">
  <img src="https://github.com/aletiffi/Switch_Sonoff/blob/main/img/HomePage.PNG" alt="Settings"/>
</p>

**Wirings**
-----------------------------------------
In this project a Sonoff r1 and a wall switch are used to power a light bulb or a wall socket.

<p align="center">
  <img src="https://github.com/aletiffi/Switch_Sonoff/blob/main/img/Schema.png" alt="Settings"/>
</p>

**Setup**
-----------------------------------------
Once the firmware has been loaded, the EEPROM is deleted at the first start, and a default name is assigned to the device.

Once connected to the wifi network named **SonoffSwitch** (default name) open the browser at 192.168.4.1. Rename the device then enter the name of the wifi network and the password to which it must connect. The other settings can be entered later. By pressing the `Save Settings` button at the bottom of the page the Sonoff restarts, then tries to connect to your network.

<p align="center">
  <img src="https://github.com/aletiffi/Switch_Sonoff/blob/main/img/Settings.PNG" alt="Settings"/>
</p>

The integrated led indicates successful connection to the wifi network.
If the LED does not light up, the network signal may be too weak, to restart the connection process press and hold the button until the LED flashes 2 times.

Now the device is connected to the local network, and it is possible to enter the MQTT parameters through the web page reachable in localhost, by entering `HostName.local` in the browser.

If the OTA Drive ProductKey is present, it is possible to manage the updating of future firmware with this platform without having to flash the device by hand.

**Integrated button functionality**
-----------------------------------------
While holding the Sonoff button, the LED will flash once per second. Based on the number of flashes reached at release, one of the following functions will be activated.

- 1:  Toggle
- 2:  Connection check
- 3:  Check FW update
- 4:  Show ip address (number of flashes equal to the last IP number)
- 5:  Wifi signal power
- 6:  Load settings (for debug purpose)
- 7:  Save settings (for debug purpose)
- 8:  EEPROM clean
- 9:  Read all EEPROM (for debug purpose)
- 10: Sonoff restart

**Info**
-----------------------------------------
Within this repository there is the .bin file to be installed in the Sonoff. If you want to recompile or modify the code you have to download the DigiOut, ReadInput and StoreStrings libraries that you find in my repositories.