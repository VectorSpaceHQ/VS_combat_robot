#ifndef WEAPON_H
#define WEAPON_H
#include "Arduino.h"
#include "driver/ledc.h"
#include "esp_err.h"

class Weapon {
public:
    Weapon(int Pin);
    bool setup();
    void arm();
    void disarm();
    void on();
    void off();
    void loop(int speed, bool enable=true);
private:
    const int _max_value = pow(2,13) - 1;
    bool _isSetup;
    int _pin;
    bool _value;
    bool _armed;
    int _on_time;
    int _no_throttle = _max_value * 0.056;
    int _full_throttle = _max_value * 0.094;
    int _neutral_throttle = _max_value * 0.075;
    ledc_channel_t _channel = LEDC_CHANNEL_4;
    ledc_timer_t _timer = LEDC_TIMER_0;
};

#endif
