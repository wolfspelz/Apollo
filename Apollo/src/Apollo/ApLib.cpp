// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApLib.h"
#include "ApLog.h"
#include "MessageDigest.h"

#if defined(WIN32)
#include "Objbase.h" // for CoInitialize
#endif

#if defined(WIN32)
  #include "shlobj.h" // for _GetProfilePath
#endif

#define APLIB_NAME "ApLib"
#define LOG_CHANNEL APLIB_NAME
#define MODULE_NAME APLIB_NAME

// -------------------------------------------------------------------

ApLib::ApLib()
:nArgc_(0)
,pszArgv_(0)
,nNextHandle_(0, 1)
,nThreadId_(Apollo::GetCurrentThreadId())
,tvNow_(Apollo::TimeValue::getTime())
,n3Timer_(0)
,n10Timer_(0)
,n60Timer_(0)
,n300Timer_(0)
,fpCall_(0)
,fpInit_(0)
,fpExit_(0)
,nRandomCnt_(1)
,hCoreDll_(0)
,sAppBasePathToken_("[[APP_BASE_PATH]]")
,sAppResourcePathToken_("[[APP_RESOURCE_PATH]]")
,sAppLibraryPathToken_("[[APP_LIBRARY_PATH]]")
,sAppModulePathToken_("[[APP_MODULE_PATH]]")
,sModuleLibraryPathToken_("[[MODULE_LIBRARY_PATH]]")
,sModuleResourcePathToken_("[[MODULE_RESOURCE_PATH]]")
,sUserProfilePathToken_("[[USER_PROFILE]]")
,sModulesDirectoryName_("modules")
{
}

ApLib::~ApLib()
{
}

// -------------------------------------------------------------------

int ApLib::callMsg(ApMessage* pMsg, int nFlags)
{
  if (fpCall_ != 0) {
    return fpCall_(pMsg, nFlags);
  } else {
    return 0;
  }
}

int ApLib::isMainThread()
{
  return (Apollo::GetCurrentThreadId() == nThreadId_);
}

// --------------------------------

String ApLib::normalizeLibraryName(const String& sFileName)
{
  String sWorkStr = sFileName;
  
  int bGNUModule = 0;
  
  if (0) {
#if defined(_DEBUG)
  } else if (sWorkStr.subString(sWorkStr.chars() - 7, 7) == "d.dylib") {
    sWorkStr = sWorkStr.subString(0, sWorkStr.chars() - 7);
    bGNUModule = 1;
  } else if (sWorkStr.subString(sWorkStr.chars() - 4, 4) == "d.so") {
    sWorkStr = sWorkStr.subString(0, sWorkStr.chars() - 4);
    bGNUModule = 1;
#else
  } else if (sWorkStr.subString(sWorkStr.chars() - 6, 6) == ".dylib") {
    sWorkStr = sWorkStr.subString(0, sWorkStr.chars() - 6);
    bGNUModule = 1;
  } else if (sWorkStr.subString(sWorkStr.chars() - 3, 3) == ".so") {
    sWorkStr = sWorkStr.subString(0, sWorkStr.chars() - 3);
    bGNUModule = 1;
#endif
  } else if (sWorkStr.subString(sWorkStr.chars() - 4, 4) == ".dll") {
    sWorkStr = sWorkStr.subString(0, sWorkStr.chars() - 4);
  }
  
  if (bGNUModule && sWorkStr.subString(0, 3) == "lib") {
    sWorkStr = sWorkStr.subString(3, sWorkStr.chars() - 3);
  }
  
#if defined(_WIN32)
  sWorkStr += ".dll";
#elif defined(LINUX) || defined(MAC)
  sWorkStr = "lib" + sWorkStr;
#if defined(_DEBUG)
  sWorkStr += "d";
#endif
#if defined(LINUX)
  sWorkStr += ".so";
#elif defined(MAC)
  sWorkStr += ".dylib";
#endif
#else
#error Module loader not implemented for this OS
#endif
  
  return sWorkStr;
}

// --------------------------------

