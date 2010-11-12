// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgScWidget.h"
#include "Button.h"

Button::Button(const ApHandle& hScene, const String& sPath)
:Widget(hScene, sPath)
,nState_(NoButtonState)
,bMouseDown_(0)
,bHasText_(0)
{
}

String Button::GetTextPath()
{
  return sPath_ + "/n_text";
}

Button::ButtonState Button::String2State(const String& sName)
{
  ButtonState nState = NoButtonState;

  if (0) {
  } else if (sName == Msg_ScWidget_ButtonState_Normal) {
    nState = NormalButtonState;
  } else if (sName == Msg_ScWidget_ButtonState_Down) {
    nState = DownButtonState;
  } else if (sName == Msg_ScWidget_ButtonState_High) {
    nState = HighButtonState;
  } else if (sName == Msg_ScWidget_ButtonState_Disabled) {
    nState = DisabledButtonState;
  }

  return nState;
}

void Button::SetState(ButtonState nState)
{
  if (nState_ != nState) {
    nState_ = nState;
    ShowState();
  }
}

void Button::Active(int bActive)
{
  bActive_ = bActive;
}

void Button::SetText(const String& sText)
{
  if (!Msg_Scene_ElementExists::_(hScene_, GetTextPath())) {
    Msg_Scene_CreateTextElement::_(hScene_, GetTextPath());
  }
  Msg_Scene_SetText::_(hScene_, GetTextPath(), sText);
  bHasText_ = 1;

  PositionText();
}

void Button::DeleteText()
{
  if (Msg_Scene_ElementExists::_(hScene_, GetTextPath())) {
    Msg_Scene_DeleteElement::_(hScene_, GetTextPath());
  }
  bHasText_ = 0;
}

void Button::SetFontFamily(const String& sFont)
{
  Msg_Scene_SetFontFamily::_(hScene_, GetTextPath(), sFont);

  if (bHasText_) {
    PositionText();
  }
}

void Button::SetFontSize(double fSize)
{
  Msg_Scene_SetFontSize::_(hScene_, GetTextPath(), fSize);

  if (bHasText_) {
    PositionText();
  }
}

void Button::SetFontFlags(int nFlags)
{
  Msg_Scene_SetFontFlags::_(hScene_, GetTextPath(), nFlags);

  if (bHasText_) {
    PositionText();
  }
}

void Button::SetFontColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  Msg_Scene_SetFillColor::_(hScene_, GetTextPath(), fRed, fGreen, fBlue, fAlpha);

  if (bHasText_) {
    PositionText();
  }
}

void Button::SetImageFile(ButtonState nState, const String& sFile)
{
  ButtonStateConfigListNode* pNode = states_.Find(nState);
  if (pNode == 0) {
    ButtonStateConfig state;
    states_.Set(nState, state);
    pNode = states_.Find(nState);
  }

  if (pNode != 0) {
    pNode->Value().sFile_ = sFile;

    double fW = 0.0;
    double fH = 0.0;
    Msg_Scene_GetImageSizeFromFile::_(hScene_, sFile, fW, fH);
    pNode->Value().fW_ = fW;
    pNode->Value().fH_ = fH;
  }
}

void Button::SetTextOffset(ButtonState nState, double fX, double fY)
{
  ButtonStateConfigListNode* pNode = states_.Find(nState);
  if (pNode == 0) {
    ButtonStateConfig state;
    states_.Set(nState, state);
    pNode = states_.Find(nState);
  }

  if (pNode != 0) {
    pNode->Value().fTextOffsetX_ = fX;
    pNode->Value().fTextOffsetY_ = fY;
    pNode->Value().bTextOffsetValid_ = 1;
  }
}

void Button::Create()
{
  Msg_Scene_CreateMouseSensor::_(hScene_, sPath_, fX_, fY_, fW_, fH_);
  Msg_Scene_CreateRectangle::_(hScene_, sPath_ + "/h_image", fX_, fY_, fW_, fH_);
}

