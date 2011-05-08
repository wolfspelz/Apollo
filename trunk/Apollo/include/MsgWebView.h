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
  static int _(const ApHandle& hWebView)
  {
    Msg_WebView_Create msg;
    msg.hWebView = hWebView;
    return msg.Request();
  }
  ApIN ApHandle hWebView;
};

class Msg_WebView_Destroy: public ApRequestMessage
{
public:
  Msg_WebView_Destroy() : ApRequestMessage("WebView_Destroy") {}
  static int _(const ApHandle& hWebView)
  {
    Msg_WebView_Destroy msg;
    msg.hWebView = hWebView;
    return msg.Request();
  }
  ApIN ApHandle hWebView;
};

class Msg_WebView_Position: public ApRequestMessage
{
public:
  Msg_WebView_Position() : ApRequestMessage("WebView_Position"), nX(0), nY(0), nW(0), nH(0) {}
  static int _(const ApHandle& hWebView, int nX, int nY, int nW, int nH)
  {
    Msg_WebView_Position msg;
    msg.hWebView = hWebView;
    msg.nX = nX;
    msg.nY = nY;
    msg.nW = nW;
    msg.nH = nH;
    return msg.Request();
  }
  ApIN ApHandle hWebView;
  ApIN int nX;
  ApIN int nY;
  ApIN int nW;
  ApIN int nH;
};

class Msg_WebView_Visibility: public ApRequestMessage
{
public:
  Msg_WebView_Visibility() : ApRequestMessage("WebView_Visibility"), bVisible(0) {}
  static int _(const ApHandle& hWebView, int bVisible)
  {
    Msg_WebView_Visibility msg;
    msg.hWebView = hWebView;
    msg.bVisible = bVisible;
    return msg.Request();
  }
  ApIN ApHandle hWebView;
  ApIN int bVisible;
};

class Msg_WebView_LoadHtml: public ApRequestMessage
{
public:
  Msg_WebView_LoadHtml() : ApRequestMessage("WebView_LoadHtml") {}
  static int _(const ApHandle& hWebView, const String& sHtml, const String& sBase)
  {
    Msg_WebView_LoadHtml msg;
    msg.hWebView = hWebView;
    msg.sHtml = sHtml;
    msg.sBase = sBase;
    return msg.Request();
  }
  ApIN ApHandle hWebView;
  ApIN String sHtml;
  ApIN String sBase;
};

class Msg_WebView_Load: public ApRequestMessage
{
public:
  Msg_WebView_Load() : ApRequestMessage("WebView_Load") {}
  static int _(const ApHandle& hWebView, const String& sUrl)
  {
    Msg_WebView_Load msg;
    msg.hWebView = hWebView;
    msg.sUrl = sUrl;
    return msg.Request();
  }
  ApIN ApHandle hWebView;
  ApIN String sUrl;
};

class Msg_WebView_CallJavaScriptFunction: public ApRequestMessage
{
public:
  Msg_WebView_CallJavaScriptFunction() : ApRequestMessage("WebView_CallJavaScriptFunction") {}
  ApIN ApHandle hWebView;
  ApIN String sMethod;
  ApIN List lArgs;
  ApOUT String sResult;
};

//--------------------------
// Events

// WebView ->
class Msg_WebView_Event_DocumentLoaded: public ApNotificationMessage
{
public:
  Msg_WebView_Event_DocumentLoaded() : ApNotificationMessage("WebView_Event_DocumentLoaded") {}
  ApIN ApHandle hWebView;
};

// WebView ->
class Msg_WebView_Event_DocumentComplete: public ApNotificationMessage
{
public:
  Msg_WebView_Event_DocumentComplete() : ApNotificationMessage("WebView_Event_DocumentComplete") {}
  ApIN ApHandle hWebView;
};

// WebView ->
class Msg_WebView_Event_BeforeNavigate: public ApFilterMessage
{
public:
  Msg_WebView_Event_BeforeNavigate() : ApFilterMessage("WebView_Event_BeforeNavigate"), bCancel(0) {}
  ApIN ApHandle hWebView;
  ApIN String sUrl;
  ApINOUT int bCancel;
};

// WebView ->
class Msg_WebView_Event_BeforeRequest: public ApFilterMessage
{
public:
  Msg_WebView_Event_BeforeRequest() : ApFilterMessage("WebView_Event_BeforeRequest") {}
  ApIN ApHandle hWebView;
  ApINOUT String sUrl;
};

#endif // !defined(MsgWebView_h_INCLUDED)
