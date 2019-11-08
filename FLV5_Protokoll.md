ForumsladerV5 – Übertragungsprotokoll
Für den Forumslader ab der Version 5 kann ein Bluetoothmodul „huckepack“ auf der
Laderplatine montiert werden. Gegenüber den vorherigen Versionen mit abgesetztem
BT-Modul ergibt sich ein wesentlich kompakterer Aufbau.

Durch den Einsatz eines spezialisierten Ladecontrollers im Forumslader V5 stehen
außerdem erweiterte Möglichkeiten des Lademanagements zur Verfügung. Somit
können durch ein überarbeitetes Übertragungsprotokoll wesentlich mehr Parameter
gegenüber den vorherigen Versionen über das Lade- und Entladeverhalten sowie den
Akkuzustand übertragen werden. Die Funktionalität des integrierten
Fahrradcomputers ist auch in einigen Details weiterentwickelt und verbessert worden.

Dieser sammelt, berechnet und speichert wie bisher autark alle relevanten Tourdaten.

Sämtliche Informationen werden in Datensätze im ASCII Format verpackt und
drahtlos mit der BT Version V2.1 über das Protokoll SPP (Serial Port Profile) an die
„Forumslader App“ für Android übermittelt.

In Vorbereitung einer Übertragung mittels BT Version V4.0 über BLE sind die
Datensätze des Forumsladers mit zusätzlichen Prüfsummen versehen. Diese werden
nach dem Standard für NMEA Datensätze gebildet.

(checksum calculator z.B.: www.hhhh.org/wiml/proj/nmeaxor.html )

Stand: 16.03.2016 Sven During © 2013-2016

Datensatz FL5

(FL->App; Takt 1s)

| $FL5 |                                           |          |                       |
| ---- | ----------------------------------------- | -------- | --------------------- |
| P1   | Status- und Errorbits                     | "00C000" | Status- und Errorbits |
| P2   | Schaltstufe                               | "0"      |                       |
| P3   | Dynamofrequenz                            | "0"      | Impulse/Sekunde       |
| P4   | Spannung Zelle1                           | "3788"   | in mV                 |
| P5   | Spannung Zelle2                           | "3789"   | in mV                 |
| P6   | Spannung Zelle3                           | "3688"   | in mV                 |
| P7   | Akkustrom                                 | "-12"    | in mA (+/-)           |
| P8   | Verbraucherstrom                          | "0"      | in mA                 |
| P9   | Lader Temperatur                          | "296"    | in K                  |
| P10  | Schaltzustand Verbraucher                 | "1"      | 1-an, 2-aus           |
| P11  | Ausschaltzeit                             | "234"    | in Sekunden           |
| P12  | MicrostepCounter → zur Streckenberechnung | "1217"   | Dynamoimpulse%4096    |
| P13  | ImpulsCounter → zur Streckenberechnung    | "3282"   | Dynamoimpulse/4096    |
| P14  | TimeCounter → Gesamtfahrminuten           | "8873"   | in Minuten            |

Beispiel:

ohne CRC: `$FL5,00C000,0,0,3788,3789,3688,-12,0,296,1,234,1217,3282,8873;CRLF`

mit CRC: `$FL5,00C000,0,0,3788,3789,3688,-12,0,296,1,234,1217,3282,8873*65CRLF`

Ausgewählte Details:

FL5.P1 → 24bit

|    |                            |
| -- | -------------------------- |
| 0  | cell1 ballance             |
| 1  | cell2 ballance             |
| 2  | cell3 ballance             |
| 3  | shortCircuit               |
| 4  | dischargingOvercurrent     |
| 5  | chargingOvercurrent        |
| 6  | dischargingHighcurrent     |
| 7  | chargingHighcurrent        |
| 8  | overloadpowerreduce        |
| 9  | overload                   |
| 10 | inDUVR                     |
| 11 | chargingProhibited         |
| 12 | dischargingProhibited      |
| 13 | fullDisChargedMarker       |
| 14 | remainingCapacityAccurate  |
| 15 | discharge                  |
| 16 | criticalConditionDetected  |
| 17 | cellTemperatureTooLow      |
| 18 | cellTemperatureTooHigh     |
| 19 | voltageTooHigh             |
| 20 | voltageTooLow              |
| 21 | reoccuringChargeProtection |
| 22 | checksumFailure            |
| 23 | systemInterrupt            |

FL5.P3

  //Geschwindigkeit berechnen

  Speed = `(FL5.P3/POLES*WHEELSIZE)/277.777;`

FL5.P13

  //Gesamtkilometer berechnen

  `double factor =0.004096*(WHEELSIZE/POLES);`
  `DISTTOTAL = FL5.P13 * factor + DISTTOTALOFFSET;`

Datensatz FLB

(FL->App; Takt 2s)

| $FLB |            |           |                        |
| ---- | ---------- | --------- | ---------------------- |
| P1   | Temperatur | "240"     | in 1/10 °C -> 24.0°C   |
| P2   | Luftdruck  | "1022772" | in Pa                  |
| P3   | Höhe       | "735"     | in 1/10 Meter -> 73.5m |
| P4   | Steigung   | "0"       | in 1/10 % -> 0.0%      |

Beispiel:

ohne CRC: `$FLB,240,102272,735,0;CRLF`

mit CRC: `$FLB,240,102272,735,0*7BCRLF`

