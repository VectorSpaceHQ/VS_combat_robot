#ifndef JOYSTICK_H
#define JOYSTICK_H
#include "Arduino.h"
#include <Preferences.h>

#define DEFAULT_JOYSTICK_NEUTRAL_VOLTAGE 1.24
#define DEFAULT_JOYSTICK_HALF_RANGE_VOLTAGE 1.18
#define DEFAULT_JOYSTICK_DEADBAND_VOLTAGE 0.1

class Joystick {
  public:
    Joystick();
    bool setup(int analogPin, float neutralVoltage, float halfRangeVoltage, float deadbandVoltage);
    bool setup(int analogPin, Preferences prefs, const char* name);
    void loop();            //sample the analog input
    int16_t getValue();     //return value as a bidirectional ratio
    bool isOK();            //return true if there are no issues (config, over/under voltage)
  private:
    bool _isSetup;
    int _pin;
    float _upperNeutralVoltage;
    float _lowerNeutralVoltage;
    float _ticksPerVolt;
    int16_t _value;

    int16_t readValue();//does the hardware read and conversion
};

#endif