ForumsladerV5 – Übertragungsprotokoll

For the forum loader from version 5, a Bluetooth module "piggyback" on the
Charger board to be mounted. Compared to the previous versions with deposed
BT module results in a much more compact design.

By using a specialized loading controller in the forum loader V5 stand
It also provides advanced charging management capabilities. Consequently
can use a revised transmission protocol significantly more parameters
Compared to the previous versions on the loading and unloading behavior and the
Battery status be transferred. The functionality of the integrated
Bicycle computers has also been further developed and improved in some details.
Dieser sammelt, berechnet und speichert wie bisher autark alle relevanten Tourdaten.

All information is packed in records in ASCII format and
wirelessly with the BT Version V2.1 via the protocol SPP (Serial Port Profile) to the
"Forum loader app" for Android subWithted.

In preparation for a transmission via BT Version V4.0 via BLE are the
Data sets of the forum loader are provided with additional checksums. These will
formed according to the standard for NMEA records.

(checksum calculator z.B.: www.hhhh.org/wiml/proj/nmeaxor.html )

Stand: 16.03.2016 Sven During © 2013-2016

Format FL5

(FL->App; Clock 1s)

| $FL5 |                                           |          |                       |
| ---- | ----------------------------------------- | -------- | --------------------- |
| P1   | Status- and Errorbits                     | "00C000" | Status- and Errorbits |
| P2   | Switching stage                           | "0"      |                       |
| P3   | Dynamo frequency                          | "0"      | Impulse/Second        |
| P4   | Voltage cell1                             | "3788"   | in mV                 |
| P5   | Voltage Cell2                             | "3789"   | in mV                 |
| P6   | Voltage Cell3                             | "3688"   | in mV                 |
| P7   | battery power                             | "-12"    | in mA (+/-)           |
| P8   | consumer power                            | "0"      | in mA                 |
| P9   | Charger temperature                       | "296"    | in K                  |
| P10  | Switching state consumer                  | "1"      | 1-in, 2-out           |
| P11  | off time                                  | "234"    | in Seconds            |
| P12  | MicrostepCounter → for route calculation  | "1217"   | Dynamoimpulse%4096    |
| P13  | ImpulsCounter → for route calculation     | "3282"   | Dynamoimpulse/4096    |
| P14  | TimeCounter → Total minutes               | "8873"   | in Minutes            |

Example:

Without CRC: `$FL5,00C000,0,0,3788,3789,3688,-12,0,296,1,234,1217,3282,8873;CRLF`

With CRC: `$FL5,00C000,0,0,3788,3789,3688,-12,0,296,1,234,1217,3282,8873*65CRLF`

Selected Details:

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

  //calculate speed

  Speed = `(FL5.P3/POLES*WHEELSIZE)/277.777;`

FL5.P13

  //calculate total kilometers

  `double factor =0.004096*(WHEELSIZE/POLES);`
  `DISTTOTAL = FL5.P13 * factor + DISTTOTALOFFSET;`

Format FLB

(FL->App; Clock 2s)

| $FLB |            |           |                        |
| ---- | ---------- | --------- | ---------------------- |
| P1   | Temperatur | "240"     | in 1/10 °C -> 24.0°C   |
| P2   | Luftdruck  | "1022772" | in Pa                  |
| P3   | Höhe       | "735"     | in 1/10 Meter -> 73.5m |
| P4   | Steigung   | "0"       | in 1/10 % -> 0.0%      |

Example:

Without CRC: `$FLB,240,102272,735,0;CRLF`

With CRC: `$FLB,240,102272,735,0*7BCRLF`

Format FLC

(FL->App; Clock 2s pro Satz → 6Sätze*2s = 12s)


| $FLC |       |                      |                     |                      |                 |                |                          |
| ---- | ----- | -------------------- | ------------------- | -------------------- | --------------- | -------------- | ------------------------ |
| P1   | Satz  | 0                    | 1                   | 2                    | 3               | 4              | 5 (from Version FL5)     |
| P2   | Value | Tour altitude Total  | Day altitude Total  | altitude Total       | all Energy      | Day Speed avg  | Startcounter             |
| P3   | Value | Tour slope Max       | Day pitch Max       | Tour pitch Min       | Tour Energy     | Tour Speed avg | Soc_State (%)            |
| P4   | Value | Tour Temperature Max | Day Temperature Max | Tour Temperature Min | Day Energy      | Day Climb avg  | fullChargeCapacity (mAh) |
| P5   | Value | Tour height Max      | Day height Max      | Day pitch Min        | BT Save Counter | Tour Climb avg | CycleCount               |
| P6   | Value | Tour Pulse Max       | Day Pulse Max       | Day Temperatur Min   | Empty           | Empty          | AccumulatedCCADCvalue    |

Example:

Without CRC: `$FLC,5,826,55,1386,31,33155403;CRLF`

With CRC: `$FLC,5,826,55,1386,31,33155403*48CRLF`

  |          |                          |
  | -------- | ------------------------ |
  | 5        | 5.Satz                   |
  | 826      | Startcounter             |
  | 55       | Soc_State (%)            |
  | 1386     | fullChargeCapacity (mAh) |
  | 31       | CycleCount               |
  | 33155403 | AccumulatedCCADCvalue    |

Format FLV

(FL->App; on demand)

| $FLV |                      |
| ---- | -------------------- |
| P1   | Firmware Forumslader |
| P2   | Firmware BT Module   |

Example:

Without CRC: `$FLV,500261115,5.51;CRLF`

With CRC: `$FLV,500261115,5.51*76CRLF`

Format FLP

(FL->App; on demand)

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

Example:

Without CRC: `$FLP,2199,14,1520,3282,8873,0,0,386,220;CRLF`

With CRC: `$FLP,2199,14,1520,3282,8873,0,0,386,220*74CRLF`

FLT Format

(App->FL)

| $FLT | P1     | P2        | P3      | Description                          |
| ---- | ------ | --------- | ------- | ------------------------------------ |
|      | ~~1~~  | ~~crc~~   |         | ~~FL Display Menu~~                  |
|      | 2      |           |         | Consumer On / Off ~~FL Display +~~   |
|      | ~~3~~  |           |         | ~~FL Display -~~                     |
|      | 4      |           |         | Return FLV record                    |
|      | 5      |           |         | Return FLP record                    |
|      | 6      |           |         | Tour reset                           |
|      | 7      |           |         | Day reset                            |
|      | ~~8~~  | ~~crc~~   |         | ~~Secure change on/off~~             |
|      | 9      | Wheelsize | Poles   | Set Values in the BT Module          |
|      | 10     | Value     |         | Altitude offset                      |
|      | ~~11~~ | ~~Value~~ | ~~crc~~ | ~~FL menu level~~                    |

Example:

Without CRC: `$FLT,9,2200,14;LF`

With CRC: `$FLT,9,2200,14*4ELF`

Example format result
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
