// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLib.h"
#include "ApLog.h"
#include "MessageDigest.h"
#include "MsgCore.h"
#include "MsgTranslation.h"
#include "MsgTimer.h"
#include "MsgFile.h"
#include "URL.h"

#define LOG_CHANNEL APOLLO_NAME
#define LOG_CONTEXT apLog_Context

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

ApLib gApLib;

// -------------------------------------------------------------------

int Apollo::Init(Apollo::ValueList& vlArgs)
{
  int ok = 0;

  ::srand((unsigned) time(0));

  ok = gApLib.Init(vlArgs);

  return ok;
}

int Apollo::Exit()
{
  int ok = 0;

  ok = gApLib.Exit();

  return ok;
}

// --------------------------------

int Apollo::callMsg(ApMessage* pMsg, int nFlags)
{
  return gApLib.callMsg(pMsg, nFlags);
}

int Apollo::hookMsg(const char* szCallName, const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition)
{
  int ok = 0;

  Msg_Core_Hook msg;
  msg.sCallName = szCallName;
  msg.sModuleName = szModuleName;
  msg.fnHandler = fnHandler;
  msg.nRef = nRef;
  msg.nPosition = nPosition;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to Hook %s %s", _sz(szCallName), _sz(szModuleName)));
  } else {
    ok = msg.apStatus == ApMessage::Ok;
  }

  return ok;
}

int Apollo::unhookMsg(const char* szCallName, const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef)
{
  int ok = 0;

  Msg_Core_Unhook msg;
  msg.sCallName = szCallName;
  msg.sModuleName = szModuleName;
  msg.fnHandler = fnHandler;
  msg.nRef = nRef;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to Unhook %s %s", _sz(szCallName), _sz(szModuleName)));
  } else {
    ok = msg.apStatus == ApMessage::Ok;
  }

  return ok;
}

ApCallbackPos Apollo::modulePos(long nBase, const char* szModuleName)
{
  int nChecksum = gApLib.checksumSigned8(szModuleName);
  ApCallbackPos nPosition = nChecksum * ApCallbackPosSegment + nBase;
  return nPosition;
}

int Apollo::isMainThread()
{
  return gApLib.isMainThread();
}

// --------------------------------

String Apollo::normalizeLibraryName(const String& sFileName)
{
  return gApLib.normalizeLibraryName(sFileName);
}

HANDLE Apollo::libraryLoad(const String& sDllPath)
{
  return gApLib.libraryLoad(sDllPath);
}

void* Apollo::libraryGetProcAddress(HANDLE hDll, const String& sSymbol)
{
  return gApLib.libraryGetProcAddress(hDll, sSymbol);
}

int Apollo::libraryFree(HANDLE hDll)
{
  return gApLib.libraryFree(hDll);
}

// --------------------------------

int Apollo::loadModule(const char* szModuleName, const char* szDllPath)
{
  int ok = 0;

  Msg_Core_LoadModule msg;
  msg.sDllPath = szDllPath;
  msg.sModuleName = szModuleName;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to load module from %s (%s)", _sz(szDllPath), _sz(szModuleName)));
  } else {
    ok = msg.apStatus == ApMessage::Ok;
  }

  return ok;
}

int Apollo::unloadModule(const char* szModuleName)
{
  Msg_Core_UnloadModule msg;
  msg.sModuleName = szModuleName;
  if (msg.Request()) {
    return msg.apStatus == ApMessage::Ok;
  }
  return 0;
}

int Apollo::isLoadedModule(const char* szModuleName)
{
  int bLoaded = 0;

  Msg_Core_IsLoadedModule msg;
  msg.sModuleName = szModuleName;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to check if module loaded"));
  } else {
    bLoaded = msg.bLoaded;
  }

  return bLoaded;
}

