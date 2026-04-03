#ifndef DRIVE_MOTOR_H
#define DRIVE_MOTOR_H

#include "Arduino.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "hardware.h"

#define DEFAULT_DRIVE_PWM_FREQUENCY 100 //very high frequencies might blow up the RZ7886 driver chip. 5k is def bad, 1k is kinda bad.
#define DEFAULT_DRIVE_PWM_RESOLUTION LEDC_TIMER_8_BIT

class DriveMotor{
  public:
    DriveMotor();
    // bool setup(int forwardPin, int backwardPin, int forwardChannel, int backwardChannel,
    //            int pwmFrequency = DEFAULT_DRIVE_PWM_FREQUENCY,
    //            int pwmResolution = DEFAULT_DRIVE_PWM_RESOLUTION);
    bool init(int PinA, int PinB, 
              ledc_channel_t channelA,
              int pwm, bool invert = false);
    void loop(int speed, bool enable = true);
    void wake();
  private:
    bool _isSetup;
    ledc_channel_t _pwmChannel;
    ledc_timer_t _timer;
    int _maxCommand = (1<<DEFAULT_DRIVE_PWM_RESOLUTION) -1;
    int _lastSpeed = 0;
    int _sel0 = SEL0;
    int _pwmPin;
    int _pinA;
    int _pinB;
    int _deadband = 500;
    int _lastcommand = 0; //0 = forward, 1 = backward, 2 = brake, 3 = coast
    bool _invert;
};

#endif
