//Forumslader basic serial code from Sven During
//BLE Battery code from https://circuitdigest.com/microcontroller-projects/esp32-ble-server-how-to-use-gatt-services-for-battery-level-indication

//TTGO T-Beam Board Defines
#define TXPIN      17
#define RXPIN      18
#define LED_BUILTIN 4 // T-Beam blue LED, see: http://tinymicros.com/wiki/TTGO_T-Beam

//Forumslader defines
//Connection point
HardwareSerial FLSerial(1);
#define FLSerialRX 4
#define FLSerialTX 23

//determined experimentally
//TODO are these set by any reported/configured values?
#define FLMVMIN 10347
#define FLMVMAX 12314

bool needFLP = true;
unsigned long FLLastFLP = 0;
#define FLREQMILLIS 3000 //don't request FLP/FLV more frequently than this

//NMEA message parsing
#include <TinyGPS++.h>
TinyGPSPlus FLParser;

//Format FL5
//FL->App; Clock 1s
TinyGPSCustom FLStatus(FLParser, "FL5", 1);      //Status- and Errorbits
TinyGPSCustom FLSwStage(FLParser, "FL5", 2);     //Switching stage (direction?)
TinyGPSCustom FLDynFreq(FLParser, "FL5", 3);     //Dynamo frequency Impulse/Second
TinyGPSCustom FLCell1(FLParser, "FL5", 4);       //Voltage cell1 in mV
TinyGPSCustom FLCell2(FLParser, "FL5", 5);       //Voltage Cell2 in mV
TinyGPSCustom FLCell3(FLParser, "FL5", 6);       //Voltage Cell3 in mV
TinyGPSCustom FLBattPower(FLParser, "FL5", 7);   //battery power in mA (+/-)
TinyGPSCustom FLConsPower(FLParser, "FL5", 8);   //consumer power in mA
TinyGPSCustom FLChargeTemp(FLParser, "FL5", 9);  //Charger temperature in K
TinyGPSCustom FLSwCons(FLParser, "FL5", 10);     //Switching state consumer 1-in, 2-out
TinyGPSCustom FLOffTime(FLParser, "FL5", 11);    //off time in Seconds
TinyGPSCustom FLMicrosteps(FLParser, "FL5", 12); //MicrostepCounter for route calculation Dynamoimpulse%4096
TinyGPSCustom FLImpulses(FLParser, "FL5", 13);   //ImpulsCounter for route calculation Dynamoimpulse%4096
TinyGPSCustom FLTotalTime(FLParser, "FL5", 14);  //TimeCounter Total minutes in Minutes
TinyGPSCustom FLtest(FLParser, "FL5", 15);  //TimeCounter Total minutes in Minutes

//Format FLB
//FL->App; Clock 2s
TinyGPSCustom FLTemp(FLParser, "FLB", 1); //Temperatur in 1/10 °C -> 24.0°C
TinyGPSCustom FLPressure(FLParser, "FLB", 2); //Pressure in Pa
TinyGPSCustom FLAlt(FLParser, "FLB", 3); //Alt in 1/10 Meter -> 73.5m
TinyGPSCustom FLSlope(FLParser, "FLB", 4); //Slope in 1/10 % -> 0.0%

//Format FLC
//FL->App; Clock 2s pro Satz → 6Sätze*2s = 12s
TinyGPSCustom FLSatz(FLParser, "FLC", 1); //Satz
TinyGPSCustom FLCP2(FLParser, "FLC", 2); //CP2
TinyGPSCustom FLCP3(FLParser, "FLC", 3); //CP3
TinyGPSCustom FLCP4(FLParser, "FLC", 4); //CP4
TinyGPSCustom FLCP5(FLParser, "FLC", 5); //CP5
TinyGPSCustom FLCP6(FLParser, "FLC", 6); //CP6

//Format FLV
//FL->App; on demand
TinyGPSCustom FLFirmware(FLParser, "FLV", 1); //Firmware Forumslader
TinyGPSCustom FLBTModule(FLParser, "FLV", 2); //Firmware BT Module

