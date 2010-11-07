// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgScene_h_INCLUDED)
#define MsgScene_h_INCLUDED

#include "ApMessage.h"

//--------------------------

class Msg_ScWidget_CreateButton: public ApRequestMessage
{
public:
  Msg_ScWidget_CreateButton() : ApRequestMessage("ScWidget_CreateButton"), fX(0), fY(0), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, double fW, double fH)
  {
    Msg_ScWidget_CreateButton msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fX = fX;
    msg.fY = fY;
    msg.fW = fW;
    msg.fH = fH;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
  ApIN double fW;
  ApIN double fH;
};

class Msg_ScWidget_Button
{
public:
  typedef enum _State { NoState
    ,StateNormal
    ,StateDown
    ,StateUp
    ,StateDisabled
  } State;
};

class Msg_ScWidget_SetButtonImageFile: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonImageFile() : ApRequestMessage("ScWidget_SetButtonImageFile"), nState(Msg_ScWidget_Button::NoState) {}
  static int _(const ApHandle& hScene, const String& sPath, Msg_ScWidget_Button::State nState, const String& sFile)
  {
    Msg_ScWidget_SetButtonImageFile msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.nState = nState;
    msg.sFile = sFile;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nState;
  ApIN String sFile;
};

class Msg_ScWidget_SetButtonState: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonState() : ApRequestMessage("ScWidget_SetButtonState"), nState(Msg_ScWidget_Button::NoState) {}
  static int _(const ApHandle& hScene, const String& sPath, Msg_ScWidget_Button::State nState)
  {
    Msg_ScWidget_SetButtonState msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.nState = nState;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nState;
};


//class Msg_ScWidget_CreateBorder: public ApRequestMessage
//{
//public:
//  Msg_ScWidget_CreateBorder() : ApRequestMessage("ScWidget_CreateBorder") {}
//  ApIN ApHandle hScene;
//  ApIN String sPath;
//  ApIN double fX;
//  ApIN double fY;
//  ApIN double fW;
//  ApIN double fH;
//};
//
//class Msg_ScWidget_SetBorderImageFile: public ApRequestMessage
//{
//public:
//  Msg_ScWidget_SetBorderImageFile() : ApRequestMessage("ScWidget_SetBorderImageFile") {}
//  ApIN ApHandle hScene;
//  ApIN String sPath;
//  ApIN int nPosition
//  ApIN double fX;
//  ApIN double fY;
//  ApIN double fW;
//  ApIN double fH;
//  ApIN String sFile;
//};


#endif // !defined(MsgScene_h_INCLUDED)
