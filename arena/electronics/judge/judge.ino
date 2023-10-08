#include <esp_now.h>
#include <WiFi.h>
#include "button.h"

ReadyButton judge_ready_button(2);
ResetButton judge_reset_button(4);
bool arena_alive;

// Arena's MAC Adress is: D4:F9:8D:03:79:1C

uint8_t broadcastAddress[] = {0xD4, 0xF9, 0x8D, 0x03, 0x79, 0x1C}; // Arena mac address

typedef struct struct_message {
    bool judge_ready;
    bool judge_reset;
} struct_message;
struct_message buttonStates;

typedef struct struct_incoming{
  bool alive;
} struct_incoming;
struct_incoming arena_message;

bool judge_ready;
bool judge_reset;

esp_now_peer_info_t peerInfo;

String success;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    Serial.print("Judge's MAC Adress is: ");
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
    judge_ready_button.loop();
    judge_reset_button.loop();

    buttonStates.judge_ready = judge_ready_button.ready;
    buttonStates.judge_reset = judge_reset_button.reset;


    if(judge_ready_button.ready)
    {
        Serial.println("judge ready!");
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &buttonStates, sizeof(buttonStates));
        delay(500);
    }
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
void OnDataRecv(const uint8_t * mac, const uint8_t *arena_message, int len) {
    memcpy(&arena_message, arena_message, sizeof(arena_message));
    Serial.print("Bytes received: ");
    Serial.println(len);
    //arena_alive = arena_message.alive;

}
