// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Browser.h"
#include "BrowserInfoModule.h"

void Browser::AddContext(const ApHandle& hContext)
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

void Browser::RemoveContext(const ApHandle& hContext)
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

Context* Browser::GetContext(const ApHandle& hContext)
{
  Context* pContext = 0;
  contexts_.Get(hContext, pContext);
  return pContext;
}

void Browser::AdjustPosition(int bVisible, int nLeft, int nBottom, int nWidth, int nHeight)
{
  int bPosChanged = 0;
  int bSizeChanged = 0;
  int bVisChanged = 0;

  if (bVisible != bVisible_) {
    bVisChanged = 1;
    bVisible_ = bVisible;
  }

  if (bVisible) {
    if (nLeft != nLeft_ || nBottom != nBottom_) {
      bPosChanged = 1;
      nLeft_ = nLeft;
      nBottom_ = nBottom;
    }

    if (nWidth != nWidth_ || nHeight != nHeight_) {
      bSizeChanged = 1;
      nWidth_ = nWidth;
      nHeight_ = nHeight;
    }
  }
  //apLog_Debug((LOG_CHANNEL, "Win32FirefoxBrowser::OnTimer", "%08x %d,%d,%d,%d", (int) hWnd, nLeft, nBottom, nWidth, nHeight));

  for (ContextNode* pNode = 0; (pNode = contexts_.Next(pNode)) != 0; ) {

    if (bPosChanged) {
      Msg_VpView_ContextPosition msg;
      msg.hContext = pNode->Key();
      msg.nLeft = nLeft;
      msg.nBottom = nBottom;
      LocalCallGuard g; msg.Send();
    }

    if (bSizeChanged) {
      Msg_VpView_ContextSize msg;
      msg.hContext = pNode->Key();
      msg.nWidth = nWidth;
      msg.nHeight = nHeight;
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
