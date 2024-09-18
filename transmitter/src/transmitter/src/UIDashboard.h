#pragma once

#include "Arduino.h"
#include "UiUiUi.h"
#include "common.h"

#define SpeedDisplayBarWidth 12

class UISpeedDisplay : public UIRows
{
  public:
    UISpeedDisplay(UIWidget* next = nullptr);
    void setSpeed(int16_t speed);

  private:
    UIProgressBar negativeSpeed;
    UIHorizontalLine speedDivider;
    UIProgressBar positiveSpeed;
};


class UIDashboard : public UIColumns
{
  public:
    UIDashboard(UIWidget* next = nullptr);
    void setValues(CommandMessage commandMessage);

  private:
    UISpeedDisplay rightSpeed;
    UIEnvelope rightSpeedPadding;
    UIEnvelope center;
    UISpeedDisplay leftSpeed;
    UIEnvelope leftSpeedPadding;
};