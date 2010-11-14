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
{
}

String Edit::GetTextPath()
{
  return sPath_ + "/n_text";
}

void Edit::Create()
{
  Msg_Scene_CreateMouseSensor::_(hScene_, sPath_, fX_, fY_, fW_, fH_);
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

void Edit::OnKeyEvent(Msg_Scene_KeyEvent* pMsg)
{
  //apLog_Debug((LOG_CHANNEL, "SensorElement::KeyEvent", "path=%s unicode=%08x key=%s", StringType(pMsg->sPath), String::UTF8_Char(pMsg->sKey), StringType(pMsg->sKey)));
  //::MessageBox(NULL, (PTSTR) pMsg->sKey, _T("Test"), MB_OK);

  if (String::UTF8_Char(pMsg->sKey) == 8) {
    sText_ = sText_.subString(0, sText_.chars() - 1);
  } else {
    sText_ += pMsg->sKey;
  }
  ShowText();
}

void Edit::SetText(const String& sText)
{
  sText_ = sText;
  ShowText();
}

void Edit::ShowText()
{
  if (!Msg_Scene_ElementExists::_(hScene_, GetTextPath())) {
    Msg_Scene_CreateText::_(hScene_, GetTextPath(), fX_, fY_ + fH_ / 2 - 12.0 / 2, sText_, "Verdana", 12, 0);
    Msg_Scene_SetFillColor::_(hScene_, GetTextPath(), 0, 0, 0, 1);
  } else {
    Msg_Scene_SetText::_(hScene_, GetTextPath(), sText_);
  }
  //Msg_Scene_Draw::_(hScene_);
}

