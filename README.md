# forumslader-enhancements
Some modifications I'm making to my Forumslader rectifier for bicycle dynamos

Upgrades to forumslader controller

# Ideas
- Blinkenlights
  - 12v light output should have a blink option
  - spec and buy MOSFET
  - write code to control mosfet from GPIO
  - see low power circuit ([attached](circuit-20191025-1530.circuitjs.txt), or just [click here](https://www.falstad.com/circuit/circuitjs.html?cct=$+1+0.000005+10.20027730826997+50+5+43%0Ar+176+80+384+80+0+10%0As+384+80+448+80+0+1+false%0Aw+176+80+176+352+0%0Ac+384+352+176+352+0+0.000015+7.517862267821689%0Al+384+80+384+352+0+1+0.012934119753820842%0Av+448+352+448+80+0+0+40+5+0+0+0.5%0Ar+384+352+448+352+0+100%0Ao+4+64+0+4099+20+0.05+0+2+4+3%0Ao+3+64+0+4099+20+0.05+1+2+3+3%0Ao+0+64+0+4099+0.625+0.05+2+2+0+3%0A38+3+0+0.000001+0.000101+Capacitance%0A38+4+0+0.01+1.01+Inductance%0A38+0+0+1+101+Resistance%0Ah+1+4+3%0A)
- CSCS GATT profile, to allow Strava, etc to receive high quality speed data via BT
- May be used to power (and control?) Electronic shifting project
- upgrade to marine grade cables & connectors
- Locus companion app for CSCS data ([API}(locus API info:https://github.com/asamm/locus-api/wiki/First-steps))

# BT Serial protocol info
- uses bluetooth uart protocol
- protocol info [attached](FLV5_Protocol_En.md) and [here](http://www.forumslader.de/fileadmin/user_upload/Individuelle%20Geraete/jensd/Download/FL-Ahead/4-fach/Manuals/Datensaetze-BT-Modul.pdf)
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
> 
> I found my [code](Forumslader_ESP32_BLE_UART.ino) approach for Espressif's ESP32 module to emulate the HM-10. I tested it a year ago and it worked. I hope that simplifies the implementation for you.
