// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApLog.h"
#include "MsgLog.h"
#include "logModule.h"

// ------------------------------------------------

void SrpcGate_Log_Line(ApSRPCMessage* pMsg)
{
  Msg_Log_Line msg;
  msg.nMask = pMsg->srpc.getInt("nMask");
  if (apLog_IsMask(msg.nMask)) {
    msg.sChannel = pMsg->srpc.getString("sChannel");
    msg.sContext = pMsg->srpc.getString("sContext");
    msg.sMessage = pMsg->srpc.getString("sMessage");
    msg.Send();
  }
}

void SrpcGate_Log_SetMask(ApSRPCMessage* pMsg)
{
  Msg_Log_SetMask msg;
  msg.nMask = pMsg->srpc.getInt("nMask");
  msg.Send();
}

// ------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(LogModule, Log_Line)

// ------------------------------------------------

void LogModule::Init()
{
  { Msg_Log_Line msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(LogModule, Log_Line), this, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }

  srpcGateRegistry_.add("Log_Line", SrpcGate_Log_Line);
  srpcGateRegistry_.add("Log_SetMask", SrpcGate_Log_SetMask);
}

void LogModule::Exit()
{
  srpcGateRegistry_.finish();

  { Msg_Log_Line msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(LogModule, Log_Line), this); }
}

static const char* szLevelNames[apLog_NLogLevels] = {
   "None....."
  ,"Fatal...."
  ,"Error...."
  ,"Warning.."
  ,"User....."
  ,"#########"
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

