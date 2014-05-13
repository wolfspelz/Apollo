// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
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

bool WindowFinder::HasCoordinates(HWND hWnd, int nLeft, int nTop, int nWidth, int nHeight)
{
  RECT r;
  ::GetWindowRect(hWnd, &r);
  if (nLeft != IgnoreCoordinate && nLeft != r.left) { return false; }
  if (nTop != IgnoreCoordinate && nTop != r.top) { return false; }
  if (nWidth != IgnoreCoordinate && nWidth != r.right - r.left) { return false; }
  if (nHeight != IgnoreCoordinate && nHeight != r.bottom - r.top) { return false; }
  return true;
}

//------------------------------

void ChildClassFinder::OnWindow(HWND hWnd)
{
  if (HasClass(hWnd, sClass_)) {
    list_.Set(hWnd, 1);
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
        list_.Set(hWnd, 1);
      } else {
        ChildClassPathFinder ccpf(hWnd, sPath);
        ccpf.Run();
        for (HWNDListNode* pNode = 0; (pNode = ccpf.list_.Next(pNode)) != 0; ) {
          list_.Set(pNode->Key(), 1);
        }
      }

    }
  }
}

//------------------------------

Firefox3Finder::Firefox3Finder()
:WindowFinder(NULL)
,sToplevelClass_("MozillaUIWindowClass")
,sChildClassPath_("MozillaWindowClass/MozillaContentWindowClass") // FF 3.6 ??
//,sChildClassPath_("MozillaWindowClass/MozillaWindowClass/MozillaWindowClass/MozillaContentWindowClass") // FF 3.5
{
}

void Firefox3Finder::OnWindow(HWND hWnd)
{
  if (HasClass(hWnd, sToplevelClass_)) {
    ChildClassPathFinder ccpf(hWnd, sChildClassPath_);
    ccpf.Run();
    if (ccpf.list_.Count() > 0) {
      list_.Set(hWnd, 1);
    }
  }
}

Apollo::WindowHandle Firefox3Finder::GetToplevelWindow(Apollo::KeyValueList& kvSignature)
{
  String sTitle = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Title].getString();
  String sLeft = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Left].getString();
  String sTop = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Top].getString();
  String sWidth = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Width].getString();
  String sHeight = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Height].getString();
  String sInnerWidth = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_InnerWidth].getString();
  String sInnerHeight = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_InnerHeight].getString();

  int nLeft = IgnoreCoordinate; if (!sLeft.empty()) { nLeft = String::atoi(sLeft); }
  int nTop = IgnoreCoordinate; if (!sTop.empty()) { nTop = String::atoi(sTop); }
  int nWidth = IgnoreCoordinate; if (!sWidth.empty()) { nWidth = String::atoi(sWidth); }
  int nHeight = IgnoreCoordinate; if (!sHeight.empty()) { nHeight = String::atoi(sHeight); }
  int nInnerWidth = IgnoreCoordinate; if (!sInnerWidth.empty()) { nInnerWidth = String::atoi(sInnerWidth); }
  int nInnerHeight = IgnoreCoordinate; if (!sInnerHeight.empty()) { nInnerHeight = String::atoi(sInnerHeight); }

  HWND hWnd = NULL;

  Firefox3Finder foxPack;
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
        if (WindowFinder::HasTitle(pNode->Key(), sTitle)) {
          nCnt++;
        }
      }

      if (nCnt == 0) {
        // do not filter by title if no match
      } else {
        for (HWNDListNode* pNode = 0; (pNode = candidates_.Next(pNode)) != 0; ) {
          if (!WindowFinder::HasTitle(pNode->Key(), sTitle)) {
            foxPack.list_.Unset(pNode->Key());
          }
        }
      }
    } // sTitle

    // Filter by coordinates
    if (nLeft != IgnoreCoordinate || nTop != IgnoreCoordinate || nWidth != IgnoreCoordinate || nHeight != IgnoreCoordinate) {
      for (HWNDListNode* pNode = 0; (pNode = candidates_.Next(pNode)) != 0; ) {
        if (!WindowFinder::HasCoordinates(pNode->Key(), nLeft, nTop, nWidth, nHeight)) {
          foxPack.list_.Unset(pNode->Key());
        }
      }

    }
  }

  if (foxPack.list_.Count() > 1) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Ambiguity: %d candidates", foxPack.list_.Count()));
  } if (foxPack.list_.Count() == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "No window found"));
  } else {
    // Extract
    hWnd = foxPack.list_.Next(0)->Key();
  }

  return hWnd;
}

//------------------------------

Firefox4Finder::Firefox4Finder()
:WindowFinder(NULL)
,sToplevelClass_("MozillaWindowClass")
{
}

void Firefox4Finder::OnWindow(HWND hWnd)
{
  if (HasClass(hWnd, sToplevelClass_)) {
    list_.Set(hWnd, 1);
  }
}

Apollo::WindowHandle Firefox4Finder::GetToplevelWindow(Apollo::KeyValueList& kvSignature)
{
  String sTitle = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Title].getString();
  String sLeft = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Left].getString();
  String sTop = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Top].getString();
  String sWidth = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Width].getString();
  String sHeight = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_Height].getString();
  String sInnerWidth = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_InnerWidth].getString();
  String sInnerHeight = kvSignature[Msg_BrowserInfo_BeginTrackCoordinates_Signature_InnerHeight].getString();

  int nLeft = IgnoreCoordinate; if (!sLeft.empty()) { nLeft = String::atoi(sLeft); }
  int nTop = IgnoreCoordinate; if (!sTop.empty()) { nTop = String::atoi(sTop); }
  int nWidth = IgnoreCoordinate; if (!sWidth.empty()) { nWidth = String::atoi(sWidth); }
  int nHeight = IgnoreCoordinate; if (!sHeight.empty()) { nHeight = String::atoi(sHeight); }
  int nInnerWidth = IgnoreCoordinate; if (!sInnerWidth.empty()) { nInnerWidth = String::atoi(sInnerWidth); }
  int nInnerHeight = IgnoreCoordinate; if (!sInnerHeight.empty()) { nInnerHeight = String::atoi(sInnerHeight); }

  HWND hWnd = NULL;

  Firefox4Finder foxPack;
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
        if (WindowFinder::HasTitle(pNode->Key(), sTitle)) {
          nCnt++;
        }
      }

      if (nCnt == 0) {
        // do not filter by title if no match
      } else {
        for (HWNDListNode* pNode = 0; (pNode = candidates_.Next(pNode)) != 0; ) {
          if (!WindowFinder::HasTitle(pNode->Key(), sTitle)) {
            foxPack.list_.Unset(pNode->Key());
          }
        }
      }
    } // sTitle

    // Filter by coordinates
    if (nLeft != IgnoreCoordinate || nTop != IgnoreCoordinate || nWidth != IgnoreCoordinate || nHeight != IgnoreCoordinate) {
      for (HWNDListNode* pNode = 0; (pNode = candidates_.Next(pNode)) != 0; ) {
        if (!WindowFinder::HasCoordinates(pNode->Key(), nLeft, nTop, nWidth, nHeight)) {
          foxPack.list_.Unset(pNode->Key());
        }
      }

    }
  }

  if (foxPack.list_.Count() > 1) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Ambiguity: %d candidates", foxPack.list_.Count()));
  } if (foxPack.list_.Count() == 0) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "No window found"));
  } else {
    // Extract
    hWnd = foxPack.list_.Next(0)->Key();
  }

  return hWnd;
}
