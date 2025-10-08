#include "Arduino.h"
#include "drive_motor.h"
#include "limits.h"


DriveMotor::DriveMotor(){
  _isSetup = false;
}


bool DriveMotor::init(int PinA, int PinB, 
                ledc_channel_t channelA,
                int pwm)
{
    Serial.println("Initializing drive motor");
    _isSetup = true;
    _pwmChannel = channelA;
    _pwmPin = pwm;
    _pinA = PinA;
    _pinB = PinB;

    pinMode(PinA, OUTPUT);
    pinMode(PinB, OUTPUT);

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = DEFAULT_DRIVE_PWM_RESOLUTION,
        .timer_num        = LEDC_TIMER_2,
        .freq_hz          = DEFAULT_DRIVE_PWM_FREQUENCY,  // Set carrier frequency of PWM
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel1 = {
        .gpio_num       = _pwmPin,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = _pwmChannel,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_2,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));

    this->wake(); // wake up motor driver

    return _isSetup;
}


void DriveMotor::loop(int speed, bool enable){
    int cmd;
    if(!_isSetup) return;

    if(enable)
    {
        if(speed > _deadband)
        {
            if (_lastcommand == 1) {
                digitalWrite(_pinA, 0);
                digitalWrite(_pinB, 0);
                ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _pwmChannel, _maxCommand) );
                ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _pwmChannel) );
                delay(60); // if changing direction, wait 100ms to avoid a big current spike
            }
            cmd = map(speed,0,0x7fff,0,_maxCommand);
            digitalWrite(_pinA, 0);
            digitalWrite(_pinB, 1);

            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _pwmChannel, cmd) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _pwmChannel) );

            _lastcommand = 0;
        } 
        else if(speed < -_deadband)
        {
            if (_lastcommand == 0) {
                digitalWrite(_pinA, 0);
                digitalWrite(_pinB, 0);
                ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _pwmChannel, _maxCommand) );
                ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _pwmChannel) );
                delay(60); // if changing direction, wait 100ms to avoid a big current spike
            }
            cmd = map(speed,0,-1*0x7fff,0,_maxCommand);
            digitalWrite(_pinA, 1);
            digitalWrite(_pinB, 0);
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _pwmChannel, cmd) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _pwmChannel) );

            _lastcommand = 1;
        } 
        else { // brakes
            digitalWrite(_pinA, 0);
            digitalWrite(_pinB, 0);
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _pwmChannel, _maxCommand) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _pwmChannel) );
            _lastcommand = 2;
        }
    } else { // disable motor (coast)
            digitalWrite(_pinA, 0);
            digitalWrite(_pinB, 0);
            ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _pwmChannel, 0) );
            ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _pwmChannel) );
    }
}

void DriveMotor::wake(){
    // The VNH7100 must be woken out of standby.
    // Toggle INA from 0 to 1
    // Toggle PWM from 0 to 1 with a 20us delay.
    pinMode(_sel0, OUTPUT);

    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _pwmChannel, 0) );
    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _pwmChannel, _maxCommand) );

    digitalWrite(_sel0, 0);
    delayMicroseconds(20);
    digitalWrite(_sel0, 1);

    Serial.println("Drive motor enabled");
}