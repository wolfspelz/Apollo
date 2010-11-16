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
  static int _(const ApHandle& hScene)
  {
    Msg_Scene_Create msg;
    msg.hScene = hScene;
    return msg.Request();
  }
  ApIN ApHandle hScene;
};

class Msg_Scene_Destroy: public ApRequestMessage
{
public:
  Msg_Scene_Destroy() : ApRequestMessage("Scene_Destroy") {}
  static int _(const ApHandle& hScene)
  {
    Msg_Scene_Destroy msg;
    msg.hScene = hScene;
    return msg.Request();
  }
  ApIN ApHandle hScene;
};

class Msg_Scene_Position: public ApRequestMessage
{
public:
  Msg_Scene_Position() : ApRequestMessage("Scene_Position"), nX(0), nY(0), nW(0), nH(0) {}
  static int _(const ApHandle& hScene, int nX, int nY, int nW, int nH)
  {
    Msg_Scene_Position msg;
    msg.hScene = hScene;
    msg.nX = nX;
    msg.nY = nY;
    msg.nW = nW;
    msg.nH = nH;
    return msg.Request();
  }
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
  static int _(const ApHandle& hScene, int bVisible)
  {
    Msg_Scene_Visibility msg;
    msg.hScene = hScene;
    msg.bVisible = bVisible;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN int bVisible;
};

class Msg_Scene_SetAutoDraw: public ApRequestMessage
{
public:
  typedef enum _AutoDrawConst { Disabled = -1 } AutoDrawConst;

  Msg_Scene_SetAutoDraw() : ApRequestMessage("Scene_SetAutoDraw"), nMilliSec(Disabled), bAsync(0) {}
  static int _(const ApHandle& hScene, int nMilliSec, int bAsync)
  {
    Msg_Scene_SetAutoDraw msg;
    msg.hScene = hScene;
    msg.nMilliSec = nMilliSec;
    msg.bAsync = bAsync;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN int nMilliSec;
  ApIN int bAsync;
};

class Msg_Scene_AutoDraw: public ApRequestMessage
{
public:
  Msg_Scene_AutoDraw() : ApRequestMessage("Scene_AutoDraw") {}
  static int _(const ApHandle& hScene)
  {
    Msg_Scene_AutoDraw msg;
    msg.hScene = hScene;
    return msg.Request();
  }
  ApIN ApHandle hScene;
};

class Msg_Scene_Draw: public ApRequestMessage
{
public:
  Msg_Scene_Draw() : ApRequestMessage("Scene_Draw") {}
  static int _(const ApHandle& hScene)
  {
    Msg_Scene_Draw msg;
    msg.hScene = hScene;
    return msg.Request();
  }
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

class Msg_Scene_ElementExists: public ApRequestMessage
{
public:
  Msg_Scene_ElementExists() : ApRequestMessage("Scene_ElementExists"), bExists(0) {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    int bExists = 0;
    Msg_Scene_ElementExists msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    int ok = msg.Request();
    if (ok) {
      bExists = msg.bExists;
    }
    return bExists; // Return code is not request success, but the query result in deviation from standard behaviour
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApOUT int bExists;
};

class Msg_Scene_GetChildren: public ApRequestMessage
{
public:
  Msg_Scene_GetChildren() : ApRequestMessage("Scene_GetChildren") {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApOUT Apollo::ValueList vlChildren;
};

//--------------------------
// Manipulate elements

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

class Msg_Scene_ShowElement: public ApRequestMessage
{
public:
  Msg_Scene_ShowElement() : ApRequestMessage("Scene_ShowElement"), bShow(1) {}
  static int _(const ApHandle& hScene, const String& sPath, int bShow)
  {
    Msg_Scene_ShowElement msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.bShow = bShow;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int bShow;
};

class Msg_Scene_SetCopyMode: public ApRequestMessage
{
public:
  // Same as cairo_operator_t
  typedef enum _CopyMode {
      Clear,
      Source,
      Over,
      In,
      Out,
      Atop,
      Dest,
      DestOver,
      DestIn,
      DestOut,
      DestAtop,
      Xor,
      Add,
      Saturate
  } CopyMode;

