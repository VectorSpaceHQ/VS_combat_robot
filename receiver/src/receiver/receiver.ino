/*
 * Vector Space Combat Robot Receiver
 * XIAO ESP32-C3
 * 11/2023
*/

#include "hardware.h"
#include "common.h"
#include "drive_motor.h"
#include "weapon.h"
#include "diagnostics.h"
#include <WiFi.h>
#include <esp_now.h>
#include <PreferencesCLI.h> // by Andrew Burks
#include <Servo.h> // ESP32 ESP32S2 AnalogWrite by David Lloyd

#define SOFTWARE_VERSION "v1.1"

#define CONNECTION_TIMEOUT_MS 1000

//globals from libraries
Preferences preferences;
SimpleCLI cli;
PreferencesCLI prefCli(preferences);

//globals for libraries
DriveMotor leftMotor;
DriveMotor rightMotor;
Diagnostics diagnostics;
Weapon weapon(D5);

//globals for receiver states
ReceiverState currentState = RECEIVER_STATE_BOOT;
ReceiverFault currentFaults = RECEIVER_FAULT_NONE;
ReceiverWarning currentWarnings = RECEIVER_WARNING_NONE;

//globals for communication
uint8_t transmitterAddress[] = {0x34, 0x85, 0x18, 0x07, 0x53, 0x5C};
//uint8_t transmitterAddress[] = {0xD4, 0xF9, 0x8D, 0x03, 0x77, 0xDC};
esp_now_peer_info_t transmitterCommsInfo;
CommandMessage commandMessage;              //incoming
ResponseMessage responseMessage;            //outgoing

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {

}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if(len != sizeof(commandMessage))
  {
    Serial.println("ERROR: Incoming message is not the right size");
    return;
  }
  memcpy(&commandMessage, incomingData, sizeof(commandMessage));
}


void setup() {
  currentState = RECEIVER_STATE_STARTUP;
  bool startupOK = true;

  Serial.begin(115200);
  Serial.println("Vector Space {Combat Robot}");
  Serial.println(SOFTWARE_VERSION);
  Serial.println("https://github.com/VectorSpaceHQ/VS_combat_robot\r\n");

  startupOK &= diagnostics.setup();
  startupOK &= leftMotor.init(PIN_LEFT_MOTOR_FORWARD, PIN_LEFT_MOTOR_BACKWARD,
                              LEDC_TIMER_2, LEDC_CHANNEL_2, LEDC_CHANNEL_3);
  startupOK &= rightMotor.init(PIN_RIGHT_MOTOR_FORWARD, PIN_RIGHT_MOTOR_BACKWARD,
                               LEDC_TIMER_2, LEDC_CHANNEL_0, LEDC_CHANNEL_1);
  startupOK &= weapon.setup();
  startupOK &= espNowSetup();

  delay(200);
  weapon.arm();

  if(!startupOK)
  {
    currentState = RECEIVER_STATE_CRITICAL_FAULT;
    Serial.println("ERROR: Critical fault during startup is preventing transition to normal operation");
  } else {
    currentState = RECEIVER_STATE_CONNECTING;
    Serial.println("Startup Successful, transitioning to connection state");
  }
}

bool espNowSetup()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("Receiver WiFi Initialized at MAC "); Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ERROR: ESPNOW failed to init");
    return false;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  preferences.begin("Comms");
  if(preferences.isKey("Address"))
  {
      size_t addressLength = preferences.getBytes("Address",transmitterCommsInfo.peer_addr,6);
      preferences.end();
      if(addressLength != 6)
      {
          Serial.println("ERROR: transmitter address stored in Comms/Address is not 6 bytes long");
          return false;
      }
  } else {
      Serial.println("WARNING: No transmitter address stored in Comms/Address");
      Serial.println("Store Comms address by typing: 'setp Comms Address Bytes D4F98D03710C'");
  }
  transmitterCommsInfo.channel = 0;
  transmitterCommsInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&transmitterCommsInfo) != ESP_OK){
    Serial.println("ERROR: Failed to add transmitter as peer");
    return false;
  }
  char messageBuffer[255];
  sprintf(messageBuffer,"Transmitter Configured at MAC %02X:%02X:%02X:%02X:%02X:%02X",
    transmitterCommsInfo.peer_addr[0],
    transmitterCommsInfo.peer_addr[1],
    transmitterCommsInfo.peer_addr[2],
    transmitterCommsInfo.peer_addr[3],
    transmitterCommsInfo.peer_addr[4],
    transmitterCommsInfo.peer_addr[5]);
  Serial.println(messageBuffer);

  commandMessage.id = 0;
  responseMessage.command_id = 0;
  return true;
}


void loop(){
  leftMotor.loop(commandMessage.left_speed, currentState == RECEIVER_STATE_OPERATION);
  rightMotor.loop(commandMessage.right_speed, currentState == RECEIVER_STATE_OPERATION);
  weapon.loop(commandMessage.weapon_speed, currentState == RECEIVER_STATE_OPERATION);
  diagnostics.loop(currentState);

  if(currentState == RECEIVER_STATE_CONNECTING)
  {
    if(commandMessage.id != responseMessage.command_id)
    {
      currentState = RECEIVER_STATE_OPERATION;
      Serial.println("Command Message Received, transitioning to normal operation");
      /* weapon.arm(); */ // need to implement non-blocking before arming here
    }
  } else if (currentState == RECEIVER_STATE_OPERATION)
  {
    if(commandMessage.id != responseMessage.command_id)
    {
      sendResponse();
    } else if((responseMessage.uptime + CONNECTION_TIMEOUT_MS) < millis())
    {
      currentState = RECEIVER_STATE_CONNECTING;
      Serial.println("ERROR: Command message timeout, transitioning to connecting state");
      weapon.disarm();
    }
  }

  delay(10); // not sure if this is necessary
}


bool sendResponse()
{
  responseMessage.command_id = commandMessage.id;
  responseMessage.uptime = millis();
  responseMessage.state = currentState;
  responseMessage.faults = currentFaults;
  responseMessage.warnings = currentWarnings;
  //responseMessage.board_voltage
  //responseMessage.battery_charge
  //responseMessage.wigi_strength

  esp_err_t result = esp_now_send(transmitterCommsInfo.peer_addr, (uint8_t *) &responseMessage, sizeof(responseMessage));
  if(result != ESP_OK)
  {
    Serial.println("WARNING: response message may not have been sent");
  }
  return result == ESP_OK;
}