HANDLE ApLib::libraryLoad(const String& sDllPath)
{
  HANDLE hDll = NULL;
#if defined(WIN32)
  hDll = (HANDLE) ::LoadLibrary(sDllPath);
#elif defined(LINUX) || defined(MAC)
  hDll = (HANDLE)dlopen(sDllPath, RTLD_NOW | RTLD_LOCAL);
#else
  #error not implemented for this os
#endif
  if (hDll == NULL) {
    #if defined(LINUX) || defined(MAC)
      String sError = dlerror();
      apLog_Warning((LOG_CHANNEL, "ApLib::libraryLoad", "dlopen(%s) failed: %s", StringType(sDllPath), StringType(sError)));
    #else
      apLog_Warning((LOG_CHANNEL, "ApLib::libraryLoad", "failed to load library %s %d", StringType(sDllPath), GetLastError()));
    #endif
    AP_DEBUG_BREAK();
  }
  return hDll;
}

void* ApLib::libraryGetProcAddress(HANDLE hDll, const String& sSymbol)
{
#if defined(WIN32)
  return (void*) ::GetProcAddress((HINSTANCE)hDll, StringType(sSymbol));
#elif defined(LINUX) || defined(MAC)
  dlerror();
  HANDLE hSym = dlsym(hDll, StringType(sSymbol));
  if (hSym == NULL) {
    String sErr = dlerror();
    if (!sErr.empty()) { apLog_Warning((LOG_CHANNEL, "ApLib::LibraryGetAddress", "dlsym failed: %s", StringType(sErr))); }
  }
  return hSym;
#else
  #error not implemented for this os
#endif
}

int ApLib::libraryFree(HANDLE hDll)
{
  int ok = 1;
  
#if defined(WIN32)
  ::FreeLibrary((HINSTANCE)hDll);
#elif defined(LINUX) || defined(MAC)
  if (dlclose(hDll) != 0) {
    apLog_Warning((LOG_CHANNEL, "ApLib::libraryFree", "dlclose failed: %s", dlerror()));
    ok = 0;
  }
#else
  #error not implemented for this os
#endif
  
  return ok;
}

// --------------------------------

ApHandle ApLib::newHandle()
{
  ++nNextHandle_;
  return nNextHandle_;
}

String ApLib::handle2String(const ApHandle& h)
{
  return h.toString();
}

ApHandle ApLib::string2Handle(const String& s)
{
  ApHandle h;
  h.fromString(s);
  return h;
}

// --------------------------------

String ApLib::getRandomString()
{
  String sData = sRandomSeed_ + String::from(nRandomCnt_++);
  Apollo::MessageDigest md((unsigned char*) sData.c_str(), sData.bytes());
  String sRandom = md.getSHA1Hex();
  return sRandom;
}

String ApLib::getRandomString(int nLength)
{
  String sRandom;

  while (sRandom.chars() < nLength) {
    String sId = getRandomString();
    sRandom += sId.subString(0, nLength - sRandom.chars());
  }

  return sRandom;
}

String ApLib::getUniqueId()
{
  return getRandomString(20);
}

// --------------------------------

Apollo::TimeValue ApLib::getNow() 
{
  return tvNow_;
}

void ApLib::setNow(Apollo::TimeValue& tvNow)
{
  tvNow_ = tvNow;
}

// --------------------------------

int ApLib::checksumSigned8(const String& sString)
{
  int nSum = 0;
  unsigned char* p = (unsigned char*) sString.c_str();
  while (*p != 0) {
    nSum += *p;
    nSum = nSum % 256;
    p++;
  }
  return nSum - 128;
}

String ApLib::shortHash(const String& sData, unsigned int nLength)
{
  Apollo::MessageDigest md((unsigned char*) sData.c_str(), sData.bytes());
  return md.getSHA1Hex().subString(0, nLength);
}

// --------------------------------

String ApLib::getCwd()
{
  Flexbuf<char> buf(4*1024);
  String sCwd = ::getcwd(buf, buf.length());
  if (!sCwd.endsWith(String::filenamePathSeparator())) { sCwd += String::filenamePathSeparator(); }
  return sCwd;
}

#if defined(WIN32)
  #include <iptypes.h>
  #include <Iphlpapi.h>
  #pragma comment(lib, "iphlpapi.lib")
#endif

