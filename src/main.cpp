#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_APDS9960.h>
#include <Arduino_HTS221.h>


static constexpr char* uuidOfService = "0000181a-0000-1000-8000-00805f9b34fb";
static constexpr char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
static constexpr char* uuidOfTxChar = "00002A58-0000-1000-8000-00805f9b34fb";
static constexpr char* nameOfPeripheral = "ComunicationPeripheral";

static const int RX_BUFFER_SIZE = 256;
static const bool RX_BUFFER_FIXED_LENGTH = false;

static BLECharacteristic rxChar(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);
static BLECharacteristic txChar(uuidOfTxChar, BLERead | BLENotify | BLEBroadcast, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);
static BLEService communicationService(uuidOfService);

class BLECommunicationManager{

  private:
    
    static void startBLE() {
      if (!BLE.begin())
      {
        Serial.println("starting BLE failed!");
        while (1);
      }
    }

    static void onBLEConnected(BLEDevice central) {
      Serial.print("Connected event, central: ");
      Serial.println(central.address());
    }

    static void onBLEDisconnected(BLEDevice central) {
      Serial.print("Disconnected event, central: ");
      Serial.println(central.address());
    }

    static void onTxRead(BLEDevice central, BLECharacteristic characteristic){
      Serial.println("Detected TX Read");
    }

    static void onRxWrite(BLEDevice central, BLECharacteristic characteristic){
        // central wrote new value to characteristic, update LED
        Serial.print("Characteristic event, read: ");
        byte tmp[256];
        int dataLength = rxChar.readValue(tmp, 256);

        for(int i = 0; i < dataLength; i++) {
          Serial.print((char)tmp[i]);
        }
        Serial.println();
        Serial.print("Value length = ");
        Serial.println(rxChar.valueLength());
    }


  public:
    
    static void begin(){
      
    
      BLECommunicationManager::startBLE();

      BLE.setEventHandler(BLEConnected, BLECommunicationManager::onBLEConnected);
      BLE.setEventHandler(BLEDisconnected, BLECommunicationManager::onBLEDisconnected);


      rxChar.setEventHandler(BLEWritten, BLECommunicationManager::onRxWrite);
      txChar.setEventHandler(BLERead, BLECommunicationManager::onTxRead);

      // Create BLE service and characteristics.
      BLE.setLocalName(nameOfPeripheral);
      BLE.setAdvertisedService(communicationService);
      communicationService.addCharacteristic(rxChar);
      communicationService.addCharacteristic(txChar);
      BLE.addService(communicationService);

      BLE.advertise();

      // Print out full UUID and MAC address.
      Serial.println("Peripheral advertising info: ");
      Serial.print("Name: ");
      Serial.println(nameOfPeripheral);
      Serial.print("MAC: ");
      Serial.println(BLE.address());
      Serial.print("Service UUID: ");
      Serial.println(communicationService.uuid());
      Serial.print("rxCharacteristic UUID: ");
      Serial.println(uuidOfRxChar);
      Serial.print("txCharacteristics UUID: ");
      Serial.println(uuidOfTxChar);
      Serial.println("Bluetooth device active, waiting for connections...");


      rxChar.setValue("I'm Rx");
      txChar.setValue("I'm TX");

    }

    static void onLoop(){
      // put your main code here, to run repeatedly:
      BLEDevice central = BLE.central();
      
      if (central)
      {
        // Only send data if we are connected to a central device.
        while (central.connected()) {
          // Serial.println("Device is connected in mainloop");
        }
      }
    }
};


void setup() {

  Serial.begin(9600);
  while (!Serial);

  BLECommunicationManager::begin();

}

void loop() {
  BLECommunicationManager::onLoop();
  
  
}



