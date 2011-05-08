// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgScWidget_h_INCLUDED)
#define MsgScWidget_h_INCLUDED

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
#define Msg_ScWidget_ButtonState_High "High"
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

class Msg_ScWidget_SetButtonActive: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonActive() : ApRequestMessage("ScWidget_SetButtonActive"), bActive(1) {}
  static int _(const ApHandle& hScene, const String& sPath, int bActive)
  {
    Msg_ScWidget_SetButtonActive msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.bActive = bActive;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  int bActive;
};

class Msg_ScWidget_SetButtonText: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonText() : ApRequestMessage("ScWidget_SetButtonText") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sText)
  {
    Msg_ScWidget_SetButtonText msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sText = sText;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sText;
};

class Msg_ScWidget_SetButtonTextFont: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonTextFont() : ApRequestMessage("ScWidget_SetButtonTextFont") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sFont)
  {
    Msg_ScWidget_SetButtonTextFont msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sFont = sFont;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sFont;
};

class Msg_ScWidget_SetButtonTextSize: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonTextSize() : ApRequestMessage("ScWidget_SetButtonTextSize"), fSize(12) {}
  static int _(const ApHandle& hScene, const String& sPath, double fSize)
  {
    Msg_ScWidget_SetButtonTextSize msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fSize = fSize;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fSize;
};

class Msg_ScWidget_SetButtonTextFlags: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonTextFlags() : ApRequestMessage("ScWidget_SetButtonTextFlags"), nFlags(0) {}
  static int _(const ApHandle& hScene, const String& sPath, int nFlags)
  {
    Msg_ScWidget_SetButtonTextFlags msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.nFlags = nFlags;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nFlags;
};

class Msg_ScWidget_SetButtonTextColor: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonTextColor() : ApRequestMessage("ScWidget_SetButtonTextColor"), fRed(0.0), fGreen(0.0), fBlue(0.0), fAlpha(1.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fRed, double fGreen, double fBlue, double fAlpha)
  {
    Msg_ScWidget_SetButtonTextColor msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fRed = fRed;
    msg.fGreen = fGreen;
    msg.fBlue = fBlue;
    msg.fAlpha = fAlpha;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fRed;
  ApIN double fGreen;
  ApIN double fBlue;
  ApIN double fAlpha;
};

class Msg_ScWidget_DeleteButtonText: public ApRequestMessage
{
public:
  Msg_ScWidget_DeleteButtonText() : ApRequestMessage("ScWidget_DeleteButtonText") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_ScWidget_DeleteButtonText msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_ScWidget_SetButtonImageFile: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonImageFile() : ApRequestMessage("ScWidget_SetButtonImageFile") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sState, const String& sFile)
  {
    Msg_ScWidget_SetButtonImageFile msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sState = sState;
    msg.sFile = sFile;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sState;
  ApIN String sFile;
};

class Msg_ScWidget_SetButtonTextOffset: public ApRequestMessage
{
public:
  Msg_ScWidget_SetButtonTextOffset() : ApRequestMessage("ScWidget_SetButtonTextOffset") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sState, double fX, double fY)
  {
    Msg_ScWidget_SetButtonTextOffset msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sState = sState;
    msg.fX = fX;
    msg.fY = fY;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sState;
  ApIN double fX;
  ApIN double fY;
};

// Widget ->
class Msg_ScWidget_ButtonEvent: public ApNotificationMessage
{
public:
  typedef enum _EventType { NoButtonEvent
    ,ButtonHigh
    ,ButtonDown
    ,ButtonClick
    ,LastButtonEvent
  } EventType;

  Msg_ScWidget_ButtonEvent() : ApNotificationMessage("ScWidget_ButtonEvent"), nEvent(NoButtonEvent) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nEvent;
};

//--------------------------

