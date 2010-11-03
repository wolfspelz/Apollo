// ============================================================================
//
// Apollo
//
// ============================================================================

#include "test.h"
#include "ApLog.h"
#include "Apcontainer.h"
#include "MsgUnitTest.h"
#include "MsgVpView.h"
#include "MsgGalileo.h"
#include "MsgTimer.h"

#if defined(AP_TEST_Arena)

class Test_Participant
{
public:
  String sNickname;

  Buffer sbAvatar;
  String sAvatarMimetype;
  String sAvatarSource;
  String sOnlineStatus;
  String sMessage;
  String sPosition;
  String sCondition;
  String sProfileUrl;
  List slChats;
};

typedef ApHandlePointerTree<Test_Participant*> Test_ParticipantList;
typedef ApHandlePointerTreeIterator<Test_Participant*> Test_ParticipantListIterator;
typedef ApHandlePointerTreeNode<Test_Participant*> Test_ParticipantListNode;

class ActionList;

class Action: public Elem
{
public:
  Action()
    :nContinueMSec_(100)
  {}
  virtual void Execute(ActionList* pList) {}
  int nContinueMSec_;
};

class NopAction: public Action
{
public:
  void Execute(ActionList* pList);
};

class ActionList: public ListT<Action, Elem>
{
public:
  ActionList(const char* szName)
    :ListT<Action, Elem>(szName)
    ,pCurrent_(0)
  {}
  virtual void Begin();
  virtual void End();
  void DoCurrent();
  static void On_Timer_Event(Msg_Timer_Event* pMsg);
  String Result() { return sError_; }
  Action* pCurrent_;
  ApHandle hTimer_;
  String sError_;
};

void NopAction::Execute(ActionList* pList) {}

void ActionList::Begin()
{
  pCurrent_ = First();
  { Msg_Timer_Event msg; msg.Hook(MODULE_NAME, (ApCallback) On_Timer_Event, this, ApCallbackPosEarly); }
  DoCurrent();
}

void ActionList::End()
{
  { Msg_Timer_Event msg; msg.UnHook(MODULE_NAME, (ApCallback) On_Timer_Event, this); }

  { Msg_UnitTest_Complete msg; msg.sName = getName(); msg.bSuccess = sError_.empty(); msg.sMessage = sError_; msg.Send(); }

  delete this;
}

void ActionList::DoCurrent()
{
  pCurrent_->Execute(this);
  int nDelayMSec = pCurrent_->nContinueMSec_;
  
  pCurrent_ = Next(pCurrent_);
  if (pCurrent_) {
    if (nDelayMSec == 0) {
      // Do nothing, wait for someone else to continue on the queue
    } else {
      int nSec = 0;
      int nMSec = nDelayMSec;
      if (nMSec > 1000) {
        nSec = nMSec % 1000;
        nMSec = nDelayMSec - nSec * 1000;
      }
      hTimer_ = Apollo::startTimeout(nSec, nMSec * 1000);
    }
  } else {
    End();
  }
}

void ActionList::On_Timer_Event(Msg_Timer_Event* pMsg)
{
  if (pMsg->Ref()) {
    ActionList* pList = (ActionList*) pMsg->Ref();
    pList->DoCurrent();
  }
}

// ------------------------------------------------------

class Test_InChangeOut: public ActionList
{
public:
  Test_InChangeOut()
    :ActionList("Test_InChangeOut")
    ,bReplayChat1_(0)
    ,bReplayChat2_(0)
  {}

  void Begin();
  void End();

  ApHandle hContext_;
  ApHandle hLocation1_;
  ApHandle hLocation2_;
  int nLeft_;
  int nBottom_;
  int nWidth_;
  int nHeight_;

  int bReplayChat1_;
  int bReplayChat2_;

  Test_ParticipantList lParticipants_;
};

// ------------------------------------------------------

static void Test_Arena_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Arena_UnitTest_TokenEnd", "Finished Test/Arena"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

// ------------------------------------------------------

