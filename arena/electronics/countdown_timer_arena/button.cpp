#include "Arduino.h"
#include "button.h"


ReadyButton::ReadyButton(int pin){
    _pin = pin;
    pinMode(_pin, INPUT_PULLUP);
    Serial.print("ReadyButton initialized on pin: ");
    Serial.println(_pin);
}

void ReadyButton::loop(){
    this->check_state();

    // timeout button press after 30 sec
    if (ready == true && (millis() - _last_press > 30000)){
        ready = false;
    }
}

void ReadyButton::check_state(){
    if (digitalRead(_pin) == 0){
        _state = true;
        ready = true;
        _last_press = millis();
    }
    else{
        _state = false;
     }
}

void ReadyButton::reset_state(){
    _state = false;
    ready = false;
}



ResetButton::ResetButton(int pin){
    _pin = pin;
    pinMode(_pin, INPUT_PULLUP);
    Serial.print("ResetButton initialized on pin: ");
    Serial.println(_pin);
}

void ResetButton::loop(){
    this->check_state();
    this->check_reset();
}

void ResetButton::check_state(){
    if (digitalRead(_pin) == 0){
        if (_state == false){
            _start_press_time = millis();
        }
        _state = true;
        _last_press = millis();
        _hold_duration = millis() - _start_press_time;
    }
    else{
        _state = false;
        _start_press_time = 0;
        _hold_duration = 0;
    }
}

void ResetButton::check_reset(){
    int hold_time = 3000; // millis
    if (_state && _hold_duration > hold_time){
        reset = true;
        Serial.println("RESET!!");
    }
    else{
        reset = false;
    }
}