class Msg_ScWidget_CreateEdit: public ApRequestMessage
{
public:
  Msg_ScWidget_CreateEdit() : ApRequestMessage("ScWidget_CreateEdit"), fX(0), fY(0), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, double fW, double fH)
  {
    Msg_ScWidget_CreateEdit msg;
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

class Msg_ScWidget_SetEditText: public ApRequestMessage
{
public:
  Msg_ScWidget_SetEditText() : ApRequestMessage("ScWidget_SetEditText") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sText)
  {
    Msg_ScWidget_SetEditText msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sText = sText;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sText;
};

class Msg_ScWidget_GetEditText: public ApRequestMessage
{
public:
  Msg_ScWidget_GetEditText() : ApRequestMessage("ScWidget_GetEditText") {}
  static int _(const ApHandle& hScene, const String& sPath, String& sText)
  {
    Msg_ScWidget_GetEditText msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    int ok = msg.Request();
    if (ok) {
      sText = msg.sText;
    } 
    return ok;
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApOUT String sText;
};

class Msg_ScWidget_SetEditPadding: public ApRequestMessage
{
public:
  Msg_ScWidget_SetEditPadding() : ApRequestMessage("ScWidget_SetEditPadding"), fPadding(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fPadding)
  {
    Msg_ScWidget_SetEditPadding msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fPadding = fPadding;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fPadding;
};

class Msg_ScWidget_SetEditAlign: public ApRequestMessage
{
public:
  // Same as ScWidget/Edit
  typedef enum _Align { NoAlign
    ,AlignLeft
    ,AlignCenter // Horizontal
    ,AlignRight
    ,AlignTop
    ,AlignMiddle // Vertical
    ,AlignBottom
    ,LastAlign
  } Align;

  Msg_ScWidget_SetEditAlign() : ApRequestMessage("ScWidget_SetEditAlign"), nAlignH(AlignLeft), nAlignV(AlignTop) {}
  static int _(const ApHandle& hScene, const String& sPath, int nAlignH, int nAlignV)
  {
    Msg_ScWidget_SetEditAlign msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.nAlignH = nAlignH;
    msg.nAlignV = nAlignV;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nAlignH;
  ApIN int nAlignV;
};

class Msg_ScWidget_SetEditTextFont: public ApRequestMessage
{
public:
  Msg_ScWidget_SetEditTextFont() : ApRequestMessage("ScWidget_SetEditTextFont") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sFont)
  {
    Msg_ScWidget_SetEditTextFont msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sFont = sFont;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sFont;
};

class Msg_ScWidget_SetEditTextSize: public ApRequestMessage
{
public:
  Msg_ScWidget_SetEditTextSize() : ApRequestMessage("ScWidget_SetEditTextSize"), fSize(12) {}
  static int _(const ApHandle& hScene, const String& sPath, double fSize)
  {
    Msg_ScWidget_SetEditTextSize msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fSize = fSize;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fSize;
};

class Msg_ScWidget_SetEditTextFlags: public ApRequestMessage
{
public:
  Msg_ScWidget_SetEditTextFlags() : ApRequestMessage("ScWidget_SetEditTextFlags"), nFlags(0) {}
  static int _(const ApHandle& hScene, const String& sPath, int nFlags)
  {
    Msg_ScWidget_SetEditTextFlags msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.nFlags = nFlags;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nFlags;
};

class Msg_ScWidget_SetEditTextColor: public ApRequestMessage
{
public:
  Msg_ScWidget_SetEditTextColor() : ApRequestMessage("ScWidget_SetEditTextColor"), fRed(0.0), fGreen(0.0), fBlue(0.0), fAlpha(1.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fRed, double fGreen, double fBlue, double fAlpha)
  {
    Msg_ScWidget_SetEditTextColor msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fRed = fRed;
    msg.fGreen = fGreen;
    msg.fBlue = fBlue;
    msg.fAlpha = fAlpha;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fRed;
  ApIN double fGreen;
  ApIN double fBlue;
  ApIN double fAlpha;
};

// Widget ->
class Msg_ScWidget_EditEvent: public ApNotificationMessage
{
public:
  typedef enum _EventType { NoEditEvent
    ,EditEnter
    ,LastEditEvent
  } EventType;

  Msg_ScWidget_EditEvent() : ApNotificationMessage("ScWidget_EditEvent"), nEvent(NoEditEvent) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nEvent;
};

#endif // !defined(MsgScWidget_h_INCLUDED)