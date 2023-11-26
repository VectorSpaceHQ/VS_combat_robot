#pragma once
#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "Arduino.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "common.h"


class LED {
public:
    LED(int pin);
    //void setup(int Pin);
    void toggle();
    void on();
    void off();
    void blink(int blink_rate);
private:
    bool _isSetup;
    int _pin;
    bool _value;
    unsigned long _last_value_change;
};

class Diagnostics {
public:
    Diagnostics();
    bool setup();
    void loop(ReceiverState currentState);
    void connected();
    void disconnected();
private:
    bool _isSetup;
    bool _isEnabled;
};
#endif
