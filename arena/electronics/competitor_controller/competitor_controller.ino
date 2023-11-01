#include <esp_now.h>
#include <WiFi.h>
#include "button.h"

ReadyButton ready_button(11); //D0
ResetButton reset_button(5); //D1, which doesn't seem friendly to buttons
int led_pin = 9; // D3

uint8_t clockAddress[] = {0x34, 0x85, 0x18, 0x06, 0x1F, 0x28}; // Judge mac address Judge's MAC Adress is: 34:85:18:06:1F:28
esp_now_peer_info_t peerInfo;
String success;


void setup() {
  Serial.begin(115200);

  pinMode(led_pin, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("This device's MAC Adress is: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, clockAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
//  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    ready_button.loop();
    reset_button.loop();

    if(ready_button.ready){
      digitalWrite(led_pin, 1);
      delay(100);
      digitalWrite(led_pin,0);
      delay(50);
    }

    delay(50);
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}
