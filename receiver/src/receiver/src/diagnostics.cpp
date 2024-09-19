#include "Arduino.h"
#include "diagnostics.h"
#include "common.h"
#include "hardware.h"




Diagnostics::Diagnostics()
{
  _isSetup = false;
  _isEnabled = false;
}

bool Diagnostics::setup()
{
  _isSetup = true;
  return _isSetup;
}

void Diagnostics::loop(ReceiverState currentState, LED *comms, LED *optional)
{
    // blink comms pin
    if (currentState == RECEIVER_STATE_CONNECTING){
        comms->blink(200);
    }
    else if(currentState == RECEIVER_STATE_PAIRING){
        comms->blink(50);
    }
    else if(currentState == RECEIVER_STATE_OPERATION){
        comms->blink(500);
    }
    // Fault: blink pins rapidly
    else if (currentState == RECEIVER_STATE_CRITICAL_FAULT){
        comms->blink(100);
        optional->blink(100);
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

void connected(LED led1, LED led2){
    // Blink LEDS in sequence that shows connection
    led1.on();
    led2.off();

    for (int i=0; i<10; i++){
        delay(20);
        led1.toggle();
        led2.toggle();
    }
    led1.off();
    led2.off();
}
