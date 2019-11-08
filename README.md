# forumslader-enhancements
Some modifications I'm making to my Forumslader rectifier for bicycle dynamos

Upgrades to forumslader controller

# Ideas
- Blinkenlights
  - 12v light output should have a blink option
  - spec and buy MOSFET
  - write code to control mosfet from GPIO
  - see low power circuit (attached, load into script from https://www.falstad.com/circuit/circuitjs.html)
- CSCS GATT profile, to allow Strava, etc to receive high quality speed data via BT
- May be used to power (and control?) Electronic shifting project
- upgrade to marine grade cables & connectors
- Locus companion app for CSCS data ([API}(locus API info:https://github.com/asamm/locus-api/wiki/First-steps))

# BT Serial protocol info
- uses bluetooth uart protocol
- protocol info attached and [here](http://www.forumslader.de/fileadmin/user_upload/Individuelle Geraete/jensd/Download/FL-Ahead/4-fach/Manuals/Datensaetze-BT-Modul.pdf)
- [sample datagrams and device emulator](https://github.com/juergenalbert/ForumsladerSimulator/blob/master/app/src/main/java/de/example/forumsladersimulator/gattserver/GattServerActivity.java)
- What is this "forumslader companion" app from the same author? Does it decode? https://github.com/juergenalbert/ForumsladerCompanion
- other gatt protocols that could be useful:
  - BAS	Battery Service (provides single 0-100% data point)
  - CSCS	Cycling Speed and Cadence Service (exposes speed data from sensor)
  - ESS	Environmental Sensing Service (exposes pressure/temp data)

According to Sven:
> - we use the BT Modul HM-10 (cc2540/cc2541 chip) with the standard firmware (we have no own firmware)
> - the communication between the FL-Mainboard and the BT-Modul is serial with 57600Baud without Flow Control
> - the transmitted and received datas and commands are in ascii format
> - the dataformat is describe in the document in this email
> - all datas are messured by the main board
> - the BT modul only transmit / receive datas from the smartphone
> I found my [code](Forumslader_ESP32_BLE_UART.ino) approach for Espressif's ESP32 module to emulate the HM-10. I tested it a year ago and it worked. I hope that simplifies the implementation for you.
