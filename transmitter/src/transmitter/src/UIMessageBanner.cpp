#include "Arduino.h"
#include "UIMessageBanner.h"

UIMessageBanner::UIMessageBanner(UIWidget *next) : UIColumns(nullptr, next),  
    detailText(u8g2_font_squeezed_r6_tr  , UIAlignment::CenterLeft),  
    titleText(u8g2_font_squeezed_b6_tr  , UIAlignment::CenterLeft, &detailText),
    textGroup(&titleText),
    textGroupPadding(UIExpansion::None, UIAlignment::Center, UISize(1,0), &textGroup),
    statusIndicatorDivider(&textGroupPadding),
    statusIndicator(),
    statusIndicatorPadding(UIExpansion::None, UIAlignment::Center, UISize(1,1), &statusIndicator, &statusIndicatorDivider)
{
  attachChildren(&statusIndicatorPadding);
}


void UIMessageBanner::setMessage(UIStatusIcon statusIcon, const char* title, const char* detail)
{
  statusIndicator.setStatusIcon(statusIcon);
  titleText.setText(title);
  detailText.setText(detail);
}