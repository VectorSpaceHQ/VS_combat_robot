#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include "Arduino.h"
#include <Servo.h> // ESP32-ESP32S2-AnalogWrite

class Weapon {
public:
    Weapon(int Pin);
    void arm();
    void on();
    void off();
    void setup();
private:
    bool _isSetup;
    int _pin;
    bool _value;
    bool _armed;
    int _on_time;
    Servo _motor = Servo();
};

#endif
