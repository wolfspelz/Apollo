// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ChatModule_H_INCLUDED)
#define ChatModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "SrpcGateHelper.h"
#include "ChatWindow.h"

typedef ApHandlePointerTree<ChatWindow*> ChatList;
typedef ApHandlePointerTreeNode<ChatWindow*> ChatListNode;
typedef ApHandlePointerTreeIterator<ChatWindow*> ChatListIterator;

class ChatModule
{
public:
  ChatModule()
    :nTheAnswer_(42)
    {}

  int Init();
  void Exit();

  void On_ChatWindow_OpenForLocation(Msg_ChatWindow_OpenForLocation* pMsg);
  void On_ChatWindow_Close(Msg_ChatWindow_Close* pMsg);
  void On_Dialog_OnOpened(Msg_Dialog_OnOpened* pMsg);
  void On_Dialog_OnClosed(Msg_Dialog_OnClosed* pMsg);
  void On_WebView_ModuleCall(Msg_WebView_ModuleCall* pMsg);
  void On_VpView_ParticipantAdded(Msg_VpView_ParticipantAdded* pMsg);
  void On_VpView_ParticipantRemoved(Msg_VpView_ParticipantRemoved* pMsg);
  void On_VpView_LocationPublicChat(Msg_VpView_LocationPublicChat* pMsg);
  void On_VpView_LocationDetailsChanged(Msg_VpView_LocationDetailsChanged* pMsg);
  void On_VpView_ParticipantDetailsChanged(Msg_VpView_ParticipantDetailsChanged* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
  friend class ChatModuleTester;
#endif

protected:
  ChatWindow* NewChat(const ApHandle& hChat);
  void DeleteChat(const ApHandle& hChat);
  ChatWindow* FindChat(const ApHandle& hChat); // return 0 if !found
  ChatWindow* GetChat(const ApHandle& hChat) throw (ApException); // ApException if !found

protected:
  int nTheAnswer_;
  ChatList chats_;

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;
  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<ChatModule> ChatModuleInstance;

//----------------------------------------------------------

#if defined(AP_TEST)

class ChatModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String Test1();
};

#endif

#endif // ChatModule_H_INCLUDED
