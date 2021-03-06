// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(BrowserInfoModule_H_INCLUDED)
#define BrowserInfoModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
#include "MsgTimer.h"
#include "MsgVp.h"
#include "MsgVpView.h"
#include "WindowHandle.h"
#include "Browser.h"

#if defined(AP_TEST)
class BrowserInfoModuleTester
{
public:
  static String GetFirefoxToplevelWindow();
  static String GetFirefoxToplevelWindowWithCoordinates();
  static String DontGetFirefoxToplevelWindowWithWrongCoordinates();
};
#endif

typedef PointerTree<Apollo::WindowHandle, Browser*, LessThan<Apollo::WindowHandle> > BrowserList;
typedef PointerTreeNode<Apollo::WindowHandle, Browser*> BrowserListNode;

class BrowserInfoModule
{
public:
  BrowserInfoModule()
    :bInLocalCall_(0)
    ,nCntTimerUser_(0)
  {}

  int Init();
  void Exit();

  void On_BrowserInfo_BeginTrackCoordinates(Msg_BrowserInfo_BeginTrackCoordinates* pMsg);
  void On_BrowserInfo_EndTrackCoordinates(Msg_BrowserInfo_EndTrackCoordinates* pMsg);

  void On_VpView_ContextVisibility(Msg_VpView_ContextVisibility* pMsg);
  void On_VpView_GetContextVisibility(Msg_VpView_GetContextVisibility* pMsg);
  void On_VpView_GetContextPosition(Msg_VpView_GetContextPosition* pMsg);
  void On_VpView_GetContextSize(Msg_VpView_GetContextSize* pMsg);

  void On_Timer_Event(Msg_Timer_Event* pMsg);

  void InLocalCall(int bState) { bInLocalCall_ = bState; }

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

protected:
  Browser* FindBrowserByContext(const ApHandle& hContext);
  void StartTimer();
  void StopTimer();

protected:
  BrowserList browsers_;
  int bInLocalCall_;

  ApHandle hTimer_;
  int nCntTimerUser_;

  AP_MSG_REGISTRY_DECLARE;

#if defined(AP_TEST)
  friend class BrowserInfoModuleTester;
#endif
};

class LocalCallGuard
{
public:
  LocalCallGuard();
  ~LocalCallGuard();
};

typedef ApModuleSingleton<BrowserInfoModule> BrowserInfoModuleInstance;

#endif // BrowserInfoModule_H_INCLUDED
