#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "Arduino.h"
#include "driver/ledc.h"
#include "esp_err.h"

class Diagnostics {
public:
    Diagnostics();
    bool setup(int _pwr_pin, int _comms_pin);
    void loop();
    void connected();
    void disconnected();
private:
    int _pwr_pin = D2;
    int _comms_pin = D4;
    bool _isSetup;
    int _comms_on_time;
    bool _comms_on;
    bool _pwr_on;
    int _pwr_on_time;
};

#endif
