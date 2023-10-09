#include <esp_now.h>
#include <WiFi.h>
#include "button.h"
#include "led_matrix.h"

ReadyButton p1_button(2); //D0
ReadyButton p2_button(3); //D1, which doesn't seem friendly to buttons
int led_pin = 4; // D3
LEDMatrix led_matrix(5,6);

uint8_t broadcastAddress[] = {0x34, 0x85, 0x18, 0x06, 0x1F, 0x28}; // Judge mac address Judge's MAC Adress is: 34:85:18:06:1F:28


typedef struct struct_message {
    bool judge_ready;
    bool judge_reset;
} struct_message;
struct_message incomingReadings;

bool judge_ready;
bool judge_reset;

esp_now_peer_info_t peerInfo;

String success;

void setup() {
  Serial.begin(115200);

  pinMode(led_pin, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("Arena's MAC Adress is: ");
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
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    p1_button.loop();
    p2_button.loop();

    if(p1_button.ready){
      digitalWrite(led_pin, 1);
      delay(100);
      digitalWrite(led_pin,0);
      delay(50);
    }
    if(p2_button.ready){
      digitalWrite(led_pin, 1);
      delay(100);
      digitalWrite(led_pin,0);
    }

    judge_ready = 1; // DEBUG TESTING
    if(p1_button.ready && p2_button.ready && judge_ready)
    {
      Serial.println("Everyone is ready!");
      digitalWrite(led_pin, 1);
      led_matrix.begin_countdown();
      led_matrix.get_time();
    }
    else{
      Serial.print(p1_button.ready);
      Serial.print(", ");
      Serial.print(p2_button.ready);
      Serial.print(", ");
      Serial.println(judge_ready);
    }

    if(judge_reset){
      Serial.println("RESETTING ALL...");
      p1_button.reset_state();
      p2_button.reset_state();
      led_matrix.reset();    
      delay(2000);  
    }
    delay(100);
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

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  //Serial.print("Bytes received: ");
  //Serial.println(len);

  judge_ready = incomingReadings.judge_ready;
  judge_reset = incomingReadings.judge_reset;


}
