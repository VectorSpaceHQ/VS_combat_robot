#include "Arduino.h"
#include "drive_motor.h"
#include "limits.h"


DriveMotor::DriveMotor(){
  _isSetup = false;
}


bool DriveMotor::init(int PinA, int PinB, ledc_timer_t timerA,
                 ledc_channel_t channelA, ledc_channel_t channelB)
{
    _isSetup = true;
    _forwardChannel = channelA;
    _backwardChannel = channelB;

    pinMode(PinA, OUTPUT);
    pinMode(PinB, OUTPUT);
    timerA = LEDC_TIMER_2;

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .timer_num        = timerA,
        .freq_hz          = 1000,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel1 = {
        .gpio_num       = PinA,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = channelA,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = timerA,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel2 = {
        .gpio_num       = PinB,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = channelB,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = timerA,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel2));


    return _isSetup;
}


void DriveMotor::loop(int speed, bool enable){
    int cmd;
    if(!_isSetup) return;

    if(enable)
    {
        if(speed > 0)
        {
            cmd = map(speed,0,0x7fff,0,_maxCommand);
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _forwardChannel, cmd) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _forwardChannel) );
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _backwardChannel, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _backwardChannel) );

        } else if(speed < 0)
        {
            // last minute bug fix. remove this
            if (speed < -30000){
              speed = -32767;
            } //--------------------------------
            cmd = map(speed,0,-1*0x7fff,0,_maxCommand);
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _forwardChannel, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _forwardChannel) );
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _backwardChannel, cmd) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _backwardChannel) );
        } else { // brakes
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _forwardChannel, _maxCommand) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _forwardChannel) );
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _backwardChannel, _maxCommand) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _backwardChannel) );
        }
    } else {
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _forwardChannel, 0) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _forwardChannel) );
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _backwardChannel, 0) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _backwardChannel) );
    }
}
