// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "BrowserInfoModule.h"

#if defined(WIN32)
#include "Win32Window.h"
#include "Firefox3Win32Browser.h"
#include "Firefox4Win32Browser.h"
#include "ChromeWin32Browser.h"
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

  String sType = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Type].getString();
  String sVersion = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Version].getString();
#if defined(WIN32)
  int nWin32HWND = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Win32HWND].getInt();
#endif // defined(WIN32)

  // sVersion
  // FF 4.0.1: Mozilla/5.0 (Windows NT 6.1; rv:2.0.1) Gecko/20100101 Firefox/4.0.1

  Apollo::WindowHandle win;
  if (0) {
#if defined(WIN32)
  } else if (nWin32HWND != 0) {
    win = (HWND) nWin32HWND;
  } else if (sType == Msg_BrowserInfo_BeginTrackCoordinates_Signature_Type_Firefox && sVersion.contains("Firefox/3.")) {
    win = Firefox3Finder::GetToplevelWindow(pMsg->kvSignature);
  } else if (sType == Msg_BrowserInfo_BeginTrackCoordinates_Signature_Type_Firefox) {
    win = Firefox4Finder::GetToplevelWindow(pMsg->kvSignature);
  } else if (sType == Msg_BrowserInfo_BeginTrackCoordinates_Signature_Type_Chrome) {
    win = ChromeFinder::GetToplevelWindow(pMsg->kvSignature);
#endif // defined(WIN32)
  }

  if (!win.isValid()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "No valid browser window, type=%s", _sz(sType)));
  } else {

    Browser* pBrowser = 0;
    browsers_.Get(win, pBrowser);
    if (pBrowser == 0) {

      if (0) {
#if defined(WIN32)
      } else if (nWin32HWND != 0) {
        pBrowser = new Win32Browser(win);
      } else if (sType == Msg_BrowserInfo_BeginTrackCoordinates_Signature_Type_Firefox && sVersion.contains("Firefox/3.")) {
        pBrowser = new Firefox3Win32Browser(win);
      } else if (sType == Msg_BrowserInfo_BeginTrackCoordinates_Signature_Type_Firefox) {
        pBrowser = new Firefox4Win32Browser(win);
      } else if (sType == Msg_BrowserInfo_BeginTrackCoordinates_Signature_Type_Chrome) {
        pBrowser = new ChromeWin32Browser(win);
#endif // defined(WIN32)
      }

      if (pBrowser == 0) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "No browser for platformand type=%s", _sz(sType)));
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
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "No browser for ctxt=" ApHandleFormat "", ApHandlePrintf(pMsg->hContext)));
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
        pContext->SetVisibility(pMsg->bVisible);
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
     //s.appendf("CreateProcess failed for command line: '%s %s', GetLastError=%d", _sz(sModule), _sz(sArgs), ::GetLastError());
  } else {
    hProcess = pi.hProcess;
  }

  return hProcess;
}

static int WaitForFirefox()
{
  int bFound = 0;

  if (!bFound) {
    // Needs at least 1 FF4 window to succeed
    int nCnt = 0;
    while (!bFound && nCnt < 20) {
      nCnt++;
      Apollo::KeyValueList kvSignature;
      Apollo::WindowHandle win = Firefox4Finder::GetToplevelWindow(kvSignature);
      HWND hWnd = win;
      if (hWnd != NULL) {
        bFound = 1;
      } else {
        ::Sleep(200);
      }
    }
  }

  if (!bFound) {
    // Needs at least 1 FF3 window to succeed
    int nCnt = 0;
    while (!bFound && nCnt < 20) {
      nCnt++;
      Apollo::KeyValueList kvSignature;
      Apollo::WindowHandle win = Firefox3Finder::GetToplevelWindow(kvSignature);
      HWND hWnd = win;
      if (hWnd != NULL) {
        bFound = 1;
      } else {
        ::Sleep(200);
      }
    }
  }

  return bFound;
}

