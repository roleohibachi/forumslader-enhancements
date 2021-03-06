# forumslader-enhancements
Some modifications I'm making to my Forumslader rectifier for bicycle dynamos

Upgrades to forumslader controller

# Ideas
## Blinkenlights
- 12v light output should have a blink option
- spec and buy mosfet
- write code to control mosfet from PWM or interrupt timers
- Consider including large capacitor to FL/battery load and prevent reboots
- ~~[low power circuit](https://www.falstad.com/circuit/circuitjs.html?cct=$+1+0.000005+0.08824969025845955+57+5+50%0AI+192+208+272+208+0+0.5+5%0AI+304+208+384+208+0+0.5+5%0Aw+272+208+304+208+0%0Ac+272+128+272+176+0+6.000000000000001e-7+1.9832537249707896%0Ad+400+80+336+80+2+default%0A162+416+256+416+304+2+default-led+1+0+0+0.01%0Ar+416+208+416+256+0+2200%0Ar+336+80+288+80+0+1000%0Ar+368+112+320+112+0+1000%0AR+80+96+80+64+0+0+40+6+0+0+0.5%0Ag+80+288+80+304+0%0Af+128+256+192+256+32+1.5+0.02%0Aw+192+208+192+240+0%0Aw+192+272+192+288+0%0Aw+80+288+192+288+0%0Aw+80+208+80+288+0%0A209+80+160+80+208+0+0.00009999999999999999+6+1%0Aw+80+96+80+160+0%0Ar+192+112+192+160+0+1000%0Aw+80+96+192+96+0%0Aw+192+160+192+208+0%0Aw+192+112+192+96+0%0Aw+272+176+272+208+0%0Aw+288+80+272+80+0%0Aw+272+80+272+128+0%0Aw+400+80+400+112+0%0Aw+368+112+400+112+0%0Aw+400+112+400+208+0%0Aw+384+208+400+208+0%0Aw+400+208+416+208+0%0Aw+416+304+192+304+0%0Aw+192+288+192+304+0%0Aw+128+112+128+256+0%0Aw+272+128+320+112+0%0Aw+272+128+128+112+0%0Ao+5+2+0+4099+2.5+0.0015625+0+2+5+3%0Ao+3+4+0+4099+5+0.05+1+2+3+3%0A)~~ Not needed, ESP will control w/ GPIO

## Misc
- May be used to power (and control?) Electronic shifting project
- upgrade to marine grade cables & connectors
- 3g modem, LoRa radio, or APRS radio
- GPS

## BLE GATT
- [Primer](https://www.oreilly.com/library/view/getting-started-with/9781491900550/ch04.html)
- [All services](https://www.bluetooth.com/specifications/gatt/services/)
- [All characteristics](https://www.bluetooth.com/specifications/gatt/characteristics/)
- [all UUIDs](https://gist.github.com/sam016/4abe921b5a9ee27f67b3686910293026)
- [CSCS and CP Walkthrough](https://www.bluetooth.com/blog/part-2-the-wheels-on-the-bike-are-bluetooth-smart-bluetooth-smart-bluetooth-smart/)
- Battery Service, to report battery level via native android/ios implementation.
  - actually three characteristics
  - Battery level (percent available)
  - Battery power state (chargeable or not; charging or discharging)
  - Battery level state, a wrapper for the other two?
  - [demo](https://circuitdigest.com/microcontroller-projects/esp32-ble-server-how-to-use-gatt-services-for-battery-level-indication) alongside serial comms
- Cycling Speed and Cadence service; or just speed characteristic, to allow Strava, etc to receive high quality speed data
- Environmental Sensing Service (ESS); or just temp and pressure characteristics; because why not! We have the sensors already
- Locus companion app for CSCS data ([locus API info](https://github.com/asamm/locus-api/wiki/First-steps))

# BT Serial protocol info
- uses bluetooth uart protocol
- protocol info [attached](FLV5_Protocol_En.md) and [here](http://www.forumslader.de/fileadmin/user_upload/Individuelle%20Geraete/jensd/Download/FL-Ahead/4-fach/Manuals/Datensaetze-BT-Modul.pdf)
- [sample datagrams and device emulator](https://github.com/juergenalbert/ForumsladerSimulator/blob/master/app/src/main/java/de/example/forumsladersimulator/gattserver/GattServerActivity.java)
- What is this "forumslader companion" app from the same author? Does it decode? https://github.com/juergenalbert/ForumsladerCompanion
- other gatt protocols that could be useful:
  - BAS	Battery Service (provides single 0-100% data point)
  - CSCS	Cycling Speed and Cadence Service (exposes speed data from sensor)
  - ESS	Environmental Sensing Service (exposes pressure/temp data) [demo](https://www.hackster.io/ManniAT/esp32-ble-environmental-gatt-server-14cceb)

According to Sven:
> - we use the BT Modul HM-10 (cc2540/cc2541 chip) with the standard firmware (we have no own firmware)
> - the communication between the FL-Mainboard and the BT-Modul is serial with 57600Baud without Flow Control
> - the transmitted and received datas and commands are in ascii format
> - the dataformat is describe in the document in this email
> - all datas are messured by the main board
> - the BT modul only transmit / receive datas from the smartphone
> 
> I found my [code](Forumslader_ESP32_BLE_UART.ino) approach for Espressif's ESP32 module to emulate the HM-10. I tested it a year ago and it worked. I hope that simplifies the implementation for you.

# 6 Jan 2020
Opened up the case. Looks like it survived the last 2000 miles of weather and vibration just fine. Clipped of the old bluetooth module, stripped the connector wires... and found out my soldering iron isn't working. Putting this on pause. When I come back, next step will be to write a library for parsing the serial data (once I can solder the leads to an arduino-or-something).

# 7 Jan 2020
Fixed soldering iron (bad power cable). Soldered leads. BT pinout corresponds to [this](https://components101.com/sites/default/files/component_pin/HM10-Pinout.png), mounted with the antenna toward the FS. Be careful, I think I got a twist in the wires or possibly put black to power and red to ground or something. Just verify the pins carefully. Be aware it's a 3.3v VCC. I/O with arduino via USB will require a level shifter (ordered 1/8, delivery expected 1/11)

# 25 Jan 2020 or so
Connected level shifter, works great. Using arduino nano (5v from usb) on the HV side, 3.3v from FS on the LV side. SoftwareSerial works to receive data at 57600 baud. Haven't tried sending yet. Next step: use Serial techniques from weather station build to make a parsing library.

# 11 Jan 2021
Somebody did this for the peloton! https://ihaque.org/posts/2021/01/04/pelomon-part-iv-software/
