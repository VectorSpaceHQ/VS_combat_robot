#include <esp_now.h>
#include <WiFi.h>
#include <EEPROM.h>

#define EEPROM_SIZE 10

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0x10, 0x91, 0xA8, 0x00, 0xB9, 0xCC};

//#define ADC_ONESHOT_FORCE_USE_ADC2_ON_C3

typedef struct struct_message {
  int leftForward;
  int leftBackward;
  int rightForward;
  int rightBackward;
  bool weapon;
} struct_message;
struct_message myData;


// Joystick variables
const int leftJoyPin = A1;
const int rightJoyPin = A2; // D3 is actual but not working
int leftVal;
int rightVal;
int deadband = 40;
int leftRestVal = 1960;
int leftRestMin = leftRestVal - deadband;
int leftRestMax = leftRestVal + deadband;
int leftMin = 1; // Find by testing
int leftMax = 4095; // Find by testing
int rightRestVal = 2070;
int rightRestMin = rightRestVal - deadband;
int rightRestMax = rightRestVal + deadband;
int rightMin = 1; // Find by testing
int rightMax = 4095; // Find by testing

int statusLED1 = D2;
int commsLED = D3;
int pwrWarningLED = D4;

unsigned long last_transmission = millis();

String success;

esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);

#if defined ARDUINO_ESP32_DEV
  Serial.println("ARDUINO_ESP32_DEV");
#elif defined ARDUINO_ESP32S2_DEV
  Serial.println("ARDUINO_ESP32S2_DEV");
#elif defined ARDUINO_ESP32C3_DEV
  Serial.println("ARDUINO_ESP32C3_DEV");
#endif

  pinMode(leftJoyPin, INPUT_PULLUP);
  pinMode(rightJoyPin, INPUT_PULLUP);

  espNowSetup();


  Serial.println("Starting up");
  // blink indicator light on controller to show life
  // establish bluetooth connection
  // blink indicator light on bot to show life
}

void espNowSetup() {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  }
  else {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print(myData.leftForward);
  Serial.print(", ");
  Serial.print(myData.rightForward);
  Serial.print(", ");
  Serial.println(myData.weapon);

}


void read_jsticks() {
  int Lcorrection = leftRestVal - 2048;
  int Rcorrection = rightRestVal - 2048;
  leftVal = leftMax - analogRead(leftJoyPin); // upside down
  rightVal = analogRead(rightJoyPin) - Rcorrection;
  rightVal = 4095 - rightVal; // installed upside down


  int leftForward; // 0 to 100
  int leftBackward;
  if (leftVal > leftRestMax) {
    leftForward = map(leftVal, leftRestMax, leftMax, 0, 100);
    leftBackward = 0;
  }
  else if (leftVal < leftRestMin) {
    leftForward = 0;
    leftBackward = map(leftVal, leftMin, leftRestMin, 100, 0);
  }
  else {
    leftBackward = 0;
    leftForward = 0;
  }

  myData.leftForward = constrain(leftForward, 0, 100);
  myData.leftBackward = constrain(leftBackward, 0, 100);


  int rightForward; // 0 to 100
  int rightBackward;
  if (rightVal > rightRestMax) {
    rightForward = map(rightVal, rightRestMax, rightMax, 0, 100);
    leftBackward = 0;
  }
  else if (rightVal < rightRestMin) {
    rightForward = 0;
    rightBackward = map(rightVal, rightMin, rightRestMin, 100, 0);
  }
  else {
    rightBackward = 0;
    rightForward = 0;
  }

  myData.rightForward = constrain(rightForward,0, 100);
  myData.rightBackward = constrain(rightBackward, 0, 100);
}


void read_weapon() {
  myData.weapon = false;
}


void send_data() {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  debug_output();
  /*
      if (result == ESP_OK) {
          Serial.println("Sent with success");
      }
      else {
          Serial.println("Error sending the data");
      }*/
}


void debug_output() {
  Serial.print("Joysticks: ");
  Serial.print(myData.leftForward);
  Serial.print(", ");
  Serial.print(myData.leftBackward);
  Serial.print(", ");
  Serial.print(myData.rightForward);
  Serial.print(", ");
  Serial.println(myData.rightBackward);
}


void check_voltage() {
  Serial.print("Controller voltage Normal");
}
/*
  void status_report(){
  check_antenna();
  check_message();
  check_voltage();
  report_failures();
  }*/


void loop() {
  read_jsticks();
  read_weapon();

  if(millis() - last_transmission > 50){
    last_transmission = millis();
    send_data();
  }
  //debug_output();
  //send_data();
  //delay(100);
}
