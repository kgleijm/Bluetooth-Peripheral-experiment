#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_APDS9960.h>

const char* nameOfPeripheral = "ComunicationPeripheral";
const char* uuidOfService = "0000181a-0000-1000-8000-00805f9b34fb";
const char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
const char* uuidOfTxChar = "00002A58-0000-1000-8000-00805f9b34fb";

// Setup the incoming data characteristic (RX).
const int RX_BUFFER_SIZE = 256;
bool RX_BUFFER_FIXED_LENGTH = false;

BLEService communicationService(uuidOfService);

BLECharacteristic rxChar(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);
BLECharacteristic txChar(uuidOfTxChar, BLERead | BLENotify | BLEBroadcast, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);

void startBLE() {
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");
    while (1);
  }
}

void onBLEConnected(BLEDevice central) {
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void onBLEDisconnected(BLEDevice central) {
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void onTxRead(BLEDevice central, BLECharacteristic characteristic){
  Serial.println("Detected TX Read");
}

void onRxWrite(BLEDevice central, BLECharacteristic characteristic){
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


void setup() {
  // put your setup code here, to run once:
  // Start serial.
  Serial.begin(9600);

  // Ensure serial port is ready.
  while (!Serial);

  Serial.println("Serial Set up!");

  // Start BLE.
  startBLE();

  // Bluetooth LE connection handlers.
  BLE.setEventHandler(BLEConnected, onBLEConnected);
  BLE.setEventHandler(BLEDisconnected, onBLEDisconnected);

  rxChar.setEventHandler(BLEWritten, onRxWrite);
  txChar.setEventHandler(BLERead, onTxRead);

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

void loop() {
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



