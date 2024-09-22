/* All bluetooth and wifi related methods */

#ifndef COMMS_H
#define COMMS_H

#include "Arduino.h"
#include <WiFi.h>
#include <esp_now.h>
#include "common.h"
#include <PreferencesCLI.h>
#include "joystick.h"
#include "hardware.h"

bool espNowSetup();
bool sendResponse(ReceiverState, ReceiverFault, ReceiverWarning);
CommandMessage GetCommandMessage();
ResponseMessage GetResponseMessage();
void SetState();
bool AddPeer(esp_now_peer_info_t);
void setMAC(uint8_t* mac);
esp_now_peer_info_t getMAC();
void printMAC(uint8_t* mac);
bool sendCommand(Joystick leftJoystick, Joystick rightJoystick);

#endif
