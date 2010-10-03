// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgScene_h_INCLUDED)
#define MsgScene_h_INCLUDED

#include "ApMessage.h"
#include "Image.h"

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
  static int Invoke(const ApHandle& hScene, const String& sPath)
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
  static int Invoke(const ApHandle& hScene, const String& sPath, double fX, double fY)
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
  static int Invoke(const ApHandle& hScene, const String& sPath, double fX, double fY)
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

class Msg_Scene_DeleteElement: public ApRequestMessage
{
public:
  Msg_Scene_DeleteElement() : ApRequestMessage("Scene_DeleteElement") {}
  static int Invoke(const ApHandle& hScene, const String& sPath)
  {
    Msg_Scene_DeleteElement msg; 
    msg.hScene = hScene; 
    msg.sPath = sPath;
    return msg.Request();
  }
  ApIN ApHandle hScene;
  ApIN String sPath;
};

class Msg_Scene_CreateRectangle: public ApRequestMessage
{
public:
  Msg_Scene_CreateRectangle() : ApRequestMessage("Scene_CreateRectangle"), fX(0), fY(0), fW(0), fH(0) {}
  static int Invoke(const ApHandle& hScene, const String& sPath, double fX, double fY, double fW, double fH)
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

class Msg_Scene_CreateImage: public ApRequestMessage
{
public:
  Msg_Scene_CreateImage() : ApRequestMessage("Scene_CreateImage"), fX(0), fY(0) {}
  static int Invoke(const ApHandle& hScene, const String& sPath, double fX, double fY, const Apollo::Image& image)
  {
    Msg_Scene_CreateImage msg; 
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
  static int Invoke(const ApHandle& hScene, const String& sPath, double fX, double fY, String sText, String sFont, double fSize, int nFlags)
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

class Msg_Scene_MeasureText: public ApRequestMessage
{
public:
  Msg_Scene_MeasureText() : ApRequestMessage("Scene_MeasureText"), fSize(12), nFlags(Msg_Scene_FontFlags::Normal) {}
  static int Invoke(const ApHandle& hScene, const String& sText, const String& sFont, double fSize, int nFlags, double& fBearingX, double& fBearingY, double& fWidth, double& fHeight, double& fAdvanceX, double& fAdvanceY)
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

class Msg_Scene_SetFillColor: public ApRequestMessage
{
public:
  Msg_Scene_SetFillColor() : ApRequestMessage("Scene_SetFillColor"), fRed(0.0), fGreen(0.0), fBlue(0.0), fAlpha(1.0) {}
  static int Invoke(const ApHandle& hScene, const String& sPath, double fRed, double fGreen, double fBlue, double fAlpha)
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
  static int Invoke(const ApHandle& hScene, const String& sPath, double fRed, double fGreen, double fBlue, double fAlpha)
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
  static int Invoke(const ApHandle& hScene, const String& sPath, double fWidth)
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
  static int Invoke(const ApHandle& hScene, const String& sPath, const Apollo::Image& image)
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

class Msg_Scene_Draw: public ApRequestMessage
{
public:
  Msg_Scene_Draw() : ApRequestMessage("Scene_Draw") {}
  ApIN ApHandle hScene;
};

#endif // !defined(MsgScene_h_INCLUDED)
