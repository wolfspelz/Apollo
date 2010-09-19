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
  Msg_Scene_Rectangle() : ApRequestMessage("Scene_Rectangle"), fX(0), fY(0), fW(0), fH(0), bFill(0), fFillRed(0.0), fFillGreen(0.0), fFillBlue(0.0), fFillAlpha(1.0), bStroke(0), fStrokeWidth(1.0), fStrokeRed(0.0), fStrokeGreen(0.0), fStrokeBlue(0.0), fStrokeAlpha(1.0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
  ApIN double fW;
  ApIN double fH;
  ApIN int bFill;
  ApIN double fFillRed;
  ApIN double fFillGreen;
  ApIN double fFillBlue;
  ApIN double fFillAlpha;
  ApIN int bStroke;
  ApIN double fStrokeWidth;
  ApIN double fStrokeRed;
  ApIN double fStrokeGreen;
  ApIN double fStrokeBlue;
  ApIN double fStrokeAlpha;
};

class Msg_Scene_Draw: public ApRequestMessage
{
public:
  Msg_Scene_Draw() : ApRequestMessage("Scene_Draw") {}
  ApIN ApHandle hScene;
};

#endif // !defined(MsgScene_h_INCLUDED)
