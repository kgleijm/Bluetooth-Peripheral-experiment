#include <Arduino.h>
#include <ArduinoBLE.h>

// string of 128 chars to force bluetooth buffer length
static constexpr char* emptyBuffer = "|NO_MSG|                                                                                                                        ";

// Name that is displayed when scanning for bluetooth devices
static constexpr char* nameOfPeripheral = "ComunicationPeripheral";

// Hardcoded BLE uuis's
static constexpr char* uuidOfService = "0000181a-0000-1000-8000-00805f9b34fb";
static constexpr char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
static constexpr char* uuidOfTxChar = "00002A58-0000-1000-8000-00805f9b34fb";

// Constants handling buffersizes
static const int BUFFER_SIZE = 128;
static const bool BUFFER_FIXED_LENGTH = false;

// Initializing characteristics used for recieving and transmitting messages
static BLECharacteristic rxChar(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, BUFFER_SIZE, BUFFER_FIXED_LENGTH);
static BLECharacteristic txChar(uuidOfTxChar, BLERead | BLENotify | BLEBroadcast, BUFFER_SIZE, BUFFER_FIXED_LENGTH);
static BLEService communicationService(uuidOfService);

// Static Wrapper class for BLE functions
class BLECommunicationManager{

  private:
    
    //wrapper for BLE startup sequence
    static void startBLE() {
      if (!BLE.begin())
      {
        Serial.println("starting BLE failed!");
        while (1);
      }
    }

    // Triggers when connection to the central device is established
    static void onBLEConnected(BLEDevice central) {
      Serial.print("Connected event, central: ");
      Serial.println(central.address());
    }

    // Triggers when connection to the central device is lost
    static void onBLEDisconnected(BLEDevice central) {
      Serial.print("Disconnected event, central: ");
      Serial.println(central.address());
    }

    // Triggers before central device reads the characteristic
    static void onTxRead(BLEDevice central, BLECharacteristic characteristic){
      Serial.println("Detected TX Read");
    }

    // Triggers when central device writes to this peripheral
    static void onRxWrite(BLEDevice central, BLECharacteristic characteristic){
        Serial.print("Received message from central device");
        byte tmp[BUFFER_SIZE];
        int dataLength = rxChar.readValue(tmp, BUFFER_SIZE);
        String incoming = "";
        for(int i = 0; i < dataLength; i++) {
          incoming += (char)tmp[i];
        }
        BLECommunicationManager::interpretMessage(incoming);
    }

    // Interpret message coming from central device
    static void interpretMessage(String incomingMessage){
      // do something with custom commands from the central device
      if (incomingMessage.substring(0,4) == "Foo"){
        Serial.println("\nMessage received: Foo!\n");
        setOutgoingTo("Readed Foo loud and clear!");
      }else if (incomingMessage.substring(0,4) == "Bar"){
        Serial.println("\nMessage received: Bar!\n");
        setOutgoingTo("Going for bar this time!");
      }else{
        Serial.println("Unknown command received: ");
        Serial.println(incomingMessage);
      }
    }

  public:

    // Set outgoing message to string x
    static void setOutgoingTo(String outgoing){
      char outgoingArr[BUFFER_SIZE];
      outgoing.toCharArray(outgoingArr, BUFFER_SIZE);
      setOutgoingTo(outgoingArr);
    }

    // Set outgoing to char array x
    static void setOutgoingTo(char outgoing[]){
      txChar.setValue(outgoing);
      // Serial.println("Set tx to:");
      // Serial.println(outgoing);
    }
    
    // Set up BLE communication variables
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

      rxChar.setValue(emptyBuffer);
      txChar.setValue(emptyBuffer);
    }

    // Do additional stuff while device is connected
    static void onLoop(){
      BLEDevice central = BLE.central();
      
      if (central)
      {
        while (central.connected()) {
          // Do stuff here when central device is connected.
        }
      }
    }
};


void setup() {

  // setup with delays to prevent serial flooding
  delay(1000);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Serial started");
  delay(1000);

  BLECommunicationManager::begin();

}

void loop() {
  BLECommunicationManager::onLoop();
  
  // you can do whatever in your loop

  delay(1000);

}



