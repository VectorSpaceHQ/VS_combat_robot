#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include "Arduino.h"

class LED {
public:
    LED(int Pin);
    //void setup(int Pin);
    void toggle();
    void on();
    void off();
    void blink();
private:
    bool _isSetup;
    int _pin;
    bool _value;
    unsigned long _last_value_change;
};


#endif
