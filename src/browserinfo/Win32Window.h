// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Win32Window_h_INCLUDED)
#define Win32Window_h_INCLUDED

#include "Apollo.h"
#include "ApContainer.h"
#include "WindowHandle.h"

typedef Tree<HWND, int, LessThan<HWND>> HWNDList;
typedef TreeNode<HWND, int> HWNDListNode;

class WindowFinder
{
public:
  WindowFinder(HWND hParent)
    :hParent_(hParent)
  {}

  virtual void Run();

  enum { MaxClassLen = 1024 };
  static bool HasClass(HWND hWnd, const String& s);
  enum { MaxTitleLen = 10*1024 };
  static bool HasTitle(HWND hWnd, const String& s);
  static bool HasCoordinates(HWND hWnd, int nLeft, int nTop, int nWidth, int nHeight);

  // override
  virtual void OnWindow(HWND hWnd) = 0;

  // in
  HWND hParent_;

  // out
  HWNDList list_;

public:
  static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
};

//------------------------------

class ChildClassFinder: public WindowFinder
{
public:
  ChildClassFinder(HWND hParent, const String sClass)
    :WindowFinder(hParent)
    ,sClass_(sClass)
  {}
  virtual void OnWindow(HWND hWnd);
  String sClass_;
};

//------------------------------

class ChildClassPathFinder: public WindowFinder
{
public:
  ChildClassPathFinder(HWND hParent, const String& sClassPath)
    :WindowFinder(hParent)
    ,sClassPath_(sClassPath)
  {}

  virtual void OnWindow(HWND hWnd);

  String sClassPath_;
};

class Firefox3Finder: public WindowFinder
{
public:
  Firefox3Finder();

  void OnWindow(HWND hWnd);

  static Apollo::WindowHandle GetToplevelWindow(Apollo::KeyValueList& kvSignature);

  String sToplevelClass_;
  String sChildClassPath_;
};

class Firefox4Finder: public WindowFinder
{
public:
  Firefox4Finder();

  void OnWindow(HWND hWnd);

  static Apollo::WindowHandle GetToplevelWindow(Apollo::KeyValueList& kvSignature);

  String sToplevelClass_;
};

class ChromeFinder: public WindowFinder
{
public:
  ChromeFinder();

  void OnWindow(HWND hWnd);

  static Apollo::WindowHandle GetToplevelWindow(Apollo::KeyValueList& kvSignature);

  String sToplevelClass_;
};

#endif // !defined(Win32Window_h_INCLUDED)