String ApLib::getMachineId()
{
  String sMachineId;

  String sData;

#if defined(WIN32)
  String sVolumeSerialNumber;
  {
    DWORD dwVolumeSerialNumber = 0;
    String sRootPathName = "C:\\";
    if (::GetVolumeInformation(
      sRootPathName, // LPCTSTR lpRootPathName,
      0,// LPTSTR lpVolumeNameBuffer,
      0,// DWORD nVolumeNameSize,
      &dwVolumeSerialNumber, // LPDWORD lpVolumeSerialNumber,
      0,// LPDWORD lpMaximumComponentLength,
      0,// LPDWORD lpFileSystemFlags,
      0,// LPTSTR lpFileSystemNameBuffer,
      0 // DWORD nFileSystemNameSize
      )) {
        sVolumeSerialNumber = String::from(dwVolumeSerialNumber);
    }
  }

  String sComputerName;
  {
    WCHAR pNameBuffer[10240+1];
    DWORD nNameBufferSize = 10240;
    if (::GetComputerName(pNameBuffer, &nNameBufferSize)) {
      pNameBuffer[nNameBufferSize] = 0;
      sComputerName = pNameBuffer;
    }
  }

  String sMacAddress;
  {
    List lAddress;

    DWORD dwBufLen = 0;
    if (::GetAdaptersInfo(0, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
      PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO) malloc(dwBufLen);
      if (pAdapterInfo != 0) {
        if (GetAdaptersInfo(pAdapterInfo, &dwBufLen) == ERROR_SUCCESS) {
          PIP_ADAPTER_INFO ppAdapterInfo = pAdapterInfo;
          do {
            String sIpAddress = (char*) & (ppAdapterInfo->IpAddressList.IpAddress);
            if (sIpAddress != "0.0.0.0") {
              String sMac;
              sMac.appendf("%02X%02X%02X%02X%02X%02X"
                , ppAdapterInfo->Address[0]
                , ppAdapterInfo->Address[1]
                , ppAdapterInfo->Address[2]
                , ppAdapterInfo->Address[3]
                , ppAdapterInfo->Address[4]
                , ppAdapterInfo->Address[5]
                );
                lAddress.AddLast(sMac, (long) ppAdapterInfo->DhcpEnabled);
            }

            ppAdapterInfo = ppAdapterInfo->Next;
          } while (ppAdapterInfo != 0);
        }
        ::free(pAdapterInfo);
        pAdapterInfo = 0;
      }
    }

    for (Elem* e = 0; (e = lAddress.Next(e)) != 0; ) {
      if (e->getInt()) {
        sMacAddress += e->getName();
      }
    }

    if (sMacAddress.empty() && lAddress.length() > 0) {
      sMacAddress = lAddress.First()->getName();
    }
  }

  if (sMacAddress.isset()) {
    sData += sMacAddress;
  } else if (sVolumeSerialNumber.isset()) {
    sData += sComputerName;
    sData += sVolumeSerialNumber;
  }

#endif

  if (sData != "") {
    //Apollo::MessageDigest digest((unsigned char *) sData.c_str(), sData.bytes());
    //sMachineId = digest.getSHA1Hex();
    sMachineId = sData;
  }

  return sMachineId;
}

#if defined(WIN32)
static String _GetProfilePath()
{
  String sPath;

  ITEMIDLIST* pidl;
  HRESULT hr = SHGetFolderLocation(NULL, CSIDL_APPDATA, NULL, 0, &pidl);
  if (hr == S_OK) {
    TCHAR szPath[MAX_PATH];
    if (SHGetPathFromIDList(pidl, szPath)) {
      sPath = szPath;
      sPath.makeTrailingSlash();
    }
    CoTaskMemFree(pidl);
  }

  return sPath;
}
#endif

#if defined(WIN32)
  #ifndef SECURITY_WIN32
    #define SECURITY_WIN32
  #endif
#include "Security.h"
#endif 

