// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgProcess_H_INCLUDED)
#define MsgProcess_H_INCLUDED

#include "ApMessage.h"

class Msg_Process_Start: public ApRequestMessage
{
public:
  Msg_Process_Start() : ApRequestMessage("Process_Start"), nPid(0) {}
  ApIN String sExePath;
  ApIN String sCwdPath;
  ApIN Apollo::ValueList vlArgs;
  ApOUT int nPid;
};

class Msg_Process_Kill: public ApRequestMessage
{
public:
  Msg_Process_Kill() : ApRequestMessage("Process_Kill"), nPid(0) {}
  ApIN int nPid;
};

class Msg_Process_GetId: public ApRequestMessage
{
public:
  Msg_Process_GetId() : ApRequestMessage("Process_GetId"), nPid(0) {}
  ApIN String sName;
  ApOUT int nPid;
};

class Msg_Process_GetInfo: public ApRequestMessage
{
public:
  Msg_Process_GetInfo() : ApRequestMessage("Process_GetInfo"), nPid(0) {}
  ApIN int nPid;
  #define Msg_Process_GetInfo_ProgramPath "Name"
  ApOUT Apollo::KeyValueList kvInfo;
};

#endif // MsgProcess_H_INCLUDED
