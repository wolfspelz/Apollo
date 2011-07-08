// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

ChatWindow* ChatModule::NewChat(const ApHandle& hChat)
{
  ChatWindow* pChat = new ChatWindow(this, hChat);
  if (pChat) {
    try {
      chats_.Set(hChat, pChat);
    } catch (ApException& ex) {
      delete pChat;
      pChat = 0;
      throw ex;
    }
  }

  return pChat;
}

void ChatModule::DeleteChat(const ApHandle& hChat)
{
  ChatWindow* pChat = FindChat(hChat);
  if (pChat) {
    chats_.Unset(hChat);
    delete pChat;
    pChat = 0;
  }
}

ChatWindow* ChatModule::FindChat(const ApHandle& hChat)
{
  ChatWindow* pChat = 0;
  chats_.Get(hChat, pChat);
  return pChat;
}

ChatWindow* ChatModule::GetChat(const ApHandle& hChat)
{
  ChatWindow* pChat = FindChat(hChat);  
  if (pChat == 0) { throw ApException(LOG_CONTEXT, "no ChatWindow=" ApHandleFormat "", ApHandlePrintf(hChat)); }
  return pChat;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(ChatModule, ChatWindow_OpenForLocation)
{
  if (chats_.Find(pMsg->hChat) != 0) { throw ApException(LOG_CONTEXT, "ChatWindow=" ApHandleFormat " already exists", ApHandlePrintf(pMsg->hChat)); }

  ChatWindow* pChat = NewChat(pMsg->hChat);
  if (pChat == 0) { throw ApException(LOG_CONTEXT, "NewChat failed"); }

  pChat->Open();
  pChat->AttachToLocation(pMsg->hLocation);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ChatModule, ChatWindow_Close)
{
  ChatWindow* pChat = GetChat(pMsg->hChat);
  pChat->Close();
  pMsg->apStatus = ApMessage::Ok;
}

//---------------------------

AP_MSG_HANDLER_METHOD(ChatModule, Dialog_OnOpened)
{
  ChatWindow* pChat = FindChat(pMsg->hDialog);
  if (pChat != 0) {
    pChat->OnLoaded();
  }
}

AP_MSG_HANDLER_METHOD(ChatModule, Dialog_OnClosed)
{
  ChatWindow* pChat = FindChat(pMsg->hDialog);
  if (pChat != 0) {
    pChat->OnUnload();
    DeleteChat(pChat->apHandle());
  }
}

//---------------------------

AP_MSG_HANDLER_METHOD(ChatModule, WebView_ModuleCall)
{
  ChatWindow* pChat = FindChat(pMsg->hView);
  if (pChat) {
    pChat->OnModuleCall(pMsg->srpc, pMsg->response);
    pMsg->apStatus = ApMessage::Ok;
  }
}

//---------------------------

AP_MSG_HANDLER_METHOD(ChatModule, VpView_ParticipantAdded)
{
  for (ChatListNode* pChatNode = 0; (pChatNode = chats_.Next(pChatNode)) != 0; ) {
    ChatWindow* pChat = pChatNode->Value();
    if (pChat) {
      if (pChat->GetLocation() == pMsg->hLocation) {
        pChat->OnParticipantAdded(pMsg->hParticipant, pMsg->bSelf);
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(ChatModule, VpView_ParticipantRemoved)
{
  for (ChatListNode* pChatNode = 0; (pChatNode = chats_.Next(pChatNode)) != 0; ) {
    ChatWindow* pChat = pChatNode->Value();
    if (pChat) {
      if (pChat->GetLocation() == pMsg->hLocation) {
        pChat->OnParticipantRemoved(pMsg->hParticipant);
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(ChatModule, VpView_LocationPublicChat)
{
  for (ChatListNode* pChatNode = 0; (pChatNode = chats_.Next(pChatNode)) != 0; ) {
    ChatWindow* pChat = pChatNode->Value();
    if (pChat) {
      if (pChat->GetLocation() == pMsg->hLocation) {
        Apollo::TimeValue tv(pMsg->nSec, pMsg->nMicroSec);
        pChat->OnReceivePublicChat(pMsg->hParticipant, pMsg->hChat, pMsg->sNickname, pMsg->sText, tv);
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(ChatModule, VpView_ParticipantDetailsChanged)
{
  for (ChatListNode* pChatNode = 0; (pChatNode = chats_.Next(pChatNode)) != 0; ) {
    ChatWindow* pChat = pChatNode->Value();
    if (pChat) {
      if (pChat->GetLocation() == pMsg->hLocation) {
        pChat->OnParticipantDetailsChanged(pMsg->hParticipant, pMsg->vlKeys);
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(ChatModule, VpView_LocationDetailsChanged)
{
  for (ChatListNode* pChatNode = 0; (pChatNode = chats_.Next(pChatNode)) != 0; ) {
    ChatWindow* pChat = pChatNode->Value();
    if (pChat) {
      if (pChat->GetLocation() == pMsg->hLocation) {
        pChat->OnLocationDetailsChanged(pMsg->vlKeys);
      }
    }
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(ChatModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ChatWindow", 0)) {
    ChatModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(ChatModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ChatWindow", 0)) {
    ChatModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(ChatModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/ChatWindow", 0)) {
    ChatModuleTester::End();
  }
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

void SrpcGate_ChatWindow_OpenForLocation(ApSRPCMessage* pMsg)
{
  Msg_ChatWindow_OpenForLocation msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_ChatWindow_Close(ApSRPCMessage* pMsg)
{
  Msg_ChatWindow_Close msg;
  msg.hChat = pMsg->srpc.getHandle("hChat");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

//----------------------------------------------------------

int ChatModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, ChatWindow_OpenForLocation, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, ChatWindow_Close, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, Dialog_OnOpened, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, Dialog_OnClosed, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, WebView_ModuleCall, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, VpView_ParticipantAdded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, VpView_ParticipantRemoved, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, VpView_LocationPublicChat, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, VpView_LocationDetailsChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ChatModule, VpView_ParticipantDetailsChanged, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("ChatWindow_OpenForLocation", SrpcGate_ChatWindow_OpenForLocation);
  srpcGateRegistry_.add("ChatWindow_Close", SrpcGate_ChatWindow_Close);

  AP_UNITTEST_HOOK(ChatModule, this);

  return ok;
}

void ChatModule::Exit()
{
  srpcGateRegistry_.finish();
  AP_UNITTEST_UNHOOK(ChatModule, this);
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

#if defined(AP_TEST)

void ChatModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(ChatModuleTester::Test1);
}

void ChatModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(ChatModuleTester::Test1);
}

void ChatModuleTester::End()
{
}

//----------------------------

String ChatModuleTester::Test1()
{
  String s;

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
