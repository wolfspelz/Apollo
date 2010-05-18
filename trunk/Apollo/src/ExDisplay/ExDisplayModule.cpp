// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "ExDisplayModule.h"

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ExDisplayModule, Fluid_Created)
{
  if (pMsg->hDisplay == hDisplay_) {
    {
      ApSRPCMessage msg("ExDisplay_Notification");
      msg.srpc.setString("Method", "Opened");
      (void) msg.Call();
    }

    {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay_;
      msg.srpc.setString("Method", Fluid_SrpcMethod_SetTitle);
      msg.srpc.setString("sTitle", Apollo::translate(MODULE_NAME, "Window", "Title"));
      (void) msg.Call();
    }

    {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay_;
      msg.srpc.setString("Method", Fluid_SrpcMethod_SetPosition);
      msg.srpc.setInt("nLeft", Apollo::getModuleConfig(MODULE_NAME, "Left", 300));
      msg.srpc.setInt("nTop", Apollo::getModuleConfig(MODULE_NAME, "Top", 200));
      msg.srpc.setInt("nWidth", Apollo::getModuleConfig(MODULE_NAME, "Width", 500));
      msg.srpc.setInt("nHeight", Apollo::getModuleConfig(MODULE_NAME, "Height", 400));
      (void) msg.Call();
    }

    {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay_;
      msg.srpc.setString("Method", Fluid_SrpcMethod_SetVisibility);
      msg.srpc.setInt("nVisible", Apollo::getModuleConfig(MODULE_NAME, "Visible", 1));
      (void) msg.Call();
    }

  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, Fluid_Loaded)
{
  if (pMsg->hDisplay == hDisplay_) {
    if (Apollo::getModuleConfig(MODULE_NAME, "Html/VisibleOnLoaded", 1)) {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay_;
      msg.srpc.setString("Method", Fluid_SrpcMethod_SetVisibility);
      msg.srpc.setInt("nVisible", 1);
      (void) msg.Call();
    }
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, Fluid_Destroyed)
{
  if (pMsg->hDisplay == hDisplay_) {
    hDisplay_ = ApNoHandle;

    ApSRPCMessage msg("ExDisplay_Notification");
    msg.srpc.setString("Method", "Closed");
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, MainLoop_EventLoopBeforeEnd)
{
  if (ApIsHandle(hDisplay_)) {
    ApSRPCMessage msg("ExDisplay_Control");
    msg.srpc.setString("Method", "Close");
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, System_RunLevel)
{
  if (0) {
  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
    if (Apollo::getModuleConfig(MODULE_NAME, "AutoStartExe", 1)) {
      ApSRPCMessage msg("ExDisplay_Control");
      msg.srpc.setString("Method", "Open");
      (void) msg.Call();
    }
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_EnterLocationRequested)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_EnterLocationRequested");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_EnterLocationBegin)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_EnterLocationBegin");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_EnterLocationComplete)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_EnterLocationComplete");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_LeaveLocationRequested)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_LeaveLocationRequested");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_LeaveLocationBegin)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_LeaveLocationBegin");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_LeaveLocationComplete)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_LeaveLocationComplete");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_LocationsChanged)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_LocationsChanged");
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_LocationContextsChanged)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_LocationContextsChanged");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ContextLocationAssigned)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ContextLocationAssigned");
    msg.srpc.setString("hContext", pMsg->hContext.toString());
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ContextLocationUnassigned)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ContextLocationUnassigned");
    msg.srpc.setString("hContext", pMsg->hContext.toString());
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ParticipantsChanged)
{
  if (ApIsHandle(hDisplay_) && Apollo::getModuleConfig(MODULE_NAME, "ForwardParticipantsChangedNotification", 0)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ParticipantsChanged");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    msg.srpc.setInt("nCount", pMsg->nCount);
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ParticipantAdded)
{
  if (ApIsHandle(hDisplay_) && Apollo::getModuleConfig(MODULE_NAME, "ForwardParticipantPresenceEvents", 0)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ParticipantAdded");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    msg.srpc.setString("hParticipant", pMsg->hParticipant.toString());
    msg.srpc.setInt("bSelf", pMsg->bSelf);
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ParticipantRemoved)
{
  if (ApIsHandle(hDisplay_) && Apollo::getModuleConfig(MODULE_NAME, "ForwardParticipantPresenceEvents", 0)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ParticipantRemoved");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    msg.srpc.setString("hParticipant", pMsg->hParticipant.toString());
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_LocationDetailsChanged)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_LocationDetailsChanged");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    msg.srpc.setList("vlKeys", pMsg->vlKeys);
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ContextDetailsChanged)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ContextDetailsChanged");
    msg.srpc.setString("hContext", pMsg->hContext.toString());
    msg.srpc.setList("vlKeys", pMsg->vlKeys);
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ParticipantDetailsChanged)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ParticipantDetailsChanged");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    msg.srpc.setString("hParticipant", pMsg->hParticipant.toString());
    msg.srpc.setList("vlKeys", pMsg->vlKeys);
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_LocationPublicChat)
{
  if (ApIsHandle(hDisplay_)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_LocationPublicChat");
    msg.srpc.setString("hLocation", pMsg->hLocation.toString());
    msg.srpc.setString("hParticipant", pMsg->hParticipant.toString());
    msg.srpc.setString("hChat", pMsg->hChat.toString());
    msg.srpc.setString("sNickname", pMsg->sNickname);
    msg.srpc.setString("sText", pMsg->sText);
    msg.srpc.setInt("nSec", pMsg->nSec);
    msg.srpc.setInt("nMicroSec", pMsg->nMicroSec);
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ContextVisibility)
{
  if (ApIsHandle(hDisplay_) && Apollo::getModuleConfig(MODULE_NAME, "ForwardContextDetailEvents", 0)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ContextVisibility");
    msg.srpc.setString("hContext", pMsg->hContext.toString());
    msg.srpc.setInt("bVisible", pMsg->bVisible);
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ContextPosition)
{
  if (ApIsHandle(hDisplay_) && Apollo::getModuleConfig(MODULE_NAME, "ForwardContextDetailEvents", 0)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ContextPosition");
    msg.srpc.setString("hContext", pMsg->hContext.toString());
    msg.srpc.setInt("nX", pMsg->nX);
    msg.srpc.setInt("nY", pMsg->nY);
    (void) msg.Call();
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, VpView_ContextSize)
{
  if (ApIsHandle(hDisplay_) && Apollo::getModuleConfig(MODULE_NAME, "ForwardContextDetailEvents", 0)) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", "VpView_ContextSize");
    msg.srpc.setString("hContext", pMsg->hContext.toString());
    msg.srpc.setInt("nWidth", pMsg->nWidth);
    msg.srpc.setInt("nHeight", pMsg->nHeight);
    (void) msg.Call();
  }
}

AP_SRPC_HANDLER_METHOD(ExDisplayModule, ExDisplay_Control, ApSRPCMessage)
{
  String sMethod = pMsg->srpc.getString("Method");
  if (0) {
  } else if (sMethod == "Open") {
    if (!ApIsHandle(hDisplay_)) {
      String sContentUrl = Apollo::getModuleConfig(MODULE_NAME, "Html/ContentUrl", "");
      Msg_Fluid_Create msg;
      msg.hDisplay = hDisplay_ = Apollo::newHandle();
      msg.sPathname = Apollo::getModuleConfig(MODULE_NAME, "ExePath", "..\\..\\..\\fl\\ApolloLib\\SimpleHTML\\SimpleHTML.exe");
      if (!sContentUrl.empty()) {
        msg.sArgs.appendf("-url \"%s\"", StringType(sContentUrl));
      }
      msg.bVisible = Apollo::getModuleConfig(MODULE_NAME, "InitiallyVisible", 0);
      int ok = msg.Request();
    }

  } else if (sMethod == "Close") {
    if (ApIsHandle(hDisplay_)) {
      Msg_Fluid_Destroy msg;
      msg.hDisplay = hDisplay_;
      int ok = msg.Request();
    }

  }
}

AP_SRPC_HANDLER_METHOD(ExDisplayModule, ExDisplay_IO, Msg_Fluid_HostCall)
{
  String sMethod = pMsg->srpc.getString("Method");
  if (0) {

  } else if (sMethod == "xx") {

  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(ExDisplayModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ExDisplay", 0)) {
    //AP_UNITTEST_REGISTER(Test_Identity_CacheBasic);
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ExDisplay", 0)) {
    //AP_UNITTEST_EXECUTE(Test_Identity_CacheBasic);
  }
}

AP_MSG_HANDLER_METHOD(ExDisplayModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int ExDisplayModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, ExDisplay_Control, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, ExDisplay_IO, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, Fluid_Created, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, Fluid_Loaded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, Fluid_Destroyed, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, MainLoop_EventLoopBeforeEnd, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, System_RunLevel, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_EnterLocationRequested, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_EnterLocationBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_EnterLocationComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_LeaveLocationRequested, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_LeaveLocationBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_LeaveLocationComplete, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_LocationsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_LocationContextsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ContextLocationAssigned, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ContextLocationUnassigned, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ParticipantsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ParticipantAdded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ParticipantRemoved, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_LocationDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ContextDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ParticipantDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_LocationPublicChat, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ContextVisibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ContextPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, VpView_ContextSize, this, ApCallbackPosNormal);

#if defined(AP_TEST)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, UnitTest_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, UnitTest_Execute, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ExDisplayModule, UnitTest_End, this, ApCallbackPosNormal);
#endif

  return ok;
}

void ExDisplayModule::exit()
{
  AP_MSG_REGISTRY_FINISH;
}
