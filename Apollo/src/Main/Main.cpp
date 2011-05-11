// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Main/Main.h"

#define LOG_CHANNEL "Main"

#if defined(WIN32)
  static int bConsole = 1;
#endif

static String sLevelNames[apLog_NLogLevels] = {
   "None....."
  ,"Fatal...."
  ,"Error...."
  ,"Warning.."
  ,""
  ,"Debug...."
  ,"Info....."
  ,"Verbose.."
  ,"Trace...."
  ,"VVerbose."
  ,"Alert...."
};

static String getTimestamp()
{
  Apollo::TimeValue tvNow = Apollo::TimeValue::getTime();
  time_t tSec = tvNow.Sec();
  struct tm tms = *(::localtime(&tSec));
  
  String s;
  s.appendf("%02d:%02d:%02d.%06d", tms.tm_hour, tms.tm_min, tms.tm_sec, tvNow.MicroSec());
  return s;
}

void On_Log_Line(Msg_Log_Line* pMsg)
{
  int nLevel = apLog_Mask2Level(pMsg->nMask);

  String sOut = getTimestamp();
  if (sLevelNames[nLevel].chars() > 0) {
    sOut += " " + sLevelNames[nLevel];
  }
  if (pMsg->sChannel.chars() > 0) {
    sOut += " " + pMsg->sChannel;
  }
  if (pMsg->sContext.chars() > 0) {
    sOut += " " + pMsg->sContext;
  }
  if (pMsg->sMessage.chars() > 0) {
    sOut += " " + pMsg->sMessage;
  }

  sOut.escape(String::EscapeLogLine);

  sOut += "\n";

#if defined(WIN32)
  if (pMsg->nMask & apLog_MaskAlert) {
    ::MessageBox(NULL, sOut, _T("On_Log_Line"), MB_OK);
  } else {
    if (bConsole) {
      ::fprintf(stderr, "%s", sOut.c_str());
    } else {
      ::OutputDebugString(sOut);
    }
  }
#else
  ::fprintf(stderr, sOut);
#endif
}

void On_Log_SetMask(Msg_Log_SetMask* pMsg)
{
  pMsg->nOldMask = apLog_SetMask(pMsg->nMask);
}

//----------------------------------------------------------

void On_MainLoop_ConsoleInput(Msg_MainLoop_ConsoleInput* pMsg)
{
  if (0) {
  } else if (pMsg->sCmdLine == "q" || pMsg->sCmdLine == "") {
    Msg_MainLoop_EndLoop msg;
    msg.Request();
  }
}
