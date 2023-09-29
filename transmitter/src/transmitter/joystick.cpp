#include "Arduino.h"
#include <Preferences.h>
#include "joystick.h"

Joystick::Joystick(){
  _isSetup = false;
  _value = 0;
}

bool Joystick::setup(int analogPin, float neutralVoltage, float halfRangeVoltage, float deadbandVoltage)
{
  _pin = analogPin;
  _upperNeutralVoltage = neutralVoltage + deadbandVoltage / 2;
  _lowerNeutralVoltage = neutralVoltage - deadbandVoltage / 2;
  float adjustedHalfRangeVoltage = halfRangeVoltage - deadbandVoltage / 2;
  if (adjustedHalfRangeVoltage = 0) return false;
  _ticksPerVolt = (float)(0x7FFF) / adjustedHalfRangeVoltage;

  pinMode(_pin,INPUT_PULLUP);

  _isSetup = true;
  return true;
}

bool Joystick::setup(int analogPin, Preferences prefs, const char* name)
{
  prefs.begin(name,true);
  
  if(!prefs.isKey("NeutralVoltage")) {Serial.print("WARNING: No NeutralVoltage setting stored for "); Serial.println(name);}
  if(!prefs.isKey("HalfRangeVoltage")) {Serial.print("WARNING: No HalfRangeVoltage setting stored for "); Serial.println(name);}
  if(!prefs.isKey("DeadbandVoltage")) {Serial.print("WARNING: No DeadbandVoltage setting stored for "); Serial.println(name);}

  float neutralVoltage = prefs.getFloat("NeutralVoltage",DEFAULT_JOYSTICK_NEUTRAL_VOLTAGE);
  float halfRangeVoltage = prefs.getFloat("HalfRangeVoltage",DEFAULT_JOYSTICK_HALF_RANGE_VOLTAGE);
  float deadbandVoltage = prefs.getFloat("DeadbandVoltage",DEFAULT_JOYSTICK_DEADBAND_VOLTAGE);
  prefs.end();
  return setup(analogPin, neutralVoltage, halfRangeVoltage, deadbandVoltage);
}

int16_t Joystick::readValue()
{
  if(!_isSetup) return 0;
  int32_t millivolts = analogReadMilliVolts(_pin);
  float volts = millivolts / 1000.0;
  if (volts > _upperNeutralVoltage)
  {
    return _ticksPerVolt * (volts - _upperNeutralVoltage);
  } 
  else if (volts < _lowerNeutralVoltage) 
  {
    return _ticksPerVolt * (volts - _upperNeutralVoltage);
  }
  else
  {
    return 0;
  }
}

void Joystick::loop()
{
  _value = readValue();
}

int16_t Joystick::getValue()
{
  return _value;
}

bool Joystick::isOK()
{
  return _isSetup && (_value != 0);
}