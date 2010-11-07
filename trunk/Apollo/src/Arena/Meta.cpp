// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgScene.h"
#include "ColorString.h"
#include "Local.h"
#include "Meta.h"
#include "Display.h"

Meta::Meta(Display* pDisplay)
:Layer(pDisplay)
,nW_(0)
,nH_(0)
,nState_(NoState)
{
  Msg_Scene_CreateRectangle::_(hScene_, ELEMENT_TRAY, 0.5, 0.5, 20, 20);
  Msg_Scene_SetStrokeColor::_(hScene_, ELEMENT_TRAY, 0, 0, 0, 1);
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, 1, 1, 1, 1);
}

Meta::~Meta()
{
  Msg_Scene_DeleteElement::_(hScene_, ELEMENT_TRAY);
}

void Meta::OnSetSize(int nW, int nH)
{
  nW_ = nW;
  nH_ = nH;

  ShowText();
}

//---------------------------

void Meta::OnEnterRequested()
{
  nState_ = StateEnterRequested;
  SetTrayColor(0.8, 0.8, 1);
}

void Meta::OnEnterBegin()
{
  nState_ = StateEnterBegin;
  SetTrayColor(0.5, 0.5, 1);
}

void Meta::OnEnterComplete()
{
  nState_ = StateEnterComplete;
  SetTrayColor(0, 0, 1);
}

void Meta::OnLeaveRequested()
{
  nState_ = StateLeaveRequested;
  SetTrayColor(1, 0, 0);
}

void Meta::OnLeaveBegin()
{
  nState_ = StateLeaveBegin;
  SetTrayColor(1, 0.6, 0.6);
}

void Meta::OnLeaveComplete()
{
  nState_ = StateLeaveComplete;
  SetTrayColor(1, 1, 1);
}

void Meta::OnDocumentUrl(const String& sUrl)
{
  sDocumentUrl_ = sUrl;
  ShowText();
}

void Meta::OnLocationUrl(const String& sUrl)
{
  sLocationUrl_ = sUrl;
  ShowText();
}

//---------------------------

void Meta::SetTrayColor(double r, double g, double b)
{
  Msg_Scene_SetFillColor::_(hScene_, ELEMENT_TRAY, r, g, b, 1);
}

void Meta::ShowText()
{
  String sTextPath = "d_meta";

  String sText;
  if (sDocumentUrl_) {
    sText += sDocumentUrl_;
  }
  if (sLocationUrl_) {
    if (sText) { sText += " "; }
    sText += sLocationUrl_;
  }

  if (Msg_Scene_ElementExists::_(hScene_, sTextPath)) {
    Msg_Scene_DeleteElement::_(hScene_, sTextPath);
  }

  if (sText) {
    String sFont = "Arial Narrow";
    int nSize = 11;
    int nFlags = 0;
    Apollo::ColorString cText = "#000000";
    Apollo::ColorString cBackground = "#ffffff";
    Apollo::ColorString cBorder = "#000000";
    int nWidth = 200;
    int nLeft = 2.5;
    int nTop = 2.5;
    double fBorderWidth = 1.0;
    double fTextPadding = 1.5;

    double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
    Msg_Scene_GetTextExtents::_(hScene_, sText, sFont, nSize, nFlags, fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY);

    Msg_Scene_CreateElement::_(hScene_, sTextPath);
    Msg_Scene_TranslateElement::_(hScene_, sTextPath, nLeft, nH_ - fTextH - nTop);

    Msg_Scene_CreateRectangle::_(hScene_, sTextPath + "/" ELEMENT_NICKNAME_BOX, -fTextPadding, -fTextPadding, fTextAdvanceX + 2 * fTextPadding + 1, fTextH + 2 * fTextPadding);
    Msg_Scene_SetFillColor::_(hScene_, sTextPath + "/" ELEMENT_NICKNAME_BOX, cBackground.r, cBackground.g, cBackground.b, cBackground.a);
    Msg_Scene_SetStrokeColor::_(hScene_, sTextPath + "/" ELEMENT_NICKNAME_BOX, cBorder.r, cBorder.g, cBorder.b, cBorder.a);
    Msg_Scene_SetStrokeWidth::_(hScene_, sTextPath + "/" ELEMENT_NICKNAME_BOX, fBorderWidth);

    Msg_Scene_CreateText::_(hScene_, sTextPath + "/" ELEMENT_NICKNAME_TEXT, 0, 0, sText, sFont, nSize, nFlags);
    Msg_Scene_SetFillColor::_(hScene_, sTextPath + "/" ELEMENT_NICKNAME_TEXT, cText.r, cText.g, cText.b, cText.a);
  }
}

