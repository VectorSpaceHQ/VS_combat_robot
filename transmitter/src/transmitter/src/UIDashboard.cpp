#include "Arduino.h"
#include "UIDashboard.h"


UISpeedDisplay::UISpeedDisplay(UIWidget* next) : UIRows(nullptr,next),
  negativeSpeed(SpeedDisplayBarWidth, UISize::MAX_LEN, UIDirection::TopToDown),
  speedDivider(&negativeSpeed),
  positiveSpeed(SpeedDisplayBarWidth, UISize::MAX_LEN, UIDirection::DownToTop, &speedDivider)
{
  attachChildren(&positiveSpeed);
}

void UISpeedDisplay::setSpeed(int16_t speed)
{
  if(speed >= 0)
  {
    negativeSpeed.clearValue();
    positiveSpeed.setValue(2 * (uint16_t)speed);
  } else {
    positiveSpeed.clearValue();
    negativeSpeed.setValue(2 * (uint16_t)(-1*speed));
  }
}

UIDashboard::UIDashboard(UIWidget* next) : UIColumns(nullptr,next),
  rightSpeed(),
  rightSpeedPadding(UIExpansion::None, UIAlignment::Center, UISize(2,2), &rightSpeed),
  centerText(u8g2_font_squeezed_b6_tr  , UIAlignment::Center, &rightSpeedPadding),
  // center(UIExpansion::Both, UIAlignment::Center, &centerText, &rightSpeedPadding),
  leftSpeed(),
  leftSpeedPadding(UIExpansion::None, UIAlignment::Center, UISize(2,2), &leftSpeed, &centerText)
{
  attachChildren(&leftSpeedPadding);
}

void UIDashboard::setValues(CommandMessage commandMessage)
{
  rightSpeed.setSpeed(commandMessage.right_speed);
  leftSpeed.setSpeed(commandMessage.left_speed);
  if(commandMessage.weapon_speed > 0)
  {
    centerText.setText("WEAPON ON!");
  } else {
    centerText.setText("WEAPON OFF");
  }
  
}