//Format FLP
//FL->App; on demand
TinyGPSCustom FLWheelsize(FLParser, "FLP", 1); //Wheel circumference in Millimeter
TinyGPSCustom FLPoles(FLParser, "FLP", 2); //Poles
TinyGPSCustom FLAltOffset(FLParser, "FLP", 3); //Altitude Offset in 1/10 Meter
TinyGPSCustom FLDayPulseOffset(FLParser, "FLP", 4); //DayPulseOffset
TinyGPSCustom FLDayTimeOffset(FLParser, "FLP", 5); //DayTimeOffset
TinyGPSCustom FLTourPulseOffset(FLParser, "FLP", 6); //TourPulseOffset
TinyGPSCustom FLTourTimeOffset(FLParser, "FLP", 7); //TourTimeOffset
TinyGPSCustom FLacc2mAhCoefficient(FLParser, "FLP", 8); //acc2mAhCoefficient
TinyGPSCustom FLPcrc(FLParser, "FLP", 9); //crc

//Format FLT
//App->FL
TinyGPSCustom FLTP1(FLParser, "FLT", 1); //number 1-11 corresponds to functions from table
TinyGPSCustom FLTP2(FLParser, "FLT", 2); //Set-value data
TinyGPSCustom FLTP3(FLParser, "FLT", 3); //Set-value data

//BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
BLEServer *MyBLEServer = NULL;
BLECharacteristic *FLCharacteristic;
BLECharacteristic *BatteryLevelCharacteristic;
bool BLEDeviceConnected = false;
bool oldDeviceConnected = false;

#define FL_SERVICE_UUID             "0000FFE0-0000-1000-8000-00805F9B34FB"
#define FL_RX_CHARACTERISTIC_UUID   "0000EF38-0000-1000-8000-00805F9B34FB"
#define FL_TX_CHARACTERISTIC_UUID   "0000EF38-0000-1000-8000-00805F9B34FB"
#define BLE_UUID_BATTERY_SERVICE    BLEUUID((uint16_t)0x180F)
#define BATTERY_CHARACTERISTIC_UUID BLEUUID((uint16_t)0x2A19)

#define   BLE_UUID_CYCLING_SPEED_AND_CADENCE   0x1816
#define   BLE_UUID_DEVICE_INFORMATION_SERVICE   0x180A

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* MyBLEServer) {
      BLEDeviceConnected = true;
    };

    void onDisconnect(BLEServer* MyBLEServer) {
      BLEDeviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *FLCharacteristic) {
      std::string rxValue = FLCharacteristic->getValue();


      if (rxValue.length() > 0) {
        Serial.print("********\t\t\t\t\t\t");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++) {
          FLSerial.print(rxValue[i]);

          Serial.print(rxValue[i]);
          if (FLParser.encode(rxValue[i])) {
            Serial.print("App -> FL Sentence: Function #");
            Serial.print(FLTP1.value());
            Serial.print(" with values ");
            Serial.print(FLTP2.value());
            Serial.print(", ");
            Serial.print(FLTP3.value());
          }
        }

        Serial.println();
      }


    }
};

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //Here we talk to the Forumslader
  Serial.begin(57600);
  FLSerial.begin(57600, SERIAL_8N1, 4, 23);

  // Create the BLE Device
  BLEDevice::init("FL_BLE_ESP32"); // Give it a name

  // Create the BLE Server
  MyBLEServer = BLEDevice::createServer();
  MyBLEServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Services
  BLEService *FLService = MyBLEServer->createService(FL_SERVICE_UUID);
  BLEService *battService = MyBLEServer->createService(BLE_UUID_BATTERY_SERVICE);
  BLEService *cycleService = MyBLEServer->createService(BLE_UUID_CYCLING_SPEED_AND_CADENCE);

  // Create BLE Characteristics
  FLCharacteristic = FLService->createCharacteristic(
                       FL_TX_CHARACTERISTIC_UUID,
                       BLECharacteristic::PROPERTY_NOTIFY |
                       BLECharacteristic::PROPERTY_WRITE
                     );

  BatteryLevelCharacteristic = battService->createCharacteristic(
                                 BATTERY_CHARACTERISTIC_UUID,
                                 BLECharacteristic::PROPERTY_READ |
                                 BLECharacteristic::PROPERTY_NOTIFY
                               );


  //cycling has characteristics:
  //* CSC Measurement (notify) 0x2A5B
  //  - Descriptor name="Client Characteristic Configuration" (read, write) 0x2902
  //* CSC Feature (notify) 0x2A5C
  //Sensor Location (read) 0x2A5D
  //SC Control Point (write, indicate) 0x2A55
  //  - Descriptor name="Client Characteristic Configuration" (read, write) 0x2902
  

  BLEDescriptor FLDescriptor(FL_TX_CHARACTERISTIC_UUID);
  FLDescriptor.setValue("Forumslader serial data");
  FLCharacteristic->addDescriptor(&FLDescriptor);
  BLE2902* p2902Descriptor = new BLE2902();
  p2902Descriptor->setNotifications(true);
  FLCharacteristic->addDescriptor(p2902Descriptor);
  FLCharacteristic->setCallbacks(new MyCallbacks());

  BLEDescriptor BatteryLevelDescriptor(BATTERY_CHARACTERISTIC_UUID);
  BatteryLevelDescriptor.setValue("Percentage 0 - 100");
  BatteryLevelCharacteristic->addDescriptor(&BatteryLevelDescriptor);
  BatteryLevelCharacteristic->addDescriptor(new BLE2902());

  // Start services
  FLService->start();
  battService->start();

  // Start advertising
  MyBLEServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

