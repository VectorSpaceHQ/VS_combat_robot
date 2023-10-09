#pragma once

#include "Arduino.h"
#include "UiUiUi.h"
#include "UIStatusIndicator.h"

class UIMessageBanner : public UIColumns
{
  public:
    UIMessageBanner(UIWidget* next = nullptr);
    void setMessage(UIStatusIcon statusIcon, const char* title, const char* detail);
  private:
    UITextLine titleText;
    UITextLine detailText;
    UIRows textGroup;
    UIEnvelope textGroupPadding;
    UIVerticalLine statusIndicatorDivider;
    UIStatusIndicator statusIndicator;
    UIEnvelope statusIndicatorPadding;
};