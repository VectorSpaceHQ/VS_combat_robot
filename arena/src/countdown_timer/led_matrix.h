#ifndef LEDMATRIX_H
#define LEDMATRIX_H
#include "Arduino.h"
#include "CountUpDownTimer.h"

class LEDMatrix{
public:
    LEDMatrix(int SDA, int SCL);
    void begin_countdown();
    void reset();
    void pause();
    void get_time();
    /* CountUpDownTimer matchTimer(bool type, bool precision=HIGH); */
private:
    int _sda_pin;
    int _scl_pin;
    bool _ready;
    bool _paused;
    int _msec_starting = 180 * 1000;
    int _msec_remaining;
};

#endif
