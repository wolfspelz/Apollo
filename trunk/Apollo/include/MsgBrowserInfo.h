// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgBrowserInfo_h_INCLUDED)
#define MsgBrowserInfo_h_INCLUDED

#include "ApMessage.h"
#include "ApContainer.h"
#include "MsgNavigation.h"

// -> browser window tracker module
class Msg_BrowserInfo_BeginTrackCoordinates: public ApRequestMessage
{
public:
  Msg_BrowserInfo_BeginTrackCoordinates() : ApRequestMessage("BrowserInfo_BeginTrackCoordinates") {}
  ApIN ApHandle hContext;
  ApIN String sType;
  #define Msg_BrowserInfo_BeginTrackCoordinates_Type_Firefox Navigation_SrpcMethod_NativeWindow_Type_Firefox
  #define Msg_BrowserInfo_BeginTrackCoordinates_Type_InternetExplorer Navigation_SrpcMethod_NativeWindow_Type_InternetExplorer
  ApIN Apollo::KeyValueList kvSignature;
  #define Msg_BrowserInfo_BeginTrackCoordinates_Signature_Title Navigation_SrpcMethod_NativeWindow_Signature_Title
  #define Msg_BrowserInfo_BeginTrackCoordinates_Signature_Left Navigation_SrpcMethod_NativeWindow_Signature_Left
  #define Msg_BrowserInfo_BeginTrackCoordinates_Signature_Top Navigation_SrpcMethod_NativeWindow_Signature_Top
  #define Msg_BrowserInfo_BeginTrackCoordinates_Signature_Width Navigation_SrpcMethod_NativeWindow_Signature_Width
  #define Msg_BrowserInfo_BeginTrackCoordinates_Signature_Height Navigation_SrpcMethod_NativeWindow_Signature_Height
};

// -> browser window tracker module
class Msg_BrowserInfo_EndTrackCoordinates: public ApRequestMessage
{
public:
  Msg_BrowserInfo_EndTrackCoordinates() : ApRequestMessage("BrowserInfo_EndTrackCoordinates") {}
  ApIN ApHandle hContext;
};

// browser window tracker module -> 
class Msg_BrowserInfo_Visibility: public ApRequestMessage
{
public:
  Msg_BrowserInfo_Visibility() : ApRequestMessage("BrowserInfo_Visibility"), bVisible(1) {}
  ApIN ApHandle hContext;
  ApIN int bVisible;
};

// browser window tracker module -> 
class Msg_BrowserInfo_Position: public ApRequestMessage
{
public:
  Msg_BrowserInfo_Position() : ApRequestMessage("BrowserInfo_Position"), nLeft(0), nBottom(0) {}
  ApIN ApHandle hContext;
  ApIN int nLeft;
  ApIN int nBottom;
};

// browser window tracker module -> 
class Msg_BrowserInfo_Size: public ApRequestMessage
{
public:
  Msg_BrowserInfo_Size() : ApRequestMessage("BrowserInfo_Size"), nWidth(0), nHeight(0) {}
  ApIN ApHandle hContext;
  ApIN int nWidth;
  ApIN int nHeight;
};

#if defined(WIN32)
// browser window tracker module -> display module
class Msg_BrowserInfo_GetContextWin32Window: public ApRequestMessage
{
public:
  Msg_BrowserInfo_GetContextWin32Window() : ApRequestMessage("BrowserInfo_GetContextWin32Window"), hWnd(NULL) {}
  ApIN ApHandle hContext;
  ApIN HWND hWnd;
};
#endif // defined(WIN32)

#endif // !defined(MsgBrowserInfo_h_INCLUDED)