int Apollo::unloadModules()
{
  Msg_Core_GetLoadedModules msg;
  if (!msg.Request()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to fetch modules"));
  } else {

    // Reverse module order for unload
    List lReverse;
    {
      for (ValueElem* e = 0; (e = msg.vlModules.nextElem(e)) != 0; ) {
        Elem* f = new Elem(e->getString());
        if (f != 0) {
          lReverse.AddFirst(f);
        }
      }
    }

    for (Elem* e = 0; (e = lReverse.Next(e)); ) {
      Msg_Core_UnloadModule msg2;
      msg2.sModuleName = e->getName();
      if (!msg2.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Failed to unload module %s", _sz(e->getString())));
      }
    }
  }

  return 1;
}

int Apollo::loadModulesFromConfig(const char* szKey)
{
  int ok = 0;

  // Strip trailing /, if any
  String sBase = szKey;
  if (sBase.endsWith("/")) {
    String sTmp = sBase;
    sBase.set(sTmp.c_str(), sTmp.chars() -1);
  }

  List lModulePaths;

  {
    Msg_Config_GetKeys msgOrderKeys;
    msgOrderKeys.sPath.appendf("%s/", _sz(sBase));
    if (msgOrderKeys.Request()) {
      for (ValueElem* eOrderKey = 0; (eOrderKey = msgOrderKeys.vlKeys.nextElem(eOrderKey)); ) {
        Msg_Config_GetKeys msgModKeys;
        msgModKeys.sPath.appendf("%s/%s/", _sz(sBase), _sz(eOrderKey->getString()));
        if (msgModKeys.Request()) {
          for (ValueElem* eModKey = 0; (eModKey = msgModKeys.vlKeys.nextElem(eModKey)); ) {
            String sKey;
            sKey.appendf("%s/%s/%s/Path", _sz(sBase), _sz(eOrderKey->getString()), _sz(eModKey->getString()));
            String sPath = Apollo::getConfig(sKey, "");
            if (!sPath.empty()) {
              lModulePaths.AddLast(eModKey->getString(), sPath);
              ok = 1;
            }
          }
        }
      }
    }
  }

  if (ok) {
    for (Elem* e = 0; (e = lModulePaths.Next(e)) != 0; ) {
      int iok = loadModule(e->getName(), e->getString());
      if (!iok) {
        apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "loadModule failed: %s %s", _sz(e->getName()), _sz(e->getString())));
      }
    }
  }

  return 1;
}

// --------------------------------

ApHandle Apollo::newHandle()
{
  return gApLib.newHandle();
}

String Apollo::handle2String(const ApHandle& h)
{
  return gApLib.handle2String(h);
}

ApHandle Apollo::string2Handle(const String& sString)
{
  return gApLib.string2Handle(sString);
}

// --------------------------------

int Apollo::getRandom(int nMax)
{
  int bDone = 0;
  int nRnd = 0;
  while (!bDone) {
    nRnd = nMax * ::rand() / RAND_MAX;
    if (nRnd < nMax) {
      bDone = 1;
    }
  }
  return nRnd;
}

String Apollo::getRandomString(int nLength)
{
  return gApLib.getRandomString(nLength);
}

String Apollo::getUniqueId()
{
  return gApLib.getUniqueId();
}

String Apollo::getShortHash(const String& sData, unsigned int nLength)
{
  return gApLib.shortHash(sData, nLength);
}

// --------------------------------

Apollo::TimeValue Apollo::getNow()
{
  return gApLib.getNow();
}

void Apollo::setNow(TimeValue& tv)
{
  gApLib.setNow(tv);
}

// --------------------------------

int Apollo::formatVarArg(char* str, size_t size, const char* format, va_list ap)
{
  int result = 0;
#if defined(WIN32)
  HRESULT hr = ::StringCbVPrintfA(str, size, format, ap);
  if (FAILED(hr)) {
    result = -1;
  }
#else
  result = ::vsnprintf(str, size, format, ap);
#endif
  return result;
}

// --------------------------------

void Apollo::setConfig(const char* szPath, int nValue)
{
  String sValue;
  sValue.appendf("%d", nValue);
  Apollo::setConfig(szPath, sValue);
}