static void Test_VpView_GetParticipants(Msg_VpView_GetParticipants* pMsg)
{
  Test_InChangeOut* t = (Test_InChangeOut*) pMsg->Ref();
  Test_ParticipantListIterator iter(t->lParticipants_);
  for (Test_ParticipantListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    pMsg->vlParticipants.add(pNode->Key());
  }
}

static void Test_VpView_SubscribeParticipantDetail(Msg_VpView_SubscribeParticipantDetail* pMsg)
{
  Test_InChangeOut* t = (Test_InChangeOut*) pMsg->Ref();
  Test_Participant* p = 0;
  t->lParticipants_.Get(pMsg->hParticipant, p);

  if (p) {
    String sKey = pMsg->sKey;
    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Nickname) {
      pMsg->bAvailable = 1;
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
      pMsg->bAvailable = 1;
    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      pMsg->bAvailable = 1;
    } else if (sKey == Msg_VpView_ParticipantDetail_Message) {
      pMsg->bAvailable = 1;
    } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
      pMsg->bAvailable = 1;
    } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {
      pMsg->bAvailable = 1;
    } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {
      pMsg->bAvailable = 1;
    }
  }
}

static void Test_VpView_GetParticipantDetailString(Msg_VpView_GetParticipantDetailString* pMsg)
{
  Test_InChangeOut* t = (Test_InChangeOut*) pMsg->Ref();
  Test_Participant* p = 0;
  t->lParticipants_.Get(pMsg->hParticipant, p);

  if (p) {
    String sKey = pMsg->sKey;
    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Nickname) {
      pMsg->sValue = p->sNickname;
    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      pMsg->sValue = p->sOnlineStatus;
    } else if (sKey == Msg_VpView_ParticipantDetail_Message) {
      pMsg->sValue = p->sMessage;
    } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
      pMsg->sValue = p->sPosition;
    } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {
      pMsg->sValue = p->sCondition;
    } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {
      pMsg->sValue = p->sProfileUrl;
    }
  }
}

static void Test_VpView_GetParticipantDetailData(Msg_VpView_GetParticipantDetailData* pMsg)
{
  Test_InChangeOut* t = (Test_InChangeOut*) pMsg->Ref();
  Test_Participant* p = 0;
  t->lParticipants_.Get(pMsg->hParticipant, p);

  if (p) {
    String sKey = pMsg->sKey;
    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
      pMsg->sbData = p->sbAvatar;
      pMsg->sMimeType = p->sAvatarMimetype;
      pMsg->sSource = p->sAvatarSource;
    }
  }
}

static void Test_VpView_ReplayLocationPublicChat(Msg_VpView_ReplayLocationPublicChat* pMsg)
{
  Test_InChangeOut* t = (Test_InChangeOut*) pMsg->Ref();

  Apollo::TimeValue tNow = Apollo::getNow();

  Test_ParticipantListIterator iter(t->lParticipants_);
  for (Test_ParticipantListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    for (Elem* e = 0; (e = pNode->Value()->slChats.Next(e)); ) {
      Msg_VpView_LocationPublicChat msg;

      msg.hLocation = pMsg->hLocation;
      msg.hParticipant = pNode->Key();
      msg.hChat = Apollo::newHandle();
      msg.sNickname = pNode->Value()->sNickname;
      msg.sText = e->getName(); // Text

      Apollo::TimeValue tAge(e->getInt(), 0); // Age in sec
      Apollo::TimeValue tThen = tNow - tAge;
      msg.nSec = tThen.Sec();
      msg.nMicroSec = tThen.MicroSec();

      msg.Send();
    }
  }

  // todo: remove me
  //Test_Arena_InChangeOut_End();
}

static void Test_Galileo_IsAnimationDataInStorage(Msg_Galileo_IsAnimationDataInStorage* pMsg)
{
  Test_InChangeOut* t = (Test_InChangeOut*) pMsg->Ref();

  pMsg->bAvailable = 1;

  pMsg->apStatus = ApMessage::Ok;
}

