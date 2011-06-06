// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "BrowserInfoModule.h"

#if defined(WIN32)
#include "Win32Window.h"
#include "Win32FirefoxBrowser.h"
#endif

//----------------------------------------------------------

LocalCallGuard::LocalCallGuard()
{
  BrowserInfoModuleInstance::Get()->InLocalCall(1);
}

LocalCallGuard::~LocalCallGuard()
{
  BrowserInfoModuleInstance::Get()->InLocalCall(0);
}

//----------------------------------------------------------

Browser* BrowserInfoModule::FindBrowserByContext(const ApHandle& hContext)
{
  Browser* pResult = 0;

  for (BrowserListNode* pNode = 0; (pNode = browsers_.Next(pNode)) != 0; ) {
    Browser* pBrowser = pNode->Value();
    if (pBrowser->HasContext(hContext)) {
      pResult = pBrowser;
      break;
    }
  }

  return pResult;
}

void BrowserInfoModule::StartTimer()
{
  nCntTimerUser_++;

  if (nCntTimerUser_ == 1) {
    if (!ApIsHandle(hTimer_)) {
      int nDelayMSec = Apollo::getModuleConfig(MODULE_NAME, "TrackDelay", 314);
      hTimer_ = Apollo::startInterval(0, nDelayMSec * 1000);
    }
  }
}