String ApLib::getUserLoginName()
{
  String s;

#if defined(WIN32)
  if (s.empty()) {
    TCHAR tzBuf[1024+1];
    DWORD nSize = 1024;
    if (::GetUserNameEx(NameDisplay, tzBuf, &nSize)) {
      s = tzBuf;
    } else {
      DWORD nError = ::GetLastError();
      nError = nError;
    }
  }

  if (s.empty()) {
    TCHAR tzBuf[UNLEN+1];
    DWORD nSize = UNLEN;
    if (::GetUserName(tzBuf, &nSize)) {
      s = tzBuf;
    }
  }

#elif defined(LINUX)
  //s = "unknown";
  #error Not implemented for this OS

#else
  #error Not implemented for this OS
#endif

  return s;
}

String ApLib::getUserProfilePath()
{
  String s;

#if defined(WIN32)
  //s = "C:\\Dokumente und Einstellungen\\wolf\\Anwendungsdaten\\Avatar\\";
  s = _GetProfilePath();
  s.makeTrailingSlash();
  s += Apollo::getConfig("System/UserProfileFolder", "Apollo");
  s.makeTrailingSlash();

#elif defined(LINUX)
  //s = "~wolf/.Avatar/";
  String sUser = Apollo::getUserLoginName();
  s = "~" + sUser + "/." + Apollo::getConfig("System/UserProfileFolder", "Apollo");
  s.makeTrailingSlash();

#else
  #error Not implemented for this OS
#endif

  int bCanReadWrite = 0;
  String sAssertWriteabilityPath = s + "base.txt";
  String sAssertWriteabilityData = s;
  if (Apollo::saveFile(sAssertWriteabilityPath, sAssertWriteabilityData)) {
    String sData;
    if (Apollo::loadFile(sAssertWriteabilityPath, sData)) {
      if (sAssertWriteabilityData == sData) {
        bCanReadWrite = 1;
      }
    }
  }
  if (!bCanReadWrite) {
    s = Apollo::getAppBasePath();
    //apLog_Error((LOG_CHANNEL, "SQLiteFile::pathFromName", "Can not read/write %s", StringType(sAssertWriteabilityPath)));
  }

  return s;
}


















// -------------------------------------------------------------------

AP_MSG_HANDLER_METHOD(ApLib, System_RunLevel)
{
  sRunLevel_ = pMsg->sLevel;
}

AP_MSG_HANDLER_METHOD(ApLib, System_GetRunLevel)
{
  pMsg->sLevel = sRunLevel_;
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ApLib, System_GetTime)
{
  Apollo::TimeValue tv = Apollo::getNow();
  pMsg->nSec = tv.Sec();
  pMsg->nMicroSec = tv.MicroSec();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ApLib, System_GetHandle)
{
  Apollo::TimeValue tv = Apollo::getNow();
  pMsg->h = Apollo::newHandle();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ApLib, System_Echo)
{
  pMsg->nOut = pMsg->nIn;
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ApLib, System_SecTimer)
{
  AP_UNUSED_ARG(pMsg);
  tvNow_ = Apollo::TimeValue::getTime();

  unsigned int nNowSec = tvNow_.Sec();

  if (nNowSec - n3Timer_ >= 3) {
    n3Timer_ = nNowSec;
    Msg_System_3SecTimer msg;
    msg.Send();
  }

  if (nNowSec - n10Timer_ >= 10) {
    n10Timer_ = nNowSec;
    Msg_System_10SecTimer msg;
    msg.Send();
  }

  if (nNowSec - n60Timer_ >= 60) {
    n60Timer_ = nNowSec;
    Msg_System_60SecTimer msg;
    msg.Send();
  }

  if (nNowSec - n300Timer_ >= 300) {
    n300Timer_ = nNowSec;
    Msg_System_5MinTimer msg;
    msg.Send();
  }
}

AP_MSG_HANDLER_METHOD(ApLib, System_GetCmdLineArgs)
{
  pMsg->nArgc = nArgc_;
  pMsg->pszArgv = pszArgv_;

  pMsg->apStatus = ApMessage::Ok;
  pMsg->Stop();
}

AP_MSG_HANDLER_METHOD(ApLib, MainLoop_EventLoopBegin)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_System_RunLevel msg; msg.sLevel = Msg_System_RunLevel_Normal; msg.Send(); }
}

AP_MSG_HANDLER_METHOD(ApLib, MainLoop_EventLoopEnd)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_System_RunLevel msg; msg.sLevel = Msg_System_RunLevel_Shutdown; msg.Send(); }
}

