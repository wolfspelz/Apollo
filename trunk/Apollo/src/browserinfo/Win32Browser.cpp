// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "apLog.h"
#include "Local.h"
#include "MsgBrowserInfo.h"
#include "Win32Window.h"
#include "Win32Browser.h"

void Win32Browser::AdjustStackingOrder()
{
  HWNDList contextWindows;

  // Make a context window list
  {
    for (ContextNode* pNode = 0; (pNode = contexts_.Next(pNode)) != 0; ) {
      Context* pContext = pNode->Value();

      Msg_BrowserInfo_GetContextWin32Window msg;
      msg.hContext = pContext->apHandle();
      if (msg.Request()) {
        contextWindows.Set(msg.hWnd, 1);
      }
    }
  }

  int bAllContextsAboveBrowser = 0;
  HWND hWndPrev = NULL;

  // Find previous (foreign) window, which is not one of the contexts
  {
    HWND hWndCurrent = win_;
    int bFoundPreviousWindowOrTop = 0;
    int nCntContextsAboveBrowser = 0;
    while (!bFoundPreviousWindowOrTop && nCntContextsAboveBrowser < contextWindows.Count()) {
      hWndPrev = ::GetNextWindow(hWndCurrent, GW_HWNDPREV);
      if (hWndPrev == NULL || contextWindows.Find(hWndPrev) == 0) {
        bFoundPreviousWindowOrTop = 1;
      } else {
        nCntContextsAboveBrowser++;
        hWndCurrent = hWndPrev;
      }
    }
    if (nCntContextsAboveBrowser == contextWindows.Count()) {
      bAllContextsAboveBrowser = 1;
    }
  }

  // Re-stack below the just found previous window
  if (!bAllContextsAboveBrowser) {
    for (HWNDListNode* pNode = 0; (pNode = contextWindows.Next(pNode)) != 0; ) {
      HWND hWnd = pNode->Key();
      ::SetWindowPos(hWnd, hWndPrev, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
      hWndPrev = hWnd;
    }
  }
}
