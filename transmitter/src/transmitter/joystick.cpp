#include "Arduino.h"
#include <Preferences.h>
#include "joystick.h"

Joystick::Joystick(){
  _isSetup = false;
  _value = 0;
}

bool Joystick::setup(int analogPin, float neutralVoltage, float halfRangeVoltage, float deadbandVoltage, bool invert)
{
  _isSetup = false;
  _pin = analogPin;
  _upperNeutralVoltage = neutralVoltage + deadbandVoltage / 2;
  _lowerNeutralVoltage = neutralVoltage - deadbandVoltage / 2;
  float adjustedHalfRangeVoltage = abs(halfRangeVoltage) - deadbandVoltage / 2;
  adjustedHalfRangeVoltage *= invert?-1:1;
  if (adjustedHalfRangeVoltage == 0) return _isSetup;
  _ticksPerVolt = (int)0x7FFF / adjustedHalfRangeVoltage;
  Serial.println(_ticksPerVolt);
  pinMode(_pin,INPUT_PULLUP);

  _isSetup = true;
  return _isSetup;
}

bool Joystick::setup(int analogPin, Preferences prefs, const char* name, bool invert)
{
  prefs.begin(name,true);

  if(!prefs.isKey("NeutralVoltage")) {Serial.print("WARNING: No NeutralVoltage setting stored for "); Serial.println(name);}
  if(!prefs.isKey("HalfRangeVoltage")) {Serial.print("WARNING: No HalfRangeVoltage setting stored for "); Serial.println(name);}
  if(!prefs.isKey("DeadbandVoltage")) {Serial.print("WARNING: No DeadbandVoltage setting stored for "); Serial.println(name);}

  float neutralVoltage = prefs.getFloat("NeutralVoltage",DEFAULT_JOYSTICK_NEUTRAL_VOLTAGE);
  float halfRangeVoltage = prefs.getFloat("HalfRangeVoltage",DEFAULT_JOYSTICK_HALF_RANGE_VOLTAGE);
  float deadbandVoltage = prefs.getFloat("DeadbandVoltage",DEFAULT_JOYSTICK_DEADBAND_VOLTAGE);
  prefs.end();
  return setup(analogPin, neutralVoltage, halfRangeVoltage, deadbandVoltage, invert);
}

void Joystick::updateValue()
{
  _value = 0;
  _volts = 0;
  if(!isOK())

  {
    Serial.println("ERROR: joystick not OK");
  }
  int32_t millivolts = analogReadMilliVolts(_pin);
  _volts = millivolts / 1000.0;
  if (_volts > _upperNeutralVoltage)
  {
    _value = _ticksPerVolt * (_volts - _upperNeutralVoltage);
    // _value is overflowing and going positive
    if(_value > 1000){
      _value = -32767;
    }
    /*
    Serial.print("backwards, ");
    Serial.print(_ticksPerVolt);
    Serial.print(", ");
    Serial.print(_volts);
    Serial.print(", ");
    Serial.print(_lowerNeutralVoltage);
    Serial.print(", ");
    Serial.println(_value);
    if(_value > 0){
      delay(2000);
    }*/
  }
  else if (_volts < _lowerNeutralVoltage)
  {
    _value = _ticksPerVolt * (_volts - _lowerNeutralVoltage);
    // _value is overflowing and going negative
    if(_value < -1000){
      _value = 32767;
    }   
  }
}

void Joystick::loop()
{
  updateValue();
}

int16_t Joystick::getValue()
{
  return _value;
}

float Joystick::getVoltage()
{
  return _volts;
}

bool Joystick::isOK()
{
  return _isSetup;
}