int Apollo::getConfig(const char* szPath, int nDefault)
{
  String sValue = Apollo::getConfig(szPath, "");
  if (sValue.empty()) {
    return nDefault;
  } else {
    return String::atoi(sValue.c_str());
  }
}

void Apollo::setConfig(const char* szPath, double fValue)
{
  String sValue;
  sValue.appendf("%f", fValue);
  Apollo::setConfig(szPath, sValue);
}

double Apollo::getConfig(const char* szPath, double fDefault)
{
  String sValue = Apollo::getConfig(szPath, "");
  if (sValue.empty()) {
    return fDefault;
  } else {
    return String::atof(sValue.c_str());
  }
}

void Apollo::setConfig(const char* szPath, const char* szValue)
{
  Msg_Config_SetValue msg;
  msg.sPath = szPath;
  msg.sValue = gApLib.InsertConfigAppTokens(szValue);
  (void) msg.Request();
}

String Apollo::getConfig(const char* szPath, const char* szDefault)
{
  Msg_Config_GetValue msg;
  msg.sPath = szPath;
  msg.sValue = gApLib.ExpandConfigAppTokens(szDefault);
  (void) msg.Request();
  return msg.sValue;
}


void Apollo::deleteConfig(const char* szPath)
{
  Msg_Config_DeleteValue msg;
  msg.sPath = szPath;
  (void) msg.Request();
}

int Apollo::getConfigKeys(const char* szPath, ValueList& vlKeys)
{
  int bOk = 0;

  Msg_Config_GetKeys msg;
  msg.sPath = szPath;
  bOk = (msg.Request() && (msg.apStatus == ApMessage::Ok));
  if (bOk) {
    for (ValueElem* e = 0; (e = msg.vlKeys.nextElem(e)) != 0; ) {
      vlKeys.add(e->getString());
    }
  }
  
  return bOk;
}

static String _getModuleConfigPath(const String& sModuleName, const String& sPath)
{
  String sFixedPath;
  if (sPath.startsWith("/")) { 
    sFixedPath = sPath.subString(1); 
  } else {
    sFixedPath = sPath;
  }

  //String sConfigKey = Apollo::getConfig("Core/ModuleConfig/ConfigKeyPrefix", "Module");
  //sConfigKey.appendf("/%s/%s", _sz(sModuleName), _sz(sPath));

  String sConfigKey;
  sConfigKey.appendf("%s/%s", _sz(sModuleName), _sz(sFixedPath));
  return sConfigKey;
}

void Apollo::setModuleConfig(const char* szModuleName, const char* szPath, int nValue)
{
  String sModuleName = szModuleName;
  String sPath = szPath;
  if (!sModuleName.empty() && !sPath.empty()) {
    String sConfigKey = _getModuleConfigPath(sModuleName, sPath);
    Apollo::setConfig(sConfigKey, nValue);
  }
}

int Apollo::getModuleConfig(const char* szModuleName, const char* szPath, int nDefault)
{
  int nValue = nDefault;

  String sModuleName = szModuleName;
  String sPath = szPath;
  if (!sModuleName.empty() && !sPath.empty()) {
    String sConfigKey = _getModuleConfigPath(sModuleName, sPath);
    nValue = Apollo::getConfig(sConfigKey, nDefault);
  }

  return nValue;
}

void Apollo::setModuleConfig(const char* szModuleName, const char* szPath, double fValue)
{
  String sModuleName = szModuleName;
  String sPath = szPath;
  if (!sModuleName.empty() && !sPath.empty()) {
    String sConfigKey = _getModuleConfigPath(sModuleName, sPath);
    Apollo::setConfig(sConfigKey, fValue);
  }
}

double Apollo::getModuleConfig(const char* szModuleName, const char* szPath, double fDefault)
{
  double fValue = fDefault;

  String sModuleName = szModuleName;
  String sPath = szPath;
  if (!sModuleName.empty() && !sPath.empty()) {
    String sConfigKey = _getModuleConfigPath(sModuleName, sPath);
    fValue = Apollo::getConfig(sConfigKey, fDefault);
  }

  return fValue;
}

