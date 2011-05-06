// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebViewModule_H_INCLUDED)
#define WebViewModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgSystem.h"
#include "MsgWebView.h"
#include "WebView.h"

typedef ApHandlePointerTree<WebView*> WebViewList;
typedef ApHandlePointerTreeNode<WebView*> WebViewListNode;
typedef ApHandlePointerTreeIterator<WebView*> WebViewListIterator;

class WebViewModule
{
public:
  WebViewModule()
    {}

  int Init();
  void Exit();

  void On_WebView_Create(Msg_WebView_Create* pMsg);
  void On_WebView_Destroy(Msg_WebView_Destroy* pMsg);
  void On_WebView_Position(Msg_WebView_Position* pMsg);
  void On_WebView_Visibility(Msg_WebView_Visibility* pMsg);
  void On_System_3SecTimer(Msg_System_3SecTimer* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  WebView* CreateWebView(const ApHandle& hWebView, const String& sHtml, const String& sBase);
  void DeleteWebView(const ApHandle& hWebView);
  WebView* FindWebView(const ApHandle& hWebView);

public:
  WebViewList webviews_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<WebViewModule> WebViewModuleInstance;

#endif // WebViewModule_H_INCLUDED
