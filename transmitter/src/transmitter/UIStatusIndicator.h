#pragma once

#include "Arduino.h"
#include "UiUiUi.h"

static const uint16_t iconWidth  = 12;
static const uint16_t iconHeight = 12;
static const uint8_t screen_icon_fault[] U8X8_PROGMEM = {
   0xff, 0x0f, 0x07, 0x0e, 0x0f, 0x0f, 0x9d, 0x0b, 0xf9, 0x09, 0xf1, 0x08,
   0xf1, 0x08, 0xf9, 0x09, 0x9d, 0x0b, 0x0f, 0x0f, 0x07, 0x0e, 0xff, 0x0f };

enum class UIStatusIcon {None, Info, Question, Warning, Fault};

class UIStatusIndicator : public UICards
{
  public:
    UIStatusIndicator(UIWidget *next = nullptr);
    void setStatusIcon(UIStatusIcon statusIcon);

  private:

    UIBitmap faultIcon;
    UIBitmap warningIcon;
    UIBitmap questionIcon;
    UIBitmap infoIcon;
};