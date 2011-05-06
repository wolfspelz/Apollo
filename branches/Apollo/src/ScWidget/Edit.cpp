// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgScWidget.h"
#include "Edit.h"

Edit::Edit(const ApHandle& hScene, const String& sPath)
:Widget(hScene, sPath)
,fSize_(12.0)
,nFlags_(0)
,nAlignH_(AlignLeft)
,nAlignV_(AlignTop)
,fPadding_(1.0)
,bHasFocus_(0)
,nInsertPos_(0)
{
}

String Edit::GetTextPath() { return sPath_ + "/n_text"; }
String Edit::GetInsertPath() { return sPath_ + "/p_insert"; }

void Edit::Create()
{
  Msg_Scene_CreateMouseSensor::_(hScene_, sPath_, fX_, fY_, fW_, fH_);
  Msg_Scene_CreateTextElement::_(hScene_, GetTextPath());

  sFont_ = "Arial";
  fSize_ = 12.0;
  nFlags_ = 0;

  Msg_Scene_SetFontFamily::_(hScene_, GetTextPath(), sFont_);
  Msg_Scene_SetFontSize::_(hScene_, GetTextPath(), fSize_);
  Msg_Scene_SetFontFlags::_(hScene_, GetTextPath(), nFlags_);
  Msg_Scene_SetFillColor::_(hScene_, GetTextPath(), 0, 0, 0, 1);

  Msg_Scene_CreateRectangleElement::_(hScene_, GetInsertPath());
  Msg_Scene_SetFillColor::_(hScene_, GetInsertPath(), 0, 0, 0, 1);
}

void Edit::OnMouseEvent(Msg_Scene_MouseEvent* pMsg)
{
  //apLog_Debug((LOG_CHANNEL, "SensorElement::MouseEvent", "event=%d button=%d x=%d y=%d inside=%d", pMsg->nEvent, pMsg->nButton, (int) pMsg->fX, (int) pMsg->fY, pMsg->bInside));

  if (pMsg->nEvent == Msg_Scene_MouseEvent::MouseDown) {
    if (pMsg->nButton == Msg_Scene_MouseEvent::LeftButton) {
      if (pMsg->bInside) {
        Msg_Scene_SetKeyboardFocus::_(hScene_, pMsg->sPath);
      }
    }
  }
}

void Edit::OnKeyboardFocusEvent(Msg_Scene_KeyboardFocusEvent* pMsg)
{
  //apLog_Debug((LOG_CHANNEL, "Edit::OnKeyboardFocusEvent", "path=%s focus=%d", StringType(pMsg->sPath), pMsg->bFocus));
}

void Edit::OnKeyEvent(Msg_Scene_KeyEvent* pMsg)
{
  //apLog_Debug((LOG_CHANNEL, "Edit::OnKeyEvent", "path=%s ev=%d key=%d unicode=%08x char=%s", StringType(pMsg->sPath), pMsg->nEvent, pMsg->nKey, String::UTF8_Char(pMsg->sChar), StringType(pMsg->sChar)));
  //::MessageBox(NULL, (PTSTR) pMsg->sKey, _T("Test"), MB_OK);

  switch (pMsg->nEvent) {
    case Msg_Scene_KeyEvent::Char:
      {
        String sLeft = sText_.subString(0, nInsertPos_);
        String sRight = sText_.subString(nInsertPos_);
        sLeft += pMsg->sChar;
        nInsertPos_ += pMsg->sChar.chars();
        sText_ = sLeft + sRight;
      } break;

    case Msg_Scene_KeyEvent::KeyDown:
      {
        switch (pMsg->nKey) {
          case Msg_Scene_KeyEvent::MoveLeft:
            {
              nInsertPos_--;
              if (nInsertPos_ < 0) {
                nInsertPos_ = 0;
              }
            } break;

          case Msg_Scene_KeyEvent::MoveRight:
            {
              nInsertPos_++;
              if (nInsertPos_ > sText_.chars()) {
                nInsertPos_ = sText_.chars();
              }
            } break;

          case Msg_Scene_KeyEvent::DeleteLeft:
            {
              if (sText_.chars() > 0) {
                String sLeft = sText_.subString(0, nInsertPos_);
                String sRight = sText_.subString(nInsertPos_);
                sLeft = sLeft.subString(0, nInsertPos_ - 1);
                sText_ = sLeft + sRight;
                nInsertPos_--;
                if (nInsertPos_ < 0) {
                  nInsertPos_ = 0;
                }
              }
            } break;

          case Msg_Scene_KeyEvent::DeleteRight:
            {
              if (sText_.chars() > 0) {
                String sLeft = sText_.subString(0, nInsertPos_);
                String sRight = sText_.subString(nInsertPos_);
                sRight = sRight.subString(1);
                sText_ = sLeft + sRight;
                //nInsertPos_--;
              }
            } break;

          case Msg_Scene_KeyEvent::Newline:
            {
              DoEnter();
            } break;

        }
      } break;

    case Msg_Scene_KeyEvent::KeyUp:
      {
      } break;
  }

  ShowText();
}

