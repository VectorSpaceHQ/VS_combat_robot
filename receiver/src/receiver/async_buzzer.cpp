#include "Arduino.h"
#include "async_buzzer.h"

AsyncBuzzer::AsyncBuzzer()
{
  _isSetup = false;
  _isEnabled = false;
}

bool AsyncBuzzer::setup(int pin, int pwmChannel)
{
  _pin = pin;
  pinMode(_pin, OUTPUT);
  delay(2000);

  ledc_timer_config_t ledc_timer = {
      .speed_mode       = LEDC_LOW_SPEED_MODE,
      .duty_resolution  = LEDC_TIMER_13_BIT,
      .timer_num        = _timer,
      .freq_hz          = 5000,
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


  _isSetup = true;
  return _isSetup;
}

void AsyncBuzzer::loop()
{
  if(_isSetup && _isEnabled && millis() >= _offTime)
  {
    // ledcWriteTone(_pwmChannel,0);
    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 0) );
    ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
    _isEnabled = false;
  }
}

void AsyncBuzzer::writeTone(int frequency, int duration)
{
  if(_isSetup){
    _offTime = millis()+duration;
    _isEnabled = true;
    ledcWriteTone(_pwmChannel,frequency);
  }
}

void AsyncBuzzer::honk(int frequency)
{
    for(int i=0;i<4;i++){
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 2000) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(50);
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 0) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(50);
    }
}

void AsyncBuzzer::comms()
{

  ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 1000) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(150);
        
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 0) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(50);

        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 1000) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(150);
        
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 0) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
}

void AsyncBuzzer::error()
{
  ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 6000) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(250);

        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 1000) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(600);
        
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 0) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
}


void AsyncBuzzer::ready()
{
  ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 1500) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(500);

        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 7000) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
        delay(500);
        
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, 0) );
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel) );
}
