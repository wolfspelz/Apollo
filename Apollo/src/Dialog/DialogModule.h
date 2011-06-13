// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(DialogModule_H_INCLUDED)
#define DialogModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgWebView.h"
#include "MsgDialog.h"
#include "SrpcGateHelper.h"
#include "Dialog.h"

typedef ApHandlePointerTree<Dialog*> DialogList;
typedef ApHandlePointerTreeNode<Dialog*> DialogListNode;
typedef ApHandlePointerTreeIterator<Dialog*> DialogListIterator;

class DialogModule
{
public:
  DialogModule()
  {}

  int Init();
  void Exit();

  void On_Dialog_Create(Msg_Dialog_Create* pMsg);
  void On_Dialog_Destroy(Msg_Dialog_Destroy* pMsg);
  void On_Dialog_GetView(Msg_Dialog_GetView* pMsg);
  void On_Dialog_SetCaption(Msg_Dialog_SetCaption* pMsg);
  void On_Dialog_SetIcon(Msg_Dialog_SetIcon* pMsg);
  void On_Dialog_CallScriptFunction(Msg_Dialog_CallScriptFunction* pMsg);

  void On_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg);
  void On_WebView_Event_ReceivedFocus(Msg_WebView_Event_ReceivedFocus* pMsg);
  void On_WebView_Event_LostFocus(Msg_WebView_Event_LostFocus* pMsg);
  void On_WebView_Event_DocumentUnload(Msg_WebView_Event_DocumentUnload* pMsg);
  void On_Dialog_ContentLoaded(Msg_Dialog_ContentLoaded* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class DialogModuleTester;
#endif

protected:
  Dialog* NewDialog(const ApHandle& hDialog, int nLeft, int nTop, int nWidth, int nHeight, int bVisible, const String& sCaption, const String& sIconUrl, const String& sContentUrl);
  void DeleteDialog(const ApHandle& hDialog);
  Dialog* FindDialog(const ApHandle& hDialog); // return 0 if !found
  Dialog* GetDialog(const ApHandle& hDialog) throw (ApException); // ApException if !found
  Dialog* FindDialogByView(const ApHandle& hView);

protected:
  DialogList dialogs_;

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;
  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<DialogModule> DialogModuleInstance;

//----------------------------------------------------------

#if defined(AP_TEST)

class DialogModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String CreateWaitCloseByContent();
  static String CallContent();
  static String ContentLoadedFromHtml();
  static String ExternalUrl();
  static String SetCaption();
};

#endif

#endif // DialogModule_H_INCLUDED
