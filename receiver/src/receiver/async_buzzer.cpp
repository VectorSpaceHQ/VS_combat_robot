#include "Arduino.h"
#include "async_buzzer.h"

AsyncBuzzer::AsyncBuzzer()
{
  _isSetup = false;
  _isEnabled = false;
}

bool AsyncBuzzer::setup(int pin, int pwmChannel)
{
  pinMode(pin,OUTPUT);
  ledcSetup(pwmChannel,DEFAULT_BUZZER_PWM_FREQUENCY,DEFAULT_BUZZER_PWM_RESOLUTION);
  ledcAttachPin(pin,pwmChannel);
  _pwmChannel = pwmChannel;
  _isSetup = true;
}

void AsyncBuzzer::loop()
{
  if(_isSetup && _isEnabled && millis() >= _offTime)
  {
    ledcWriteTone(_pwmChannel,0);
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