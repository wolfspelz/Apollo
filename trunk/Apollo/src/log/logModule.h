// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(logModule_H_INCLUDED)
#define logModule_H_INCLUDED

#include "Apollo.h"
#include "SrpcGateHelper.h"
#include "logFile.h"

#define LOG_CHANNEL "log"
#define MODULE_NAME "log"

// ----------------------------------

class Msg_Core_BeforeModuleUnloaded;
class Msg_MainLoop_EventLoopBeforeEnd;
class Msg_Log_Line;

// ----------------------------------

class LogModule
{
public:
  LogModule() : bInShutdown_(0) {}
public:
  void Init();
  void Exit();

public:
  void On_Core_BeforeModuleUnloaded(Msg_Core_BeforeModuleUnloaded* pMsg);
  void On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg);
  void On_Log_Line(Msg_Log_Line* pMsg);

protected:
  FileLogger fileLog_;

protected:
  int bInShutdown_;
  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;
};

typedef ApModuleSingleton<LogModule> LogModuleInstance;

#endif // logModule_H_INCLUDED
