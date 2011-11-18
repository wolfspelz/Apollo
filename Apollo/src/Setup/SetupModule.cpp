// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "SRegistry.h"

void SetupModule::SetupDone()
{
  if (bInterseptedRunLevelNormal_) {
    bInterseptedRunLevelNormal_ = 0;
    SendRunLevelNormal();
  }
}

void SetupModule::SendRunLevelNormal()
{
  bInSendRunLevelNormal_ = 1;

  Msg_System_RunLevel msg; 
  msg.sLevel = Msg_System_RunLevel_Normal; 
  msg.Send();
  
  bInSendRunLevelNormal_ = 0;
}

static String _CanonicalizePath(const String& sPath)
{
  String sPrefixedPath;
  String sCanonicalizedPath;
  String sPrefix = "file://";

  // Remove ../ ./ if necessary
  // canonicalizeUrl only works with URLs, but we need a path here
  // Add prefix, canonicalize, remove prefix

  int bWasPrefixed = 0;
  if (sPath.startsWith(sPrefix)) {
    sPrefixedPath = sPath;
    bWasPrefixed = 1;
  } else {
    sPrefixedPath = sPrefix + sPath;
  }

  sCanonicalizedPath = Apollo::canonicalizeUrl(sPrefixedPath);

  if (!bWasPrefixed) {
    if (sCanonicalizedPath.startsWith(sPrefix)) {
      sCanonicalizedPath = sCanonicalizedPath.subString(sPrefix.chars());
    }
  }

  return sCanonicalizedPath;
}

