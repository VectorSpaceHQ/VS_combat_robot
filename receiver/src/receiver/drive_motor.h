#ifndef DRIVE_MOTOR_H
#define DRIVE_MOTOR_H

#include "Arduino.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define DEFAULT_DRIVE_PWM_FREQUENCY 5000
#define DEFAULT_DRIVE_PWM_RESOLUTION 8

class DriveMotor{
  public:
    DriveMotor();
    bool setup(int forwardPin, int backwardPin, int forwardChannel, int backwardChannel,
               int pwmFrequency = DEFAULT_DRIVE_PWM_FREQUENCY,
               int pwmResolution = DEFAULT_DRIVE_PWM_RESOLUTION);
    bool init(int PinA, int PinB, ledc_timer_t timerA,
               ledc_channel_t channelA, ledc_channel_t channelB);
    void loop(int speed, bool enable = true);
  private:
    bool _isSetup;
    ledc_channel_t _forwardChannel;
    ledc_channel_t _backwardChannel;
    ledc_timer_t _timer;
    int _maxCommand = (1<<DEFAULT_DRIVE_PWM_RESOLUTION) -1;
};

#endif