AP_MSG_HANDLER_METHOD(ApLib, Config_Loaded)
{
  AP_UNUSED_ARG(pMsg);
  
  sModulesDirectoryName_ = Apollo::getConfig("Core/ModulesDirectory", sModulesDirectoryName_);
  sAppBasePathToken_ = Apollo::getConfig("Core/ModuleConfig/AppBasePathToken", sAppBasePathToken_);
  sAppResourcePathToken_ = Apollo::getConfig("Core/ModuleConfig/AppResourcePathToken", sAppResourcePathToken_);
  sAppLibraryPathToken_ = Apollo::getConfig("Core/ModuleConfig/AppLibraryPathToken", sAppLibraryPathToken_);
  sAppModulePathToken_ = Apollo::getConfig("Core/ModuleConfig/AppModulePathToken", sAppModulePathToken_);
  sModuleLibraryPathToken_ = Apollo::getConfig("Core/ModuleConfig/ModuleLibraryPathToken", sModuleLibraryPathToken_);
  sModuleResourcePathToken_ = Apollo::getConfig("Core/ModuleConfig/ModuleResourcePathToken", sModuleResourcePathToken_);
  sUserProfilePathToken_ = Apollo::getConfig("Core/ModuleConfig/UserProfilePathToken", sUserProfilePathToken_);

  lPathVars_["[[USER_PROFILE]]"] = Apollo::getUserProfilePath();
}

// --------------------------------

AP_MSG_HANDLER_METHOD(ApLib, File_Load)
{
  int ok = 0;

  xFile f(Apollo::applyPathVars(pMsg->sPathName));
  ok = f.Load();
  if (ok) {
    f.GetData(pMsg->sbData);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ApLib, File_Save)
{
  int ok = 0;

  xFile f(Apollo::applyPathVars(pMsg->sPathName));
  f.SetData(pMsg->sbData.Data(), pMsg->sbData.Length());
  ok = f.Save(pMsg->nFlags & Msg_File_Flag_CreatePath ? xFile::SaveFlag_CreatePath : 0);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ApLib, File_Append)
{
  int ok = 0;

  xFile f(Apollo::applyPathVars(pMsg->sPathName));
  ok = f.AppendToFile(pMsg->sbData.Data(), pMsg->sbData.Length(), pMsg->nFlags & Msg_File_Flag_CreatePath ? xFile::SaveFlag_CreatePath : 0);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ApLib, File_Delete)
{
  int ok = 0;

  xFile f(Apollo::applyPathVars(pMsg->sPathName));
  ok = f.Delete();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ApLib, File_Exists)
{
  int ok = 1;

  xFile f(Apollo::applyPathVars(pMsg->sPathName));
  pMsg->bExists = f.Exists();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ApLib, File_Rename)
{
  int ok = 1;

  xFile f(Apollo::applyPathVars(pMsg->sPathName));
  ok = f.Rename(Apollo::applyPathVars(pMsg->sNewPathName));

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

// --------------------------------

#if defined(WIN32)
static String _FormatLastError()
{
  LPVOID lpMsgBuf = 0;
  ::FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM | 
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0,
      NULL 
  );
  String sReason = (PWSTR) lpMsgBuf;
  LocalFree(lpMsgBuf);
  return sReason;
}
#endif

#if defined(WIN32)
#include "ShellAPI.h"
#endif

AP_MSG_HANDLER_METHOD(ApLib, OS_StartProcess)
{
  int ok = 0;

#if defined(WIN32)
  //::ShellExecute(NULL, _T("open"), pMsg->sExePath, pMsg->sArgs,  String::filenameBasePath(pMsg->sExePath), SW_SHOW);
  //int bDone = 0;
  //while (!bDone) {
  //  Msg_OS_GetProcessId msg;
  //  msg.sName = String::filenameFile(pMsg->sExePath);
  //  if (msg.Request()) {
  //    pMsg->nPid = msg.nPid;
  //    bDone = 1;
  //    ok = 1;
  //  } else {
  //    bDone = 0;
  //    ::Sleep(200);
  //  }
  //}

  String sCmdLine;
  sCmdLine.appendf("\"%s\" %s", StringType(pMsg->sExePath), StringType(pMsg->sArgs));

  STARTUPINFO si; 
  memset(&si, '\0', sizeof STARTUPINFO);
  si.cb = sizeof(STARTUPINFO);
  si.wShowWindow = SW_NORMAL;

  PROCESS_INFORMATION pi;
  ok = ::CreateProcess(
                      NULL,     // module name NULL, use command line.
                      sCmdLine, // Command line. 
                      NULL,     // Process handle not inheritable. 
                      NULL,     // Thread handle not inheritable. 
                      FALSE,    // Set handle inheritance to FALSE. 
                      0,        // creation flags. 
                      NULL,     // Use parent's environment block. 
                      pMsg->sCwdPath,     // Use parent's starting directory. 
                      &si,      // Pointer to STARTUPINFO structure.
                      &pi       // Pointer to PROCESS_INFORMATION structure.
                    );
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "ApLib::On_OS_StartProcess", "CreateProcess failed for command line: %s %s", StringType(sCmdLine), StringType(_FormatLastError())));
  } else {
    apLog_Info((LOG_CHANNEL, "ApLib::On_OS_StartProcess", "%s", StringType(sCmdLine)));
    pMsg->nPid = pi.dwProcessId;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
#else
  // not implemented
#endif
}

