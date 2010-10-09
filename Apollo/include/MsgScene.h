// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgScene_h_INCLUDED)
#define MsgScene_h_INCLUDED

#include "ApMessage.h"
#include "Image.h"

//--------------------------
// Manage scenes

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

class Msg_Scene_SetAutoDraw: public ApRequestMessage
{
public:
  typedef enum _AutoDrawConst { Disabled = -1 } AutoDrawConst;

  Msg_Scene_SetAutoDraw() : ApRequestMessage("Scene_SetAutoDraw"), nMilliSec(Disabled), bAsync(0) {}
  ApIN ApHandle hScene;
  ApIN int nMilliSec;
  ApIN int bAsync;
};

class Msg_Scene_AutoDraw: public ApRequestMessage
{
public:
  Msg_Scene_AutoDraw() : ApRequestMessage("Scene_AutoDraw") {}
  ApIN ApHandle hScene;
};

class Msg_Scene_Draw: public ApRequestMessage
{
public:
  Msg_Scene_Draw() : ApRequestMessage("Scene_Draw") {}
  ApIN ApHandle hScene;
};

//--------------------------
// Manage elements

class Msg_Scene_CreateElement: public ApRequestMessage
{
public:
  Msg_Scene_CreateElement() : ApRequestMessage("Scene_CreateElement") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_CreateElement msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_Scene_TranslateElement: public ApRequestMessage
{
public:
  Msg_Scene_TranslateElement() : ApRequestMessage("Scene_TranslateElement"), fX(0), fY(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY)
  {
    Msg_Scene_TranslateElement msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath;
    msg.fX = fX; 
    msg.fY = fY; 
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
};

class Msg_Scene_ScaleElement: public ApRequestMessage
{
public:
  Msg_Scene_ScaleElement() : ApRequestMessage("Scene_ScaleElement"), fX(0), fY(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY)
  {
    Msg_Scene_ScaleElement msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath;
    msg.fX = fX; 
    msg.fY = fY; 
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
};

class Msg_Scene_RotateElement: public ApRequestMessage
{
public:
  Msg_Scene_RotateElement() : ApRequestMessage("Scene_RotateElement"), fAngle(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fAngle)
  {
    Msg_Scene_RotateElement msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath;
    msg.fAngle = fAngle; 
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fAngle;
};

class Msg_Scene_HideElement: public ApRequestMessage
{
public:
  Msg_Scene_HideElement() : ApRequestMessage("Scene_HideElement"), bHide(1) {}
  static int _(const ApHandle& hScene, const String& sPath, int bHide)
  {
    Msg_Scene_HideElement msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath;
    msg.bHide = bHide; 
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int bHide;
};

class Msg_Scene_DeleteElement: public ApRequestMessage
{
public:
  Msg_Scene_DeleteElement() : ApRequestMessage("Scene_DeleteElement") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_DeleteElement msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

//--------------------------
// Create primitives

class Msg_Scene_CreateRectangle: public ApRequestMessage
{
public:
  Msg_Scene_CreateRectangle() : ApRequestMessage("Scene_CreateRectangle"), fX(0), fY(0), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, double fW, double fH)
  {
    Msg_Scene_CreateRectangle msg; 
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

class Msg_Scene_CreateImageFromData: public ApRequestMessage
{
public:
  Msg_Scene_CreateImageFromData() : ApRequestMessage("Scene_CreateImageFromData"), fX(0), fY(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, const Apollo::Image& image)
  {
    Msg_Scene_CreateImageFromData msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath; 
    msg.fX = fX; 
    msg.fY = fY; 
    msg.image.CopyReference(image);
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
  ApIN Apollo::Image image;
};

class Msg_Scene_CreateImageFromFile: public ApRequestMessage
{
public:
  Msg_Scene_CreateImageFromFile() : ApRequestMessage("Scene_CreateImageFromFile"), fX(0), fY(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, const String& sFile)
  {
    Msg_Scene_CreateImageFromFile msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath; 
    msg.fX = fX; 
    msg.fY = fY; 
    msg.sFile = sFile;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
  ApIN String sFile;
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

class Msg_Scene_CreateText: public ApRequestMessage
{
public:
  Msg_Scene_CreateText() : ApRequestMessage("Scene_CreateText"), fX(0), fY(0), fSize(12), nFlags(Msg_Scene_FontFlags::Normal) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, String sText, String sFont, double fSize, int nFlags)
  {
    Msg_Scene_CreateText msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath; 
    msg.fX = fX; 
    msg.fY = fY; 
    msg.sText = sText; 
    msg.sFont = sFont; 
    msg.fSize = fSize; 
    msg.nFlags = nFlags; 
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
  ApIN String sText;
  ApIN String sFont;
  ApIN double fSize;
  ApIN int nFlags;
};

//--------------------------
// Change properties

class Msg_Scene_SetFillColor: public ApRequestMessage
{
public:
  Msg_Scene_SetFillColor() : ApRequestMessage("Scene_SetFillColor"), fRed(0.0), fGreen(0.0), fBlue(0.0), fAlpha(1.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fRed, double fGreen, double fBlue, double fAlpha)
  { 
    Msg_Scene_SetFillColor msg; 
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

class Msg_Scene_SetStrokeColor: public ApRequestMessage
{
public:
  Msg_Scene_SetStrokeColor() : ApRequestMessage("Scene_SetStrokeColor"), fRed(0.0), fGreen(0.0), fBlue(0.0), fAlpha(1.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fRed, double fGreen, double fBlue, double fAlpha)
  {
    Msg_Scene_SetStrokeColor msg; 
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

class Msg_Scene_SetStrokeWidth: public ApRequestMessage
{
public:
  Msg_Scene_SetStrokeWidth() : ApRequestMessage("Scene_SetStrokeWidth"), fWidth(1.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fWidth)
  {
    Msg_Scene_SetStrokeWidth msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fWidth = fWidth;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fWidth;
};

class Msg_Scene_SetImageData: public ApRequestMessage
{
public:
  Msg_Scene_SetImageData() : ApRequestMessage("Scene_SetImageData") {}
  static int _(const ApHandle& hScene, const String& sPath, const Apollo::Image& image)
  {
    Msg_Scene_SetImageData msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.image.CopyReference(image);
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN Apollo::Image image;
};

class Msg_Scene_DeleteImageData: public ApRequestMessage
{
public:
  Msg_Scene_DeleteImageData() : ApRequestMessage("Scene_DeleteImageData") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_DeleteImageData msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_Scene_SetImageFile: public ApRequestMessage
{
public:
  Msg_Scene_SetImageFile() : ApRequestMessage("Scene_SetImageFile") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sFile)
  {
    Msg_Scene_SetImageFile msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sFile = sFile;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sFile;
};

class Msg_Scene_DeleteImageFile: public ApRequestMessage
{
public:
  Msg_Scene_DeleteImageFile() : ApRequestMessage("Scene_DeleteImageFile") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_DeleteImageFile msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

//--------------------------
// Info query without element

class Msg_Scene_MeasureText: public ApRequestMessage
{
public:
  Msg_Scene_MeasureText() : ApRequestMessage("Scene_MeasureText"), fSize(12), nFlags(Msg_Scene_FontFlags::Normal) {}
  static int _(const ApHandle& hScene, const String& sText, const String& sFont, double fSize, int nFlags, double& fBearingX, double& fBearingY, double& fWidth, double& fHeight, double& fAdvanceX, double& fAdvanceY)
  { 
    Msg_Scene_MeasureText msg; 
    msg.hScene = hScene; 
    msg.sText = sText;
    msg.sFont = sFont;
    msg.fSize = fSize;
    msg.nFlags = nFlags;
    int ok = msg.Request();
    if (ok) {
      fBearingX = msg.fBearingX;
      fBearingY = msg.fBearingY;
      fWidth = msg.fWidth;
      fHeight = msg.fHeight;
      fAdvanceX = msg.fAdvanceX;
      fAdvanceY = msg.fAdvanceY;
    } 
    return ok;
  }
  ApIN ApHandle hScene;
  ApIN String sText;
  ApIN String sFont;
  ApIN double fSize;
  ApIN int nFlags;
  ApOUT double fBearingX;
  ApOUT double fBearingY;
  ApOUT double fWidth;
  ApOUT double fHeight;
  ApOUT double fAdvanceX;
  ApOUT double fAdvanceY;
};

class Msg_Scene_GetImageSizeFromData: public ApRequestMessage
{
public:
  Msg_Scene_GetImageSizeFromData() : ApRequestMessage("Scene_GetImageSizeFromData"), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const Apollo::Image& image, double& fW, double& fH)
  {
    Msg_Scene_GetImageSizeFromData msg; 
    msg.hScene = hScene; 
    msg.image.CopyReference(image);
    int ok = msg.Request();
    if (ok) {
      fW = msg.fW; 
      fH = msg.fH; 
    } 
    return ok;
  }
  ApIN ApHandle hScene;
  ApIN Apollo::Image image;
  ApOUT double fW;
  ApOUT double fH;
  ApIN String sFile;
};

class Msg_Scene_GetImageSizeFromFile: public ApRequestMessage
{
public:
  Msg_Scene_GetImageSizeFromFile() : ApRequestMessage("Scene_GetImageSizeFromFile"), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const String& sFile, double& fW, double& fH)
  {
    Msg_Scene_GetImageSizeFromFile msg; 
    msg.hScene = hScene; 
    msg.sFile = sFile;
    int ok = msg.Request();
    if (ok) {
      fW = msg.fW; 
      fH = msg.fH; 
    } 
    return ok;
  }
  ApIN ApHandle hScene;
  ApIN String sFile;
  ApOUT double fW;
  ApOUT double fH;
};

#endif // !defined(MsgScene_h_INCLUDED)