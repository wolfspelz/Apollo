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

AP_MSG_HANDLER_METHOD(LogModule, Log_Line)
{
  int nLevel = apLog_Mask2Level(pMsg->nMask);
  String sLevel = szLevelNames[nLevel];

  fileLog_.appendLog(sLevel, pMsg->sChannel, pMsg->sContext, pMsg->sMessage);
}

// ------------------------------------------------

void LogModule::Init()
{
  AP_MSG_REGISTRY_ADD(MODULE_NAME, LogModule, Log_Line, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("Log_Line", SrpcGate_Log_Line);
  srpcGateRegistry_.add("Log_SetMask", SrpcGate_Log_SetMask);
}

void LogModule::Exit()
{
  AP_MSG_REGISTRY_FINISH;
  srpcGateRegistry_.finish();
}

