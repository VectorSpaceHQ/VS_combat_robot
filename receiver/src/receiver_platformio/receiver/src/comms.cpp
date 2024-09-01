#include "comms.h"

//globals for communication
esp_now_peer_info_t transmitterCommsInfo;
CommandMessage commandMessage;              //incoming
ResponseMessage responseMessage;            //outgoing


void SetState(){

}


CommandMessage GetCommandMessage(){
    return commandMessage;
}
ResponseMessage GetResponseMessage(){
    return responseMessage;
}

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




bool espNowSetup()
{
    Preferences prefs;


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
  prefs.begin("Comms");
  if(prefs.isKey("Address"))
  {
      size_t addressLength = prefs.getBytes("Address",transmitterCommsInfo.peer_addr,6);
      prefs.end();
      if(addressLength != 6)
      {
          Serial.println("ERROR: transmitter address stored in Comms/Address is not 6 bytes long");
          return false;
      }
  } else {
      Serial.println("WARNING: No transmitter address stored in Comms/Address");
      Serial.println("Store Comms address by typing: 'setp Comms Address Bytes FFFFFFFFFFFF'");
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





bool sendResponse(ReceiverState currentState, ReceiverFault currentFaults, ReceiverWarning currentWarnings)
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
