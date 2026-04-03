#include "Arduino.h"
#include "servo.h"

WeaponServo::WeaponServo(int pin) {
  _pin = pin;
  _angle = 0;
  _isSetup = false;
  
}

bool WeaponServo::setup(){
    _isSetup = true;
    pinMode(_pin, OUTPUT);

    return _isSetup;
}

void cw(){
    _servo.write(_pin, 0);
}


void ccw(){
    _servo.write(_pin, 180);
}

void hold(){
    _servo.write(_pin, 90); // Set pin to 90 degrees to hold position
}

void loop(int direction){
    if(!_isSetup){
        return;
    }
    if (direction == 1){
        cw();
    }
    else if (direction == -1){
        ccw();
    }
    else {
        hold();
    }
}