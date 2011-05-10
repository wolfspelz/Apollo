// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Apollo_h_INCLUDED)
#define Apollo_h_INCLUDED

#if defined(WIN32)
  #undef _WIN32_WINNT
  #define _WIN32_WINNT 0x0501

  // Exclude rarely-used stuff from Windows headers, especially <winsock.h>
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #undef WIN32_LEAN_AND_MEAN

  #include <tchar.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>

#define STRSAFE_NO_DEPRECATE 1
  #include <strsafe.h>
#undef STRSAFE_NO_DEPRECATE

#else
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <dirent.h>
  #include <unistd.h>
#if defined(LINUX) || defined(MAC)
  #include <sys/syscall.h> // SYSs_gettid
  #include <dlfcn.h>
#endif

#endif

#include "SSystem.h"
#include "ApCompiler.h"
#include "ApExports.h"
#include "Project.h"
#include "ApMessage.h"
#include "ApModule.h"

// -------------------------------------------------------------------

#define APOLLO_NAME "Apollo"

#if defined(WIN32)
typedef HWND ApWindowHandle;
#else
typedef void *ApWindowHandle;
#endif

#define AP_TEST_RELEASE 1
#if defined(_DEBUG) || defined(AP_TEST_RELEASE)
  #define AP_TEST
#endif

// -------------------------------------------------------------------

AP_NAMESPACE_BEGIN

APOLLO_API int Init(int nArgc, char** pszArgv);
APOLLO_API int Exit();

enum CallFlag {
   CF_UNSYNCHRONIZED = 0x01
  ,CF_ASYNC = 0x02
};

APOLLO_API int callMsg(ApMessage* pMsg, int nFlags = 0);

APOLLO_API int hookMsg(const char* szCallName, const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition);
APOLLO_API int unhookMsg(const char* szCallName, const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef);
APOLLO_API ApCallbackPos modulePos(long nBase, const char* szModuleName);
APOLLO_API int isMainThread();

APOLLO_API String normalizeLibraryName(const String& sFileName);
APOLLO_API HANDLE libraryLoad(const String& sDllPath);
APOLLO_API void* libraryGetProcAddress(HANDLE hDll, const String& sSymbol);
APOLLO_API int libraryFree(HANDLE hDll);

APOLLO_API ApHandle newHandle();
APOLLO_API String handle2String(const ApHandle& h);
APOLLO_API ApHandle string2Handle(const String& sString);

APOLLO_API int getRandom(int nMax);
APOLLO_API String getRandomString(int nLength);
APOLLO_API String getUniqueId();
APOLLO_API String getShortHash(const String& sData, unsigned int nLength);

APOLLO_API TimeValue getNow();
APOLLO_API void setNow(TimeValue& tv);

APOLLO_API int formatVarArg(char* str, size_t size, const char* format, va_list ap);

APOLLO_API int loadModule(const char* szModuleName, const char* szDllPath);
APOLLO_API int loadModulesFromDir(const char* szDirPath);
APOLLO_API int loadModulesFromConfig(const char* szKey);
APOLLO_API int isLoadedModule(const char* szModuleName);
APOLLO_API int unloadModules();
APOLLO_API int unloadModule(const char* szModuleName);

APOLLO_API void sendLog(int nMask, const char* szChannel, const char* szContext, const char* szMessage);

APOLLO_API void setConfig(const char* szPath, int nValue);
APOLLO_API int getConfig(const char* szPath, int nDefault);
APOLLO_API void setConfig(const char* szPath, double fValue);
APOLLO_API double getConfig(const char* szPath, double fDefault);
APOLLO_API void setConfig(const char* szPath, const char* szValue);
APOLLO_API String getConfig(const char* szPath, const char* szDefault);
APOLLO_API void deleteConfig(const char* szPath);
APOLLO_API int getConfigKeys(const char* szPath, ValueList& vlKeys);
APOLLO_API void setModuleConfig(const char* szModuleName, const char* szPath, int nValue);
APOLLO_API int getModuleConfig(const char* szModuleName, const char* szPath, int nDefault);
APOLLO_API void setModuleConfig(const char* szModuleName, const char* szPath, double fValue);
APOLLO_API double getModuleConfig(const char* szModuleName, const char* szPath, double fDefault);
APOLLO_API void setModuleConfig(const char* szModuleName, const char* szPath, const char* szValue);
APOLLO_API String getModuleConfig(const char* szModuleName, const char* szPath, const char* szDefault);
APOLLO_API void deleteModuleConfig(const char* szModuleName, const char* szPath);
APOLLO_API int getModuleConfigKeys(const char* szModuleName, const char* szPath, ValueList& vlKeys);

