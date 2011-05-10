// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgWebView_h_INCLUDED)
#define MsgWebView_h_INCLUDED

#include "ApMessage.h"

//--------------------------
// Manage views

class Msg_WebView_Create: public ApRequestMessage
{
public:
  Msg_WebView_Create() : ApRequestMessage("WebView_Create") {}
  static int _(const ApHandle& hView)
  {
    Msg_WebView_Create msg;
    msg.hView = hView;
    return msg.Request();
  }
  ApIN ApHandle hView;
};

class Msg_WebView_Destroy: public ApRequestMessage
{
public:
  Msg_WebView_Destroy() : ApRequestMessage("WebView_Destroy") {}
  static int _(const ApHandle& hView)
  {
    Msg_WebView_Destroy msg;
    msg.hView = hView;
    return msg.Request();
  }
  ApIN ApHandle hView;
};

class Msg_WebView_Position: public ApRequestMessage
{
public:
  Msg_WebView_Position() : ApRequestMessage("WebView_Position"), nX(0), nY(0), nW(0), nH(0) {}
  static int _(const ApHandle& hView, int nX, int nY, int nW, int nH)
  {
    Msg_WebView_Position msg;
    msg.hView = hView;
    msg.nX = nX;
    msg.nY = nY;
    msg.nW = nW;
    msg.nH = nH;
    return msg.Request();
  }
  ApIN ApHandle hView;
  ApIN int nX;
  ApIN int nY;
  ApIN int nW;
  ApIN int nH;
};

class Msg_WebView_Visibility: public ApRequestMessage
{
public:
  Msg_WebView_Visibility() : ApRequestMessage("WebView_Visibility"), bVisible(0) {}
  static int _(const ApHandle& hView, int bVisible)
  {
    Msg_WebView_Visibility msg;
    msg.hView = hView;
    msg.bVisible = bVisible;
    return msg.Request();
  }
  ApIN ApHandle hView;
  ApIN int bVisible;
};

class Msg_WebView_LoadHtml: public ApRequestMessage
{
public:
  Msg_WebView_LoadHtml() : ApRequestMessage("WebView_LoadHtml") {}
  static int _(const ApHandle& hView, const String& sHtml, const String& sBase)
  {
    Msg_WebView_LoadHtml msg;
    msg.hView = hView;
    msg.sHtml = sHtml;
    msg.sBase = sBase;
    return msg.Request();
  }
  ApIN ApHandle hView;
  ApIN String sHtml;
  ApIN String sBase;
};

class Msg_WebView_Load: public ApRequestMessage
{
public:
  Msg_WebView_Load() : ApRequestMessage("WebView_Load") {}
  static int _(const ApHandle& hView, const String& sUrl)
  {
    Msg_WebView_Load msg;
    msg.hView = hView;
    msg.sUrl = sUrl;
    return msg.Request();
  }
  ApIN ApHandle hView;
  ApIN String sUrl;
};

class Msg_WebView_Reload: public ApRequestMessage
{
public:
  Msg_WebView_Reload() : ApRequestMessage("WebView_Reload") {}
  ApIN ApHandle hView;
};

class Msg_WebView_CallScriptFunction: public ApRequestMessage
{
public:
  Msg_WebView_CallScriptFunction() : ApRequestMessage("WebView_CallScriptFunction") {}
  ApIN ApHandle hView;
  ApIN String sMethod;
  ApIN List lArgs;
  ApOUT String sResult;
};

class Msg_WebView_SetScriptAccess: public ApRequestMessage
{
  #define Msg_WebView_SetScriptAccess_Denied "Denied"
  #define Msg_WebView_SetScriptAccess_Allowed "Allowed"
public:
  Msg_WebView_SetScriptAccess() : ApRequestMessage("WebView_SetScriptAccess") {}
  ApIN ApHandle hView;
  ApIN String sAccess;
  static int Allow(const ApHandle& hView)
  {
    Msg_WebView_SetScriptAccess msg;
    msg.hView = hView;
    msg.sAccess = Msg_WebView_SetScriptAccess_Allowed;
    return msg.Request();
  }
};

class Msg_WebView_MoveBy: public ApRequestMessage
{
public:
  Msg_WebView_MoveBy() : ApRequestMessage("WebView_MoveBy"), nX(0), nY(0) {}
  ApIN ApHandle hView;
  ApIN int nX;
  ApIN int nY;
};

class Msg_WebView_SizeBy: public ApRequestMessage
{
public:
  enum _Direction { NoDirection = 0
    ,DirectionLeft
    ,DirectionTop
    ,DirectionRight
    ,DirectionBottom
    ,DirectionTopLeft
    ,DirectionTopRight
    ,DirectionBottomLeft
    ,DirectionBottomRight
  };
  Msg_WebView_SizeBy() : ApRequestMessage("WebView_SizeBy"), nW(0), nH(0), nDirection(DirectionBottomRight) {}
  ApIN ApHandle hView;
  ApIN int nW;
  ApIN int nH;
  ApIN int nDirection;
};

class Msg_WebView_MouseCapture: public ApRequestMessage
{
public:
  Msg_WebView_MouseCapture() : ApRequestMessage("WebView_MouseCapture") {}
  ApIN ApHandle hView;
};

class Msg_WebView_MouseRelease: public ApRequestMessage
{
public:
  Msg_WebView_MouseRelease() : ApRequestMessage("WebView_MouseRelease") {}
  ApIN ApHandle hView;
};

class Msg_WebView_GetPosition: public ApRequestMessage
{
public:
  Msg_WebView_GetPosition() : ApRequestMessage("WebView_GetPosition"), nX(0), nY(0), nW(0), nH(0) {}
  ApIN ApHandle hView;
  ApOUT int nX;
  ApOUT int nY;
  ApOUT int nW;
  ApOUT int nH;
};

class Msg_WebView_GetVisibility: public ApRequestMessage
{
public:
  Msg_WebView_GetVisibility() : ApRequestMessage("WebView_GetVisibility"), bVisible(0) {}
  ApIN ApHandle hView;
  ApOUT int bVisible;
};

//--------------------------
// Events

// WebView ->
class Msg_WebView_Event_DocumentLoaded: public ApNotificationMessage
{
public:
  Msg_WebView_Event_DocumentLoaded() : ApNotificationMessage("WebView_Event_DocumentLoaded") {}
  ApIN ApHandle hView;
};

// WebView ->
class Msg_WebView_Event_DocumentComplete: public ApNotificationMessage
{
public:
  Msg_WebView_Event_DocumentComplete() : ApNotificationMessage("WebView_Event_DocumentComplete") {}
  ApIN ApHandle hView;
};

// WebView ->
class Msg_WebView_Event_BeforeNavigate: public ApFilterMessage
{
public:
  Msg_WebView_Event_BeforeNavigate() : ApFilterMessage("WebView_Event_BeforeNavigate"), bCancel(0) {}
  ApIN ApHandle hView;
  ApIN String sUrl;
  ApINOUT int bCancel;
};

// WebView ->
class Msg_WebView_Event_BeforeRequest: public ApFilterMessage
{
public:
  Msg_WebView_Event_BeforeRequest() : ApFilterMessage("WebView_Event_BeforeRequest") {}
  ApIN ApHandle hView;
  ApINOUT String sUrl;
};

#endif // !defined(MsgWebView_h_INCLUDED)
