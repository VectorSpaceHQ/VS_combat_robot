#include "Arduino.h"
#include "weapon.h"


Weapon::Weapon(int Pin){
    _pin = Pin;
    _value = 0;
    _isSetup = true;
    _armed = false;
}

bool Weapon::setup(){
    _isSetup = true;
    pinMode(_pin, OUTPUT);
    // delay(3000); // IS this necessary??

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .timer_num        = _timer,
        .freq_hz          = 50,  // Set output frequency at 50 Hz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel1 = {
        .gpio_num       = _pin,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = _channel,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = _timer,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));

    return _isSetup;
}


void Weapon::arm() // https://www.helifreak.com/showthread.php?t=412147
{
    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, _neutral_throttle) );
    ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
    delay(1000);

    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, _no_throttle) );
    ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
    delay(1000);
    _armed = true;
}

void Weapon::disarm(){
    // Don't believe this is possible for BLHeli ESCs
}

void Weapon::on(){
    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, _full_throttle) );
    ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
}

void Weapon::off(){
    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, _no_throttle) );
    ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
}

void Weapon::loop(int speed, bool enable){
    if(!_isSetup) return;
    if(speed > 20 && enable){
        this->on();
    }
    else{
        this->off();
    }
}
