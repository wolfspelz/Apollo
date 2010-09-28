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

class Msg_Scene_Rectangle: public ApRequestMessage
{
public:
  Msg_Scene_Rectangle() : ApRequestMessage("Scene_Rectangle"), fX(0), fY(0), fW(0), fH(0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
  ApIN double fW;
  ApIN double fH;
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
  Msg_Scene_SetStrokeColor() : ApRequestMessage("Scene_SetStrokeColor"), fWidth(1.0), fRed(0.0), fGreen(0.0), fBlue(0.0), fAlpha(1.0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fWidth;
  ApIN double fRed;
  ApIN double fGreen;
  ApIN double fBlue;
  ApIN double fAlpha;
};

class Msg_Scene_DeleteElement: public ApRequestMessage
{
public:
  Msg_Scene_DeleteElement() : ApRequestMessage("Scene_DeleteElement") {}
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_Scene_Draw: public ApRequestMessage
{
public:
  Msg_Scene_Draw() : ApRequestMessage("Scene_Draw") {}
  ApIN ApHandle hScene;
};

#endif // !defined(MsgScene_h_INCLUDED)
