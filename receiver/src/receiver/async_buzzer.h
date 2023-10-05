#ifndef ASYNC_BUZZER_H
#define ASYNC_BUZZER_H

#include "Arduino.h"

#define DEFAULT_BUZZER_PWM_FREQUENCY 5000
#define DEFAULT_BUZZER_PWM_RESOLUTION 8

class AsyncBuzzer {
  public:
    AsyncBuzzer();
    bool setup(int pin, int pwmChannel);
    void loop();
    void writeTone(int frequency, int duration);
    void honk(int frequency);
    void comms();
    void error();
  private:
    bool _isSetup;
    bool _isEnabled;
    int _pwmChannel;
    long _offTime;
};

#endif