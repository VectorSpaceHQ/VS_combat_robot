#ifndef DRIVE_MOTOR_H
#define DRIVE_MOTOR_H

#include "Arduino.h"

#define DEFAULT_DRIVE_PWM_FREQUENCY 5000
#define DEFAULT_DRIVE_PWM_RESOLUTION 8

class DriveMotor{
  public:
    DriveMotor();
    bool setup(int forwardPin, int backwardPin, int forwardChannel, int backwardChannel, int pwmFrequency = DEFAULT_DRIVE_PWM_FREQUENCY, int pwmResolution = DEFAULT_DRIVE_PWM_RESOLUTION);
    void loop(int speed, bool enable = true);
  private:
    bool _isSetup;
    int _forwardChannel;
    int _backwardChannel;
    int _maxCommand;
};

#endif