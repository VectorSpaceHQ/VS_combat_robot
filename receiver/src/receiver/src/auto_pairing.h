#ifndef AUTO_PAIRING_H
#define AUTO_PAIRING_H
#include "Arduino.h"
#include "wifi_comms.h"
#include "diagnostics.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

void PairSetup();
void PairLoop();
void loopWaiting();
void loopDiscovering();
void loopDiscovered();

enum ButtonState {
    ButtonDown, // The button is being pressed/held
    ButtonUp    // The button has been released
};

class PairButton{
public:
    PairButton(int Pin);
    void loop(LED *commsLED);
    ButtonState getButtonState();
    ButtonState setButtonState(ButtonState state);
private:
    bool _isSetup;
    int _pin;
    unsigned long _buttonHoldStart;
    int _buttonHoldTime = 3000;
    ButtonState _buttonState = ButtonUp;
    bool _pairing_state;
};

#endif
