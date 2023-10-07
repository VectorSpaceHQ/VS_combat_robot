#ifndef ASYNC_BUZZER_H
#define ASYNC_BUZZER_H

#include "Arduino.h"
#include "driver/ledc.h"
#include "esp_err.h"

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
    void ready();
  private:
    int _pin;
    bool _isSetup;
    bool _isEnabled;
    int _pwmChannel;
    long _offTime;
    ledc_channel_t _channel = LEDC_CHANNEL_5;
    ledc_timer_t _timer = LEDC_TIMER_1;
};

#endif