//const String myString = "$FL5,00C000,0,0,4000,4000,4000,-12,0,296,1,234,1217,3282,8873;\r\n"; //debug

void loop() {

  //the light show is unnecessary, as my board already has an LED attached to the serial line
  //This is here because other boards don't have that feature.
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  String myString = FLSerial.readStringUntil('\n');// default timeout is 1 sec
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

  if (myString.length()) {

    myString += '\n'; //add back what was removed by readStringUntil

    char thismsg = myString.charAt(myString.indexOf(',') - 1);
    Serial.print("Received sentence from FLSerial of type ");
    Serial.println(thismsg);

    if (thismsg != '5' && needFLP) {
      //this request is slightly synchronous.
      //the FL considers it impolite to interrupt and will ignore my request otherwise
      
      if (millis() > FLLastFLP + FLREQMILLIS) {
        Serial.println("Requesting FLP sentence ($FLT,5*47)");

        FLSerial.print("$FLT,5*47\r\n"); //request FLP page

        FLLastFLP = millis();
      }
    } else if (thismsg = 'P') {
      needFLP = false;
    }

    //forward serial data to BLE in 20-char chunks
    for (int i = 0; i < myString.length(); i += 20) {
      char txString[21];
      myString.substring(i, i + 20).toCharArray(txString, 21);
      FLCharacteristic->setValue(txString);
      if (BLEDeviceConnected) { FLCharacteristic->notify(); delay(10); }
      //Serial.println(txString); //debug
    }
    if (BLEDeviceConnected) Serial.println("Forwarded sentence from FL -> BLE device");

    //feed sentence to parser one character at a time
    for (int i = 0; i < myString.length(); i++) {
      if (FLParser.encode(myString.charAt(i))) {
        Serial.println("Parsed sentence."); //debug

        //update BLE battery
        if (FLCell1.isUpdated() || FLCell2.isUpdated() || FLCell3.isUpdated()) {

          uint8_t level = map(
                            (atoi(FLCell1.value()) + atoi(FLCell2.value()) + atoi(FLCell3.value())),
                            FLMVMIN, FLMVMAX, 0, 100);

          BatteryLevelCharacteristic->setValue(&level, 1);

          if (BLEDeviceConnected) {
            BatteryLevelCharacteristic->notify();
            delay(10);
          }

          //          Serial.print("Updated BLE Battery Level: ");
          //          Serial.println(level);
        }

        //update speed
        if (FLDynFreq.isUpdated() || FLPoles.isUpdated() || FLWheelsize.isUpdated()) {

          float FLSpeed =
            atof(FLDynFreq.value()) / atof(FLPoles.value()) * atof(FLWheelsize.value()) / 277.777;

          if (isnan(FLSpeed)) {
            needFLP = true;
          } else {
            //CycleSpeedCharacteristic->setValue(&FLSpeed, 1);
            //if (BLEDeviceConnected) { CycleSpeedCharacteristic->notify(); delay(10); }

            Serial.print("Updated BLE Cycle Speed: ");
            Serial.print(FLDynFreq.value());
            Serial.print(" / ");
            Serial.print(FLPoles.value());
            Serial.print(" * ");
            Serial.print(FLWheelsize.value());
            Serial.print(" / 277.777 = ");
            Serial.println(FLSpeed);
          }
        }
      }
    }

    Serial.println();

  } else {
    Serial.println("No data received this second.");
  }


  // disconnecting
  if (!BLEDeviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    MyBLEServer->startAdvertising(); // restart advertising
    Serial.println("Device disconnected. Advertising...");
    oldDeviceConnected = BLEDeviceConnected;
  }
  // connecting
  if (BLEDeviceConnected && !oldDeviceConnected) {
    Serial.println("Device connected!");
    oldDeviceConnected = BLEDeviceConnected;
  }
}
