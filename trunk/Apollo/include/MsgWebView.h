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
  Msg_WebView_Position() : ApRequestMessage("WebView_Position"), nLeft(0), nTop(0), nWidth(0), nHeight(0) {}
  static int _(const ApHandle& hView, int nLeft, int nTop, int nWidth, int nHeight)
  {
    Msg_WebView_Position msg;
    msg.hView = hView;
    msg.nLeft = nLeft;
    msg.nTop = nTop;
    msg.nWidth = nWidth;
    msg.nHeight = nHeight;
    return msg.Request();
  }
  ApIN ApHandle hView;
  ApIN int nLeft;
  ApIN int nTop;
  ApIN int nWidth;
  ApIN int nHeight;
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
  ApIN String sFunction;
  ApIN List lArgs;
  ApOUT String sResult;
};

class Msg_WebView_CallScriptSrpc: public ApRequestMessage
{
public:
  Msg_WebView_CallScriptSrpc() : ApRequestMessage("WebView_CallScriptSrpc") {}
  ApIN ApHandle hView;
  ApIN String sFunction;
  ApIN Apollo::SrpcMessage srpc;
  ApOUT Apollo::SrpcMessage response;
};

class Msg_WebView_SetScriptAccessPolicy: public ApRequestMessage
{
  #define Msg_WebView_SetScriptAccessPolicy_Denied "Denied"
  #define Msg_WebView_SetScriptAccessPolicy_Allowed "Allowed"
public:
  Msg_WebView_SetScriptAccessPolicy() : ApRequestMessage("WebView_SetScriptAccessPolicy") {}
  ApIN ApHandle hView;
  ApIN String sPolicy;
  static int Allow(const ApHandle& hView)
  {
    Msg_WebView_SetScriptAccessPolicy msg;
    msg.hView = hView;
    msg.sPolicy = Msg_WebView_SetScriptAccessPolicy_Allowed;
    return msg.Request();
  }
};

class Msg_WebView_SetNavigationPolicy: public ApRequestMessage
{
  #define Msg_WebView_SetNavigationPolicy_Denied "Denied"
  #define Msg_WebView_SetNavigationPolicy_Allowed "Allowed"
public:
  Msg_WebView_SetNavigationPolicy() : ApRequestMessage("WebView_SetNavigationPolicy") {}
  ApIN ApHandle hView;
  ApIN String sPolicy;
  static int Deny(const ApHandle& hView)
  {
    Msg_WebView_SetNavigationPolicy msg;
    msg.hView = hView;
    msg.sPolicy = Msg_WebView_SetNavigationPolicy_Denied;
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
  Msg_WebView_SizeBy() : ApRequestMessage("WebView_SizeBy"), nX(0), nY(0), nDirection(DirectionBottomRight) {}
  ApIN ApHandle hView;
  ApIN int nX;
  ApIN int nY;
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
  Msg_WebView_GetPosition() : ApRequestMessage("WebView_GetPosition"), nLeft(0), nTop(0), nWidth(0), nHeight(0) {}
  ApIN ApHandle hView;
  ApOUT int nLeft;
  ApOUT int nTop;
  ApOUT int nWidth;
  ApOUT int nHeight;
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

// WebView ->
class Msg_WebView_Event_DocumentUnload: public ApNotificationMessage
{
public:
  Msg_WebView_Event_DocumentUnload() : ApNotificationMessage("WebView_Event_DocumentUnload") {}
  ApIN ApHandle hView;
};

// WebView ->
class Msg_WebView_Event_ReceivedFocus: public ApNotificationMessage
{
public:
  Msg_WebView_Event_ReceivedFocus() : ApNotificationMessage("WebView_Event_ReceivedFocus") {}
  ApIN ApHandle hView;
};

// WebView ->
class Msg_WebView_Event_LostFocus: public ApNotificationMessage
{
public:
  Msg_WebView_Event_LostFocus() : ApNotificationMessage("WebView_Event_LostFocus") {}
  ApIN ApHandle hView;
};

#endif // !defined(MsgWebView_h_INCLUDED)