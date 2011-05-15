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

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class DialogModuleTester;
#endif

protected:
  Dialog* NewDialog(const ApHandle& hDialog, int nLeft, int nTop, int nWidth, int nHeight, int bVisible, const String& sContentUrl);
  void DeleteDialog(const ApHandle& hDialog);
  Dialog* FindDialog(const ApHandle& hDialog);

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

  static String Test1();
};

#endif

#endif // DialogModule_H_INCLUDED
