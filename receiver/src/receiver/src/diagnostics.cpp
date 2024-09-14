#include "Arduino.h"
#include "diagnostics.h"
#include "common.h"
#include "hardware.h"

LED pwr(PIN_PWR_LED);
LED comms(PIN_COMMS_LED);
  
Diagnostics::Diagnostics()
{
  _isSetup = false;
  _isEnabled = false;
}

bool Diagnostics::setup()
{
  pwr.on();

  _isSetup = true;
  return _isSetup;
}

void Diagnostics::loop(ReceiverState currentState)
{
    // blink comms pin
    if (currentState == RECEIVER_STATE_CONNECTING){
        comms.blink(200);
    }
    else if(currentState == RECEIVER_STATE_OPERATION){
        pwr.on();
        comms.on();
    }
    // blink pwr pin on fault
    else if (currentState == RECEIVER_STATE_CRITICAL_FAULT){
        pwr.blink(100);
    }
}

LED::LED(int Pin){
    _isSetup = false;
    _pin = Pin;
    _value = 0;
    pinMode(_pin, OUTPUT);
    _isSetup = true;
}

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

void LED::blink(int blink_rate){
    if(millis() - _last_value_change > blink_rate){
        (*this).toggle();
        _last_value_change = millis();
    }
}
