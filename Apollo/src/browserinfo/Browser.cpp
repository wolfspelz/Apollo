// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Browser.h"
#include "BrowserInfoModule.h"

void Context::AdjustPosition(int bBrowserVisible, int nBrowserLeft, int nBrowserBottom, int nBrowserWidth, int nBrowserHeight)
{
  int bCombinedVisible = bBrowserVisible && bVisible_;
  int nCombinedLeft = nBrowserLeft;
  int nCombinedBottom = nBrowserBottom;
  int nCombinedWidth = nBrowserWidth;
  int nCombinedHeight = nBrowserHeight;

  int bPosChanged = 0;
  int bSizeChanged = 0;
  int bVisChanged = 0;

  if (bCombinedVisible != bCombinedVisible_) {
    bVisChanged = 1;
    bCombinedVisible_ = bCombinedVisible;
  }

  if (bCombinedVisible) {
    if (nCombinedLeft != nLeft_ || nCombinedBottom != nBottom_) {
      bPosChanged = 1;
      nLeft_ = nCombinedLeft;
      nBottom_ = nCombinedBottom;
    }

    if (nCombinedWidth != nWidth_ || nCombinedHeight != nHeight_) {
      bSizeChanged = 1;
      nWidth_ = nCombinedWidth;
      nHeight_ = nCombinedHeight;
    }
  }

  if (bPosChanged) {
    Msg_VpView_ContextPosition msg;
    msg.hContext = apHandle();
    msg.nLeft = nCombinedLeft;
    msg.nBottom = nCombinedBottom;
    LocalCallGuard g; msg.Send();
  }

  if (bSizeChanged) {
    Msg_VpView_ContextSize msg;
    msg.hContext = apHandle();
    msg.nWidth = nCombinedWidth;
    msg.nHeight = nCombinedHeight;
    LocalCallGuard g; msg.Send();
  }

  if (bVisChanged) {
    Msg_VpView_ContextVisibility msg;
    msg.hContext = apHandle();
    msg.bVisible = bCombinedVisible;
    LocalCallGuard g; msg.Send();
  }
}

// ------------------------------------------------------------

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
  for (ContextNode* pNode = 0; (pNode = contexts_.Next(pNode)) != 0; ) {

    bVisible_ = bVisible;
    nLeft_ = nLeft;
    nBottom_ = nBottom;
    nWidth_ = nWidth;
    nHeight_ = nHeight;

    //apLog_Debug((LOG_CHANNEL, "Win32FirefoxBrowser::OnTimer", "%08x %d,%d,%d,%d", (int) hWnd, nLeft, nBottom, nWidth, nHeight));

    Context* pContext = pNode->Value();
    if (pContext) {
      pContext->AdjustPosition(bVisible, nLeft, nBottom, nWidth, nHeight);
    }

  }
}
