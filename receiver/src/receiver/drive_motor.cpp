#include "Arduino.h"
#include "drive_motor.h"
#include "limits.h"

DriveMotor::DriveMotor(){
  _isSetup = false;
}

bool DriveMotor::setup(int forwardPin, int backwardPin, int forwardChannel, int backwardChannel, int pwmFrequency, int pwmResolution)
{
  _isSetup = true;

  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);
  _isSetup &= ledcSetup(forwardChannel,pwmFrequency,pwmResolution) > 0;
  _isSetup &= ledcSetup(backwardChannel,pwmFrequency,pwmResolution) > 0;
  ledcAttachPin(forwardPin,forwardChannel);
  ledcAttachPin(backwardPin,backwardChannel);

  _forwardChannel = forwardChannel;
  _backwardChannel = backwardChannel;
  _maxCommand = (1<<pwmResolution) -1;

  return _isSetup;
}

void DriveMotor::loop(int speed, bool enable)
{
  if(!_isSetup) return;
  Serial.print("drive speed: ");
  Serial.println(speed);

  if(enable)
  {
    if(speed > 0)
    {
      ledcWrite(_forwardChannel,map(speed,0,0x7fff,0,_maxCommand));
      ledcWrite(_backwardChannel,0);
    } else if(speed < 0)
    {
      ledcWrite(_forwardChannel,0);
      ledcWrite(_backwardChannel,map(speed,0,-1*0x7fff,0,_maxCommand));
    } else {
      ledcWrite(_forwardChannel,_maxCommand);
      ledcWrite(_backwardChannel,_maxCommand);
    }
  } else {
    ledcWrite(_forwardChannel,0);
    ledcWrite(_backwardChannel,0);
  }
}