void Apollo::setModuleConfig(const char* szModuleName, const char* szPath, const char* szValue)
{
  String sModuleName = szModuleName;
  String sPath = szPath;
  if (!sModuleName.empty() && !sPath.empty()) {
    String sValue = gApLib.InsertConfigAppTokens(szValue);
    if (sValue == szValue) { sValue = gApLib.InsertConfigModuleTokens(szModuleName, sValue); }
    
    String sConfigKey = _getModuleConfigPath(sModuleName, sPath);
    Apollo::setConfig(sConfigKey, sValue);
  }
}

String Apollo::getModuleConfig(const char* szModuleName, const char* szPath, const char* szDefault)
{
  String sValue = szDefault;

  String sModuleName = szModuleName;
  String sPath = szPath;
  
  if (!sModuleName.empty() && !sPath.empty()) {
    String sConfigKey = _getModuleConfigPath(sModuleName, sPath);
    sValue = Apollo::getConfig(sConfigKey, szDefault);
  }
  
  String sReplacedValue = gApLib.ExpandConfigAppTokens(sValue);
  if (sReplacedValue == sValue) { sReplacedValue = gApLib.ExpandConfigModuleTokens(szModuleName, sReplacedValue); }
  
  return sReplacedValue;
}

void Apollo::deleteModuleConfig(const char* szModuleName, const char* szPath)
{
  String sModuleName = szModuleName;
  String sPath = szPath;
  
  if (!sModuleName.empty() && !sPath.empty()) {
    String sConfigKey = _getModuleConfigPath(sModuleName, sPath);
    Apollo::deleteConfig(sConfigKey);
  }
}

int Apollo::getModuleConfigKeys(const char* szModuleName, const char* szPath, ValueList& vlKeys)
{
  String sModuleName = szModuleName;
  String sPath = szPath;
  if (!sModuleName.empty() && !sPath.empty()) {
    String sConfigKey = _getModuleConfigPath(sModuleName, sPath);
    return Apollo::getConfigKeys(sConfigKey, vlKeys);
  }
  return 0;
}

// --------------------------------

String Apollo::translate(const char* szModule, const char* szContext, const char* szText)
{
  Msg_Translation_Get msg;
  msg.sModule = szModule;
  msg.sContext = szContext;
  msg.sText = szText;
  (void) msg.Request();
  String sTranslated;
  if (msg.sTranslated.empty()) {
    sTranslated = szText;
  } else {
    sTranslated = msg.sTranslated;
  }
  return sTranslated;
}

String Apollo::translate(const char* szModule, const char* szContext, const char* szText, List& lVars)
{
  AP_UNUSED_ARG(szModule);
  AP_UNUSED_ARG(szContext);
  AP_UNUSED_ARG(szText);
  AP_UNUSED_ARG(lVars);
  
  String sTranslated;
  return sTranslated;
}

// --------------------------------

ApHandle Apollo::startInterval(int nSec, int nMicroSec)
{
  ApHandle hTimer = Apollo::newHandle();

  Msg_Timer_Start msg;
  msg.hTimer = hTimer;
  msg.nSec = nSec;
  msg.nMicroSec = nMicroSec;
  msg.nCount = 0; // 0 means infinite
  if (!msg.Request()) {
    hTimer = ApNoHandle;
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Timer_Start failed"));
  }

  return hTimer;
}

ApHandle Apollo::startTimeout(int nSec, int nMicroSec)
{
  ApHandle hTimer = Apollo::newHandle();

  Msg_Timer_Start msg;
  msg.hTimer = hTimer;
  msg.nSec = nSec;
  msg.nMicroSec = nMicroSec;
  msg.nCount = 1;
  if (!msg.Request()) {
    hTimer = ApNoHandle;
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Timer_Start failed"));
  }

  return hTimer;
}