static void KillFirefox(HANDLE hProcess)
{
  ::TerminateProcess(hProcess, 0);

  int bTerminated = false;
  int nCnt = 0;
  while (!bTerminated && nCnt < 20) {
    nCnt++;
    DWORD nExitCode = 0;
    BOOL bSuccess = ::GetExitCodeProcess(hProcess, &nExitCode);
    if (nExitCode == STILL_ACTIVE) {
      ::Sleep(200);
    } else {
      bTerminated = 1;
    }
  }
}

String BrowserInfoModuleTester::GetFirefoxToplevelWindow()
{
  String s;

  HANDLE hProcess = StartFirefox();

  if (!s) {
    if (!WaitForFirefox()) {
      s = "No Firefox found (need FF3/4)";
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
    if (!WaitForFirefox()) {
      s = "No Firefox started (need FF3/4)";
    }
  }

  HWND hPositionedWindow = NULL;

  if (!s) {
    // Arrange cooredinates
    Apollo::KeyValueList kvSignature;
    Apollo::WindowHandle win = Firefox4Finder::GetToplevelWindow(kvSignature);
    ::MoveWindow(win, 100, 100, 800, 600, FALSE);
    hPositionedWindow = win;
  }

  if (!s) {
    // Act
    // Find the one with the right coordinates
    Apollo::KeyValueList kvSignature;
    kvSignature.add(Msg_BrowserInfo_BeginTrackCoordinates_Signature_Left, 100);
    kvSignature.add(Msg_BrowserInfo_BeginTrackCoordinates_Signature_Top, 100);
    kvSignature.add(Msg_BrowserInfo_BeginTrackCoordinates_Signature_Width, 800);
    kvSignature.add(Msg_BrowserInfo_BeginTrackCoordinates_Signature_Height, 600);
    Apollo::WindowHandle win = Firefox4Finder::GetToplevelWindow(kvSignature);
    HWND hWnd = win;
    if (hWnd != hPositionedWindow) {
      s = "Positioned Firefox window not found";
    }
  }

  if (hProcess1 != NULL) { KillFirefox(hProcess1); }
  if (hProcess2 != NULL) { KillFirefox(hProcess2); }

  return s;
}

String BrowserInfoModuleTester::DontGetFirefoxToplevelWindowWithWrongCoordinates()
{
  String s;

  HANDLE hProcess1 = StartFirefox();
  HANDLE hProcess2 = StartFirefox();

  if (!s) {
    if (!WaitForFirefox()) {
      s = "No Firefox started (need FF3/4)";
    }
  }

  if (!s) {
    // Arrange coordinates
    Apollo::KeyValueList kvSignature;
    Apollo::WindowHandle win = Firefox4Finder::GetToplevelWindow(kvSignature);
    ::MoveWindow(win, 100, 100, 800, 600, FALSE);
  }

  if (!s) {
    // Act
    // Find the one with the right coordinates
    Apollo::KeyValueList kvSignature;
    kvSignature.add(Msg_BrowserInfo_BeginTrackCoordinates_Signature_Left, 111); // <----------- X is wrong
    kvSignature.add(Msg_BrowserInfo_BeginTrackCoordinates_Signature_Top, 100);
    kvSignature.add(Msg_BrowserInfo_BeginTrackCoordinates_Signature_Width, 800);
    kvSignature.add(Msg_BrowserInfo_BeginTrackCoordinates_Signature_Height, 600);
    Apollo::WindowHandle win = Firefox4Finder::GetToplevelWindow(kvSignature);
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
      AP_UNITTEST_REGISTER(BrowserInfoModuleTester::DontGetFirefoxToplevelWindowWithWrongCoordinates)
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
      AP_UNITTEST_EXECUTE(BrowserInfoModuleTester::DontGetFirefoxToplevelWindowWithWrongCoordinates)
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
