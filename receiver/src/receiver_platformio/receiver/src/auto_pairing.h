#ifndef AUTO_PAIRING_H
#define AUTO_PAIRING_H
#include "Arduino.h"
#include "comms.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

void PairSetup();
void PairLoop();
void loopWaiting();
void loopDiscovering();
void loopDiscovered();

#endif
