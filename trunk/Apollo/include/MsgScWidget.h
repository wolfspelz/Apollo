// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgScWidget_h_INCLUDED)
#define MsgScWidget_h_INCLUDED

#include "ApMessage.h"

//--------------------------

class Msg_ScWidget_Show: public ApRequestMessage
{
public:
  Msg_ScWidget_Show() : ApRequestMessage("ScWidget_Show") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_ScWidget_Show msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

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

class Msg_ScWidget_SetCoordinates: public ApRequestMessage
{
public:
  Msg_ScWidget_SetCoordinates() : ApRequestMessage("ScWidget_SetCoordinates"), fX(0), fY(0), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, double fW, double fH)
  {
    Msg_ScWidget_SetCoordinates msg;
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

#define Msg_ScWidget_ButtonState_Normal "Normal"
#define Msg_ScWidget_ButtonState_Down "Down"
#define Msg_ScWidget_ButtonState_Up "Up"
#define Msg_ScWidget_ButtonState_Disabled "Disabled"

class Msg_ScWidget_SetButtonState: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonState() : ApRequestMessage("ScWidget_SetButtonState") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sState)
  {
    Msg_ScWidget_SetButtonState msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sState = sState;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sState;
};

class Msg_ScWidget_SetButtonImageFile: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonImageFile() : ApRequestMessage("ScWidget_SetButtonImageFile"), fX(0), fY(0) {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sState, const String& sFile, double fX, double fY)
  {
    Msg_ScWidget_SetButtonImageFile msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sState = sState;
    msg.sFile = sFile;
    msg.fX = fX;
    msg.fY = fY;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sState;
  ApIN String sFile;
  ApIN double fX;
  ApIN double fY;
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


#endif // !defined(MsgScWidget_h_INCLUDED)
