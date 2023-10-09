#pragma once

#include "Arduino.h"
#include "UIStatusIndicator.h"

UIStatusIndicator::UIStatusIndicator(UIWidget *next) : UICards(nullptr, next),
  faultIcon(statusIconWidth, statusIconHeight, screen_icon_fault),
  warningIcon(statusIconWidth, statusIconHeight, screen_icon_warning, &faultIcon),
  questionIcon(statusIconWidth, statusIconHeight, screen_icon_question, &warningIcon),
  infoIcon(statusIconWidth, statusIconHeight, screen_icon_info, &questionIcon)
{
  attachChildren(&infoIcon);
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


UIWifiIndicator::UIWifiIndicator(UIWidget *next) : UICards(nullptr, next),
  wifi4(wifiIconWidth, wifiIconHeight, wifi_bitmap4),
  wifi3(wifiIconWidth, wifiIconHeight, wifi_bitmap3, &wifi4),
  wifi2(wifiIconWidth, wifiIconHeight, wifi_bitmap2, &wifi3),
  wifi1(wifiIconWidth, wifiIconHeight, wifi_bitmap1, &wifi2)
{
  attachChildren(&wifi1);
}

void UIWifiIndicator::setWifiIcon(uint16_t strength)
{
  if(strength == 0)
  {
    hide();
  } else if(strength <= 0x3FFF)
  {
    setVisibleWidget(&wifi1);
  }else if(strength <= 0x7FFF)
  {
    setVisibleWidget(&wifi2);
  }else if(strength <= 0xAFFF)
  {
    setVisibleWidget(&wifi3);
  }else
  {
    setVisibleWidget(&wifi4);
  }
}