AP_MSG_HANDLER_METHOD(ApLib, OS_KillProcess)
{
  int ok = 0;

#if defined(WIN32)
  HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pMsg->nPid);
  if (hProcess != NULL) {
    ok = ::TerminateProcess(hProcess, 0);
    if (!ok) {
      pMsg->sComment = _FormatLastError();
    }
    ::CloseHandle(hProcess);
    hProcess = NULL;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
#else
  // not implemented
#endif
}

#if defined(WIN32)
#include "Psapi.h"
#endif

AP_MSG_HANDLER_METHOD(ApLib, OS_GetProcessId)
{
  int ok = 0;

#if defined(WIN32)
  int bFinished = 0;
  int nSize = 1024;
  while (!bFinished) {
    Flexbuf<DWORD> aProcessIds(nSize);
    DWORD nBytesIn = nSize * sizeof(DWORD);
    DWORD nBytesReturned = 0;
    BOOL bOk = EnumProcesses(aProcessIds.get(), nBytesIn, &nBytesReturned);
    if (!bOk) {
      pMsg->sComment = _FormatLastError();
      bFinished = 1;
    } else {
      if (nBytesIn == nBytesReturned) {
        nSize = nSize * 2;
        if (nSize > 1000000) { bFinished = 1; }
      } else {
        bFinished = 1;
        int nSizeReturned = nBytesReturned / sizeof(DWORD);
        for (int i = 0; i < nSize; i++) {
          DWORD nPid = aProcessIds.get()[i];
          HANDLE hProcess = hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, nPid);
          if (hProcess != NULL) {
            int nSize = Flexbuf_DefaultLength < 2048 ? 2048 : Flexbuf_DefaultLength;
            Flexbuf<WCHAR> wzBaseName(nSize);
            DWORD dwLength = ::GetProcessImageFileName(hProcess, wzBaseName.get(), nSize);
            if (dwLength > 0) {
              String sBaseName = wzBaseName.get();
              String sFileName = String::filenameFile(sBaseName);
              if (sFileName == pMsg->sName) {
                pMsg->nPid = nPid;
                ok = 1;
                break; // for
              }
            }
            ::CloseHandle(hProcess);
          }
        }
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
#else
  // not implemented
#endif
}

AP_MSG_HANDLER_METHOD(ApLib, OS_GetProcessInfo)
{
  int ok = 0;

#if defined(WIN32)
  HANDLE hProcess = hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pMsg->nPid);
  if (hProcess != NULL) {
    int nSize = Flexbuf_DefaultLength < 2048 ? 2048 : Flexbuf_DefaultLength;
    Flexbuf<WCHAR> wzBaseName(nSize);
    DWORD dwLength = ::GetProcessImageFileName(hProcess, wzBaseName.get(), nSize);
    if (dwLength > 0) {
      String sProgramPath = wzBaseName;
      pMsg->kvInfo.add(Msg_OS_GetProcessInfo_ProgramPath, sProgramPath);
      ok = 1;
    } else {
      pMsg->sComment = _FormatLastError();
    }
    ::CloseHandle(hProcess);
    hProcess = NULL;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
#else
  // not implemented
#endif
}

// -------------------------------------------------------------------





















// -------------------------------------------------------------------

int ApLib::Init(int nArgc, char** pszArgv)
{
  int ok = 1;

  int nPid = 0;
  #if defined(WIN32)
  nPid = ::GetCurrentProcessId();
  #else
  nPid = ::getpid();
  #endif
  sRandomSeed_.appendf("%d%d%d", (unsigned) time(0), rand(), nPid);

  nArgc_ = nArgc;
  pszArgv_ = pszArgv;

  for (int n = 0; n < nArgc; n++) {
    lArgv_.AddLast(pszArgv[n]);
  }

#if defined(WIN32)
  ::CoInitialize(NULL);
#endif

  String sApCoreDll = "ApCore"; 
  sApCoreDll = Apollo::normalizeLibraryName(sApCoreDll);
  sApCoreDll = Apollo::getAppLibraryPath().append(sApCoreDll);
  
  hCoreDll_ = Apollo::libraryLoad(sApCoreDll);
  if (hCoreDll_ == 0) {
    ok = 0;
    apLog_Error((LOG_CHANNEL, "ApCore::Init", "Apollo::libraryLoad %d", StringType(sApCoreDll)));
  } else {
    { String sSymbol = "Init"; fpInit_ = (ApCoreInitF) Apollo::libraryGetProcAddress(hCoreDll_, sSymbol); }
    { String sSymbol = "Exit"; fpExit_ = (ApCoreExitF) Apollo::libraryGetProcAddress(hCoreDll_, sSymbol); }
    { String sSymbol = "Call"; fpCall_ = (ApCoreCallF) Apollo::libraryGetProcAddress(hCoreDll_, sSymbol); }
  }

  if (fpCall_ == 0 || fpInit_ == 0 || fpExit_ == 0) {
    ok = 0;
    apLog_Error((LOG_CHANNEL, "ApCore::Init", "Missing interface Call=0x&08x Init=0x&08x Exit=0x&08x", (long) fpCall_, (long) fpInit_, (long) fpExit_));
  }

  if (ok) {
    if (fpInit_ != 0) {
      ok = fpInit_();
      if (! ok) {
        apLog_Error((LOG_CHANNEL, "ApCore::Init", "hCoreDll_->fpInit_ failed"));
      }
    }
  }

  nThreadId_ = Apollo::GetCurrentThreadId();

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, System_RunLevel, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, System_GetRunLevel, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, System_GetTime, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, System_GetHandle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, System_Echo, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, MainLoop_EventLoopBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, MainLoop_EventLoopEnd, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, System_SecTimer, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, System_GetCmdLineArgs, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, Config_Loaded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, File_Load, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, File_Save, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, File_Append, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, File_Delete, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, File_Exists, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, File_Rename, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, OS_StartProcess, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, OS_KillProcess, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, OS_GetProcessId, this, ApCallbackPosLate);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ApLib, OS_GetProcessInfo, this, ApCallbackPosLate);

  return ok;
}