static void Test_Galileo_LoadAnimationDataFromStorage(Msg_Galileo_LoadAnimationDataFromStorage* pMsg)
{
  Test_InChangeOut* t = (Test_InChangeOut*) pMsg->Ref();

  String sFile = String::filenameFile(pMsg->sUrl);
  Apollo::loadFile(Apollo::getAppResourcePath() + "test/tassadar/" + sFile, pMsg->sbData);
  pMsg->sMimeType = "image/gif";

  pMsg->apStatus = ApMessage::Ok;
}

static void Test_VpView_GetLocationContexts(Msg_VpView_GetLocationContexts* pMsg)
{
}

static void Test_VpView_SubscribeContextDetail(Msg_VpView_SubscribeContextDetail* pMsg)
{
}

static void Test_VpView_SubscribeLocationDetail(Msg_VpView_SubscribeLocationDetail* pMsg)
{
}

static void Test_VpView_GetContextDetail(Msg_VpView_GetContextDetail* pMsg)
{
}

static void Test_VpView_GetLocationDetail(Msg_VpView_GetLocationDetail* pMsg)
{
}

// ------------------------------------------------------

class Test_InChangeOut_CreateContextAction: public Action
{
public:
  void Execute(ActionList* pList)
  {
    {
      Msg_VpView_ContextCreated msg;
      msg.hContext = ((Test_InChangeOut*) pList)->hContext_;
      msg.Send();
    }

    {
      Msg_VpView_ContextVisibility msg;
      msg.hContext = ((Test_InChangeOut*) pList)->hContext_;
      msg.bVisible = 0;
      msg.Send();
    }

    {
      Msg_VpView_ContextPosition msg;
      msg.hContext = ((Test_InChangeOut*) pList)->hContext_;
      msg.nX = ((Test_InChangeOut*) pList)->nLeft_;
      msg.nY = ((Test_InChangeOut*) pList)->nBottom_;
      msg.Send();
    }

    {
      Msg_VpView_ContextSize msg;
      msg.hContext = ((Test_InChangeOut*) pList)->hContext_;
      msg.nWidth = ((Test_InChangeOut*) pList)->nWidth_;
      msg.nHeight = ((Test_InChangeOut*) pList)->nHeight_;
      msg.Send();
    }

    {
      Msg_VpView_ContextVisibility msg;
      msg.hContext = ((Test_InChangeOut*) pList)->hContext_;
      msg.bVisible = 1;
      msg.Send();
    }
  }
};

class Test_InChangeOut_ContextLocationAssigned1: public Action
{
public:
  void Execute(ActionList* pList)
  {
    Msg_VpView_ContextLocationAssigned msg;
    msg.hContext = ((Test_InChangeOut*) pList)->hContext_;
    msg.hLocation = ((Test_InChangeOut*) pList)->hLocation1_;
    msg.Send();
  }
};

class Test_InChangeOut_EnterLocationRequested1: public Action
{
public:
  void Execute(ActionList* pList)
  {
    Msg_VpView_EnterLocationRequested msg;
    msg.hLocation = ((Test_InChangeOut*) pList)->hLocation1_;
    msg.Send();
  }
};

class Test_InChangeOut_LocationContextsChanged1: public Action
{
public:
  void Execute(ActionList* pList)
  {
    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = ((Test_InChangeOut*) pList)->hLocation1_;
    msg.Send();
  }
};

// VpView_GetLocationContexts
// VpView_SubscribeContextDetail
// VpView_SubscribeLocationDetail
// VpView_GetContextDetail
// VpView_GetLocationDetail

class Test_InChangeOut_EnterLocationBegin1: public Action
{
public:
  void Execute(ActionList* pList)
  {
    Msg_VpView_EnterLocationBegin msg;
    msg.hLocation = ((Test_InChangeOut*) pList)->hLocation1_;
    msg.Send();
  }
};

class Test_InChangeOut_ParticipantsChanged1: public Action
{
public:
  void Execute(ActionList* pList)
  {
    Msg_VpView_ParticipantsChanged msg;
    msg.hLocation = ((Test_InChangeOut*) pList)->hLocation1_;
    msg.nCount = ((Test_InChangeOut*) pList)->lParticipants_.Count();
    msg.Send();
  }
};

