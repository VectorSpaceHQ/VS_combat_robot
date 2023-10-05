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
  ledcSetup(pwmChannel, DEFAULT_BUZZER_PWM_FREQUENCY, DEFAULT_BUZZER_PWM_RESOLUTION);
  ledcAttachPin(pin, pwmChannel);
  _pwmChannel = pwmChannel;
  _isSetup = true;
  return _isSetup;
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

void AsyncBuzzer::honk(int frequency)
{
  ledcWriteTone(_pwmChannel, frequency);
  delay(50);
  ledcWriteTone(_pwmChannel, 0);
  delay(50);
  ledcWriteTone(_pwmChannel, frequency);
  delay(50);
  ledcWriteTone(_pwmChannel, 0);
  delay(50);
  ledcWriteTone(_pwmChannel, frequency);
  delay(50);
  ledcWriteTone(_pwmChannel, 0);
  delay(20);
}

void AsyncBuzzer::comms()
{
  ledcWriteTone(_pwmChannel, 300);
  delay(150);
  ledcWriteTone(_pwmChannel, 300);
  delay(150);
  ledcWriteTone(_pwmChannel, 300);
  delay(150);
  ledcWriteTone(_pwmChannel, 0);
  delay(20);
}

void AsyncBuzzer::error()
{
  ledcWriteTone(_pwmChannel, 550);
  delay(250);
  ledcWriteTone(_pwmChannel, 150);
  delay(600);
  ledcWriteTone(_pwmChannel, 0);
  delay(20);
}
