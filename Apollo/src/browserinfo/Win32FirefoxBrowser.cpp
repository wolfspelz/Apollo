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
      HWND hParent = ::GetParent(pNode->Value());
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
  HWND hWnd = win_;

  if (hWnd != NULL) {
    ContextNode* pNode = contexts_.Next(0);
    if (pNode) {
      Context* pContext = pNode->Value();
      if (pContext) {

        Msg_BrowserInfo_GetContextWin32Window msg;
        msg.hContext = pContext->apHandle();
        if (msg.Request()) {
          PutWindowBeforeBrowser(msg.hWnd);
        }

      }
    }
  }
}

void Win32FirefoxBrowser::PutWindowBeforeBrowser(HWND hWnd)
{
  // Are we still in front of the browser?
  HWND hWndNext = ::GetNextWindow(hWnd, GW_HWNDNEXT);
  if (hWndNext != win_) {
    // No -> re-stack

    // Get window in front of browser and put us behind it = in between
    HWND hWndPrev = ::GetNextWindow(win_, GW_HWNDPREV);
    // If hWndPrev == NULL, then hWndPrev == HWND_TOP, because HWND_TOP == ((HWND)0)
    ::SetWindowPos(hWnd, hWndPrev, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
  }
}
