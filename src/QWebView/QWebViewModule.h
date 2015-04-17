// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(QWebViewModule_H_INCLUDED)
#define QWebViewModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "SrpcGateHelper.h"
#include "View.h"

typedef ApHandlePointerTree<View*> ViewList;
typedef ApHandlePointerTreeNode<View*> ViewListNode;
typedef ApHandlePointerTreeIterator<View*> ViewListIterator;

class QWebViewModule
{
public:
  QWebViewModule()
    :bWebKitUsed_(0)
    {}

  int Init();
  void Exit();

  void On_QWebView_Create(Msg_WebView_Create* pMsg);
  void On_QWebView_Destroy(Msg_WebView_Destroy* pMsg);
  void On_QWebView_Position(Msg_WebView_Position* pMsg);
  void On_QWebView_Visibility(Msg_WebView_Visibility* pMsg);
  void On_QWebView_SetWindowFlags(Msg_WebView_SetWindowFlags* pMsg);
  void On_QWebView_LoadHtml(Msg_WebView_LoadHtml* pMsg);
  void On_QWebView_Load(Msg_WebView_Load* pMsg);
  void On_QWebView_Reload(Msg_WebView_Reload* pMsg);
  void On_QWebView_CallScriptFunction(Msg_WebView_CallScriptFunction* pMsg);
  void On_QWebView_GetElementValue(Msg_WebView_GetElementValue* pMsg);
  void On_QWebView_ViewCall(Msg_WebView_ViewCall* pMsg);
  void On_QWebView_SetScriptAccessPolicy(Msg_WebView_SetScriptAccessPolicy* pMsg);
  void On_QWebView_SetNavigationPolicy(Msg_WebView_SetNavigationPolicy* pMsg);
  void On_QWebView_MoveBy(Msg_WebView_MoveBy* pMsg);
  void On_QWebView_SizeBy(Msg_WebView_SizeBy* pMsg);
  void On_QWebView_MakeFrontWindow(Msg_WebView_MakeFrontWindow* pMsg);
  void On_QWebView_MouseCapture(Msg_WebView_MouseCapture* pMsg);
  void On_QWebView_MouseRelease(Msg_WebView_MouseRelease* pMsg);
  void On_QWebView_GetPosition(Msg_WebView_GetPosition* pMsg);
  void On_QWebView_GetVisibility(Msg_WebView_GetVisibility* pMsg);
  #if defined(WIN32)
  void On_QWebView_GetWin32Window(Msg_WebView_GetWin32Window* pMsg);
  #endif // defined(WIN32)
  void On_QWebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg);
  void On_QWebView_Event_LoadError(Msg_WebView_Event_LoadError* pMsg);
  void On_System_3SecTimer(Msg_System_3SecTimer* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

public:
  View* CreateView(const ApHandle& hView, int nLeft, int nTop, int nWidth, int nHeight);
  void DeleteView(const ApHandle& hView);
  View* FindView(const ApHandle& hView);

public:
  ViewList views_;
  int bWebKitUsed_;

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;
  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<QWebViewModule> QWebViewModuleInstance;

#endif // QWebViewModule_H_INCLUDED
