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
#include "Win32FirefoxBrowser.h"

void Win32FirefoxBrowser::OnTimer()
{
  HWND hWnd = win_;

  if (hWnd != NULL) {
    HWND hContent = NULL;

    // Firefox before 3.6.8?
    //ChildClassPathFinder ccpf(hWnd, "MozillaWindowClass/MozillaWindowClass/MozillaWindowClass/MozillaContentWindowClass");
    //ccpf.Run();
    //// Assuming tabs are stacked and at the same position
    //// Find grandparent and check valid size (invisible tabs have size 0x0)
    //// Then take the grandparent as content rect
    //for (HWNDListNode* pNode = 0; (pNode = ccpf.list_.Next(pNode)) != 0; ) {
    //  RECT rContent;
    //  HWND hParent = ::GetParent(pNode->Value());
    //  HWND hGrandParent = ::GetParent(hParent);
    //  ::GetWindowRect(hGrandParent, &rContent);
    //  if (rContent.right - rContent.left != 0 && rContent.bottom - rContent.top) {
    //    hContent = hGrandParent;
    //    break;
    //  }
    //}

    // Firefox 3.6.8
    ChildClassPathFinder ccpf(hWnd, "MozillaWindowClass/MozillaContentWindowClass/MozillaWindowClass");
    ccpf.Run();
    // Assuming tabs are stacked and at the same position
    // Find parent and check valid size (invisible tabs have size 0x0)
    // Then take the parent as content rect
    for (HWNDListNode* pNode = 0; (pNode = ccpf.list_.Next(pNode)) != 0; ) {
      RECT rContent;
      HWND hParent = ::GetParent(pNode->Key());
      ::GetWindowRect(hParent, &rContent);
      if (rContent.right - rContent.left != 0 && rContent.bottom - rContent.top) {
        hContent = hParent;
        break;
      }
    }

    RECT rect;
    ::GetWindowRect(hContent, &rect);

    int bVisible = 1;
    int nLeft = rect.left;
    int nBottom = rect.bottom - 1;
    int nWidth = rect.right - rect.left;
    int nHeight = rect.bottom - rect.top;

    if (rect.top < -30000) {
      bVisible = 0;
    }

    AdjustPosition(bVisible, nLeft, nBottom, nWidth, nHeight);
    AdjustStackingOrder();

  } // hWnd != NULL
}

void Win32FirefoxBrowser::AdjustStackingOrder()
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
