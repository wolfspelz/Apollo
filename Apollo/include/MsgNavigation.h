// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgNavigation_h_INCLUDED)
#define MsgNavigation_h_INCLUDED

#include "ApMessage.h"

// navigation module -> navigator via TCP connection
class Msg_Navigation_Send: public ApRequestMessage
{
public:
  Msg_Navigation_Send() : ApRequestMessage("Navigation_Send") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
};

// navigator via TCP connection -> navigation module
class Msg_Navigation_Receive: public ApRequestMessage
{
public:
  Msg_Navigation_Receive() : ApRequestMessage("Navigation_Receive") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
};

// navigation module -> navigation module
class Msg_Navigation_Connected: public ApNotificationMessage
{
public:
  Msg_Navigation_Connected() : ApNotificationMessage("Navigation_Connected") {}
  ApIN ApHandle hConnection;
};

// navigation module -> navigation module
class Msg_Navigation_Disconnected: public ApNotificationMessage
{
public:
  Msg_Navigation_Disconnected() : ApNotificationMessage("Navigation_Disconnected") {}
  ApIN ApHandle hConnection;
};

// -------------------------------

// navigator connection -> navigation
class Msg_Navigation_NavigatorHello: public ApRequestMessage
{
public:
  Msg_Navigation_NavigatorHello() : ApRequestMessage("Navigation_NavigatorHello") {}
  ApIN ApHandle hConnection;
};

// navigator connection -> navigation
class Msg_Navigation_NavigatorBye: public ApRequestMessage
{
public:
  Msg_Navigation_NavigatorBye() : ApRequestMessage("Navigation_NavigatorBye") {}
  ApIN ApHandle hConnection;
};

// navigator connection -> navigation
class Msg_Navigation_ContextOpen: public ApRequestMessage
{
public:
  Msg_Navigation_ContextOpen() : ApRequestMessage("Navigation_ContextOpen") {}
  ApIN ApHandle hConnection;
  ApIN ApHandle hContext;
};

// navigator connection -> navigation
// Does Navigation_ContextOpen implicitly 
class Msg_Navigation_ContextNavigate: public ApRequestMessage
{
public:
  Msg_Navigation_ContextNavigate() : ApRequestMessage("Navigation_ContextNavigate") {}
  ApIN ApHandle hConnection;
  ApIN ApHandle hContext;
  ApIN String sUrl;
};

// -> navigation
class Msg_Navigation_ContextClose: public ApRequestMessage
{
public:
  Msg_Navigation_ContextClose() : ApRequestMessage("Navigation_ContextClose") {}
  ApIN ApHandle hContext;
};

// -> navigation
class Msg_Navigation_ContextShow: public ApRequestMessage
{
public:
  Msg_Navigation_ContextShow() : ApRequestMessage("Navigation_ContextShow") {}
  ApIN ApHandle hContext;
};

// -> navigation
class Msg_Navigation_ContextHide: public ApRequestMessage
{
public:
  Msg_Navigation_ContextHide() : ApRequestMessage("Navigation_ContextHide") {}
  ApIN ApHandle hContext;
};

// -> navigation
class Msg_Navigation_ContextPosition: public ApRequestMessage
{
public:
  Msg_Navigation_ContextPosition() : ApRequestMessage("Navigation_ContextPosition"), nLeft(0), nBottom(0) {}
  ApIN ApHandle hContext;
  ApIN int nLeft;
  ApIN int nBottom;
};

// -> navigation
class Msg_Navigation_ContextSize: public ApRequestMessage
{
public:
  Msg_Navigation_ContextSize() : ApRequestMessage("Navigation_ContextSize"), nWidth(0), nHeight(0) {}
  ApIN ApHandle hContext;
  ApIN int nWidth;
  ApIN int nHeight;
};

// -> navigation
#define Navigation_ContextNativeWindow_Signature_Type "sType"
  #define Navigation_ContextNativeWindow_Signature_Type_Firefox "Firefox"
  #define Navigation_ContextNativeWindow_Signature_Type_InternetExplorer "InternetExplorer"
  #define Navigation_ContextNativeWindow_Signature_Type_Chrome "Chrome"
#define Navigation_ContextNativeWindow_Signature_Version "sVersion" // HTTP User-Agent
#define Navigation_ContextNativeWindow_Signature_Title "sTitle" // Browser window title
#define Navigation_ContextNativeWindow_Signature_Left "nLeft"
#define Navigation_ContextNativeWindow_Signature_Top "nTop"
#define Navigation_ContextNativeWindow_Signature_Width "nWidth"
#define Navigation_ContextNativeWindow_Signature_Height "nHeight"
#define Navigation_ContextNativeWindow_Signature_InnerWidth "nInnerWidth"
#define Navigation_ContextNativeWindow_Signature_InnerHeight "nInnerHeight"
#define Navigation_ContextNativeWindow_Signature_Win32HWND "nWin32HWND"
#define Navigation_ContextNativeWindow_Signature_Win32HWNDTarget "nWin32HWNDTarget"
class Msg_Navigation_ContextNativeWindow: public ApRequestMessage
{
  public:
  Msg_Navigation_ContextNativeWindow() : ApRequestMessage("Navigation_ContextNativeWindow") {}
  ApIN ApHandle hContext;
  ApIN Apollo::KeyValueList kvSignature;
};

#endif // !defined(MsgNavigation_h_INCLUDED)