APOLLO_API String translate(const char* szModule, const char* szContext, const char* szText);
APOLLO_API String translate(const char* szModule, const char* szContext, const char* szText, List& lVars);

APOLLO_API ApHandle startInterval(int nSec, int nMicroSec);
APOLLO_API ApHandle startTimeout(int nSec, int nMicroSec);
APOLLO_API int cancelInterval(const ApHandle& hTimer);
APOLLO_API int cancelTimeout(const ApHandle& hTimer);

APOLLO_API int loadFile(const String& sFile, Buffer& sbData);
APOLLO_API int loadFile(const String& sFile, String& sbData);
APOLLO_API int saveFile(const String& sFile, const Buffer& sbData);
APOLLO_API int saveFile(const String& sFile, const String& sData);
APOLLO_API int appendFile(const String& sFile, const Buffer& sbData);
APOLLO_API int appendFile(const String& sFile, const String& sData);
APOLLO_API int deleteFile(const String& sFile);
APOLLO_API int fileExists(const String& sFile);
APOLLO_API int renameFile(const String& sFile, const String& sNewName);

APOLLO_API String getCwd();
APOLLO_API String getMachineId();
APOLLO_API String getUserLoginName();
APOLLO_API String getUserProfilePath();















APOLLO_API String getAppBasePath();
APOLLO_API String getAppResourcePath();
APOLLO_API String getAppLibraryPath();
APOLLO_API String getAppModulePath();
// Module Paths
APOLLO_API String getModuleLibraryPath(const String& sModuleBaseName);
APOLLO_API String getModuleResourcePath(const String& sModuleBaseName);
APOLLO_API String applyPathVars(const String& sPath);
APOLLO_API String getModulePackageName(const String& sModuleName);









#if defined (AP_TEST)
typedef enum _Test_CompareLists_Flags { Test_CompareLists_Flag_IgnoreOrder = 1 };
APOLLO_API String Test_CompareLists(const char* szText, List& lData, List& lExpected, int nFlags = 0);
APOLLO_API String Test_CompareLists(const char* szText, Apollo::KeyValueList& kvData, Apollo::KeyValueList& kvExpected, int nFlags = 0);
APOLLO_API String Test_CompareLists(const char* szText, Apollo::ValueList& vlData, Apollo::ValueList& vlExpected, int nFlags = 0);
#endif

AP_NAMESPACE_END

// -------------------------------------------------------------------

#include "ApOS.h"

// Log
extern "C" {
  typedef int (*ApHookMsgF) (const char* szCallName, const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition);

  APOLLO_API void apLog_FatalI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);
  APOLLO_API void apLog_ErrorI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);
  APOLLO_API void apLog_WarningI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);
  APOLLO_API void apLog_UserI(const char* _szFmt, ...);
  APOLLO_API void apLog_DebugI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);
  APOLLO_API void apLog_InfoI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);
  APOLLO_API void apLog_VerboseI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);
  APOLLO_API void apLog_TraceI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);
  APOLLO_API void apLog_VeryVerboseI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);
  APOLLO_API void apLog_AlertI(const char* _szChannel, const char* _szContext, const char* _szFmt, ...);

  APOLLO_API int apLog_SetMask(int _nMask);
  APOLLO_API int apLog_IsMask(int _nMask);
  APOLLO_API int apLog_Mask2Level(int _nMask);
  APOLLO_API int apLog_Level2Mask(int _nLevel);
  APOLLO_API void apLog_LogMessage(int _nMask, const char* _szChannel, const char* _szContext, const char* _szMessage);
}

class APOLLO_API apLog_TraceObject
{
public:
  apLog_TraceObject(const char* _szChannel, const char* _szContext);
  virtual ~apLog_TraceObject();
};

#endif // !defined(Apollo_h_INCLUDED)
