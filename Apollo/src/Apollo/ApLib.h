// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApLib_h_INCLUDED)
#define ApLib_h_INCLUDED

#include "Apollo.h"
#include "ApTypes.h"
#include "MsgSystem.h"
#include "MsgMainLoop.h"
#include "MsgConfig.h"
#include "MsgUnitTest.h"
#include "MsgFile.h"
#include "MsgOS.h"

// -------------------------------------------------------------------

typedef int (*ApCoreCallF) (ApMessage* pMsg, int nFlags);
typedef int (*ApCoreInitF) ();
typedef int (*ApCoreExitF) ();

class ApLib
{
public:
  ApLib();
  virtual ~ApLib();

  int Init(int nArgc, char** pszArgv);
  int Exit();

  int callMsg(ApMessage* pMsg, int nFlags);
  int isMainThread();

  String normalizeLibraryName(const String& sFileName);
  HANDLE libraryLoad(const String& sDllPath);
  void* libraryGetProcAddress(HANDLE hDll, const String& sSymbol);
  int libraryFree(HANDLE hDll);

  ApHandle newHandle();
  String handle2String(ApHandle h);
  ApHandle string2Handle(const String& sString);

  String getRandomString(); // Unknown length
  String getRandomString(int nLength);
  String getUniqueId();

  Apollo::TimeValue getNow();
  void setNow(Apollo::TimeValue& tvNow);

  int checksumSigned8(const String& sString);

  String getCwd();
  String getMachineId();
  String getUserLoginName();
  String getUserProfilePath();







  // Message handlers
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);
  void On_System_GetRunLevel(Msg_System_GetRunLevel* pMsg);
  void On_System_GetTime(Msg_System_GetTime* pMsg);
  void On_System_GetHandle(Msg_System_GetHandle* pMsg);
  void On_System_SecTimer(Msg_System_SecTimer* pMsg);
  void On_System_GetCmdLineArgs(Msg_System_GetCmdLineArgs* pMsg);
  void On_MainLoop_EventLoopBegin(Msg_MainLoop_EventLoopBegin* pMsg);
  void On_MainLoop_EventLoopEnd(Msg_MainLoop_EventLoopEnd* pMsg);
  void On_Config_Loaded(Msg_Config_Loaded* pMsg);

  void On_File_Load(Msg_File_Load* pMsg);
  void On_File_Save(Msg_File_Save* pMsg);
  void On_File_Append(Msg_File_Append* pMsg);
  void On_File_Delete(Msg_File_Delete* pMsg);
  void On_File_Exists(Msg_File_Exists* pMsg);
  void On_File_Rename(Msg_File_Rename* pMsg);

  void On_OS_StartProcess(Msg_OS_StartProcess* pMsg);
  void On_OS_KillProcess(Msg_OS_KillProcess* pMsg);
  void On_OS_GetProcessId(Msg_OS_GetProcessId* pMsg);
  void On_OS_GetProcessInfo(Msg_OS_GetProcessInfo* pMsg);



















  // Config token replacement
  String InsertConfigAppTokens(const char* szConfigValue);
  String InsertConfigModuleTokens(const char* szModuleName, const char* szConfigValue);
  String ExpandConfigAppTokens(const char* szConfigValue);
  String ExpandConfigModuleTokens(const char* szModuleName, const char* szConfigValue);

  // Modules directory getter
  String getModulesDirectoryName();

  // Config token getters
  String AppBasePathToken();
  String AppResourcePathToken();
  String AppLibraryPathToken();
  String AppModulePathToken();
  String ModuleLibraryPathToken();
  String ModuleResourcePathToken();
  String UserProfilePathToken();

//protected: Should be protected, but, hey the Apollo namespace is our friend
  int nArgc_;
  char** pszArgv_;
  List lArgv_;
  ApHandle nNextHandle_;
  UINT nThreadId_;
  Apollo::TimeValue tvNow_;
  int n3Timer_;
  int n10Timer_;
  int n60Timer_;
  int n300Timer_;
  HANDLE hCoreDll_;
  ApCoreCallF fpCall_;
  ApCoreInitF fpInit_;
  ApCoreExitF fpExit_;
  
  String sRandomSeed_;
  int nRandomCnt_;

  // Modules subdirectory of libraries path
  String sModulesDirectoryName_;
  
  // Config tokens
  List lPathVars_;
  String sAppBasePathToken_;
  String sAppResourcePathToken_;
  String sAppLibraryPathToken_;
  String sAppModulePathToken_;
  String sModuleLibraryPathToken_;
  String sModuleResourcePathToken_;
  String sUserProfilePathToken_;

  String sRunLevel_;

  AP_MSG_REGISTRY_DECLARE;
};

extern ApLib gApLib;

#endif // !defined(ApLib_h_INCLUDED)