  Msg_Scene_SetCopyMode() : ApRequestMessage("Scene_SetCopyMode"), nMode(0) {}
  static int _(const ApHandle& hScene, const String& sPath, int nMode)
  {
    Msg_Scene_SetCopyMode msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.nMode = nMode;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nMode;
};

//--------------------------
// Get info

class Msg_Scene_GetTranslateElement: public ApRequestMessage
{
public:
  Msg_Scene_GetTranslateElement() : ApRequestMessage("Scene_GetTranslateElement"), fX(0), fY(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double& fX, double& fY)
  {
    Msg_Scene_GetTranslateElement msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    int ok = msg.Request();
    if (ok) {
      fX = msg.fX;
      fY = msg.fY;
    } 
    return ok;
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
};

class Msg_Scene_GetScaleElement: public ApRequestMessage
{
public:
  Msg_Scene_GetScaleElement() : ApRequestMessage("Scene_GetScaleElement"), fX(0), fY(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double& fX, double& fY)
  {
    Msg_Scene_GetScaleElement msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    int ok = msg.Request();
    if (ok) {
      fX = msg.fX;
      fY = msg.fY;
    } 
    return ok;
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fX;
  ApIN double fY;
};

class Msg_Scene_GetRotateElement: public ApRequestMessage
{
public:
  Msg_Scene_GetRotateElement() : ApRequestMessage("Scene_GetRotateElement"), fAngle(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double& fAngle)
  {
    Msg_Scene_GetRotateElement msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    int ok = msg.Request();
    if (ok) {
      fAngle = msg.fAngle;
    } 
    return ok;
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fAngle;
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

class Msg_Scene_CreateRectangleElement: public ApRequestMessage
{
public:
  Msg_Scene_CreateRectangleElement() : ApRequestMessage("Scene_CreateRectangleElement") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_CreateRectangleElement msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_Scene_CreateImage: public ApRequestMessage
{
public:
  Msg_Scene_CreateImage() : ApRequestMessage("Scene_CreateImage"), fX(0), fY(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY)
  {
    Msg_Scene_CreateImage msg;
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

class Msg_Scene_FontFlags
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

class Msg_Scene_CreateTextElement: public ApRequestMessage
{
public:
  Msg_Scene_CreateTextElement() : ApRequestMessage("Scene_CreateTextElement") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_CreateTextElement msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

//--------------------------
// Change properties

//Msg_Scene_SetFontFamily' : is not a class or namespace name
//Msg_Scene_SetFontSize' : is not a class or namespace name
//Msg_Scene_SetFontFlags' : is not a class or namespace name

class Msg_Scene_SetText: public ApRequestMessage
{
public:
  Msg_Scene_SetText() : ApRequestMessage("Scene_SetText") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sText)
  {
    Msg_Scene_SetText msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sText = sText;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sText;
};

class Msg_Scene_SetFontFamily: public ApRequestMessage
{
public:
  Msg_Scene_SetFontFamily() : ApRequestMessage("Scene_SetFontFamily") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sFont)
  {
    Msg_Scene_SetFontFamily msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sFont = sFont;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sFont;
};

class Msg_Scene_SetFontSize: public ApRequestMessage
{
public:
  Msg_Scene_SetFontSize() : ApRequestMessage("Scene_SetFontSize") {}
  static int _(const ApHandle& hScene, const String& sPath, double fSize)
  {
    Msg_Scene_SetFontSize msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fSize = fSize;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fSize;
};

class Msg_Scene_SetFontFlags: public ApRequestMessage
{
public:
  Msg_Scene_SetFontFlags() : ApRequestMessage("Scene_SetFontFlags") {}
  static int _(const ApHandle& hScene, const String& sPath, int nFlags)
  {
    Msg_Scene_SetFontFlags msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.nFlags = nFlags;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nFlags;
};

class Msg_Scene_SetPosition: public ApRequestMessage
{
public:
  Msg_Scene_SetPosition() : ApRequestMessage("Scene_SetPosition"), fX(0.0), fY(0.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY)
  {
    Msg_Scene_SetPosition msg;
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

class Msg_Scene_SetSize: public ApRequestMessage
{
public:
  Msg_Scene_SetSize() : ApRequestMessage("Scene_SetSize"), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fW, double fH)
  {
    Msg_Scene_SetSize msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fW = fW;
    msg.fH = fH;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fW;
  ApIN double fH;
};

class Msg_Scene_SetRectangle: public ApRequestMessage
{
public:
  Msg_Scene_SetRectangle() : ApRequestMessage("Scene_SetRectangle"), fX(0.0), fY(0.0), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, double fW, double fH)
  {
    Msg_Scene_SetRectangle msg;
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

class Msg_Scene_SetRoundedRectangle: public ApRequestMessage
{
public:
  Msg_Scene_SetRoundedRectangle() : ApRequestMessage("Scene_SetRoundedRectangle"), fRadius(0.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fRadius)
  {
    Msg_Scene_SetRoundedRectangle msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fRadius = fRadius;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fRadius;
};

class Msg_Scene_SetCurvedRectangle: public ApRequestMessage
{
public:
  Msg_Scene_SetCurvedRectangle() : ApRequestMessage("Scene_SetCurvedRectangle") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_SetCurvedRectangle msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

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

class Msg_Scene_SetStrokeImageFile: public ApRequestMessage
{
public:
  Msg_Scene_SetStrokeImageFile() : ApRequestMessage("Scene_SetStrokeImageFile") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sFile)
  {
    Msg_Scene_SetStrokeImageFile msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sFile = sFile;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sFile;
};

class Msg_Scene_SetFillImageFile: public ApRequestMessage
{
public:
  Msg_Scene_SetFillImageFile() : ApRequestMessage("Scene_SetFillImageFile") {}
  static int _(const ApHandle& hScene, const String& sPath, const String& sFile)
  {
    Msg_Scene_SetFillImageFile msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.sFile = sFile;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN String sFile;
};

class Msg_Scene_SetStrokeImageOffset: public ApRequestMessage
{
public:
  Msg_Scene_SetStrokeImageOffset() : ApRequestMessage("Scene_SetStrokeImageOffset"), fX(0.0), fY(0.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY)
  {
    Msg_Scene_SetStrokeImageOffset msg;
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

class Msg_Scene_SetFillImageOffset: public ApRequestMessage
{
public:
  Msg_Scene_SetFillImageOffset() : ApRequestMessage("Scene_SetFillImageOffset"), fX(0.0), fY(0.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY)
  {
    Msg_Scene_SetFillImageOffset msg;
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

class Msg_Scene_SetImageAlpha: public ApRequestMessage
{
public:
  Msg_Scene_SetImageAlpha() : ApRequestMessage("Scene_SetImageAlpha"), fAlpha(1.0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fAlpha)
  {
    Msg_Scene_SetImageAlpha msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    msg.fAlpha = fAlpha;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN double fAlpha;
};

//--------------------------
// Info query without element

class Msg_Scene_GetTextExtents: public ApRequestMessage
{
public:
  Msg_Scene_GetTextExtents() : ApRequestMessage("Scene_GetTextExtents"), fSize(12), nFlags(Msg_Scene_FontFlags::Normal) {}
  static int _(const ApHandle& hScene, const String& sText, const String& sFont, double fSize, int nFlags, double& fBearingX, double& fBearingY, double& fWidth, double& fHeight, double& fAdvanceX, double& fAdvanceY)
  {
    Msg_Scene_GetTextExtents msg;
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

class Msg_Scene_MeasureText: public ApRequestMessage
{
public:
  Msg_Scene_MeasureText() : ApRequestMessage("Scene_MeasureText") {}
  static int _(const ApHandle& hScene, const String& sPath, double& fBearingX, double& fBearingY, double& fWidth, double& fHeight, double& fAdvanceX, double& fAdvanceY)
  {
    Msg_Scene_MeasureText msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
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
  ApIN String sPath;
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

//--------------------------
// Sensors

class Msg_Scene_CreateMouseSensor: public ApRequestMessage
{
public:
  Msg_Scene_CreateMouseSensor() : ApRequestMessage("Scene_CreateMouseSensor"), fX(0), fY(0), fW(0), fH(0) {}
  static int _(const ApHandle& hScene, const String& sPath, double fX, double fY, double fW, double fH)
  {
    Msg_Scene_CreateMouseSensor msg;
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

class Msg_Scene_CaptureMouse: public ApRequestMessage
{
public:
  Msg_Scene_CaptureMouse() : ApRequestMessage("Scene_CaptureMouse") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_CaptureMouse msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_Scene_ReleaseMouse: public ApRequestMessage
{
public:
  Msg_Scene_ReleaseMouse() : ApRequestMessage("Scene_ReleaseMouse") {}
  static int _(const ApHandle& hScene)
  {
    Msg_Scene_ReleaseMouse msg;
    msg.hScene = hScene;
    return msg.Request();
  }
  ApIN ApHandle hScene;
};

// Scene ->
class Msg_Scene_MouseEvent: public ApNotificationMessage
{
public:
  typedef enum _EventType { NoEventType
    ,MouseMove
    ,MouseDown
    ,MouseUp
    ,MouseClick
    ,MouseDoubleClick
    ,MouseOut
    ,LastEventType
  } EventType;

  typedef enum _MouseButton { NoMouseButton
    ,LeftButton
    ,MiddleButton
    ,RightButton
    ,LastMouseButton
  } MouseButton;

  Msg_Scene_MouseEvent() : ApNotificationMessage("Scene_MouseEvent"), nEvent(NoEventType), nButton(NoMouseButton), fX(0), fY(0), bInside(0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nEvent;
  ApIN int nButton;
  ApIN double fX;
  ApIN double fY;
  ApIN int bInside;
};

class Msg_Scene_SetKeyboardFocus: public ApRequestMessage
{
public:
  Msg_Scene_SetKeyboardFocus() : ApRequestMessage("Scene_SetKeyboardFocus") {}
  static int _(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_SetKeyboardFocus msg;
    msg.hScene = hScene;
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

// Scene ->
class Msg_Scene_KeyEvent: public ApNotificationMessage
{
public:
  //8 Bs
  //38 Up
  //40 Down
  //37 Left
  //39 Right
  //46 Del
  //16 Shift
  //17 Ctrl
  typedef enum _EventType { NoEventType
    ,KeyDown
    ,KeyUp
    ,Char
    ,LastEventType
  } EventType;

  typedef enum _KeyCode { NoKeyCode
    ,MoveLeft
    ,MoveRight
    ,MoveUp
    ,MoveDown
    ,DeleteLeft
    ,DeleteRight
    ,Shift
    ,Control
    ,Newline
    ,LastKeyCode
  } KeyCode;

  Msg_Scene_KeyEvent() : ApNotificationMessage("Scene_KeyEvent"), nEvent(0), nKey(0), nFlags(0) {}
  ApIN ApHandle hScene;
  ApIN String sPath;
  ApIN int nEvent;
  ApIN int nKey;
  ApIN String sChar;
  ApIN int nFlags;
};

#endif // !defined(MsgScene_h_INCLUDED)
