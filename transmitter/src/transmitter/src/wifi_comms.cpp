#include "wifi_comms.h"
#include "auto_pairing.h"

//globals for communication
esp_now_peer_info_t receiverCommsInfo;
CommandMessage commandMessage;              //incoming
ResponseMessage responseMessage;            //outgoing
//long responseMessageTime;
long roundtripTime;


CommandMessage GetCommandMessage(){
    return commandMessage;
}
ResponseMessage GetResponseMessage(){
    return responseMessage;
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    roundtripTime = 0;
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&responseMessage, incomingData, sizeof(responseMessage));
    responseMessage.responseMessageTime = millis();
    if(responseMessage.command_id == commandMessage.id)
    {
        roundtripTime = millis() - commandMessage.send_time;
    } else {
        Serial.println("ERROR: response message received out of order");
    }
}



bool sendCommand(Joystick leftJoystick, Joystick rightJoystick) {
    commandMessage.id++;
    commandMessage.send_time = millis();
    commandMessage.left_speed = leftJoystick.getValue();
    commandMessage.right_speed = rightJoystick.getValue();
    commandMessage.weapon_speed = digitalRead(PIN_WEAPON_TOGGLE_SWITCH) ? DEFAULT_WEAPON_SPEED : 0;
    commandMessage.horn_frequency = digitalRead(PIN_RIGHT_THUMB_SWITCH) ? 0 : DEFAULT_HORN_FREQUENCY;
    commandMessage.servo_position = 0;
    commandMessage.clear_faults = RECEIVER_FAULT_NONE;
    commandMessage.clear_warnings = RECEIVER_WARNING_NONE;

    esp_err_t result = esp_now_send(receiverCommsInfo.peer_addr, (uint8_t *) &commandMessage, sizeof(commandMessage));
    if(result != ESP_OK)
    {
        Serial.println("WARNING: command message may not have been sent");
    }
    return result == ESP_OK;

    commandMessage.command = REMOTE_COMMAND_NONE;//clear out all commands (they are oneshots)
}

void printMAC(uint8_t* mac){
  char macStr[18] = { 0 };
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(String(macStr));
}

void setMAC(uint8_t* mac){
  // Put Peer's mac in prefs
  Preferences prefs;
  prefs.begin("Comms");
  prefs.putBytes("Address", mac, 6);
  prefs.end();
}

esp_now_peer_info_t getMAC(){
  // Get Peer's mac from prefs
  esp_now_peer_info_t mac;

  Preferences prefs;
  prefs.begin("Comms");

  if(prefs.isKey("Address"))
  {
      size_t addressLength = prefs.getBytes("Address", mac.peer_addr, 6);
      prefs.end();
      if(addressLength != 6)
      {
          Serial.println("ERROR: transmitter address stored in Comms/Address is not 6 bytes long");
          //return false;
      }
  } else {
      Serial.println("WARNING: No transmitter address stored in Comms/Address");
      Serial.println("Store Comms address by typing: 'setp Comms Address Bytes FFFFFFFFFFFF'");
  }
  mac.channel = 0;
  mac.encrypt = false;

  prefs.end();

  char macStr[18] = { 0 };
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac.peer_addr[0], mac.peer_addr[1], mac.peer_addr[2], mac.peer_addr[3], mac.peer_addr[4], mac.peer_addr[5]);
  Serial.print("Mac Address pulled from Prefs: ");
  Serial.println(String(macStr));

  return mac;
}


bool espNowSetup()
{
    Preferences prefs;


  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("Transmitter WiFi Initialized at MAC "); Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ERROR: ESPNOW failed to init");
    return false;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  prefs.begin("Comms");
  if(prefs.isKey("Address"))
  {
      size_t addressLength = prefs.getBytes("Address", receiverCommsInfo.peer_addr,6);
      prefs.end();
      if(addressLength != 6)
      {
          Serial.println("ERROR: MAC address stored in Comms/Address is not 6 bytes long");
          return false;
      }
  } else {
      Serial.println("WARNING: No MAC address stored in Comms/Address");
      Serial.println("Store Comms address by typing: 'setp Comms Address Bytes FFFFFFFFFFFF'");
  }
  receiverCommsInfo.channel = 0;
  receiverCommsInfo.encrypt = false;

  AddPeer(receiverCommsInfo);

  commandMessage.id = 1;
  commandMessage.send_time = millis();
  return true;
}


bool AddPeer(esp_now_peer_info_t CommsInfo){
  Serial.print("Adding peer at MAC: ");
  printMAC(CommsInfo.peer_addr);

  //if (esp_now_is_peer_exist(CommsInfo.peer_addr))
  if (esp_now_add_peer(&CommsInfo) != ESP_OK){
      Serial.println("ERROR: Failed to add transmitter as peer");
      return false;
  }
  else{
      Serial.println("Successfully added.");
      return true;
  }
}

bool sendResponse(ReceiverState currentState, ReceiverFault currentFaults, ReceiverWarning currentWarnings)
{
  responseMessage.command_id = commandMessage.id;
  responseMessage.uptime = millis();
  responseMessage.state = currentState;
  responseMessage.faults = currentFaults;
  responseMessage.warnings = currentWarnings;

  esp_err_t result = esp_now_send(receiverCommsInfo.peer_addr, (uint8_t *) &responseMessage, sizeof(responseMessage));
  if(result != ESP_OK)
  {
    Serial.println("WARNING: response message may not have been sent");
  }
  return result == ESP_OK;
}
