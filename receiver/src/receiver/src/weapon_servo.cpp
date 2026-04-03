#include "Arduino.h"
#include "weapon_servo.h"

WeaponServo::WeaponServo(int pin) {
  _pin = pin;
  _isSetup = false;
  
}

bool WeaponServo::setup(){
    _isSetup = true;
    //pinMode(_pin, OUTPUT);
    

    return _isSetup;
}

void WeaponServo::cw(){
    _servo.write(_pin, 0);
}


void WeaponServo::ccw(){
    _servo.write(_pin, 180);
}

void WeaponServo::hold(){
    _servo.write(_pin, 90); // Set pin to 90 degrees to hold position
}

void WeaponServo::loop(int direction, bool enable){
    if(!_isSetup){
        return;
    }
    Serial.println("Weapon servo direction: " + String(direction) + " enable: " + String(enable));
    if (enable){
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
}