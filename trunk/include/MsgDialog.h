// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgDialog_h_INCLUDED)
#define MsgDialog_h_INCLUDED

#include "ApMessage.h"

// -> Dialog 
class Msg_Dialog_Create: public ApRequestMessage
{
public:
  Msg_Dialog_Create() : ApRequestMessage("Dialog_Create"), nLeft(100), nTop(100), nWidth(400), nHeight(300), bVisible(1) {}
  ApIN ApHandle hDialog;
  ApIN int nLeft;
  ApIN int nTop;
  ApIN int nWidth;
  ApIN int nHeight;
  ApIN int bVisible;
  ApIN String sCaption;
  ApIN String sIconUrl;
  ApIN String sContentUrl;
};

// -> Dialog 
class Msg_Dialog_Destroy: public ApRequestMessage
{
public:
  Msg_Dialog_Destroy() : ApRequestMessage("Dialog_Destroy") {}
  ApIN ApHandle hDialog;
};

// -> Dialog 
class Msg_Dialog_GetView: public ApRequestMessage
{
public:
  Msg_Dialog_GetView() : ApRequestMessage("Dialog_GetView") {}
  ApIN ApHandle hDialog;
  ApOUT ApHandle hView;

  static ApHandle _(const ApHandle& hDialog)
  {
    Msg_Dialog_GetView msg;
    msg.hDialog = hDialog;
    if (msg.Request()) {
      return msg.hView;
    }
    return ApNoHandle;
  }
};

// -> Dialog
class Msg_Dialog_GetContentRect: public ApRequestMessage
{
public:
  Msg_Dialog_GetContentRect() : ApRequestMessage("Dialog_GetContentRect"), nLeft(0), nTop(0), nWidth(0), nHeight(0) {}
  ApIN ApHandle hDialog;
  ApOUT int nLeft;
  ApOUT int nTop;
  ApOUT int nWidth;
  ApOUT int nHeight;
};

// -> Dialog 
class Msg_Dialog_SetCaption: public ApRequestMessage
{
public:
  Msg_Dialog_SetCaption() : ApRequestMessage("Dialog_SetCaption") {}
  ApIN ApHandle hDialog;
  ApIN String sCaption;
};

// -> Dialog 
class Msg_Dialog_SetIcon: public ApRequestMessage
{
public:
  Msg_Dialog_SetIcon() : ApRequestMessage("Dialog_SetIcon") {}
  ApIN ApHandle hDialog;
  ApIN String sIconUrl;
};

class Msg_Dialog_CallScriptFunction: public ApRequestMessage
{
public:
  Msg_Dialog_CallScriptFunction() : ApRequestMessage("Dialog_CallScriptFunction") {}
  ApIN ApHandle hDialog;
  ApIN String sFunction;
  ApIN List lArgs;
  ApOUT String sResult;
};

// Module DLL -> script in dialog content
class Msg_Dialog_ContentCall: public ApRequestMessage
{
public:
  Msg_Dialog_ContentCall() : ApRequestMessage("Dialog_ContentCall") {}
  ApIN ApHandle hDialog;
  ApIN String sFunction;
  ApIN Apollo::SrpcMessage srpc;
  ApOUT Apollo::SrpcMessage response;

  static int _(const ApHandle& hDialog, const String& sFunction, const String& sMethod)
  {
    Msg_Dialog_ContentCall msg;
    msg.hDialog = hDialog;
    msg.sFunction = sFunction;
    msg.srpc.set(Srpc::Key::Method, sMethod);
    return msg.Request();
  }
  static int _(const ApHandle& hDialog, const String& sFunction, Apollo::SrpcMessage& srpc)
  {
    Msg_Dialog_ContentCall msg;
    msg.hDialog = hDialog;
    msg.sFunction = sFunction;
    srpc >> msg.srpc;
    return msg.Request();
  }
  static int _(const ApHandle& hDialog, const String& sFunction, Apollo::SrpcMessage& srpc, Apollo::SrpcMessage& response)
  {
    Msg_Dialog_ContentCall msg;
    msg.hDialog = hDialog;
    msg.sFunction = sFunction;
    srpc >> msg.srpc;
    int ok = msg.Request();
    msg.response >> response;
    return ok;
  }
};

// ------------------------------------

// Dialog -> 
class Msg_Dialog_OnOpened: public ApNotificationMessage
{
public:
  Msg_Dialog_OnOpened() : ApNotificationMessage("Dialog_OnOpened") {}
  ApIN ApHandle hDialog;
};

// Dialog -> 
class Msg_Dialog_OnClosed: public ApNotificationMessage
{
public:
  Msg_Dialog_OnClosed() : ApNotificationMessage("Dialog_OnClosed") {}
  ApIN ApHandle hDialog;
};

#endif // !defined(MsgDialog_h_INCLUDED)