int ApLib::Exit()
{
  AP_MSG_REGISTRY_FINISH;

  if (fpExit_ != 0) {
    (void) fpExit_();
  }

   Apollo::libraryFree(hCoreDll_);

  return 1;
}
























String ApLib::getModulesDirectoryName() { return sModulesDirectoryName_; }
String ApLib::AppBasePathToken() { return sAppBasePathToken_; }
String ApLib::AppResourcePathToken() { return sAppResourcePathToken_; }
String ApLib::AppLibraryPathToken() { return sAppLibraryPathToken_; }
String ApLib::AppModulePathToken() { return sAppModulePathToken_; }
String ApLib::ModuleLibraryPathToken() { return sModuleLibraryPathToken_; }
String ApLib::ModuleResourcePathToken() { return sModuleResourcePathToken_; }
String ApLib::UserProfilePathToken() { return sUserProfilePathToken_; }

// -------------------------------------------------------------------

String ApLib::InsertConfigAppTokens(const char* szConfigValue)
{
  String sValue = szConfigValue; 
  
  String sAppBasePath = Apollo::getAppBasePath();
  String sAppResourcePath = Apollo::getAppResourcePath();
  String sAppLibraryPath = Apollo::getAppLibraryPath();
  String sAppModulePath = Apollo::getAppModulePath();
  
  {
    List lReplace; lReplace.AddLast(String::filenamePathSeparator(), "/");
    sValue.replace(lReplace);
    
    sAppBasePath.replace(lReplace);
    sAppResourcePath.replace(lReplace);
    sAppLibraryPath.replace(lReplace);
    sAppModulePath.replace(lReplace);
  }
  {
    List lReplace;
    lReplace.AddLast(sAppBasePath, AppBasePathToken() + "/");
    lReplace.AddLast(sAppResourcePath, AppResourcePathToken() + "/");
    lReplace.AddLast(sAppLibraryPath, AppLibraryPathToken() + "/");
    lReplace.AddLast(sAppModulePath, AppModulePathToken() + "/");
    
    if (sValue.replace(lReplace)) { return sValue; } // Config value is a path, return replaced and normalized
    else {} // Not a (known to be) path, leave untouched
  }
  return szConfigValue;
}

