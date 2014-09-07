// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "LogWindow.h"
#include "MsgLogWindow.h"
#include "MsgDialog.h"
#include "MsgTranslation.h"
#include "MsgLog.h"
#include "MsgWebView.h"
#include "MsgSystem.h"
#include "SrpcGateHelper.h"
#include "SrpcMessage.h"

#define MODULE_NAME "LogWindow"
#define LOG_CHANNEL MODULE_NAME
#define LOG_CONTEXT apLog_Context

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME, // szName
  "LogWindow", // szServices
  "Log window module", // szLongName
  "1", // szVersion
  "Show a (singleton) log window.", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

LOGWINDOW_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class LogWindow
{
public:
  LogWindow()
    :bReady_(false)
    ,bInside_(false)
    ,nMask_(-1)
  {}

  int Init();
  void Exit();

  void On_LogWindow_Open(Msg_LogWindow_Open* pMsg);
  void On_LogWindow_Close(Msg_LogWindow_Close* pMsg);
  void On_LogWindow_SetFilterMask(Msg_LogWindow_SetFilterMask* pMsg);
  void On_Dialog_OnOpened(Msg_Dialog_OnOpened* pMsg);
  void On_Dialog_OnClosed(Msg_Dialog_OnClosed* pMsg);
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);
  void On_Log_Line(Msg_Log_Line* pMsg);

protected:
  ApHandle hView_;
  bool bReady_;
  bool bInside_;
  int nMask_;

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;
  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<LogWindow> LogWindowInstance;

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(LogWindow, LogWindow_Open)
{
  if (ApIsHandle(hView_)) {
    Msg_WebView_MakeFrontWindow::_(hView_);
  } else {
    nMask_ = Apollo::getModuleConfig(MODULE_NAME, "Mask", -1);

    hView_ = Apollo::newHandle();

    Msg_Dialog_Create msg;
    msg.hDialog = hView_; 
    msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 300);
    msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 300);
    msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
    msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
    msg.bVisible = 1;
    msg.sCaption = Msg_Translation_Get::_(MODULE_NAME, "", "Log Window");
    msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
    msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "index.html";
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

    if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView_)) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(LogWindow, LogWindow_Close)
{
  bReady_ = false;

  Msg_Dialog_Destroy msg;
  msg.hDialog = hView_;
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(LogWindow, LogWindow_SetFilterMask)
{
  nMask_ = pMsg->nMask;
  Apollo::setModuleConfig(MODULE_NAME, "Mask", nMask_);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(LogWindow, Dialog_OnOpened)
{
  if (pMsg->hDialog == hView_) {

    Apollo::setModuleConfig(MODULE_NAME, "Open", 1);

    Msg_Dialog_CallScriptFunction msg;
    msg.hDialog = hView_;
    msg.sFunction = "Clear";
    (void) msg.Request();
    
    bReady_ = true;
  }
}

AP_MSG_HANDLER_METHOD(LogWindow, Dialog_OnClosed)
{
  if (pMsg->hDialog == hView_) {
    bReady_ = false;
    hView_ = ApNoHandle;
    Apollo::setModuleConfig(MODULE_NAME, "Open", 0);
  }
}

AP_MSG_HANDLER_METHOD(LogWindow, System_RunLevel)
{
  if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
    if (Apollo::getModuleConfig(MODULE_NAME, "Open", 0)) {
      Msg_LogWindow_Open msg; msg.Request();
    }
  }
}

static const char* szLevelNames[apLog_NLogLevels] = {
   ""
  ,"Fatal"
  ,"Error"
  ,"Warning"
  ,"User"
  ,"#########"
  ,"Info"
  ,"Verbose"
  ,"Trace"
  ,"..."
  ,"Alert"
};

AP_MSG_HANDLER_METHOD(LogWindow, Log_Line)
{
  if (bReady_ && !bInside_ &&  (nMask_ & pMsg->nMask) != 0) {
    bInside_ = true;

    Msg_Dialog_CallScriptFunction msg;
    msg.hDialog = hView_;
    msg.sFunction = "Line";
    int nLevel = apLog_Mask2Level(pMsg->nMask);
    String sLevel = nLevel < apLog_NLogLevels ? szLevelNames[nLevel] : "";
    msg.lArgs.AddLast(sLevel);
    msg.lArgs.AddLast(pMsg->sChannel);
    msg.lArgs.AddLast(pMsg->sContext);
    msg.lArgs.AddLast(pMsg->sMessage);
    (void) msg.Request();

    bInside_ = false;
  }
}

//----------------------------------------------------------

void SrpcGate_LogWindow_Open(ApSRPCMessage* pMsg)
{
  Msg_LogWindow_Open msg;
  msg.hLogWindow = pMsg->srpc.getHandle("hLogWindow");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_LogWindow_Close(ApSRPCMessage* pMsg)
{
  Msg_LogWindow_Close msg;
  msg.hLogWindow = pMsg->srpc.getHandle("hLogWindow");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_LogWindow_SetFilterMask(ApSRPCMessage* pMsg)
{
  Msg_LogWindow_SetFilterMask msg;
  msg.hLogWindow = pMsg->srpc.getHandle("hLogWindow");
  msg.nMask = pMsg->srpc.getInt("nMask");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

//----------------------------------------------------------

int LogWindow::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, LogWindow, LogWindow_Open, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, LogWindow, LogWindow_Close, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, LogWindow, LogWindow_SetFilterMask, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, LogWindow, Dialog_OnOpened, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, LogWindow, Dialog_OnClosed, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, LogWindow, System_RunLevel, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, LogWindow, Log_Line, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("LogWindow_Open", SrpcGate_LogWindow_Open);
  srpcGateRegistry_.add("LogWindow_Close", SrpcGate_LogWindow_Close);
  srpcGateRegistry_.add("LogWindow_SetFilterMask", SrpcGate_LogWindow_SetFilterMask);

  return ok;
}

void LogWindow::Exit()
{
  srpcGateRegistry_.finish();
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

LOGWINDOW_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  int ok = 0;

  LogWindowInstance::Delete();
  if (LogWindowInstance::Get() != 0) { ok = LogWindowInstance::Get()->Init(); }

  return ok;
}

LOGWINDOW_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  int ok = 1;

  if (LogWindowInstance::Get() != 0) {
    LogWindowInstance::Get()->Exit();
    LogWindowInstance::Delete();
  }

  return ok;
}
