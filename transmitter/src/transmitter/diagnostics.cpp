#include "Arduino.h"
#include "diagnostics.h"

LED::LED(int Pin){
    _isSetup = false;
    _pin = Pin;
    _value = 0;
    pinMode(_pin, OUTPUT);
    _isSetup = true;
}

// void LED::setup(int Pin){
//     _pin = Pin;
//     pinMode(_pin, OUTPUT);

//     _isSetup = true;
// }

void LED::on(){
    digitalWrite(_pin, 1);
    _value = 1;
}

void LED::off(){
    digitalWrite(_pin, 0);
    _value = 0;
}

void LED::toggle(){
    if (_value == 1){
        (*this).off();
    }
    else{
        (*this).on();
    }
    _last_value_change = millis();
}

void LED::blink(){
    if(millis() - _last_value_change > 500){
        (*this).toggle();
        _last_value_change = millis();
    }
}
