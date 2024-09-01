/* All bluetooth and wifi related methods */

#ifndef COMMS_H
#define COMMS_H

#include "Arduino.h"
#include <WiFi.h>
#include <esp_now.h>
#include "common.h"
#include <PreferencesCLI.h>


bool espNowSetup(Preferences);
bool sendResponse(ReceiverState, ReceiverFault, ReceiverWarning);
CommandMessage GetCommandMessage();
ResponseMessage GetResponseMessage();

#endif