// VpView_GetParticipants
// VpView_SubscribeParticipantDetail
// VpView_GetParticipantDetailString
// VpView_GetParticipantDetailData
// Galileo_IsAnimationDataInStorage
// Galileo_LoadAnimationDataFromStorage

class Test_InChangeOut_EnterLocationComplete1: public Action
{
public:
  void Execute(ActionList* pList)
  {
    Msg_VpView_EnterLocationComplete msg;
    msg.hLocation = ((Test_InChangeOut*) pList)->hLocation1_;
    msg.Send();
  }
};

class Test_InChangeOut_Wait: public Action
{
public:
  void Execute(ActionList* pList)
  {
    nContinueMSec_ = 0;
  }
};

// VpView_ReplayLocationPublicChat
//
//  // CHANGE: Navigate
//  {
//    Msg_VpView_ContextVisibility msg;
//    msg.hContext = hContext_;
//    msg.bVisible = 1;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_LeaveLocationRequested msg;
//    msg.hLocation = hLocation1_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_LocationContextsChanged msg;
//    msg.hLocation = hLocation1_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_ContextLocationUnassigned msg;
//    msg.hContext = hContext_;
//    msg.hLocation = hLocation1_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_ContextLocationAssigned msg;
//    msg.hContext = hContext_;
//    msg.hLocation = hLocation2_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_EnterLocationRequested msg;
//    msg.hLocation = hLocation2_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_LocationContextsChanged msg;
//    msg.hLocation = hLocation2_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_LeaveLocationBegin msg;
//    msg.hLocation = hLocation1_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_ParticipantsChanged msg;
//    msg.hLocation = hLocation1_;
//    msg.nCount = 0;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_LeaveLocationComplete msg;
//    msg.hLocation = hLocation1_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_EnterLocationBegin msg;
//    msg.hLocation = hLocation2_;
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_ParticipantsChanged msg;
//    msg.hLocation = hLocation2_;
//    msg.nCount = lParticipants_.Count();
//    msg.Send();
//  }
//
//  {
//    Msg_VpView_EnterLocationComplete msg;
//    msg.hLocation = hLocation2_;
//    msg.Send();
//  }
//
//  /*
//  // OUT: CloseContext
//  {
//    Msg_VpView_LeaveLocationRequested msg;
//  }
//
//  {
//    Msg_VpView_LocationContextsChanged msg;
//  }
//
//  {
//    Msg_VpView_ContextLocationUnassigned msg;
//  }
//
//  {
//    Msg_VpView_ContextDestroyed msg;
//  }
//
//  {
//    Msg_VpView_LeaveLocationBegin msg;
//  }
//
//  {
//    Msg_VpView_ParticipantsChanged msg;
//  }
//
//  {
//    Msg_VpView_LeaveLocationComplete msg;
//  }
//  */
//}

