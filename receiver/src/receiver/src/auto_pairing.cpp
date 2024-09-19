#include "auto_pairing.h"

#define PIN_PAIR_BUTTON    D9 // Same as "B" boot button
#define UUID_SERVICE          "d91fdc86-46f8-478f-8dec-ebdc0a1188b2"
#define UUID_CHARACTERISTIC   "56100987-749a-4014-bc22-0be2f5af59d0"

BLEScan* pBLEScan;
BLEClient* pBLEClient;
BLERemoteService* pRemoteService;
BLERemoteCharacteristic* pRemoteCharacteristic;
BLEAdvertisedDevice _advertisedDevice;
bool deviceFound = false;
bool clientConnected = false;
unsigned long discoveredAt;
bool _paired_state = false;
bool _initialized = false;


// We use an Enum to define the Mode of our Device
enum DeviceMode {
  Waiting, // Not Discovering, not timed out
  Discovering, // We're in Discovering mode
  Discovered,  // Discovering Succeeded
  Failed,  // Discovering Failed (Timed Out)
};

DeviceMode deviceMode = Waiting; // We are initially Waiting

ButtonState buttonState;


PairButton::PairButton(int Pin){
    _isSetup = true;
    _pin = Pin;
    pinMode(_pin, INPUT);
    _pairing_state = false;

}

ButtonState PairButton::getButtonState(){
  int currentState = digitalRead(_pin);
    if (_buttonState == ButtonUp && currentState == LOW)
    {
      Serial.println("button hold started");
      _buttonHoldStart = millis();
      _buttonState = ButtonDown;
      return ButtonDown;
    }
    else if (_buttonState == ButtonDown && currentState == LOW){
      _buttonState = ButtonDown;
      return ButtonDown;
    }
    else{
      _buttonState = ButtonUp;
      return ButtonUp;
    }
}


// if button pressed for duration, go into pairing mode and blink led
void PairButton::loop(LED *commsLED){
  (*this).getButtonState();

  if (_initialized == false){
    PairSetup();
    _initialized = true;
  }

  if (_paired_state == true){
    return;
  }
  if (_buttonState == ButtonDown and _pairing_state == false){
    commsLED->on();
  }
  /* else if (_buttonState == ButtonDown and getButtonState() == ButtonUp){
    commsLED->off();
  } */

  if (_buttonState == ButtonDown && millis() - _buttonHoldStart > _buttonHoldTime){
    _pairing_state = true;
  }
  if (millis() - _buttonHoldStart > 20000 && _pairing_state){
      Serial.println("giving up on pairing");
      _pairing_state = false;
      commsLED->off();
  }

  if (_pairing_state == true){
      Serial.println("pairing");
      commsLED->blink(20);
      loopDiscovering();
  }

}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (clientConnected) { return; };

      if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().toString() == UUID_SERVICE) {
        Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
        _advertisedDevice = advertisedDevice;
        deviceFound = true;
        return;
      }
    }
};

inline ButtonState getButtonState() {
  return digitalRead(PIN_PAIR_BUTTON) == LOW ? ButtonDown : ButtonUp;
}

void PairSetup() {
  Serial.println("Pair Setup starting");
  BLEDevice::init("Flowduino Auto-Discovery Demo - Slave");

  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  // Get the initial state of our Button
  buttonState = getButtonState();

}

unsigned long buttonHoldStart; // The millis() value of the initial Button push down
#define BUTTON_HOLD_TIME  3000 // The number of millis for which we must hold the button
unsigned long discoveryStart; // The millis() value at which Discovering started
#define DISCOVERY_TIMEOUT   15000 // 15 seconds in milliseconds for Timeout

