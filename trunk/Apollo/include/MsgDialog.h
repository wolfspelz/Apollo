// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSample_h_INCLUDED)
#define MsgSample_h_INCLUDED

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

#endif // !defined(MsgSample_h_INCLUDED)