int Apollo::cancelTimeout(const ApHandle& hTimer)
{
  int ok = 0;

  Msg_Timer_Cancel msg;
  msg.hTimer = hTimer;
  msg.Request();
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Timer_Cancel failed"));
  }

  return ok;
}

int Apollo::cancelInterval(const ApHandle& hTimer) { return cancelTimeout(hTimer); }

// --------------------------------

int Apollo::loadFile(const String& sFile, Buffer& sbData)
{
  Msg_File_Load msg;
  msg.sPathName = sFile;
  int ok = msg.Request();
  if (ok) {
    sbData.SetData(msg.sbData.Data(), msg.sbData.Length());
  }

  return ok;
}

int Apollo::loadFile(const String& sFile, String& sData)
{
  Msg_File_Load msg;
  msg.sPathName = sFile;
  int ok = msg.Request();
  if (ok) {
    msg.sbData.GetString(sData);
  }

  return ok;
}

int Apollo::saveFile(const String& sFile, const Buffer& sbData)
{
  Msg_File_Save msg;
  msg.sPathName = sFile;
  msg.nFlags = Msg_File_Flag_CreatePath;
  msg.sbData.SetData(sbData.Data(), sbData.Length());
  
  return msg.Request();
}

int Apollo::saveFile(const String& sFile, const String& sData)
{
  Msg_File_Save msg;
  msg.sPathName = sFile;
  msg.nFlags = Msg_File_Flag_CreatePath;
  msg.sbData.SetData(sData);
  
  return msg.Request();
}

int Apollo::appendFile(const String& sFile, const Buffer& sbData)
{
  Msg_File_Append msg;
  msg.sPathName = sFile;
  msg.nFlags = Msg_File_Flag_CreatePath;
  msg.sbData.SetData(sbData.Data(), sbData.Length());
  
  return msg.Request();
}

int Apollo::appendFile(const String& sFile, const String& sData)
{
  Msg_File_Append msg;
  msg.sPathName = sFile;
  msg.nFlags = Msg_File_Flag_CreatePath;
  msg.sbData.SetData(sData);
  
  return msg.Request();
}

int Apollo::deleteFile(const String& sFile)
{
  Msg_File_Delete msg;
  msg.sPathName = sFile;

  return msg.Request();
}

int Apollo::fileExists(const String& sFile)
{
  Msg_File_Exists msg;
  msg.sPathName = sFile;
  if (msg.Request()) {
    return msg.bExists;
  }
  return 0;
}

int Apollo::renameFile(const String& sFile, const String& sNewName)
{
  Msg_File_Rename msg;
  msg.sPathName = sFile;
  msg.sNewPathName = sNewName;

  return msg.Request();
}

// --------------------------------

String Apollo::getCwd()
{
  return gApLib.getCwd();
}

String Apollo::getMachineId()
{
  return gApLib.getMachineId();
}

String Apollo::getUserLoginName()
{
  return gApLib.getUserLoginName();
}

String Apollo::getUserProfilePath()
{
  return gApLib.getUserProfilePath();
}

void Apollo::splitCommandlineArguments(const String& sCmdline, Apollo::ValueList& vlArgs)
{
  int nChars = sCmdline.chars();
  int bInQuotes = 0;
  int bEscape = 0;
  String sAccumulator;

  for (int i = 0; i < nChars; ++i) {

    String sChar = sCmdline.subString(i, 1);
    switch (String::UTF8_Char(sChar)) {
      case '\\':
        bEscape = 1;
        break;
      case '"':
        if (bEscape) {
          bEscape = 0;
          sAccumulator += sChar;
        } else {
          bInQuotes = !bInQuotes;
        }
        break;
      case ' ':
        if (bEscape) {
          bEscape = 0;
          sAccumulator += "\\";
        }
        if (bInQuotes) {
          sAccumulator += sChar;
        } else {
          if (sAccumulator) {
            vlArgs.add(sAccumulator);
            sAccumulator = "";
          }
        }
        break;
      default:
        if (bEscape) {
          bEscape = 0;
          sAccumulator += "\\";
        }
        sAccumulator += sChar;
        break;
    }

  }

  if (sAccumulator) {
    vlArgs.add(sAccumulator);
  }
}

