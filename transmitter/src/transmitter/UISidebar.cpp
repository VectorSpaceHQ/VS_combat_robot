#pragma once

#include "Arduino.h"
#include "UISidebar.h"

UISidebar::UISidebar(const uint8_t* icon, UIWidget* next) : UIRows(&sidebarIcon, next),
  statusIndicator(),
  batteryIndicator(8,UISize::MAX_LEN,UIDirection::DownToTop),
  batteryEnvelope(UIExpansion::None,UIAlignment::Center, UISize(2,2), &batteryIndicator, &statusIndicator),
  wifiIndicator(&batteryIndicator),
  sidebarIcon(12,7,icon,&wifiIndicator)
{

}

void UISidebar::updateWifiStrength(int16_t rssi)
{

}

void UISidebar::updateBatteryLevel(uint16_t stateOfCharge)
{
  batteryIndicator.setValue(stateOfCharge);
}

void UISidebar::updateStatusIndicator(UIStatusIcon statusIcon)
{
  statusIndicator.setStatusIcon(statusIcon);
}