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

void Win32Browser::OnTimer()
{
  HWND hWnd = win_;

  if (hWnd != NULL) {
    HWND hContent = hWnd;

    RECT wr;
    ::GetWindowRect(hContent, &wr);

    int bVisible = 1;
    int nWidth = wr.right - wr.left;
    int nHeight = wr.bottom - wr.top;
    int nLeft = wr.left;
    int nBottom = wr.top + nHeight;

    if (wr.top < -30000) {
      bVisible = 0;
    }

    AdjustPosition(bVisible, nLeft, nBottom, nWidth, nHeight);
    AdjustStackingOrder();

  } // hWnd != NULL
}

void Win32Browser::AdjustStackingOrder()
{
  HWNDList contextWindows;

  // Make a context window list
  {
    for (ContextNode* pNode = 0; (pNode = contexts_.Next(pNode)) != 0; ) {
      Context* pContext = pNode->Value();

      Apollo::WindowHandle win = pContext->GetWindow();
      if (!win.isValid()) {
        Msg_BrowserInfo_GetContextWin32Window msg;
        msg.hContext = pContext->apHandle();
        if (msg.Request()) {
          pContext->SetWindow(msg.hWnd);
          win = msg.hWnd;
        }
      }
      contextWindows.Set(win, 1);
    }
  }

  int bAllContextsAboveBrowser = 0;
  HWND hWndPrev = NULL;

  // Find previous (foreign) window, which is not one of the contexts
  //{
  //  HWND hWndCurrent = win_;
  //  int bFoundPreviousWindowOrTop = 0;
  //  int nCntContextsAboveBrowser = 0;
  //  while (!bFoundPreviousWindowOrTop && nCntContextsAboveBrowser < contextWindows.Count()) {
  //    hWndPrev = ::GetNextWindow(hWndCurrent, GW_HWNDPREV);
  //    if (hWndPrev == NULL || contextWindows.Find(hWndPrev) == 0) {
  //      bFoundPreviousWindowOrTop = 1;
  //    } else {
  //      nCntContextsAboveBrowser++;
  //      hWndCurrent = hWndPrev;
  //    }
  //  }
  //  if (nCntContextsAboveBrowser == contextWindows.Count()) {
  //    bAllContextsAboveBrowser = 1;
  //  }
  //}

  {
    HWND hWndCurrent = win_;
    int bFoundPreviousWindowOrTop = 0;
    int nCntContextsAboveBrowser = 0;
    while (!bFoundPreviousWindowOrTop && nCntContextsAboveBrowser < contextWindows.Count()) {
      hWndPrev = ::GetNextWindow(hWndCurrent, GW_HWNDPREV);
      if (hWndPrev == NULL || contextWindows.Find(hWndPrev) == 0) {
        if (hWndPrev == NULL) {
          bFoundPreviousWindowOrTop = 1;
        } else {
          Flexbuf<TCHAR> buf(10240);
          ::GetClassName(hWndPrev, (TCHAR*) buf, 10240);
          String sClass = (TCHAR*) buf;
          if (sClass == "SWindow_Floating_Image" || sClass == "FancyBaseWin_WINDOWCLASS") {
            // Skip weblin windows
            //bFoundPreviousWindowOrTop = 1;
            hWndCurrent = hWndPrev;
          } else {
            bFoundPreviousWindowOrTop = 1;
          }
        }
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
