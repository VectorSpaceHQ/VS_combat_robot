#ifndef WEAPON_SERVO_H
#define WEAPON_SERVO_H
#include "Arduino.h"
//#include "driver/ledc.h"
#include "esp_err.h"
//#include <ESP32Servo.h>
#include <Servo.h> // ESP32 ESP32S2 AnalogWrite by David Lloyd



class WeaponServo {
public:
    WeaponServo(int Pin);
    bool setup();
    void cw();
    void ccw();
    void hold();
    void loop(int direction, bool enable=true);
private:
    Servo _servo = Servo();
    int _currentSpeed;
    int _pin;
    bool _isSetup;
};

#endif
