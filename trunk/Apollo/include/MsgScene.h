// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgScene_h_INCLUDED)
#define MsgScene_h_INCLUDED

#include "ApMessage.h"

class Msg_Scene_Create: public ApRequestMessage
{
public:
  Msg_Scene_Create() : ApRequestMessage("Scene_Create") {}
  ApIN ApHandle hScene;
};

class Msg_Scene_Destroy: public ApRequestMessage
{
public:
  Msg_Scene_Destroy() : ApRequestMessage("Scene_Destroy") {}
  ApIN ApHandle hScene;
};

class Msg_Scene_Position: public ApRequestMessage
{
public:
  Msg_Scene_Position() : ApRequestMessage("Scene_Position"), nX(0), nY(0), nW(0), nH(0) {}
  ApIN ApHandle hScene;
  ApIN int nX;
  ApIN int nY;
  ApIN int nW;
  ApIN int nH;
};

class Msg_Scene_Visibility: public ApRequestMessage
{
public:
  Msg_Scene_Visibility() : ApRequestMessage("Scene_Visibility"), bVisible(0) {}
  ApIN ApHandle hScene;
  ApIN int bVisible;
};

class Msg_Scene_CreateElement: public ApRequestMessage
{
public:
  Msg_Scene_CreateElement() : ApRequestMessage("Scene_CreateElement") {}
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_Scene_DeleteElement: public ApRequestMessage
{
public:
  Msg_Scene_DeleteElement() : ApRequestMessage("Scene_DeleteElement") {}
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_Scene_SetRectangle: public ApRequestMessage
{
public:
  Msg_Scene_SetRectangle() : ApRequestMessage("Scene_SetRectangle"), fX(0), fY(0), fW(0), fH(0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
  ApIN double fW;
  ApIN double fH;
};

class Msg_Scene_FontFlags: public ApRequestMessage
{
public:
  typedef enum _FontFlags { Normal = 0
    ,Italic = 1 << 0
    ,Bold = 1 << 1
    ,LastFlag = 1 << 2
  } FontFlags;
};

class Msg_Scene_SetText: public ApRequestMessage
{
public:
  Msg_Scene_SetText() : ApRequestMessage("Scene_SetText"), fX(0), fY(0), fSize(12), nFlags(Msg_Scene_FontFlags::Normal) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
  ApIN String sText;
  ApIN double fSize;
  ApIN String sFont;
  ApIN int nFlags;
};

class Msg_Scene_MeasureText: public ApRequestMessage
{
public:
  Msg_Scene_MeasureText() : ApRequestMessage("Scene_MeasureText"), fSize(12), nFlags(Msg_Scene_FontFlags::Normal) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sText;
  ApIN double fSize;
  ApIN String sFont;
  ApIN int nFlags;
  ApOUT double fBearingX;
  ApOUT double fBearingY;
  ApOUT double fWidth;
  ApOUT double fHeight;
  ApOUT double fAdvanceX;
  ApOUT double fAdvanceY;
};

class Msg_Scene_SetFillColor: public ApRequestMessage
{
public:
  Msg_Scene_SetFillColor() : ApRequestMessage("Scene_SetFillColor"), fRed(0.0), fGreen(0.0), fBlue(0.0), fAlpha(1.0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fRed;
  ApIN double fGreen;
  ApIN double fBlue;
  ApIN double fAlpha;
};

class Msg_Scene_SetStrokeColor: public ApRequestMessage
{
public:
  Msg_Scene_SetStrokeColor() : ApRequestMessage("Scene_SetStrokeColor"), fRed(0.0), fGreen(0.0), fBlue(0.0), fAlpha(1.0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fRed;
  ApIN double fGreen;
  ApIN double fBlue;
  ApIN double fAlpha;
};

class Msg_Scene_SetStrokeWidth: public ApRequestMessage
{
public:
  Msg_Scene_SetStrokeWidth() : ApRequestMessage("Scene_SetStrokeWidth"), fWidth(1.0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fWidth;
};

class Msg_Scene_Draw: public ApRequestMessage
{
public:
  Msg_Scene_Draw() : ApRequestMessage("Scene_Draw") {}
  ApIN ApHandle hScene;
};

#endif // !defined(MsgScene_h_INCLUDED)