String SetupModule::GetInstallFirefoxExtensionCommandline()
{
  String sCmdline;

  // "C:\Program Files\Mozilla Firefox\firefox.exe" -requestPending -osint -url "%1"
  String sFirefoxCmd;
  if (!sFirefoxCmd) { sFirefoxCmd = SRegistry::GetString(HKEY_CLASSES_ROOT, "FirefoxHTML\\shell\\open\\command", "", ""); }
  if (!sFirefoxCmd) { sFirefoxCmd = SRegistry::GetString(HKEY_CLASSES_ROOT, "FirefoxURL\\shell\\open\\command", "", ""); }
  if (!sFirefoxCmd) { sFirefoxCmd = SRegistry::GetString(HKEY_CURRENT_USER, "Software\\Classes\\FirefoxHTML\\shell\\open\\command", "", ""); }
  if (!sFirefoxCmd) { sFirefoxCmd = SRegistry::GetString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\FirefoxHTML\\shell\\open\\command", "", ""); }

  String sUrl = Apollo::getModuleConfig("Navigation", "FirefoxExtensionInstallUrl", "file://" + Apollo::getCwd() + "modules/navigation/AvatarNavigator.xpi");
  //if (!sUrl.startsWith("http:") && !sUrl.startsWith("https:")) {
  //  if (sUrl.subString(1, 1) == ":") {
  //    sUrl = "file://" + sUrl;
  //  }
  //}

  sCmdline = sFirefoxCmd;
  sCmdline.replace("%1", sUrl);

  return sCmdline;
}

String SetupModule::GetInstallChromeExtensionCommandline()
{
  String sCmdline;

  // "C:\Program Files\Mozilla Firefox\firefox.exe" -requestPending -osint -url "%1"
  String sChromeCmd;
  if (!sChromeCmd) { sChromeCmd = SRegistry::GetString(HKEY_CLASSES_ROOT, "ChromeHTML\\shell\\open\\command", "", ""); }
  if (!sChromeCmd) { sChromeCmd = SRegistry::GetString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\ChromeHTML\\shell\\open\\command", "", ""); }

  String sUrl = Apollo::getModuleConfig("Navigation", "ChromeExtensionInstallUrl", "file://" + Apollo::getCwd() + "modules/navigation/AvatarNavigator.crx");

  sCmdline = sChromeCmd;
  sCmdline.replace("%1", sUrl);

  return sCmdline;
}

//#include "ShellAPI.h"

void SetupModule::InstallFirefoxExtensionByFirefox()
{
  String sCmdline = GetInstallFirefoxExtensionCommandline();
  if (!sCmdline) { throw ApException(LOG_CONTEXT, "No command line"); }

  Apollo::ValueList vlParts;
  Apollo::splitCommandlineArguments(sCmdline, vlParts);

  String sExe = vlParts.atIndex(0, "");
  vlParts.deleteElem(vlParts.elemAtIndex(0));
  if (!sExe) { throw ApException(LOG_CONTEXT, "No exe path"); }

  {
    Msg_OS_StartProcess msg;
    msg.sExePath = sExe;
    msg.vlArgs = vlParts;
    msg.sCwdPath = String::filenameBasePath(sExe);
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed to start %s: %s", _sz(msg.Type()), _sz(msg.sExePath), _sz(msg.sComment)); }
  }
}

void SetupModule::InstallFirefoxExtension()
{
  String sId = Apollo::getModuleConfig("Navigation", "FirefoxExtensionId", "AvatarNavigator@OpenVirtualWorld.org");
  String sPath = Apollo::getModuleConfig("Navigation", "FirefoxExtensionPath", Apollo::getCwd() + "modules/navigation/AvatarNavigator.xpi");
  
  sPath = _CanonicalizePath( sPath);

  SRegistry::SetString(HKEY_CURRENT_USER, "Software\\Mozilla\\Firefox\\Extensions", sId, sPath);
}

void SetupModule::UninstallFirefoxExtension()
{
  String sId = Apollo::getModuleConfig("Navigation", "FirefoxExtensionId", "AvatarNavigator@OpenVirtualWorld.org");
  
  SRegistry::Delete(HKEY_CURRENT_USER, "Software\\Mozilla\\Firefox\\Extensions", sId);
}

void SetupModule::InstallChromeExtensionByChrome()
{
  String sCmdline = GetInstallChromeExtensionCommandline();
  if (!sCmdline) { throw ApException(LOG_CONTEXT, "No command line"); }

  Apollo::ValueList vlParts;
  Apollo::splitCommandlineArguments(sCmdline, vlParts);

  String sExe = vlParts.atIndex(0, "");
  vlParts.deleteElem(vlParts.elemAtIndex(0));
  if (!sExe) { throw ApException(LOG_CONTEXT, "No exe path"); }

  {
    Msg_OS_StartProcess msg;
    msg.sExePath = sExe;
    msg.vlArgs = vlParts;
    msg.sCwdPath = String::filenameBasePath(sExe);
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed to start %s: %s", _sz(msg.Type()), _sz(msg.sExePath), _sz(msg.sComment)); }
  }
}

void SetupModule::InstallChromeExtension()
{
  String sId = Apollo::getModuleConfig("Navigation", "ChromeExtensionId", "cdegcijmfiaehalcfbpdnfbchlafenke");
  String sPath = Apollo::getModuleConfig("Navigation", "ChromeExtensionPath", Apollo::getCwd() + "modules/navigation/AvatarNavigator.crx");
  String sVersion = Apollo::getModuleConfig("Navigation", "ChromeExtensionVersion", "1.0");
  
  sPath = _CanonicalizePath( sPath);

  SRegistry::SetString(HKEY_LOCAL_MACHINE, "Software\\Google\\Chrome\\Extensions\\" + sId, "path", sPath);
  SRegistry::SetString(HKEY_LOCAL_MACHINE, "Software\\Google\\Chrome\\Extensions\\" + sId, "version", sVersion);
}

void SetupModule::UninstallChromeExtension()
{
  String sId = Apollo::getModuleConfig("Navigation", "ChromeExtensionId", "cdegcijmfiaehalcfbpdnfbchlafenke");
  
  SRegistry::Delete(HKEY_LOCAL_MACHINE, "Software\\Google\\Chrome\\Extensions\\" + sId);
}

void SetupModule::InstallInternetExplorerExtension()
{
  String sPath = Apollo::getModuleConfig("Navigation", "InternetExplorerExtensionInstallFile", Apollo::getCwd() + "modules/navigation/AvatarNavigator.msi");
  if (!sPath) { throw ApException(LOG_CONTEXT, "No installer path"); }

  sPath = _CanonicalizePath( sPath);

  {
    Msg_OS_StartProcess msg;
    msg.sExePath = "msiexec";
    msg.vlArgs.add("/package");
    msg.vlArgs.add(sPath);
    msg.vlArgs.add("/quiet");
    msg.sCwdPath = String::filenameBasePath(sPath);
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed to start %s: %s", _sz(msg.Type()), _sz(msg.sExePath), _sz(msg.sComment)); }
  }
}

void SetupModule::UninstallInternetExplorerExtension()
{
  String sPath = Apollo::getModuleConfig("Navigation", "InternetExplorerExtensionInstallFile", Apollo::getCwd() + "modules/navigation/AvatarNavigator.msi");
  if (!sPath) { throw ApException(LOG_CONTEXT, "No installer path"); }

  sPath = _CanonicalizePath( sPath);

  {
    Msg_OS_StartProcess msg;
    msg.sExePath = "msiexec";
    msg.vlArgs.add("/uninstall");
    msg.vlArgs.add(sPath);
    msg.vlArgs.add("/quiet");
    msg.sCwdPath = String::filenameBasePath(sPath);
    if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed to start %s: %s", _sz(msg.Type()), _sz(msg.sExePath), _sz(msg.sComment)); }
  }
}

void SetupModule::InstallBrowserExtensions()
{
}

void SetupModule::UninstallBrowserExtensions()
{
}

#include "ShellAPI.h"

void SetupModule::InstallInternetExplorerExtensionInteractive()
{
   String sFile = Apollo::getModuleConfig("Navigation", "InternetExplorerExtensionInstallFile", Apollo::getCwd() + "modules/navigation/AvatarNavigator.msi");
   if (!sFile) { throw ApException(LOG_CONTEXT, "No installer path"); }
   HINSTANCE hInst = ::ShellExecute(NULL, String("open"), sFile, NULL, NULL, SW_SHOW);
   if (hInst <= 0) {
     throw ApException(LOG_CONTEXT, "ShellExecute failed: %s error=%d", _sz(sFile), ::GetLastError());   }
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(SetupModule, Setup_Open)
{
  if (ApIsHandle(hDialog_)) { throw ApException(LOG_CONTEXT, "ignored, because already open"); }

  ApHandle hDialog = Apollo::newHandle(); 

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog; 
  msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  msg.bVisible = 1;
  msg.sCaption = Apollo::translate(MODULE_NAME, "", "Setup");
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Setup.html";
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  hDialog_ = hDialog;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SetupModule, Setup_Close)
{
  if (!ApIsHandle(hDialog_)) { throw ApException(LOG_CONTEXT, "ignored, because not open"); }

  ApHandle hDialog = Apollo::newHandle(); 

  Msg_Dialog_Destroy msg;
  msg.hDialog = hDialog_;
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SetupModule, Dialog_OnOpened)
{
  if (pMsg->hDialog == hDialog_) {
    Msg_Dialog_ContentCall msg;
    msg.hDialog = hDialog_;
    msg.sFunction = "Start";
    msg.Request();
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, Dialog_OnClosed)
{
  if (pMsg->hDialog == hDialog_) {
    hDialog_ = ApNoHandle;

    SetupDone();
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, System_RunLevel)
{
  if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
    if (!bInSendRunLevelNormal_) {
      if (Apollo::getModuleConfig(MODULE_NAME, "FirstStart", 0)) {

        // Stop the "Normal" run level until setup is done
        pMsg->Stop();

        Apollo::setModuleConfig(MODULE_NAME, "FirstStart", 0);
        bInterseptedRunLevelNormal_ = 1;

        Msg_Setup_Open msg; 
        msg.Request();

      }
    }
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, System_BeforeEventLoop)
{
  if (Apollo::getModuleConfig(MODULE_NAME, "AlwaysFirstStart", 0)) {
    Apollo::setModuleConfig(MODULE_NAME, "FirstStart", 1);
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, WebView_ModuleCall)
{
  if (pMsg->hView == hDialog_) {
    String sMethod = pMsg->srpc.getString(Srpc::Key::Method);

    if (0){
    } else if (sMethod == "InstallFirefoxExtensionByFirefox") {
      InstallFirefoxExtensionByFirefox();

    } else if (sMethod == "InstallFirefoxExtension") {
      InstallFirefoxExtension();

    } else if (sMethod == "UninstallFirefoxExtension") {
      UninstallFirefoxExtension();

    } else if (sMethod == "InstallChromeExtensionByChrome") {
      InstallChromeExtensionByChrome();

    } else if (sMethod == "InstallChromeExtension") {
      InstallChromeExtension();

    } else if (sMethod == "UninstallChromeExtension") {
      UninstallChromeExtension();

    } else if (sMethod == "InstallInternetExplorerExtensionInteractive") {
      InstallInternetExplorerExtensionInteractive();

    } else if (sMethod == "InstallInternetExplorerExtension") {
      InstallInternetExplorerExtension();

    } else if (sMethod == "UninstallInternetExplorerExtension") {
      UninstallInternetExplorerExtension();

    } else if (sMethod == "InstallBrowserExtensions") {
      InstallBrowserExtensions();

    } else if (sMethod == "UninstallBrowserExtensions") {
      UninstallBrowserExtensions();

    } else {
      throw ApException(LOG_CONTEXT, "Unknown Method=%s", _sz(sMethod));
    }

    pMsg->apStatus = ApMessage::Ok;
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

class SetupModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String GetInstallFirefoxExtensionCommandline();
  static String GetInstallChromeExtensionCommandline();
  static String Dev();
};

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Setup", 0)) {
    SetupModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Setup", 0)) {
    SetupModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(SetupModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Setup", 0)) {
    SetupModuleTester::End();
  }
} 

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int SetupModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Setup_Open, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Setup_Close, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Dialog_OnOpened, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, Dialog_OnClosed, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, System_RunLevel, this, ApCallbackPosVeryEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, System_BeforeEventLoop, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SetupModule, WebView_ModuleCall, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(SetupModule, this);

  return ok;
}

void SetupModule::Exit()
{
  AP_UNITTEST_UNHOOK(SetupModule, this);
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

#if defined(AP_TEST)

String SetupModuleTester::GetInstallFirefoxExtensionCommandline()
{
  String s;

  if (!s) {
    String sSub1 = "firefox.exe";
    String sSub2 = "avatarnavigator.xpi";
    String sResult = String::toLower(SetupModule::GetInstallFirefoxExtensionCommandline());
    if (!sResult.contains(sSub1)) {
      s.appendf("got=%s expected substring=%s", _sz(sResult), _sz(sSub1));
    }
    if (!sResult.contains(sSub2)) {
      s.appendf("got=%s expected substring=%s", _sz(sResult), _sz(sSub2));
    }
  }

  return s;
}

String SetupModuleTester::GetInstallChromeExtensionCommandline()
{
  String s;

  if (!s) {
    String sSub1 = "chrome.exe";
    String sSub2 = "avatarnavigator.crx";
    String sResult = String::toLower(SetupModule::GetInstallChromeExtensionCommandline());
    if (!sResult.contains(sSub1)) {
      s.appendf("got=%s expected substring=%s", _sz(sResult), _sz(sSub1));
    }
    if (!sResult.contains(sSub2)) {
      s.appendf("got=%s expected substring=%s", _sz(sResult), _sz(sSub2));
    }
  }

  return s;
}

void SetupModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(SetupModuleTester::GetInstallFirefoxExtensionCommandline);
  AP_UNITTEST_REGISTER(SetupModuleTester::GetInstallChromeExtensionCommandline);
}

void SetupModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(SetupModuleTester::GetInstallFirefoxExtensionCommandline);
  AP_UNITTEST_EXECUTE(SetupModuleTester::GetInstallChromeExtensionCommandline);
  SetupModuleTester::Dev();
}

void SetupModuleTester::End()
{
}

//----------------------------

String SetupModuleTester::Dev()
{
  String s;

  Msg_Setup_Open msg;
  if (!msg.Request()) { s = msg.Type() + " failed: " + msg.sComment; }

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
