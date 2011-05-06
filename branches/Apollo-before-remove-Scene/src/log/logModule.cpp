// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApLog.h"
#include "MsgLog.h"
#include "logModule.h"

// ------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(LogModule, Log_Line)

// ------------------------------------------------

void LogModule::Init()
{
  { Msg_Log_Line msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(LogModule, Log_Line), this, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
}

void LogModule::Exit()
{
  { Msg_Log_Line msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(LogModule, Log_Line), this); }
}

static const char* szLevelNames[apLog_NLogLevels] = {
   "None....."
  ,"Fatal...."
  ,"Error...."
  ,"Warning.."
  ,"Debug...."
  ,"Info....."
  ,"Verbose.."
  ,"Trace...."
  ,"VVerbose."
  ,"Alert...."
};

void LogModule::On_Log_Line(Msg_Log_Line* pMsg)
{
  int nLevel = apLog_Mask2Level(pMsg->nMask);
  String sLevel = szLevelNames[nLevel];

  fileLog_.appendLog(sLevel, pMsg->sChannel, pMsg->sContext, pMsg->sMessage);
}

