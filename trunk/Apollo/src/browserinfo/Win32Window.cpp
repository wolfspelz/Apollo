// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Win32Window.h"

BOOL CALLBACK WindowFinder::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
  WindowFinder *pWindowFinder = (WindowFinder*) lParam;
  if (pWindowFinder) {
    pWindowFinder->OnWindow(hWnd);
  }
  return TRUE;
}

void WindowFinder::Run()
{
  if (hParent_ == NULL) {
    ::EnumWindows((WNDENUMPROC) WindowFinder::EnumWindowsProc, (LPARAM) this);
  } else {
    ::EnumChildWindows(hParent_, (WNDENUMPROC) WindowFinder::EnumWindowsProc, (LPARAM) this);
  }
}

bool WindowFinder::HasClass(HWND hWnd, const String& s)
{
  TCHAR szClass[MaxClassLen];
  int nLen = ::GetClassName(hWnd, szClass, MaxClassLen);
  if (nLen > 0) {
    String sClass = szClass;
    if (s == sClass) {
      return true;
    }
  }
  return false;
}

bool WindowFinder::HasTitle(HWND hWnd, const String& s)
{
  TCHAR szTitle[MaxTitleLen];
  int nLen = ::GetWindowText(hWnd, szTitle, MaxTitleLen);
  if (nLen > 0) {
    String sTitle = szTitle;
    if (s == sTitle) {
      return true;
    }
  }
  return false;
}

//------------------------------

void ChildClassFinder::OnWindow(HWND hWnd)
{
  if (HasClass(hWnd, sClass_)) {
    list_.Set(list_.Count(), hWnd);
  }
}

//------------------------------

void ChildClassPathFinder::OnWindow(HWND hWnd)
{
  String sPath = sClassPath_;
  String sClass;
  if (sPath.nextToken("/", sClass)) {
    if (HasClass(hWnd, sClass)) {

      if (sPath.empty()) {
        list_.Set(list_.Count(), hWnd);
      } else {
        ChildClassPathFinder ccpf(hWnd, sPath);
        ccpf.Run();
        for (HWNDListNode* pNode = 0; (pNode = ccpf.list_.Next(pNode)) != 0; ) {
          list_.Set(list_.Count(), pNode->Value());
        }
      }

    }
  }
}

//------------------------------

FirefoxFinder::FirefoxFinder()
:WindowFinder(NULL)
,sToplevelClass_("MozillaUIWindowClass")
,sChildClassPath_("MozillaWindowClass/MozillaWindowClass/MozillaWindowClass/MozillaContentWindowClass")
{
}

void FirefoxFinder::OnWindow(HWND hWnd)
{
  if (HasClass(hWnd, sToplevelClass_)) {
    ChildClassPathFinder ccpf(hWnd, sChildClassPath_);
    ccpf.Run();
    if (ccpf.list_.Count() > 0) {
      list_.Set(list_.Count(), hWnd);
    }
  }
}

Apollo::WindowHandle FirefoxFinder::GetFirefoxToplevelWindow(const String& sTitle, int nX, int nY, int nWidth, int nHeight)
{
  HWND hWnd = NULL;

  FirefoxFinder foxPack;
  foxPack.Run();
  if (foxPack.list_.Count() == 1) {
    // Just 1 -> take it anyway

  } else {

    HWNDList candidates_ = foxPack.list_;

    // Filter by title
    if (!sTitle.empty()) {
      // Count how many titles match
      int nCnt = 0;
      for (HWNDListNode* pNode = 0; (pNode = candidates_.Next(pNode)) != 0; ) {
        if (WindowFinder::HasTitle(pNode->Value(), sTitle)) {
          nCnt++;
        }
      }

      if (nCnt == 0) {
        // do not filter by title if no match
      } else {
        for (HWNDListNode* pNode = 0; (pNode = candidates_.Next(pNode)) != 0; ) {
          if (!WindowFinder::HasTitle(pNode->Value(), sTitle)) {
            foxPack.list_.Unset(pNode->Key());
          }
        }
      }
    } // sTitle

    // Filter by coordinates
    if (nX != IgnoreCoordinate || nY != IgnoreCoordinate || nWidth != IgnoreCoordinate || nHeight != IgnoreCoordinate) {
    }
  }

  if (foxPack.list_.Count() > 1) {
    apLog_Error((LOG_CHANNEL, "BrowserInfoModule::GetFirefoxToplevelWindow", "Ambiguity: %d candidates", foxPack.list_.Count()));
  } if (foxPack.list_.Count() == 0) {
    apLog_Error((LOG_CHANNEL, "BrowserInfoModule::GetFirefoxToplevelWindow", "No window found"));
  } else {
    // Extract
    hWnd = foxPack.list_.Next(0)->Value();
  }

  return hWnd;
}