String Edit::GetText()
{
  return sText_;
}

void Edit::SetAlign(int nAlignH, int nAlignV)
{
  nAlignH_ = nAlignH;
  nAlignV_ = nAlignV;
  PositionText();
}

void Edit::SetPadding(double fPadding)
{
  fPadding_ = fPadding;
  PositionText();
}

void Edit::SetFontFamily(const String& sFont)
{
  sFont_ = sFont;
  Msg_Scene_SetFontFamily::_(hScene_, GetTextPath(), sFont);
  PositionText();
}

void Edit::SetFontSize(double fSize)
{
  fSize_ = fSize;
  Msg_Scene_SetFontSize::_(hScene_, GetTextPath(), fSize);
  PositionText();
}

void Edit::SetFontFlags(int nFlags)
{
  nFlags_ = nFlags;
  Msg_Scene_SetFontFlags::_(hScene_, GetTextPath(), nFlags);
  PositionText();
}

void Edit::SetFontColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  Msg_Scene_SetFillColor::_(hScene_, GetTextPath(), fRed, fGreen, fBlue, fAlpha);
  PositionText();
}

void Edit::SetText(const String& sText)
{
  sText_ = sText;
  nInsertPos_ = sText_.chars();

  ShowText();
}

class SceneDrawingSuspender
{
public:
  SceneDrawingSuspender(ApHandle hScene) : hScene_(hScene) { Msg_Scene_AutoDrawSuspend::_(hScene_); }
  ~SceneDrawingSuspender() {  Msg_Scene_AutoDrawResume::_(hScene_); }
protected:
  ApHandle hScene_;
};

void Edit::ShowText()
{
  SceneDrawingSuspender s(hScene_);

  Msg_Scene_SetText::_(hScene_, GetTextPath(), sText_);
  PositionText();
}

void Edit::PositionText()
{
  double fTextX, fTextY, fTextW, fTextH;
  GetTextRect(fTextX, fTextY, fTextW, fTextH);
  Msg_Scene_SetPosition::_(hScene_, GetTextPath(), fTextX, fTextY);

  double fInsertX = fTextX;
  double fInsertY = fTextY;
  String sLeft = sText_.subString(0, nInsertPos_);
  double fLeftBearingX, fLeftBearingY, fLeftWidth, fLeftHeight, fLeftAdvanceX, fLeftAdvanceY;
  if (Msg_Scene_GetTextExtents::_(hScene_, sLeft, sFont_, fSize_, nFlags_, fLeftBearingX, fLeftBearingY, fLeftWidth, fLeftHeight, fLeftAdvanceX, fLeftAdvanceY)) {
    fInsertX = fTextX + fLeftAdvanceX;
  }

  Msg_Scene_SetRectangle::_(hScene_, GetInsertPath(), fInsertX - 0.5, fInsertY - 2, 1, fTextH + 4);
}

void Edit::GetTextRect(double& fTextX, double& fTextY, double& fTextW, double& fTextH)
{
  double fLineHeight = fSize_;
  double fEmBearingX, fEmBearingY, fEmWidth, fEmHeight, fEmAdvanceX, fEmAdvanceY;
  if (Msg_Scene_GetTextExtents::_(hScene_, "M", sFont_, fSize_, nFlags_, fEmBearingX, fEmBearingY, fEmWidth, fEmHeight, fEmAdvanceX, fEmAdvanceY)) {
    fLineHeight = fEmHeight;
  }

  double fTextBearingX, fTextBearingY, fTextWidth, fTextHeight, fTextAdvanceX, fTextAdvanceY;
  if (Msg_Scene_MeasureText::_(hScene_, GetTextPath(), fTextBearingX, fTextBearingY, fTextWidth, fTextHeight, fTextAdvanceX, fTextAdvanceY)) {

    switch (nAlignH_) {
    case AlignLeft: { fTextX = fX_ + fPadding_; } break;
    case AlignCenter: { fTextX = fX_ + fW_ / 2 - fTextAdvanceX / 2; } break;
    case AlignRight: { fTextX = fX_ + fW_ - fPadding_ - fTextAdvanceX; } break;
    default: fTextX = fX_;
    }

    switch (nAlignV_) {
    case AlignTop: { fTextY = fY_ + fH_ - fPadding_ - fLineHeight; } break;
    case AlignMiddle: { fTextY = fY_ + fH_ / 2 - fLineHeight / 2; } break;
    case AlignBottom: { fTextY = fY_ + fPadding_; } break;
    default: fTextY = fY_;
    }

    fTextW = fTextWidth;
    fTextH = fLineHeight;

  } else {
    fTextX = 0;
    fTextY = 0;
    fTextW = 0;
    fTextH = 0;
  }

}

void Edit::DoEnter()
{
  Msg_ScWidget_EditEvent msg;
  msg.hScene = hScene_;
  msg.sPath = sPath_;
  msg.nEvent = Msg_ScWidget_EditEvent::EditEnter;
  msg.Send();
}

