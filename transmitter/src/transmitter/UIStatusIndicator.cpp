#pragma once

#include "Arduino.h"
#include "UIStatusIndicator.h"

UIStatusIndicator::UIStatusIndicator(UIWidget *next) : UICards(&infoIcon, next),
  faultIcon(iconWidth, iconHeight, screen_icon_fault),
  warningIcon(iconWidth, iconHeight, screen_icon_fault, &faultIcon),
  questionIcon(iconWidth, iconHeight, screen_icon_fault, &warningIcon),
  infoIcon(iconWidth, iconHeight, screen_icon_fault, &questionIcon)
{

}

void UIStatusIndicator::setStatusIcon(UIStatusIcon statusIcon)
{
  switch(statusIcon)
  {
    case UIStatusIcon::Info:
      setVisibleWidget(&infoIcon);
      break;
    case UIStatusIcon::Question:
      setVisibleWidget(&questionIcon);
      break;\
    case UIStatusIcon::Warning:
      setVisibleWidget(&warningIcon);
      break;
    case UIStatusIcon::Fault:
      setVisibleWidget(&faultIcon);
      break;
    default:
      hide();
  }
}