void BrowserInfoModule::StopTimer()
{
  nCntTimerUser_--;

  if (nCntTimerUser_ == 0) {
    if (ApIsHandle(hTimer_)) {
      Apollo::cancelInterval(hTimer_);
      hTimer_ = ApNoHandle;
    }
  }
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(BrowserInfoModule, BrowserInfo_BeginTrackCoordinates)
{
  ApHandle hContext = pMsg->hContext;

  String sTitle = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Title].getString();
  String sLeft = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Left].getString();
  String sTop = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Top].getString();
  String sWidth = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Width].getString();
  String sHeight = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Height].getString();

  int nLeft = IgnoreCoordinate; if (!sLeft.empty()) { nLeft = String::atoi(sLeft); }
  int nTop = IgnoreCoordinate; if (!sTop.empty()) { nTop = String::atoi(sTop); }
  int nWidth = IgnoreCoordinate; if (!sWidth.empty()) { nWidth = String::atoi(sWidth); }
  int nHeight = IgnoreCoordinate; if (!sHeight.empty()) { nHeight = String::atoi(sHeight); }

  Apollo::WindowHandle win;
  if (0) {
  } else if (pMsg->sType == Msg_BrowserInfo_BeginTrackCoordinates_Type_Firefox) {
    win = FirefoxFinder::GetFirefoxToplevelWindow(sTitle, nLeft, nTop, nWidth, nHeight);
  }

  if (!win.isValid()) {
    apLog_Error((LOG_CHANNEL, "BrowserInfoModule::BrowserInfo_BeginTrackCoordinates", "No valid browser window, type=%s", StringType(pMsg->sType)));
  } else {

    Browser* pBrowser = 0;
    browsers_.Get(win, pBrowser);
    if (pBrowser == 0) {

      if (0) {
#if defined(WIN32)
      } else if (pMsg->sType == Msg_BrowserInfo_BeginTrackCoordinates_Type_Firefox) {
        pBrowser = new Win32FirefoxBrowser(win);
#endif // defined(WIN32)
      }

      if (pBrowser == 0) {
        apLog_Error((LOG_CHANNEL, "BrowserInfoModule::BrowserInfo_BeginTrackCoordinates", "No browser for platformand type=%s", StringType(pMsg->sType)));
      } else {
        browsers_.Set(win, pBrowser);

        StartTimer();
      }
    }
    if (pBrowser != 0) {
      pBrowser->AddContext(hContext);
    }

  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, BrowserInfo_EndTrackCoordinates)
{
  Browser* pBrowser = FindBrowserByContext(pMsg->hContext);
  if (pBrowser == 0) {
    apLog_Error((LOG_CHANNEL, "BrowserInfoModule::BrowserInfo_EndTrackCoordinates", "No browser for ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
  } else {
    pBrowser->RemoveContext(pMsg->hContext);
    
    if (!pBrowser->HasContexts()) {
      StopTimer();

      browsers_.Unset(pBrowser->GetWindow());
      delete pBrowser;
      pBrowser = 0;
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, VpView_ContextVisibility)
{
  Browser* pBrowser = FindBrowserByContext(pMsg->hContext);
  if (pBrowser) {
    Context* pContext = pBrowser->GetContext(pMsg->hContext);
    if (pContext) {
      if (bInLocalCall_) {
        //
      } else {
        if (!pBrowser->IsVisible()) {
          pMsg->bVisible = 0;
        }
      }
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, VpView_GetContextVisibility)
{
  Browser* pBrowser = FindBrowserByContext(pMsg->hContext);
  if (pBrowser) {
    Context* pContext = pBrowser->GetContext(pMsg->hContext);
    if (pContext) {
      pMsg->bVisible = pContext->IsVisible();
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, VpView_GetContextPosition)
{
  Browser* pBrowser = FindBrowserByContext(pMsg->hContext);
  if (pBrowser) {
    Context* pContext = pBrowser->GetContext(pMsg->hContext);
    if (pContext) {
      pContext->GetPosition(pMsg->nLeft, pMsg->nBottom);
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, VpView_GetContextSize)
{
  Browser* pBrowser = FindBrowserByContext(pMsg->hContext);
  if (pBrowser) {
    Context* pContext = pBrowser->GetContext(pMsg->hContext);
    if (pContext) {
      pContext->GetSize(pMsg->nWidth, pMsg->nHeight);
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, Timer_Event)
{
  for (BrowserListNode* pNode = 0; (pNode = browsers_.Next(pNode)) != 0; ) {
    Browser* pBrowser = pNode->Value();
    if (pBrowser) {
      pBrowser->OnTimer();
    }
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

#if defined(WIN32)

#include "SRegistry.h"

static HANDLE StartFirefox()
{
  HANDLE hProcess = NULL;

  String sModule;
  String sArgs = "-new-window about:blank";

  // Start a FF and check the result
  String sCmd = SRegistry::GetString(HKEY_CLASSES_ROOT, "FirefoxHTML\\shell\\open\\command", "", "");
  if (sCmd.startsWith("\"")) {
    String sPart;
    sCmd.nextToken("\"", sPart);
    sCmd.nextToken("\"", sPart);
    sModule = sPart;
  } else {
    String sPart;
    sCmd.nextToken(" ", sPart);
    sModule = sPart;
  }

  STARTUPINFO	si;	
  ZeroMemory (&si, sizeof(si));
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi;

  if (! CreateProcess(
          sModule, // Module name 
          sArgs,   // Command line. 
          NULL,    // Process handle not inheritable. 
          NULL,    // Thread handle not inheritable. 
          FALSE,   // Set handle inheritance to FALSE.  
          0,       // creation flags. 
          NULL,    // Use parent's environment block. 
          NULL,    // Use parent's starting directory. 
          &si,     // Pointer to si structure.
          &pi)     // Pointer to PROCESS_INFORMATION structure.
          ) {
     //s.appendf("CreateProcess failed for command line: '%s %s', GetLastError=%d", StringType(sModule), StringType(sArgs), ::GetLastError());
  } else {
    hProcess = pi.hProcess;
  }

  return hProcess;
}

static void KillFirefox(HANDLE hProcess)
{
  ::TerminateProcess(hProcess, 0);
}

String BrowserInfoModuleTester::GetFirefoxToplevelWindow()
{
  String s;

  HANDLE hProcess = StartFirefox();

  if (!s) {
    // Needs at least 1 FF window to succeed
    int bFound = 0;
    int nCnt = 0;
    while (!bFound && nCnt < 50) {
      nCnt++;
      Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("", IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate);
      HWND hWnd = win;
      if (hWnd != NULL) {
        bFound = 1;
      } else {
        ::Sleep(200);
      }
    }

    if (!bFound) {
      s = "No Firefox found";
    }
  }

  if (hProcess != NULL) { KillFirefox(hProcess); }
  
  return s;
}

String BrowserInfoModuleTester::GetFirefoxToplevelWindowWithCoordinates()
{
  String s;

  HANDLE hProcess1 = StartFirefox();
  HANDLE hProcess2 = StartFirefox();

  if (!s) {
    // Needs at least 1 FF window to succeed, wait for one
    int bFound = 0;
    int nCnt = 0;
    while (!bFound && nCnt < 50) {
      nCnt++;
      Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("", IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate);
      HWND hWnd = win;
      if (hWnd != NULL) {
        bFound = 1;
      } else {
        ::Sleep(200);
      }
    }
    if (!bFound){
      s = "No Firefox started";
    }
  }

  HWND hPositionedWindow = NULL;

  if (!s) {
    // Arrange cooredinates
    Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("", IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate);
    ::MoveWindow(win, 100, 100, 800, 600, FALSE);
    hPositionedWindow = win;
  }

  if (!s) {
    // Act
    // Find the one with the right coordinates
    Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("", 100, 100, 800, 600);
    HWND hWnd = win;
    if (hWnd != hPositionedWindow) {
      s = "Positioned Firefox window not found";
    }
  }

  if (hProcess1 != NULL) { KillFirefox(hProcess1); }
  if (hProcess2 != NULL) { KillFirefox(hProcess2); }

  return s;
}

String BrowserInfoModuleTester::DontGetFirefoxWithWrongCoordinates()
{
  String s;

  HANDLE hProcess1 = StartFirefox();
  HANDLE hProcess2 = StartFirefox();

  if (!s) {
    // Needs at least 1 FF window to succeed, wait for one
    int bFound = 0;
    int nCnt = 0;
    while (!bFound && nCnt < 50) {
      nCnt++;
      Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("", IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate);
      HWND hWnd = win;
      if (hWnd != NULL) {
        bFound = 1;
      } else {
        ::Sleep(200);
      }
    }
    if (!bFound){
      s = "No Firefox started";
    }
  }

  if (!s) {
    // Arrange cooredinates
    Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("", IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate, IgnoreCoordinate);
    ::MoveWindow(win, 100, 100, 800, 600, FALSE);
  }

  if (!s) {
    // Act
    // Find the one with the right coordinates
    Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("", 111, 100, 800, 600); // <----------- X is wrong
    HWND hWnd = win;
    if (hWnd != NULL) {
      s = "Should not find window";
    }
  }

  if (hProcess1 != NULL) { KillFirefox(hProcess1); }
  if (hProcess2 != NULL) { KillFirefox(hProcess2); }

  return s;
}
#endif

AP_MSG_HANDLER_METHOD(BrowserInfoModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/BrowserInfo", 0)) {
    #if defined(WIN32)
      AP_UNITTEST_REGISTER(BrowserInfoModuleTester::GetFirefoxToplevelWindow)
      AP_UNITTEST_REGISTER(BrowserInfoModuleTester::GetFirefoxToplevelWindowWithCoordinates)
      AP_UNITTEST_REGISTER(BrowserInfoModuleTester::DontGetFirefoxWithWrongCoordinates)
    #endif
  }
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/BrowserInfo", 0)) {
    #if defined(WIN32)
      AP_UNITTEST_EXECUTE(BrowserInfoModuleTester::GetFirefoxToplevelWindow)
      AP_UNITTEST_EXECUTE(BrowserInfoModuleTester::GetFirefoxToplevelWindowWithCoordinates)
      AP_UNITTEST_EXECUTE(BrowserInfoModuleTester::DontGetFirefoxWithWrongCoordinates)
    #endif
  }
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int BrowserInfoModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, BrowserInfo_BeginTrackCoordinates, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, BrowserInfo_EndTrackCoordinates, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_ContextVisibility, this, ApCallbackPosEarly); // Catch before UI responds and filter with toplevel window state
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_GetContextVisibility, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_GetContextPosition, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_GetContextSize, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, Timer_Event, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(BrowserInfoModule, this);

  return ok;
}

void BrowserInfoModule::Exit()
{
  AP_UNITTEST_UNHOOK(BrowserInfoModule, this);
  AP_MSG_REGISTRY_FINISH;
}
