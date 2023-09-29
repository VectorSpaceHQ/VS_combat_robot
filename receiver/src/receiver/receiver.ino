#include <WiFi.h>
#include <esp_now.h>
#include "hardware.h"
#include "common.h"
#include "drive_motor.h"
#include "async_buzzer.h"

#define SOFTWARE_VERSION "v0.1"

#define CONNECTION_TIMEOUT_MS 1000

//globals for libraries
AsyncBuzzer buzzer;
DriveMotor leftMotor;
DriveMotor rightMotor;

//globals for receiver states
ReceiverState currentState = RECEIVER_STATE_BOOT;
ReceiverFault currentFaults = RECEIVER_FAULT_NONE;
ReceiverWarning currentWarnings = RECEIVER_WARNING_NONE;

//globals for communication
uint8_t transmitterAddress[] = {0xD4, 0xF9, 0x8D, 0x03, 0x77, 0xDC};
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
  while (!Serial);//wait for serial to begin
  Serial.println("Vector Space {Combat Robot}");
  Serial.println(SOFTWARE_VERSION);
  Serial.println("https://github.com/VectorSpaceHQ/VS_combat_robot\r\n");

  startupOK &= buzzer.setup(PIN_BUZZER, PWM_CHANNEL_BUZZER);
  Serial.println("buzz");
  startupOK &= leftMotor.setup(PIN_LEFT_MOTOR_FORWARD, PIN_LEFT_MOTOR_BACKWARD, PWM_CHANNEL_LEFT_FORWARD, PWM_CHANNEL_LEFT_BACKWARD);
  Serial.println("left");
  startupOK &= rightMotor.setup(PIN_RIGHT_MOTOR_FORWARD, PIN_RIGHT_MOTOR_BACKWARD, PWM_CHANNEL_RIGHT_FORWARD, PWM_CHANNEL_RIGHT_BACKWARD);
  Serial.println("right");

  sound_on();
  
  startupOK &= espNowSetup();

  if(!startupOK)
  {
    currentState = RECEIVER_STATE_CRITICAL_FAULT;
    Serial.println("ERROR: Critical fault during startup is preventing transition to normal operation");
    sound_error();
  } else {
    currentState = RECEIVER_STATE_CONNECTING;
    Serial.println("Startup Successful, transitioning to connection state");
    sound_ready();
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

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
      // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(transmitterCommsInfo.peer_addr, transmitterAddress, 6);
  transmitterCommsInfo.channel = 0;  
  transmitterCommsInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&transmitterCommsInfo) != ESP_OK){
    Serial.println("ERROR: Failed to add transmitter as peer");
    sound_error();
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

void sound_on(){
  // Do not use tone as it interferes with ledc and the motor control
  ledcWriteTone(PWM_CHANNEL_BUZZER, 247);
  delay(250);
  ledcWriteTone(PWM_CHANNEL_BUZZER, 494);
  delay(500);
  ledcWriteTone(PWM_CHANNEL_BUZZER, 0);
  delay(20);
}

void sound_ready(){
  ledcWriteTone(PWM_CHANNEL_BUZZER, 400);
  delay(250);
  ledcWriteTone(PWM_CHANNEL_BUZZER, 400);
  delay(250);
  ledcWriteTone(PWM_CHANNEL_BUZZER, 2000);
  delay(500);
  ledcWriteTone(PWM_CHANNEL_BUZZER, 0);
  delay(20);
  }

void sound_error(){/*
  tone(buzzerPin, 550, 250);
  delay(7);
  tone(buzzerPin, 150, 600);
  noTone(buzzerPin);
  delay(10);*/
}


void loop(){

  buzzer.loop();
  leftMotor.loop(commandMessage.left_speed, currentState == RECEIVER_STATE_OPERATION);
  rightMotor.loop(commandMessage.right_speed, currentState == RECEIVER_STATE_OPERATION);
  
  if(currentState == RECEIVER_STATE_CONNECTING)
  {
    if(commandMessage.id != responseMessage.command_id)
    {
      currentState = RECEIVER_STATE_OPERATION;
      Serial.println("Command Message Received, transitioning to normal operation");
      sound_ready();
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
      sound_error();
    }
  }

  delay(10);
  
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
