// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebViewModule_H_INCLUDED)
#define WebViewModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
//#include "MsgWebView.h"

class WebViewModule
{
public:
  WebViewModule()
    :nTheAnswer_(42)
    {}

  int init();
  void exit();

  //void On_WebView_Get(Msg_WebView_Get* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  int nTheAnswer_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<WebViewModule> WebViewModuleInstance;

#endif // WebViewModule_H_INCLUDED
