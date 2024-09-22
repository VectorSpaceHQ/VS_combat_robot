#include <Arduino.h>
#include "auto_pairing.h"
#include <Preferences.h>
#include "wifi_comms.h"


// We use an Enum to define the Mode of our Device
enum DeviceMode {
  Waiting, // Not discovering, not timed out
  Discovering, // We're in Discovery mode
  Discovered,  // Discovery Succeeded
  Failed,  // Discovery Failed (Timed Out)
};

DeviceMode deviceMode = Waiting; // We are initially Waiting


ButtonState buttonState;


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define UUID_SERVICE          "d91fdc86-46f8-478f-8dec-ebdc0a1188b2"
#define UUID_CHARACTERISTIC   "56100987-749a-4014-bc22-0be2f5af59d0"

BLEClient* pBLEClient;
BLERemoteService* pRemoteService;

BLEServer *bleServer;
BLEService *bleService;
BLECharacteristic *bleCharacteristic;
BLEAdvertising *bleAdvertising;
bool bleClientConnected = false;
unsigned long discoveredAt;

class BLECallbacks: public BLEServerCallbacks {
   void onConnect(BLEServer* pServer) {
      Serial.println("BLE Client Connected!");
      bleClientConnected = true;

    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("BLE Client Disconnected!");
      bleClientConnected = false;
      deviceMode = Discovered;
      discoveredAt = 0;
    }
};

inline void startDiscovering() {
  if (bleServer == nullptr) {
    Serial.println("First Time Discovering");
    // Get self MAC Address
    WiFi.mode(WIFI_MODE_STA);
    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);

    // Prepare our BLE Server
    bleServer = BLEDevice::createServer(); 
    bleServer->setCallbacks(new BLECallbacks());

    // Prepare our Service
    bleService = bleServer->createService(UUID_SERVICE);

    // A Characteristic is what we shall use to provide Clients/Slaves with our MAC Address.
    bleCharacteristic = bleService->createCharacteristic(UUID_CHARACTERISTIC, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    // Provide our Characteristic with the MAC Address "Payload"
    bleCharacteristic->setValue(&mac[0], 6);
    // Make the Property visible to Clients/Slaves.
    bleCharacteristic->setBroadcastProperty(true);

    // Start the BLE Service
    bleService->start();

    // Advertise it!
    bleAdvertising = BLEDevice::getAdvertising();
    bleAdvertising->addServiceUUID(UUID_SERVICE);
    bleAdvertising->setScanResponse(true);
    bleAdvertising->setMinPreferred(0x06);
    bleAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    return;
  }
  // Start the BLE Service
  bleService->start();

  // Advertise it!
  bleAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();
  Serial.println("Done startDiscovering");
}

inline void stopDiscovering() {
  BLEDevice::stopAdvertising();
  bleService->stop();
  Serial.println("Stopped BLE");
}


void PairSetup() {
  BLEDevice::init("Flowduino Auto-Discovery Demo - Master");
}

PairButton::PairButton(int Pin1, int Pin2, int Pin3){
    _isSetup = true;
    _pin1 = Pin1;
    _pin2 = Pin2;
    _pin3 = Pin3;
    pinMode(_pin1, INPUT);
    pinMode(_pin2, INPUT);
    pinMode(_pin3, INPUT);
    _pairing_state = false;
}

ButtonState PairButton::getButtonState(){
  int currentB1State = digitalRead(_pin1);
  int currentB2State = digitalRead(_pin2);
  int currentB3State = digitalRead(_pin3);
    if (_button1State == ButtonUp && currentB1State == LOW &&
        _button2State == ButtonUp && currentB2State == LOW &&
        _button3State == ButtonUp && currentB3State == LOW)
    {
      Serial.println("all 3 buttons hold started");
      _buttonHoldStart = millis();
      _button1State = ButtonDown;
      _button2State = ButtonDown;
      _button3State = ButtonDown;
      return ButtonDown;
    }
    else if (_button1State == ButtonDown && currentB1State == LOW &&
             _button2State == ButtonDown && currentB2State == LOW &&
             _button3State == ButtonDown && currentB3State == LOW)
    {
        _button1State = ButtonDown;
        _button2State = ButtonDown;
        _button3State = ButtonDown;
        return ButtonDown;
    }
    else{
      _button1State = ButtonUp;
      _button2State = ButtonUp;
      _button3State = ButtonUp;
      return ButtonUp;
    }
}

bool PairButton::loop(LED *commsLED){
  (*this).getButtonState();

  if (bleClientConnected){
    delay(1000); // if this function called too soon it won't have the updated characteristic from the rx
    char macStr[18] = { 0 };
    const char* rawData = bleCharacteristic->getValue().c_str();
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", rawData[0], rawData[1], rawData[2], rawData[3], rawData[4], rawData[5]);
    Serial.print("The receiver's MAC address is: ");
    Serial.println(String(macStr));
    
    setMAC(bleCharacteristic->getData());
    // Disconnect BLE and make Wifi connection using MACs
    //    AddPeer(transmitterCommsInfo);

    // Blink 3 times
    for (int i=0;i <3; i++){
      commsLED->on();
      delay(200);
      commsLED->off();
      delay(200);
    }
    return true; // we are paired
  }

  if (_button1State == ButtonDown &&
      _button2State == ButtonDown &&
      _button3State == ButtonDown &&
      millis() - _buttonHoldStart > _buttonHoldTime)
  {
      _pairing_state = true;
      startDiscovering();
      deviceMode = Discovering;
  }
  if (millis() - _buttonHoldStart > 15000){
      _pairing_state = false;
      commsLED->off();
  }
  if (_pairing_state == true){
      commsLED->blink(30);
      switch (deviceMode) {
      case (Discovering):
        //Serial.println("loopDiscovering");
        loopDiscovering();
        break;
      case (Discovered):
      Serial.println("loopDiscovered");
        loopDiscovered();
        break;
    }
  }
  return false;
}


unsigned long buttonHoldStart; // The millis() value of the initial Button push down
#define BUTTON_HOLD_TIME  3000 // The number of millis for which we must hold the button
unsigned long discoveryStart; // The millis() value at which Discovery started
#define DISCOVERY_TIMEOUT   15000 // 30 seconds in milliseconds for Timeout


inline void loopDiscovering() {
  if (bleClientConnected) { return; }
}

// The Loop routine when our Device is in Discovered Mode
inline void loopDiscovered() {
  if (discoveredAt == 0) {
    stopDiscovering();
    discoveredAt = millis();
    return;
  }

  if (millis() > discoveredAt + BUTTON_HOLD_TIME) {
    
    deviceMode = Waiting;
    Serial.println("Going back to Waiting mode");
  }
}

