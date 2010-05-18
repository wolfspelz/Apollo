// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Browser.h"
#include "BrowserInfoModule.h"

void Browser::AddContext(ApHandle hContext)
{
  int bWasEmpty = !HasContexts();

  Context* pContext = new Context(hContext);
  if (pContext) {
    contexts_.Set(hContext, pContext);
  }

  if (bWasEmpty && HasContexts()) {
    StartTracking();
  }
}

void Browser::RemoveContext(ApHandle hContext)
{
  int bWasEmpty = !HasContexts();

  Context* pContext = GetContext(hContext);
  if (pContext) {
    contexts_.Unset(hContext);
    delete pContext;
    pContext = 0;
  }

  if (!bWasEmpty && !HasContexts()) {
    StopTracking();
  }
}

Context* Browser::GetContext(ApHandle hContext)
{
  Context* pContext = 0;
  contexts_.Get(hContext, pContext);
  return pContext;
}

void Browser::Evaluate(int bVisible, int nX, int nY, int nW, int nH)
{
  int bPosChanged = 0;
  int bSizeChanged = 0;
  int bVisChanged = 0;

  if (bVisible != bVisible_) {
    bVisChanged = 1;
    bVisible_ = bVisible;
  }

  if (bVisible) {
    if (nX != nX_ || nY != nY_) {
      bPosChanged = 1;
      nX_ = nX;
      nY_ = nY;
    }

    if (nW != nW_ || nH != nH_) {
      bSizeChanged = 1;
      nW_ = nW;
      nH_ = nH;
    }
  }
  //apLog_Debug((LOG_CHANNEL, "Win32FirefoxBrowser::SecTimer", "%08x %d,%d,%d,%d", (int) hWnd, nX, nY, nW, nH));

  for (ContextNode* pNode = 0; (pNode = contexts_.Next(pNode)) != 0; ) {

    if (bPosChanged) {
      Msg_VpView_ContextPosition msg;
      msg.hContext = pNode->Key();
      msg.nX = nX;
      msg.nY = nY;
      LocalCallGuard g; msg.Send();
    }

    if (bSizeChanged) {
      Msg_VpView_ContextSize msg;
      msg.hContext = pNode->Key();
      msg.nWidth = nW;
      msg.nHeight = nH;
      LocalCallGuard g; msg.Send();
    }

    if (bVisChanged) {
      Context* pContext = pNode->Value();
      Msg_VpView_ContextVisibility msg;
      msg.hContext = pNode->Key();
      if (bVisible) {
        msg.bVisible = pContext->IsVisible();
      } else {
        msg.bVisible = 0;
      }
      LocalCallGuard g; msg.Send();
    }

  }
}