String Apollo::joinCommandlineArguments(Apollo::ValueList& vlArgs)
{
  String sResult;

  for (Apollo::ValueElem* e = 0; (e = vlArgs.nextElem(e)) != 0; ) {
    String sPart = e->getString();
    int bAddQuotes = 0;
    int bEscapeQuote = 0;

    if (sPart.contains(" ")) {
      bAddQuotes = 1;
    }

    if (sPart.contains("\"")) {
      sPart.replace("\"", "\\\"");
    }

    if (sResult) { sResult += " "; }
    if (bAddQuotes) { sResult += "\""; }
    sResult += sPart;
    if (bAddQuotes) { sResult += "\""; }
  }

  return sResult;
}

String Apollo::canonicalizeUrl(const String& sUrl)
{
  Apollo::URL url = sUrl;
  String sPath = url.path();
  String sFile = url.file();

  if (sFile == "." || sFile == ".."){
    sPath += sFile;
    sFile = "";
  }

  int bTrailingSlash = sPath.endsWith("/") || sPath.endsWith("\\");
  sPath.trim("/\\");
  
  Apollo::ValueList vlSegments;
  String sSegment;
  while (sPath.nextToken("/\\", sSegment)) {
    vlSegments.add(sSegment);
  }

  Apollo::ValueList vlResult;
  for (Apollo::ValueElem* e = 0; (e = vlSegments.nextElem(e)) != 0; ) {
    if (e->getString() == ".") {
      // Skip
    } else if (e->getString() == "..") {
      // Remove previous segment
      int nMinSegements = 0;
      if (url.protocol() == "file") { nMinSegements = 1; }
      if (vlResult.length() > nMinSegements) {
        vlResult.deleteElem(vlResult.elemAtIndex(vlResult.length() - 1));
      }
    } else {
      vlResult.add(e->getString());
    }
  }

  sPath = "";
  if (url.protocol() == "file") {
    for (Apollo::ValueElem* e = 0; (e = vlResult.nextElem(e)) != 0; ) {
      if (sPath) { sPath += String::filenamePathSeparator(); }
      int bDriveLetter = 0;
      #if defined(WIN32)
      if (sPath.empty()) {
        if (e->getString().chars() == 1) {
          bDriveLetter = 1;
        }
      }
      #endif
      if (bDriveLetter) {
        sPath += String::toUpper(e->getString());
        sPath += ":";
      } else {
        sPath += e->getString();
      }
    }
    if (bTrailingSlash) { sPath += String::filenamePathSeparator(); }
  } else {
    for (Apollo::ValueElem* e = 0; (e = vlResult.nextElem(e)) != 0; ) {
      sPath += "/";
      sPath += e->getString();
    }
    if (bTrailingSlash) { sPath += "/"; }
    if (sPath == "") { sPath = "/"; }
  }

  String sOut;
  if (url.protocol() == "http" || url.protocol() == "https") {
    sOut.appendf("%s://%s%s%s%s%s", _sz(url.protocol()), _sz(url.host()), _sz(url.port()), _sz(sPath), _sz(sFile), _sz(url.args()));
  } else {
    sOut.appendf("%s://%s%s", _sz(url.protocol()), _sz(sPath), _sz(sFile));
  }

  return sOut;
}

// --------------------------------

// --------------------------------

// --------------------------------

// --------------------------------

// --------------------------------

// --------------------------------

// --------------------------------

// --------------------------------































// -------------------------------------------------------------------

String Apollo::getAppBasePath()
{
  String sExecutablePath = gApLib.vlArgs_.atIndex(0, "");

  if (!String::filenameIsAbsolutePath(sExecutablePath)) {
    String sCwd = Apollo::getCwd();
    if (!sCwd.empty()) {
      sExecutablePath = sCwd + String::filenamePathSeparator() + sExecutablePath;
    }
  }

  String sAppBasePath = String::filenameBasePath(sExecutablePath);

  return sAppBasePath;
}

