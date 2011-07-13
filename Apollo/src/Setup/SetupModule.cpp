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

#include "ShellAPI.h"

String SetupModule::GetInstallFirefoxExtensionCommandline()
{
  String sCmdline;

  // "C:\Program Files\Mozilla Firefox\firefox.exe" -requestPending -osint -url "%1"
  String sFirefoxCmd;
  if (!sFirefoxCmd) { sFirefoxCmd = SRegistry::GetString(HKEY_CLASSES_ROOT, "FirefoxHTML\\shell\\open\\command", "", ""); }
  if (!sFirefoxCmd) { sFirefoxCmd = SRegistry::GetString(HKEY_CLASSES_ROOT, "FirefoxURL\\shell\\open\\command", "", ""); }
  if (!sFirefoxCmd) { sFirefoxCmd = SRegistry::GetString(HKEY_CURRENT_USER, "Software\\Classes\\FirefoxHTML\\shell\\open\\command", "", ""); }
  if (!sFirefoxCmd) { sFirefoxCmd = SRegistry::GetString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\FirefoxHTML\\shell\\open\\command", "", ""); }

  String sUrl = Apollo::getModuleConfig("Navigation", "FirefoxExtensionInstallUrl", "");
  //if (!sUrl.startsWith("http:") && !sUrl.startsWith("https:")) {
  //  if (sUrl.subString(1, 1) == ":") {
  //    sUrl = "file://" + sUrl;
  //  }
  //}

  sCmdline = sFirefoxCmd;
  sCmdline.replace("%1", sUrl);

  return sCmdline;
}

void SetupModule::GetInstallFirefoxExtensionParams(const String& sCmdline, String& sPath, String& sArgs)
{
  int nChars = sCmdline.chars();
  int bInQuotes = 0;
  String sBuf;
  List lParts;
  for (int i = 0; i < nChars; ++i) {
    String sChar = sCmdline.subString(i, 1);
    switch (String::UTF8_Char(sChar)) {
      case '"': 
        bInQuotes = !bInQuotes;
        break;
      case ' ': 
        if (bInQuotes) {
          sBuf += sChar;
        } else {
          if (sBuf) {
            lParts.AddLast(sBuf);
            sBuf = "";
          }
        }
        break;
      default: 
        sBuf += sChar;
        break;
    }
  }
  if (sBuf) {
    lParts.AddLast(sBuf);
  }

  int nCnt = 0;
  for (Elem* e = 0; (e = lParts.Next(e)) != 0; nCnt++) {
    if (nCnt == 0) {
      sPath = e->getName();
    } else {
      if (sArgs) { sArgs += " "; }
      int bAddQuotes = 0;
      if (e->getName().contains(" ")) { bAddQuotes = 1; }
      if (bAddQuotes) { sArgs += "\""; }
      sArgs += e->getName();
      if (bAddQuotes) { sArgs += "\""; }
    }
  }
}

void SetupModule::InstallFirefoxExtensionByFirefox()
{
  String sCmdline = GetInstallFirefoxExtensionCommandline();
  if (!sCmdline) { throw ApException(LOG_CONTEXT, "No commandline"); }

  String sPath;
  String sArgs;
  GetInstallFirefoxExtensionParams(sCmdline, sPath, sArgs);

  ::ShellExecute(NULL, _T("open"), sPath, sArgs, String::filenameBasePath(sPath), SW_SHOW);
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

    } else if (sMethod == "InstallInternetExplorerExtension") {

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
  static String GetInstallFirefoxExtensionParams();
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
    SetupModule m;
    String sSub1 = "firefox.exe";
    String sSub2 = "avatarnavigator.xpi";
    String sResult = String::toLower(m.GetInstallFirefoxExtensionCommandline());
    if (!sResult.contains(sSub1)) {
      s.appendf("got=%s expected substring=%s", _sz(sResult), _sz(sSub1));
    }
    if (!sResult.contains(sSub2)) {
      s.appendf("got=%s expected substring=%s", _sz(sResult), _sz(sSub2));
    }
  }

  return s;
}

String SetupModuleTester::GetInstallFirefoxExtensionParams()
{
  String s;

  if (!s) {
    String sCmdline = "\"abs def\" -ghi \"jkl mno\" pqr stu";
    String sPath;
    String sArgs;
    SetupModule m;
    m.GetInstallFirefoxExtensionParams(sCmdline, sPath, sArgs);
    if (sPath != "abs def") { s += String::from(__LINE__); }
    if (sArgs != "-ghi \"jkl mno\" pqr stu") { s += String::from(__LINE__); }
  }

  return s;
}

void SetupModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(SetupModuleTester::GetInstallFirefoxExtensionCommandline);
  AP_UNITTEST_REGISTER(SetupModuleTester::GetInstallFirefoxExtensionParams);
}

void SetupModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(SetupModuleTester::GetInstallFirefoxExtensionCommandline);
  AP_UNITTEST_EXECUTE(SetupModuleTester::GetInstallFirefoxExtensionParams);
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
