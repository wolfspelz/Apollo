// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSystem_h_INCLUDED)
#define MsgSystem_h_INCLUDED

#include "ApMessage.h"

// core -> 
class APOLLO_API Msg_System_SecTimer: public ApNotificationMessage
{
public:
  Msg_System_SecTimer() : ApNotificationMessage("System_SecTimer") {}
};

// core -> 
class APOLLO_API Msg_System_3SecTimer: public ApNotificationMessage
{
public:
  Msg_System_3SecTimer() : ApNotificationMessage("System_3SecTimer") {}
};

// core -> 
class APOLLO_API Msg_System_10SecTimer: public ApNotificationMessage
{
public:
  Msg_System_10SecTimer() : ApNotificationMessage("System_10SecTimer") {}
};

// core -> 
class APOLLO_API Msg_System_60SecTimer: public ApNotificationMessage
{
public:
  Msg_System_60SecTimer() : ApNotificationMessage("System_60SecTimer") {}
};

// core -> 
class APOLLO_API Msg_System_5MinTimer: public ApNotificationMessage
{
public:
  Msg_System_5MinTimer() : ApNotificationMessage("System_5MinTimer") {}
};

// -------------------------------

// main -> 
class APOLLO_API Msg_System_AfterLoadModules: public ApNotificationMessage
{
public:
  Msg_System_AfterLoadModules() : ApNotificationMessage("System_AfterLoadModules") {}
};

// main -> 
class APOLLO_API Msg_System_BeforeUnloadModules: public ApNotificationMessage
{
public:
  Msg_System_BeforeUnloadModules() : ApNotificationMessage("System_BeforeUnloadModules") {}
};

// -------------------------------

// main -> 
class APOLLO_API Msg_System_BeforeEventLoop: public ApNotificationMessage
{
public:
  Msg_System_BeforeEventLoop() : ApNotificationMessage("System_BeforeEventLoop") {}
};

// main -> 
class APOLLO_API Msg_System_AfterEventLoop: public ApNotificationMessage
{
public:
  Msg_System_AfterEventLoop() : ApNotificationMessage("System_AfterEventLoop") {}
};

// -------------------------------

// main -> 
class APOLLO_API Msg_System_RunMode: public ApNotificationMessage
{
public:
  Msg_System_RunMode() : ApNotificationMessage("System_RunMode") {}
  enum { NoMode = 0
    ,RunModeConsole = 1
    ,RunModeGUI = 2
    ,RunModeDeamon = 3
  };
  ApIN int nMode;
};

// Start normal processing. Prior to this notification the system might be in test mode
// -> modules
#define Msg_System_RunLevel_PreBoot "PreBoot"
#define Msg_System_RunLevel_Boot "Boot"
#define Msg_System_RunLevel_Normal "Normal"
#define Msg_System_RunLevel_Shutdown "Shutdown"

class APOLLO_API Msg_System_RunLevel: public ApNotificationMessage
{
public:
  Msg_System_RunLevel() : ApNotificationMessage("System_RunLevel") {}
  ApIN String sLevel;
};

class APOLLO_API Msg_System_GetRunLevel: public ApRequestMessage
{
public:
  Msg_System_GetRunLevel() : ApRequestMessage("System_GetRunLevel") {}
  ApOUT String sLevel;
};

// -------------------------------------------------------------------

// -> mainloop
class APOLLO_API Msg_System_ThreadMessage: public ApMessage
{
public:
  Msg_System_ThreadMessage() : ApMessage("System_ThreadMessage") {}
  ApIN ApMessage* pMsg;
};

// -------------------------------------------------------------------

// -> main()
class APOLLO_API Msg_System_GetCmdLineArgs : public ApRequestMessage
{
public:
  Msg_System_GetCmdLineArgs() : ApRequestMessage("System_GetCmdLineArgs") {}
public:
  ApOUT Apollo::ValueList vlArgs;
};

class APOLLO_API Msg_System_GetCurrentWorkingDirectory : public ApRequestMessage
{
public:
  Msg_System_GetCurrentWorkingDirectory() : ApRequestMessage("System_GetCurrentWorkingDirectory") {}
  static String _() { Msg_System_GetCurrentWorkingDirectory msg; msg.Request(); return msg.sCwd; }
public:
  ApOUT String sCwd;
};

class APOLLO_API Msg_System_GetMachineId : public ApRequestMessage
{
public:
  Msg_System_GetMachineId() : ApRequestMessage("System_GetMachineId") {}
  static String _() { Msg_System_GetMachineId msg; msg.Request(); return msg.sId; }
public:
  ApOUT String sId;
};

class APOLLO_API Msg_System_GetUserLoginName : public ApRequestMessage
{
public:
  Msg_System_GetUserLoginName() : ApRequestMessage("System_GetUserLoginName") {}
  static String _() { Msg_System_GetUserLoginName msg; msg.Request(); return msg.sName; }
public:
  ApOUT String sName;
};

class APOLLO_API Msg_System_GetUserProfilePath : public ApRequestMessage
{
public:
  Msg_System_GetUserProfilePath() : ApRequestMessage("System_GetUserProfilePath") {}
  static String _() { Msg_System_GetUserProfilePath msg; msg.Request(); return msg.sPath; }
public:
  ApOUT String sPath;
};

// -------------------------------------------------------------------

// -> system
class APOLLO_API Msg_System_GetTime : public ApRequestMessage
{
public:
  Msg_System_GetTime() : ApRequestMessage("System_GetTime"), nSec(0), nMicroSec(0) {}
public:
  ApOUT int nSec;
  ApOUT int nMicroSec;
};

// -> system
class APOLLO_API Msg_System_GetHandle : public ApRequestMessage
{
public:
  Msg_System_GetHandle() : ApRequestMessage("System_GetHandle") {}
public:
  ApOUT ApHandle hResult;
};

// -> system
class APOLLO_API Msg_System_Echo : public ApRequestMessage
{
public:
  Msg_System_Echo() : ApRequestMessage("System_Echo") {}
public:
  ApIN int nIn;
  ApOUT int nOut;
};

#endif // !defined(MsgSystem_h_INCLUDED)