String Apollo::getAppResourcePath()
{
  return Apollo::getAppBasePath();
}

String Apollo::getAppLibraryPath()
{
  return Apollo::getAppBasePath();
}

String Apollo::getAppModulePath()
{
  return Apollo::getAppLibraryPath() + gApLib.getModulesDirectoryName() + String::filenamePathSeparator();
}

String Apollo::getModuleLibraryPath(const String& sModuleBaseName)
{
  return  Apollo::getAppModulePath() + Apollo::getModulePackageName(sModuleBaseName) + String::filenamePathSeparator();  
}

String Apollo::getModuleResourcePath(const String& sModuleBaseName)
{
  return Apollo::getModuleLibraryPath(sModuleBaseName);
}

String Apollo::applyPathVars(const String& sPath)
{
  String sNewPath = sPath;
  sNewPath.replace(gApLib.lPathVars_);
  return sNewPath;
}

String Apollo::getModulePackageName(const String& sModuleName)
{
  Msg_Core_ModulePackageName msg; 
  msg.sModuleName = sModuleName; 
  if (msg.Request()) {
    return msg.sPackageName;
  }  
  return msg.sModuleName;
}

#if defined (AP_TEST)

String Apollo::Test_CompareLists(const char* szText, List& lData, List& lExpected, int nFlags)
{
  String s;

  if (!s) {
    Elem* e2 = 0;
    for (Elem* e1 = 0; e1 = lData.Next(e1); ) {
      if (nFlags & Test_CompareLists_Flag_IgnoreOrder) {
        if (e1->getName().empty()) {
          e2 = lExpected.FindByString(e1->getString());
        } else {
          e2 = lExpected.FindByName(e1->getName());
        }
      } else {
        e2 = lExpected.Next(e2);
      }
      if (e2 == 0) {
        s.appendf("%s: missing expected for input: %s=%s", szText, _sz(e1->getName()), _sz(e1->getString()));
        break;
      } else {
        if (e1->getName() != e2->getName() || e1->getString() != e2->getString()) {
          s.appendf("%s: mismatch got: %s=%s, expected: %s=%s", szText, _sz(e1->getName()), _sz(e1->getString()), _sz(e2->getName()), _sz(e2->getString()));
          break;
        }
      }
    }
  }

  if (!s) {
    Elem* e2 = 0;
    for (Elem* e1 = 0; e1 = lExpected.Next(e1); ) {
      if (nFlags & Test_CompareLists_Flag_IgnoreOrder) {
        if (e1->getName().empty()) {
          e2 = lData.FindByString(e1->getString());
        } else {
          e2 = lData.FindByName(e1->getName());
        }
      } else {
        e2 = lData.Next(e2);
      }
      if (e2 == 0) {
        s.appendf("%s: missing input for expected: %s=%s", szText, _sz(e1->getName()), _sz(e1->getString()));
        break;
      } else {
        if (e1->getName() != e2->getName() || e1->getString() != e2->getString()) {
          s.appendf("%s: mismatch expected: %s=%s, got: %s=%s", szText, _sz(e1->getName()), _sz(e1->getString()), _sz(e2->getName()), _sz(e2->getString()));
          break;
        }
      }
    }
  }

  return s;
}

String Apollo::Test_CompareLists(const char* szText, Apollo::KeyValueList& kvData, Apollo::KeyValueList& kvExpected, int nFlags)
{
  List lData; 
  List lExpected;

  kvData.toList(lData);
  kvExpected.toList(lExpected);

  return Test_CompareLists(szText, lData, lExpected, nFlags);
}

String Apollo::Test_CompareLists(const char* szText, Apollo::ValueList& vlData, Apollo::ValueList& vlExpected, int nFlags)
{
  List lData; 
  List lExpected;

  vlData.toList(lData);
  vlExpected.toList(lExpected);

  return Test_CompareLists(szText, lData, lExpected, nFlags);
}

#endif
