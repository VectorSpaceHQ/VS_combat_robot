#ifndef AUTO_PAIRING_H
#define AUTO_PAIRING_H
#include "Arduino.h"
//#include "comms.h"
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
    PairButton(int Pin1, int Pin2, int Pin3);
    bool loop(LED *commsLED);
    ButtonState getButtonState();
    ButtonState setButtonState(ButtonState state);
private:
    bool _isSetup;
    int _pin1;
    int _pin2;
    int _pin3;
    unsigned long _buttonHoldStart;
    int _buttonHoldTime = 3000;
    ButtonState _button1State = ButtonUp;
    ButtonState _button2State = ButtonUp;
    ButtonState _button3State = ButtonUp;
    bool _pairing_state;
};

#endif
