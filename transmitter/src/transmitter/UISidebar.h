#pragma once

#include "Arduino.h"
#include "UiUiUi.h"
#include "UIStatusIndicator.h"

static const uint8_t screen_icon_controller[] U8X8_PROGMEM = { //12x7
   0xfc, 0x03, 0xfe, 0x07, 0xff, 0x0f, 0xf3, 0x0c, 0xf3, 0x0c, 0x9f, 0x0f,
   0x0e, 0x07 };
static const uint8_t screen_icon_robot[] U8X8_PROGMEM = { //12x7
   0xf8, 0x01, 0x08, 0x01, 0xfb, 0x0d, 0xfb, 0x0d, 0xff, 0x0f, 0xfb, 0x0d,
   0x03, 0x0c };

class UISidebar : public UIRows
{
  public:
    UISidebar(const uint8_t* icon, UIWidget* next = nullptr);

    void updateWifiStrength(int16_t rssi);//probably a number between -50 and -80, where larger (-50) is a 'better' strength
    void updateBatteryLevel(uint16_t stateOfCharge);//0 - 2^16-1 represents 0% - 100%
    void updateStatusIndicator(UIStatusIcon statusIcon);

  private:

    UIStatusIndicator statusIndicator;
    UIEnvelope statusIndicatorPadding;
    UIProgressBar batteryIndicator;
    UIBorder batteryBorder;
    UIEnvelope batteryPadding;
    UIWifiIndicator wifiIndicator;
    UIEnvelope wifiIndicatorPadding;
    UIBitmap sidebarIcon;
    UIEnvelope sidebarIconPadding;
};