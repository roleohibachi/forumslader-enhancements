/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

    Adapted for Forumslader (www.forumslader.de) as an alternative BLE Modul by Sven During
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

HardwareSerial FL(2);

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;
const int readPin = 32; // Use GPIO number. See ESP32 board pinouts
const int LED = 2; // Could be different depending on the dev board. I used the DOIT ESP32 dev board.

//std::string rxValue; // Could also make this a global var to access it in loop()

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "0000FFE0-0000-1000-8000-00805F9B34FB" // UART service UUID
#define CHARACTERISTIC_UUID_RX "0000EF38-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID_TX "0000EF38-0000-1000-8000-00805F9B34FB"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      
      Serial.println("OnWrite");
      if (rxValue.length() > 0) {
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
          FL.print(rxValue[i]);
        }

      }
      
    }
/*
    void onRead(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      Serial.println("OnRead");
   }
*/   
};

void setup() {
  Serial.begin(115200);
  FL.begin(57600);

  // Create the BLE Device
  BLEDevice::init("FL_BLE_ESP32"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                     // BLECharacteristic::PROPERTY_READ  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                     // BLECharacteristic::PROPERTY_INDICATE |
                      BLECharacteristic::PROPERTY_WRITE
                     
                    );

  // Create a BLE Descriptor
  BLE2902* p2902Descriptor = new BLE2902(); p2902Descriptor->setNotifications(true);
  pCharacteristic->addDescriptor(p2902Descriptor);
  //pCharacteristic->addDescriptor(new BLE2902());

  pCharacteristic->setCallbacks(new MyCallbacks());
 
  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {

        char txString[21];
        //String myString = "$FL5,00C000,0,0,3788,3789,3688,-12,0,296,1,234,1217,3282,8873*65\r\n";
        String myString = "";
        myString.reserve(100);
        while(FL.available())
        {
          char inChar = (char) FL.read();
           myString += inChar;
        }
          
        for(int i=0;i<myString.length();)
        {
          String myString1 = myString.substring(i,i+20);
          myString1.toCharArray(txString,21);
  //      Serial.println(txString);
          pCharacteristic->setValue(txString);
          pCharacteristic->notify();
          i=i+20;
        }
       
  }
  delay(100);
}
