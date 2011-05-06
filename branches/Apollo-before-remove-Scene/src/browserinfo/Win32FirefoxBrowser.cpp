// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "apLog.h"
#include "Local.h"
#include "Win32Window.h"
#include "Win32FirefoxBrowser.h"

void Win32FirefoxBrowser::SecTimer()
{
  HWND hWnd = win_;
  
  if (hWnd != NULL) {
    HWND hContent = NULL;
    ChildClassPathFinder ccpf(hWnd, "MozillaWindowClass/MozillaWindowClass/MozillaWindowClass/MozillaContentWindowClass");
    ccpf.Run();

    // Assuming tabs are stacked and at the same position
    // Find grandparent and check valid size (invisible tabs have size 0x0)
    // Then take the grandparent as content rect
    for (HWNDListNode* pNode = 0; (pNode = ccpf.list_.Next(pNode)) != 0; ) {
      RECT rContent;
      HWND hParent = ::GetParent(pNode->Value());
      HWND hGrandParent = ::GetParent(hParent);
      ::GetWindowRect(hGrandParent, &rContent);
      if (rContent.right - rContent.left != 0 && rContent.bottom - rContent.top) {
        hContent = hGrandParent;
        break;
      }
    }

    RECT rect;
    ::GetWindowRect(hContent, &rect);

    int bVisible = 1;
    int nX = rect.left;
    int nY = rect.bottom - 1;
    int nW = rect.right - rect.left;
    int nH = rect.bottom - rect.top;

    if (rect.top < -30000) {
      bVisible = 0;
    }

    Evaluate(bVisible, nX, nY, nW, nH);

  } // hWnd != NULL
}
