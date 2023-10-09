#include "Arduino.h"
#include "UIDashboard.h"


UISpeedDisplay::UISpeedDisplay(UIWidget* next) : UIRows(nullptr,next),
  negativeSpeed(SpeedDisplayBarWidth, UISize::MAX_LEN, UIDirection::TopToDown),
  speedDivider(&negativeSpeed),
  positiveSpeed(SpeedDisplayBarWidth, UISize::MAX_LEN, UIDirection::TopToDown, &speedDivider)
{
  attachChildren(&positiveSpeed);
}

void UISpeedDisplay::setSpeed(int16_t speed)
{
  if(speed >= 0)
  {
    negativeSpeed.clearValue();
    positiveSpeed.setValue((uint16_t)speed);
  } else {
    positiveSpeed.clearValue();
    negativeSpeed.setValue((uint16_t)(-1*speed));
  }
}

UIDashboard::UIDashboard(UIWidget* next) : UIColumns(nullptr,next),
  rightSpeed(),
  rightSpeedPadding(UIExpansion::None, UIAlignment::Center, UISize(2,2), &rightSpeed),
  center(UIExpansion::Both, UIAlignment::Center, nullptr, &rightSpeedPadding),
  leftSpeed(),
  leftSpeedPadding(UIExpansion::None, UIAlignment::Center, UISize(2,2), &leftSpeed, &center)
{
  attachChildren(&leftSpeedPadding);
}

void UIDashboard::setValues(CommandMessage commandMessage)
{
  rightSpeed.setSpeed(commandMessage.right_speed);
  leftSpeed.setSpeed(commandMessage.left_speed);
}