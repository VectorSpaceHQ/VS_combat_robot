#include "Arduino.h"
#include "diagnostics.h"

Diagnostics::Diagnostics()
{
  _isSetup = false;
  _isEnabled = false;
}

bool Diagnostics::setup()
{
  pinMode(_pwr_pin, OUTPUT);
  pinMode(_comms_pin, OUTPUT);

  digitalWrite(_pwr_pin, HIGH);

  _isSetup = true;
  return _isSetup;
}

void Diagnostics::loop(currentState)
{
    // blink comms pin
    if (currentState == RECEIVER_STATE_CONNECTING){
        if(millis() - _comms_on_time > 200){
            _comms_on_time = millis();
            if(_comms_on){
                digitalWrite(_comms_pin, LOW);
                _comms_on = false;
            }
            else{
                digitalWrite(_comms_pin, HIGH);
                _comms_on = true;
            }
        }
    }
    else if(currentState == RECEIVER_STATE_OPERATION){
        digitalWrite(_pwr_pin, HIGH);
        digitalWrite(_comms_pin, HIGH);
    }
    // blink pwr pin on fault
    else if (currentState == RECEIVER_STATE_CRITICAL_FAULT){
        if(millis() - _pwr_on_time > 100){
            _pwr_on_time = millis();
            if(_pwr_on){
                digitalWrite(_pwr_pin, LOW);
                _pwr_on = false;
            }
            else{
                digitalWrite(_pwr_pin, HIGH);
                _pwr_on = true;
            }
        }
    }
}
