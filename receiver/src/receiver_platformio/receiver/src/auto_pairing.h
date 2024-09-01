#ifndef AUTO_PAIRING_H
#define AUTO_PAIRING_H
#include "Arduino.h"

void PairSetup();
void PairLoop();
void loopWaiting();
void loopDiscovering();
void loopDiscovered();

#endif
