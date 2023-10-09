#pragma once

#include "Arduino.h"
#include "UISidebar.h"

UISidebar::UISidebar(const uint8_t* icon, UIWidget* next) : UIRows(nullptr, next),
  statusIndicator(),
  statusIndicatorPadding(UIExpansion::None, UIAlignment::Center, UISize(1,1), &statusIndicator),
  batteryIndicator(8,UISize::MAX_LEN,UIDirection::DownToTop),
  batteryBorder(1,&batteryIndicator),
  batteryPadding(UIExpansion::None,UIAlignment::Center, UISize(1,1), &batteryBorder, &statusIndicatorPadding),
  wifiIndicator(),
  wifiIndicatorPadding(UIExpansion::None, UIAlignment::Center, UISize(1,1), &wifiIndicator, &batteryPadding),
  sidebarIcon(12,7,icon),
  sidebarIconPadding(UIExpansion::None, UIAlignment::Center, UISize(1,1), &sidebarIcon,&wifiIndicatorPadding)
{
  attachChildren(&sidebarIconPadding);
}

void UISidebar::updateWifiStrength(int16_t rssi)
{
  uint16_t strength = map(rssi,-80,-40,0,0xFFFF);
  wifiIndicator.setWifiIcon(strength);
}

void UISidebar::updateBatteryLevel(uint16_t stateOfCharge)
{
  batteryIndicator.setValue(stateOfCharge);
}

void UISidebar::updateStatusIndicator(UIStatusIcon statusIcon)
{
  statusIndicator.setStatusIcon(statusIcon);
}