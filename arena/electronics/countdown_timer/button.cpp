#include "Arduino.h"
#include "button.h"


Button::Button(int pin, bool judge){
    _pin = pin;
    _judge = judge;
    pinMode(_pin, INPUT_PULLUP);
}

void Button::loop(){
    this->checkstate();
    this->check_reset();

    // timeout button press after 10 sec
    if (ready == true && (millis() - _last_press > 10000)){
        ready = false;
    }

    // timeout reset after 3 sec
    if (reset == true && (millis() - _last_press > 3000)){
        reset = false;
    }
}

void Button::checkstate(){
    if (digitalRead(_pin) == 0){
        if (_state == false){
            _start_press_time = millis();
        }
        _state = true;
        ready = true;
        _last_press = millis();
        _hold_duration = millis() - _start_press_time;
    }
    else{
        _state = false;
        _start_press_time = 0;
        _hold_duration = 0;
     }
}

void Button::reset_state(){
    _state = false;
    ready = false;
}

void Button::check_reset(){
    int hold_time = 3000; // millis
    // Serial.print(_judge);
    // Serial.print(", ");
    // Serial.print(_state);
    // Serial.print(", ");
    // Serial.println(_hold_duration);
    // delay(100);
    if (_judge && _state && _hold_duration > hold_time){
        ready = false;
        reset = true;
        // send reset all message
        Serial.println("RESET!!");
        delay(1000);
    }
}
