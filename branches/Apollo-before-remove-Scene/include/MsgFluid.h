// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgFluid_h_INCLUDED)
#define MsgFluid_h_INCLUDED

#include "ApMessage.h"

// -> fluid
class Msg_Fluid_Create: public ApRequestMessage
{
public:
  Msg_Fluid_Create() : ApRequestMessage("Fluid_Create"), bVisible(1) {}
  ApIN ApHandle hDisplay;
  ApIN String sPathname;
  ApIN String sArgs;
  ApIN int bVisible;
};

// fluid ->
class Msg_Fluid_Created: public ApNotificationMessage
{
public:
  Msg_Fluid_Created() : ApNotificationMessage("Fluid_Created") {}
  ApIN ApHandle hDisplay;
  ApIN ApHandle hConnection;
};

// fluid ->
class Msg_Fluid_Loaded: public ApNotificationMessage
{
public:
  Msg_Fluid_Loaded() : ApNotificationMessage("Fluid_Loaded") {}
  ApIN ApHandle hDisplay;
};

// fluid ->
class Msg_Fluid_BeforeUnload: public ApNotificationMessage
{
public:
  Msg_Fluid_BeforeUnload() : ApNotificationMessage("Fluid_BeforeUnload") {}
  ApIN ApHandle hDisplay;
};

// fluid ->
class Msg_Fluid_Unloaded: public ApNotificationMessage
{
public:
  Msg_Fluid_Unloaded() : ApNotificationMessage("Fluid_Unloaded") {}
  ApIN ApHandle hDisplay;
};

// -> fluid
class Msg_Fluid_Destroy: public ApRequestMessage
{
public:
  Msg_Fluid_Destroy() : ApRequestMessage("Fluid_Destroy") {}
  ApIN ApHandle hDisplay;
};

// fluid ->
class Msg_Fluid_Destroyed: public ApNotificationMessage
{
public:
  Msg_Fluid_Destroyed() : ApNotificationMessage("Fluid_Destroyed") {}
  ApIN ApHandle hDisplay;
};

// -------------------------------

// fluid module -> flash ui
class Msg_Fluid_Send: public ApRequestMessage
{
public:
  Msg_Fluid_Send() : ApRequestMessage("Fluid_Send") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
};

// flash ui -> fluid module
class Msg_Fluid_Receive: public ApRequestMessage
{
public:
  Msg_Fluid_Receive() : ApRequestMessage("Fluid_Receive") {}
  ApIN ApHandle hConnection;
  ApIN Apollo::SrpcMessage srpc;
};

// -------------------------------

// -> fluid -> flash
class Msg_Fluid_DisplayCall: public ApSRPCMessage
{
public:
  Msg_Fluid_DisplayCall() : ApSRPCMessage("Fluid_DisplayCall") {}
  ApIN ApHandle hDisplay;
};

// flash -> fluid -> 
class Msg_Fluid_HostCall: public ApSRPCMessage
{
public:
  Msg_Fluid_HostCall(const char* szMethod) : ApSRPCMessage(szMethod) {}
  ApIN ApHandle hDisplay;
};

// -------------------------------

// SRPC via TCP navigator protocol

#define Fluid_SrpcMethodPrefix_Local "Local."
#define Fluid_SrpcMethod_Console "Local.Console"
#define Fluid_SrpcMethod_Connected "Local.Connected"
#define Fluid_SrpcMethod_Disconnected "Local.Disconnected"
#define Fluid_SrpcMethod_Shutdown "Local.Shutdown"
#define Fluid_SrpcMethod_Startparam "Local.Startparam"

#define Fluid_SrpcMethodPrefix_Host "Host."
#define Fluid_SrpcMethod_Hello "Host.Hello"
// sId

#define Fluid_SrpcMethod_Log "Host.Log"
// nLevel
// sContext
// sMessage

#define Fluid_SrpcMethod_GetConfig "Host.GetConfig"
// sModule
// sKey

#define Fluid_SrpcMethod_SetConfig "Host.SetConfig"
// sModule
// sKey
// sValue

#define Fluid_SrpcMethodPrefix_Display "Display."
#define Fluid_SrpcMethod_SetTitle "Display.SetTitle"
// sTitle

#define Fluid_SrpcMethod_SetVisibility "Display.SetVisibility"
// nVisible

#define Fluid_SrpcMethod_SetPosition "Display.SetPosition"
// nLeft
// nTop
// nWidth
// nHeight

#define Fluid_SrpcMethod_Loaded "Display.Loaded"

#define Fluid_SrpcMethod_BeforeUnload "Display.BeforeUnload"

#define Fluid_SrpcMethod_Unloaded "Display.Unloaded"

#endif // !defined(MsgFluid_h_INCLUDED)
