// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgOs_H_INCLUDED)
#define MsgOs_H_INCLUDED

#include "ApMessage.h"

class Msg_OS_StartProcess: public ApRequestMessage
{
public:
  Msg_OS_StartProcess() : ApRequestMessage("OS_StartProcess"), nPid(0) {}
  ApIN String sExePath;
  ApIN String sCwdPath;
  ApIN String sArgs;
  ApOUT int nPid;
};

class Msg_OS_KillProcess: public ApRequestMessage
{
public:
  Msg_OS_KillProcess() : ApRequestMessage("OS_KillProcess"), nPid(0) {}
  ApIN int nPid;
};

class Msg_OS_GetProcessId: public ApRequestMessage
{
public:
  Msg_OS_GetProcessId() : ApRequestMessage("OS_GetProcessId"), nPid(0) {}
  ApIN String sName;
  ApOUT int nPid;
};

class Msg_OS_GetProcessInfo: public ApRequestMessage
{
public:
  Msg_OS_GetProcessInfo() : ApRequestMessage("OS_GetProcessInfo"), nPid(0) {}
  ApIN int nPid;
  #define Msg_OS_GetProcessInfo_ProgramPath "Name"
  ApOUT Apollo::KeyValueList kvInfo;
};

#endif // MsgOs_H_INCLUDED
