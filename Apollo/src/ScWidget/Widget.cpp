// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Widget.h"
#include "Button.h"

void Widget::SetCoordinates(double fX, double fY, double fW, double fH)
{
  fX_ = fX;
  fY_ = fY;
  fW_ = fW;
  fH_ = fH;
}

Button* Widget::AsButton()
{
  Button* pWidget = (Button*) this;
  if (!pWidget->IsButton()) { throw ApException("Widget::AsButton not a button scene=" ApHandleFormat " path=" StringFormat "", ApHandleType(hScene_), StringType(sPath_)); }
  return pWidget;
}