void Button::ShowState()
{
  ButtonStateConfigListNode* pNode = states_.Find(nState_);
  if (pNode) {
    Msg_Scene_SetFillImageFile::_(hScene_, sPath_ + "/h_image", pNode->Value().sFile_);
    Msg_Scene_SetFillImageOffset::_(hScene_, sPath_ + "/h_image", fX_, - fY_ - pNode->Value().fH_);
    //Msg_Scene_SetFillImageOffset::_(hScene_, sPath_ + "/h_image", fX_ + pNode->Value().fX_, fY_ + pNode->Value().fY_);
  } else {
    Msg_Scene_SetFillColor::_(hScene_, sPath_ + "/h_image", 1, 0, 0, 1);
  }

  if (bHasText_) {
    PositionText();
  }
}

void Button::PositionText()
{
  double fTextX, fTextY;
  GetTextPos(fTextX, fTextY);
  Msg_Scene_SetPosition::_(hScene_, GetTextPath(), fTextX, fTextY);
}

void Button::GetTextPos(double& fTextX, double& fTextY)
{
  double fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY;
  if (Msg_Scene_MeasureText::_(hScene_, GetTextPath(), fTextBearingX, fTextBearingY, fTextW, fTextH, fTextAdvanceX, fTextAdvanceY)) {

    double fOffsetX = 0.0;
    double fOffsetY = 0.0;

    ButtonStateConfigListNode* pNode = states_.Find(nState_);
    if (pNode) {
      if (pNode->Value().bTextOffsetValid_) {
        fOffsetX = pNode->Value().fTextOffsetX_;
        fOffsetY = pNode->Value().fTextOffsetY_;
      } else {
        if (nState_ == Button::DownButtonState) {
          fOffsetX = 1.0;
          fOffsetY = -1.0;
        }
      }
    }

    fTextX = fX_ + fW_/2 - fTextW / 2 + fOffsetX;
    fTextY = fY_ + fH_/2 - fTextH / 2 + fOffsetY;

  } else {
    fTextX = 0;
    fTextY = 0;
  }
}

void Button::OnMouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  //apLog_Debug((LOG_CHANNEL, "SensorElement::MouseEvent", "event=%d button=%d x=%d y=%d inside=%d", pMsg->nEvent, pMsg->nButton, (int) pMsg->fX, (int) pMsg->fY, pMsg->bInside));
  if (bActive_) {
  
    switch (pMsg->nEvent) {
      case Msg_Scene_MouseEvent::MouseMove: {
        if (bMouseDown_) {
          if (pMsg->bInside) {
            SetState(DownButtonState);
          } else {
            SetState(NormalButtonState);
          }
        } else {
          SetState(HighButtonState);
        }
      } break;

      case Msg_Scene_MouseEvent::MouseDown: {
        if (pMsg->nButton == Msg_Scene_MouseEvent::LeftButton) {
          Msg_Scene_CaptureMouse::_(hScene_, sPath_);
          SetState(DownButtonState);
          bMouseDown_ = 1;
        }
      } break;

      case Msg_Scene_MouseEvent::MouseUp: {
        if (pMsg->nButton == Msg_Scene_MouseEvent::LeftButton) {
          if (bMouseDown_) {
            if (nState_ == DownButtonState) {
              DoClick();
            }
          }
          bMouseDown_ = 0;
          SetState(NormalButtonState);
          Msg_Scene_ReleaseMouse::_(hScene_);
        }
      } break;

      case Msg_Scene_MouseEvent::MouseOut: {
        SetState(NormalButtonState);
      } break;
    }

  } // if bActive_
}

void Button::DoClick()
{
  Msg_ScWidget_ButtonEvent msg;
  msg.hScene = hScene_;
  msg.sPath = sPath_;
  msg.nEvent = Msg_ScWidget_ButtonEvent::ButtonClick;
  msg.Send();
}

