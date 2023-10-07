#ifndef BUTTON_H
#define BUTTON_H
#include "Arduino.h"


class Button {
public:
    Button(int pin, bool judge=false);
    void checkstate();
    void loop();
    void reset_state();
    void check_reset();

    bool ready = false;
    bool reset = false;
private:
    int _pin;
    bool _state = false;
    long _last_press;
    int _hold_duration;
    long _start_press_time;
    bool _judge;
};


#endif
