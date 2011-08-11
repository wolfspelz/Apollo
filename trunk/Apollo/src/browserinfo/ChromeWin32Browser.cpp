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
#include "ChromeWin32Browser.h"

void ChromeWin32Browser::OnTimer()
{
  HWND hWnd = win_;

  if (hWnd != NULL) {
    HWND hContent = NULL;

    // No tabs as Win32 HWNDs
    // Then take the main HWND's client rect as content rect and manipulate the rectangle
    hContent = hWnd;
    
    ChildClassPathFinder ccpf(hWnd, "Chrome_WidgetWin_0/Chrome_RenderWidgetHostHWND");
    ccpf.Run();
    for (HWNDListNode* pNode = 0; (pNode = ccpf.list_.Next(pNode)) != 0; ) {
      hContent = pNode->Key();
    }
    RECT wr;
    ::GetWindowRect(hContent, &wr);
    RECT cr;
    ::GetClientRect(hContent, &cr);

    int bVisible = 1;
    int nLeft = wr.left + cr.left;
    int nBottom = wr.top + cr.bottom;
    int nWidth = cr.right - cr.left;
    int nHeight = cr.bottom - cr.top;

    if (wr.top < -30000) {
      bVisible = 0;
    }

    AdjustPosition(bVisible, nLeft, nBottom, nWidth, nHeight);
    AdjustStackingOrder();

  } // hWnd != NULL
}
