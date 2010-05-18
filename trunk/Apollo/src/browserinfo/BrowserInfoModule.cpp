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

Browser* BrowserInfoModule::FindBrowserByContext(ApHandle hContext)
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

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(BrowserInfoModule, BrowserInfo_BeginTrackCoordinates)
{
  ApHandle hContext = pMsg->hContext;

  String sTitle = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Title].getString();
  String sX = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_X].getString();
  String sY = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Y].getString();
  String sWidth = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Width].getString();
  String sHeight = pMsg->kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Height].getString();

  int nX = IgnoreCoordinate; if (!sX.empty()) { nX = String::atoi(sX); }
  int nY = IgnoreCoordinate; if (!sY.empty()) { nY = String::atoi(sY); }
  int nWidth = IgnoreCoordinate; if (!sWidth.empty()) { nWidth = String::atoi(sWidth); }
  int nHeight = IgnoreCoordinate; if (!sHeight.empty()) { nHeight = String::atoi(sHeight); }

  Apollo::WindowHandle win;
  if (0) {
  } else if (pMsg->sType == Msg_BrowserInfo_BeginTrackCoordinates_Type_Firefox) {
    win = FirefoxFinder::GetFirefoxToplevelWindow(sTitle, nX, nY, nWidth, nHeight);
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
#endif
      }

      if (pBrowser == 0) {
        apLog_Error((LOG_CHANNEL, "BrowserInfoModule::BrowserInfo_BeginTrackCoordinates", "No browser for platformand type=%s", StringType(pMsg->sType)));
      } else {
        browsers_.Set(win, pBrowser);
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

AP_MSG_HANDLER_METHOD(BrowserInfoModule, VpView_ContextPosition)
{
  Browser* pBrowser = FindBrowserByContext(pMsg->hContext);
  if (pBrowser) {
    Context* pContext = pBrowser->GetContext(pMsg->hContext);
    if (pContext) {
      pContext->SetPosition(pMsg->nX, pMsg->nY);
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, VpView_ContextSize)
{
  Browser* pBrowser = FindBrowserByContext(pMsg->hContext);
  if (pBrowser) {
    Context* pContext = pBrowser->GetContext(pMsg->hContext);
    if (pContext) {
      pContext->SetSize(pMsg->nWidth, pMsg->nHeight);
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
      pContext->GetPosition(pMsg->nX, pMsg->nY);
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

AP_MSG_HANDLER_METHOD(BrowserInfoModule, System_SecTimer)
{
  for (BrowserListNode* pNode = 0; (pNode = browsers_.Next(pNode)) != 0; ) {
    Browser* pBrowser = pNode->Value();
    if (pBrowser) {
      pBrowser->SecTimer();
    }
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

#if defined(WIN32)
String BrowserInfoModuleTester::GetFirefoxToplevelWindow()
{
  String s;

  // Should start a FF and check the result
  //Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("Google Mail - Inbox - wolf.heiner@googlemail.com - Mozilla Firefox", 0, 0, 0, 0);

  // Needs at least 1 FF window to succeed
  Apollo::WindowHandle win = FirefoxFinder::GetFirefoxToplevelWindow("", 0, 0, 0, 0);

  HWND hWnd = win;
  if (hWnd == NULL) {
    s = "No Firefox found";
  }

  return s;
}
#endif

AP_MSG_HANDLER_METHOD(BrowserInfoModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/BrowserInfo", 0)) {
    #if defined(WIN32)
      AP_UNITTEST_REGISTER(BrowserInfoModuleTester::GetFirefoxToplevelWindow)
    #endif
  }
}

AP_MSG_HANDLER_METHOD(BrowserInfoModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/BrowserInfo", 0)) {
    #if defined(WIN32)
      AP_UNITTEST_EXECUTE(BrowserInfoModuleTester::GetFirefoxToplevelWindow)
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_ContextPosition, this, ApCallbackPosEarly); // Catch before UI responds and filter with toplevel window state
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_ContextSize, this, ApCallbackPosEarly); // Catch before UI responds and filter with toplevel window state
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_GetContextVisibility, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_GetContextPosition, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, VpView_GetContextSize, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, BrowserInfoModule, System_SecTimer, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(BrowserInfoModule, this);

  return ok;
}

void BrowserInfoModule::Exit()
{
  AP_UNITTEST_UNHOOK(BrowserInfoModule, this);
  AP_MSG_REGISTRY_FINISH;
}