String ApLib::InsertConfigModuleTokens(const char* szModuleName, const char* szConfigValue)
{
  String sValue = szConfigValue; 
  
  String sModuleLibraryPath = Apollo::getModuleLibraryPath(szModuleName);
  String sModuleResourcePath = Apollo::getModuleResourcePath(szModuleName);
  
  {
    List lReplace; lReplace.AddLast(String::filenamePathSeparator(), "/");
    sValue.replace(lReplace);
    
    sModuleLibraryPath.replace(lReplace);
    sModuleResourcePath.replace(lReplace);
  }
  {
    List lReplace;    
    lReplace.AddLast(sModuleLibraryPath, ModuleLibraryPathToken() + "/");
    lReplace.AddLast(sModuleResourcePath, ModuleResourcePathToken() + "/");
    
    if (sValue.replace(lReplace)) { return sValue; } // Config value is a path, return replaced and normalized
    else {} // Not a (known to be) path, leave untouched
  }
  return szConfigValue;
}

String ApLib::ExpandConfigAppTokens(const char* szConfigValue)
{
  String sValue = szConfigValue;
  
  String sAppBasePath = Apollo::getAppBasePath();
  String sAppResourcePath = Apollo::getAppResourcePath();
  String sAppLibraryPath = Apollo::getAppLibraryPath();
  String sAppModulePath = Apollo::getAppModulePath();
  
  sValue.replace(String::filenamePathSeparator(), "/");
  {
    List lReplace;
    lReplace.AddLast(AppBasePathToken() + "/", sAppBasePath);
    lReplace.AddLast(AppResourcePathToken() + "/", sAppResourcePath);
    lReplace.AddLast(AppLibraryPathToken() + "/", sAppLibraryPath);
    lReplace.AddLast(AppModulePathToken() + "/", sAppModulePath);
    
    if (sValue.replace(lReplace)) {
      // Config value is a path, bring back to native form and return replaced
      sValue.replace("/", String::filenamePathSeparator());
      return sValue;
    } else {} // Not a (known to be) path, leave untouched
  }
  return szConfigValue;
}

String ApLib::ExpandConfigModuleTokens(const char* szModuleName, const char* szConfigValue)
{
  String sValue = szConfigValue;
  
  String sModuleLibraryPath = Apollo::getModuleLibraryPath(szModuleName);
  String sModuleResourcePath = Apollo::getModuleResourcePath(szModuleName); 
  
  sValue.replace(String::filenamePathSeparator(), "/");
  {
    List lReplace;
    lReplace.AddLast(ModuleLibraryPathToken() + "/", sModuleLibraryPath);
    lReplace.AddLast(ModuleResourcePathToken() + "/", sModuleResourcePath);
    
    if (sValue.replace(lReplace)) {
      // Config value is a path, bring back to native form and return replaced
      sValue.replace("/", String::filenamePathSeparator());
      return sValue;
    } else {} // Not a (known to be) path, leave untouched
  }
  return szConfigValue;
}