void Test_InChangeOut::Begin()
{
  hContext_ = Apollo::newHandle();
  hLocation1_ = Apollo::newHandle();
  hLocation2_ = Apollo::newHandle();
  nLeft_ = 100;
  nBottom_ = 400;
  nWidth_ = 600;
  nHeight_ = 200;

  {
    Test_Participant* p = new Test_Participant();
    if (p) {
      p->sNickname = "Tassadar";
      Apollo::loadFile(Apollo::getAppResourcePath() + "test/tassadar/" + "config.xml", p->sbAvatar);
      p->sAvatarMimetype = "avatar/gif";
      p->sAvatarSource = "IdentityItemUrl=http://ydentiti.org/test/Tassadar/config.xml";
      p->sOnlineStatus = "";
      p->sMessage = "Hallo";
      p->sPosition = "x=234\ny=0\nz=0";
      p->sCondition;
      p->sProfileUrl;
      p->slChats.AddLast("2 Hello World Hello World Hello World Hello World", 2);
      p->slChats.AddLast("10 Hello World Hello World Hello World Hello World", 10);
      lParticipants_.Set(Apollo::newHandle(), p);
    }
  }

  { Msg_VpView_GetParticipants msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipants, this, ApCallbackPosEarly); }
  { Msg_VpView_SubscribeParticipantDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeParticipantDetail, this, ApCallbackPosEarly); }
  { Msg_VpView_GetParticipantDetailString msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailString, this, ApCallbackPosEarly); }
  { Msg_VpView_GetParticipantDetailData msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailData, this, ApCallbackPosEarly); }
  { Msg_VpView_ReplayLocationPublicChat msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_ReplayLocationPublicChat, this, ApCallbackPosEarly); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_IsAnimationDataInStorage, this, ApCallbackPosEarly); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_LoadAnimationDataFromStorage, this, ApCallbackPosEarly); }
  { Msg_VpView_GetLocationContexts msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetLocationContexts, this, ApCallbackPosEarly); }
  { Msg_VpView_SubscribeContextDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeContextDetail, this, ApCallbackPosEarly); }
  { Msg_VpView_SubscribeLocationDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeLocationDetail, this, ApCallbackPosEarly); }
  { Msg_VpView_GetContextDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetContextDetail, this, ApCallbackPosEarly); }
  { Msg_VpView_GetLocationDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetLocationDetail, this, ApCallbackPosEarly); }

  // IN: Open Context, Navigate
  AddLast(new Test_InChangeOut_CreateContextAction());
  AddLast(new Test_InChangeOut_ContextLocationAssigned1());
  AddLast(new Test_InChangeOut_EnterLocationRequested1());
  AddLast(new Test_InChangeOut_LocationContextsChanged1());
  AddLast(new Test_InChangeOut_EnterLocationBegin1());
  AddLast(new Test_InChangeOut_ParticipantsChanged1());
  AddLast(new Test_InChangeOut_EnterLocationComplete1());
  AddLast(new Test_InChangeOut_Wait());

  ActionList::Begin();
}

void Test_InChangeOut::End()
{
  { Msg_VpView_GetParticipants msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipants, this); }
  { Msg_VpView_SubscribeParticipantDetail msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeParticipantDetail, this); }
  { Msg_VpView_GetParticipantDetailString msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailString, this); }
  { Msg_VpView_GetParticipantDetailData msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailData, this); }
  { Msg_VpView_ReplayLocationPublicChat msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_ReplayLocationPublicChat, this); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_IsAnimationDataInStorage, this); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_LoadAnimationDataFromStorage, this); }
  { Msg_VpView_GetLocationContexts msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetLocationContexts, this); }
  { Msg_VpView_SubscribeContextDetail msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeContextDetail, this); }
  { Msg_VpView_SubscribeLocationDetail msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeLocationDetail, this); }
  { Msg_VpView_GetContextDetail msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetContextDetail, this); }
  { Msg_VpView_GetLocationDetail msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetLocationDetail, this); }

  ActionList::End();

  Test_Arena_UnitTest_TokenEnd();
}

// ------------------------------------------------------

Test_InChangeOut* gTest_InChangeOut = 0;

static void Test_Arena_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Arena_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Arena_UnitTest_Token", "Starting Test/Arena"));
  int bTokenEndNow = 0;

  gTest_InChangeOut->Begin();

  if (bTokenEndNow) { Test_Arena_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#endif // AP_TEST_Arena

void Test_Arena_Register()
{
#if defined(AP_TEST_Arena)
  if (Apollo::isLoadedModule("Arena")) {

    { gTest_InChangeOut = new Test_InChangeOut(); Msg_UnitTest_Register msg; msg.sName = gTest_InChangeOut->getName(); msg.Send(); }
  
    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Arena_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
#endif // AP_TEST_Arena
}

void Test_Arena_Begin()
{
#if defined(AP_TEST_Arena)
  if (Apollo::isLoadedModule("Arena")) {
  }
#endif // AP_TEST_Arena
}

void Test_Arena_End()
{
#if defined(AP_TEST_Arena)
  if (Apollo::isLoadedModule("Arena")) {
  }
#endif // AP_TEST_Arena
}