Datensatz FLC

(FL->App; Takt 2s pro Satz → 6Sätze*2s = 12s)


| $FLC |       |                               |                      |                     |                 |                |                          |
| ---- | ----- | ----------------------------- | -------------------- | ------------------- | --------------- | -------------- | ------------------------ |
| P1   | Satz  | 0                             | 1                    | 2                   | 3               | 4              | 5 (ab Version FL5)       |
| P2   | Value | Tour Höhenmeter Total         | Day Höhenmeter Total | Höhenmeter Total    | all Energy      | Day Speed avg  | Startcounter             |
| P3   | Value | Tour Steigung Max             | Day Steigung Max     | Tour Steigung Min   | Tour Energy     | Tour Speed avg | Soc_State (%)            |
| P4   | Value | Tour Temperatur Max           | Day Temperatur Max   | Tour Temperatur Min | Day Energy      | Day Climb avg  | fullChargeCapacity (mAh) |
| P5   | Value | Tour Höhe Max                 | Day Höhe Max         | Day Steigung Min    | BT Save Counter | Tour Climb avg | CycleCount               |
| P6   | Value | Tour Pulse Max                | Day Pulse Max        | Day Temperatur Min  | Leer            | Leer           | AccumulatedCCADCvalue    |

Beispiel:

ohne CRC: `$FLC,5,826,55,1386,31,33155403;CRLF`

mit CRC: `$FLC,5,826,55,1386,31,33155403*48CRLF`

  |          |                          |
  | -------- | ------------------------ |
  | 5        | 5.Satz                   |
  | 826      | Startcounter             |
  | 55       | Soc_State (%)            |
  | 1386     | fullChargeCapacity (mAh) |
  | 31       | CycleCount               |
  | 33155403 | AccumulatedCCADCvalue    |

Datensatz FLV

(FL->App; auf Anfrage)

| $FLV |                      |
| ---- | -------------------- |
| P1   | Firmware Forumslader |
| P2   | Firmware BT Modul    |

Beispiel:

ohne CRC: `$FLV,500261115,5.51;CRLF`

mit CRC: `$FLV,500261115,5.51*76CRLF`

Datensatz FLP

(FL->App; auf Anfrage)

| $FLP | return values to App for verification |
| ---- | ------------------------------------- |
| P1   | Wheelsize in Millimeter               |
| P2   | Poles                                 |
| P3   | Altitude Offset in 1/10 Meter         |
| P4   | DayPulseOffset                        |
| P5   | DayTimeOffset                         |
| P6   | TourPulseOffset                       |
| P7   | TourTimeOffset                        |
| P8   | acc2mAhCoefficient                    |
| P9   | crc                                   |

Beispiel:

ohne CRC: `$FLP,2199,14,1520,3282,8873,0,0,386,220;CRLF`

mit CRC: `$FLP,2199,14,1520,3282,8873,0,0,386,220*74CRLF`

Datensatz FLT

(App->FL)

| $FLT | P1     | P2        | P3      | Description                          |
| ---- | ------ | --------- | ------- | ------------------------------------ |
|      | ~~1~~  | ~~crc~~   |         | ~~FL Display Menü~~                  |
|      | 2      |           |         | Verbraucher Ein/Aus ~~FL Display +~~ |
|      | ~~3~~  |           |         | ~~FL Display -~~                     |
|      | 4      |           |         | Return FLV Datensatz                 |
|      | 5      |           |         | Return FLP Datensatz                 |
|      | 6      |           |         | Tour reset                           |
|      | 7      |           |         | Day reset                            |
|      | ~~8~~  | ~~crc~~   |         | ~~Secure change on/off~~             |
|      | 9      | Wheelsize | Poles   | Set Values in the BT Modul           |
|      | 10     | Value     |         | Altitude offset                      |
|      | ~~11~~ | ~~Value~~ | ~~crc~~ | ~~FL Menüebene~~                     |

Beispiel:

ohne CRC: `$FLT,9,2200,14;LF`

mit CRC: `$FLT,9,2200,14*4ELF`

Beispiel Datensatzfolge
```
$FL5,00C000,0,0,3789,3788,3688,-11,0,296,1,231,1217,3282,8873*63
$FLB,240,102272,735,0*7B
$FL5,00C000,0,0,3789,3789,3688,-12,0,296,1,230,1217,3282,8873*60
$FLC,2,176240,-207,0,0,210*56
$FL5,00C000,0,0,3788,3789,3689,-12,0,296,1,229,1217,3282,8873*68
$FLB,240,102275,733,0*7A
$FL5,00C000,0,0,3788,3789,3688,-8,0,296,1,228,1217,3282,8873*53
$FLC,3,3545,3545,0,484,0*42
$FL5,00C000,0,0,3789,3789,3688,-9,0,296,1,227,1217,3282,8873*5C
$FLB,240,102269,737,0*73
$FL5,00C000,0,0,3788,3789,3688,-8,0,296,1,226,1217,3282,8873*5D
$FLC,4,0,200,0,16,0*78
$FL5,00C000,0,0,3789,3788,3688,-7,0,296,1,225,1217,3282,8873*51
$FLB,240,102280,728,0*7A
$FL5,00C000,0,0,3788,3789,3688,-8,0,296,1,224,1217,3282,8873*5F
$FLC,5,826,55,1386,31,33156710*4A
```