// The Loop routine when our Device is in Waiting Mode
inline void loopWaiting() {
  ButtonState currentState = getButtonState();

  // Down to Up
  if (buttonState == ButtonDown && currentState == ButtonUp) {
    buttonState = currentState; // Update the global variable accordingly
    return; // Need not proceed further
  }

  // Up to Down
  if (buttonState == ButtonUp && currentState == ButtonDown) {
    // The Button was just pressed down...
    buttonHoldStart = millis();
    buttonState = currentState;
    Serial.println("Button Hold Started");

    return; // Need not proceed further
  }

  // Held Down
  if (buttonState == ButtonDown && currentState == ButtonDown && millis() > buttonHoldStart + BUTTON_HOLD_TIME) {
    // We now initiate Discovering!
    Serial.println("Initiating Discovering");
    deviceMode = Discovering;
    discoveryStart = millis();
    buttonHoldStart = discoveryStart;
  }
}

inline bool connectToDevice() {
  // BLE Connect to peer, add their MAC to prefs, send our MAC to their characteristic, AddPeer by Wifi
  pBLEClient = BLEDevice::createClient();
  Serial.println("Connecting To \"Flowduino Auto-Discovery Demo - Master\"");
  clientConnected = pBLEClient->connect(&_advertisedDevice);

  if (!clientConnected) {
    Serial.println("Connection Failed!");
    return false;
  }

  Serial.println("Connected... let's get the ESPNow Address!");
  pRemoteService = pBLEClient->getService(UUID_SERVICE);
  if (pRemoteService == nullptr) {
    Serial.println("Unable to get our Service from the Controller!");
    return false;
  }

  Serial.println("Service acquired!");

  pRemoteCharacteristic = pRemoteService->getCharacteristic(UUID_CHARACTERISTIC);

  if (pRemoteCharacteristic == nullptr) {
    Serial.println("Couldn't get the pRemoteCharacteristic");
    return false;
  }

  Serial.println("YAY! We got the pRemoteCharacteristic!");

  const char* rawData = pRemoteCharacteristic->readValue().c_str(); // this is needed before doing readRawData somehow

  uint8_t* transmitterMac = pRemoteCharacteristic->readRawData();
  Serial.print("Storing Transmitter Mac in Prefs: ");
  printMAC(transmitterMac);

  Preferences prefs;
  prefs.begin("Comms");

  esp_now_peer_info_t oldTransmitterMac;
  size_t addressLength = prefs.getBytes("Address",oldTransmitterMac.peer_addr,6);
  Serial.print("Old transmitter mac is: ");
  printMAC(oldTransmitterMac.peer_addr);

  prefs.putBytes("Address", transmitterMac, 6);

  esp_now_peer_info_t transmitterCommsInfo;
  //size_t addressLength = prefs.getBytes("Address",transmitterCommsInfo.peer_addr,6);
  addressLength = prefs.getBytes("Address",transmitterCommsInfo.peer_addr,6);  
  transmitterCommsInfo.channel = 0;
  transmitterCommsInfo.encrypt = false;
  prefs.end();
  
  Serial.print("Transmitter Mac Address pulled after prefs mod: ");
  printMAC(transmitterCommsInfo.peer_addr);

  if (macCompare(transmitterCommsInfo.peer_addr, oldTransmitterMac.peer_addr)){
    Serial.println("new MAC matches old. stopping here.");
    return true;
  }

  // Send the receiver's mac address back to the transmitter
  String newValue = WiFi.macAddress();
  Serial.print("Setting characteristic value to RX MAC: ");
  Serial.println(newValue.c_str() ); 
  pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  
  // This is sending by wifi, not BLE. NEed to send BLE first.
  _paired_state = AddPeer(transmitterCommsInfo);

  deviceMode = Discovered;
  discoveredAt = millis();

  return true;
}

bool macCompare(uint8_t *a, uint8_t *b){
  bool match = true;
  for (int i=0; i<6; i++){
    if(a[i] != b[i]){
      match = false;
    }
  }

  if (match == true)
  {
    return true;
  }
  else{
    return false;
  }
}

// The Loop routine when our Device is in Discovering Mode
inline void loopDiscovering() {
  // Scan for BLE Devices
  deviceFound = false;
  BLEScanResults foundDevices = pBLEScan->start(3, false);
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

  if (deviceFound) {
    if (connectToDevice()) { return; }
  }
}
