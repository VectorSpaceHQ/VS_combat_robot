#ifndef BUTTON_H
#define BUTTON_H
#include "Arduino.h"


class ReadyButton {
public:
    ReadyButton(int pin);
    void check_state();
    void loop();
    void reset_state();
    void check_reset();

    bool ready = false;
    bool _state = false;
private:
    int _pin;

    long _last_press;
};

class ResetButton {
public:
    ResetButton(int pin);
    void loop();
    void check_state();
    void reset_state();
    void check_reset();

    bool reset = false;
private:
    int _pin;
    bool _state = false;
    long _last_press;
    int _hold_duration;
    long _start_press_time;
};


#endif
