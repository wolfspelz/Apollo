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
  String sAvatarUrl;
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
    :nDelayMSec_(100)
  {}
  virtual void SetList(ActionList* pList) {}
  virtual void Execute() {}
  int nDelayMSec_;
};

class NopAction: public Action
{
public:
  void Execute();
};

class ActionList: public ListT<Action, Elem>
{
public:
  ActionList(const char* szName)
    :ListT<Action, Elem>(szName)
    ,pNext_(0)
  {}
  virtual void Begin();
  virtual void End();
  void Proceed();
  static void On_Timer_Event(Msg_Timer_Event* pMsg);
  String Result() { return sError_; }
  Action* pNext_;
  ApHandle hTimer_;
  String sError_;
};

void NopAction::Execute()
{
}

void ActionList::Begin()
{
  pNext_ = First();
  { Msg_Timer_Event msg; msg.Hook(MODULE_NAME, (ApCallback) On_Timer_Event, this, ApCallbackPosEarly); }
  Proceed();
}

void ActionList::End()
{
  { Msg_Timer_Event msg; msg.UnHook(MODULE_NAME, (ApCallback) On_Timer_Event, this); }

  { Msg_UnitTest_Complete msg; msg.sName = getName(); msg.bSuccess = sError_.empty(); msg.sMessage = sError_; msg.Send(); }

  delete this;
}

void ActionList::Proceed()
{
  if (pNext_ == 0) {
    End();
  } else {

    pNext_->SetList(this);
    pNext_->Execute();
    int nDelayMSec = pNext_->nDelayMSec_;

    pNext_ = Next(pNext_);

    if (nDelayMSec == 0) {
      // Do nothing, wait for someone else to call Proceed()
    } else {
      int nSec = 0;
      int nMSec = nDelayMSec;
      if (nMSec > 1000) {
        nSec = nMSec % 1000;
        nMSec = nDelayMSec - nSec * 1000;
      }
      hTimer_ = Apollo::startTimeout(nSec, nMSec * 1000);
    }
  }
}

void ActionList::On_Timer_Event(Msg_Timer_Event* pMsg)
{
  if (pMsg->Ref()) {
    ActionList* pList = (ActionList*) pMsg->Ref();
    if (pMsg->hTimer == pList->hTimer_) {
      pList->Proceed();
    }
  }
}

// ------------------------------------------------------

class Test_InNavigateChatOut: public ActionList
{
public:
  Test_InNavigateChatOut()
    :ActionList("Test_InNavigateChatOut")
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

  int nPhase_;

  Test_ParticipantList lParticipants_;
};

class Test_InNavigateChatOut_Action: public Action
{
public:
  Test_InNavigateChatOut_Action()
    :t(0)
  {}

  void SetList(ActionList* pList) { t = (Test_InNavigateChatOut*) pList; }

  Test_InNavigateChatOut* t;
};

// ------------------------------------------------------

static void Test_Arena_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Arena_UnitTest_TokenEnd", "Finished Test/WebArena"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

// ------------------------------------------------------

static void Test_VpView_GetParticipants(Msg_VpView_GetParticipants* pMsg)
{
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();
  Test_ParticipantListIterator iter(t->lParticipants_);
  for (Test_ParticipantListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    pMsg->vlParticipants.add(pNode->Key());
  }
}

static void Test_VpView_SubscribeParticipantDetail(Msg_VpView_SubscribeParticipantDetail* pMsg)
{
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();
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
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();
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
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();
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

static void Test_VpView_GetParticipantDetailRef(Msg_VpView_GetParticipantDetailRef* pMsg)
{
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();
  Test_Participant* p = 0;
  t->lParticipants_.Get(pMsg->hParticipant, p);

  if (p) {
    String sKey = pMsg->sKey;
    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
      pMsg->sUrl = p->sAvatarUrl;
      pMsg->sMimeType = p->sAvatarMimetype;
    }
  }
}

static void Test_VpView_ReplayLocationPublicChat(Msg_VpView_ReplayLocationPublicChat* pMsg)
{
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();

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

  if (pMsg->hLocation == t->hLocation1_) {
    t->Proceed();
  }
  if (pMsg->hLocation == t->hLocation2_) {
    t->Proceed();
  }
}

static void Test_Galileo_IsAnimationDataInStorage(Msg_Galileo_IsAnimationDataInStorage* pMsg)
{
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();

  pMsg->bAvailable = 1;

  pMsg->apStatus = ApMessage::Ok;
}

static void Test_Galileo_LoadAnimationDataFromStorage(Msg_Galileo_LoadAnimationDataFromStorage* pMsg)
{
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();

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
  Test_InNavigateChatOut* t = (Test_InNavigateChatOut*) pMsg->Ref();

  String sKey = pMsg->sKey;
  String sValue;
  if (0) {
  } else if (sKey == Msg_VpView_ContextDetail_DocumentUrl) {
    if (t->nPhase_ == 0) {
    } else if (t->nPhase_ == 1) {
      sValue = "http://www.document.url/folder/1.html";
    } else if (t->nPhase_ == 2) {
      sValue = "http://www.document.url/folder/2.html";
    }

  } else if (sKey == Msg_VpView_ContextDetail_LocationUrl) {
    if (t->nPhase_ == 0) {
    } else if (t->nPhase_ == 1) {
      sValue = "xmpp:11111111111111@chat.location.url";
    } else if (t->nPhase_ == 2) {
      sValue = "xmpp:22222222222222@chat.location.url";
    }
  }

  pMsg->sValue = sValue;
  pMsg->apStatus = ApMessage::Ok;
}

static void Test_VpView_GetLocationDetail(Msg_VpView_GetLocationDetail* pMsg)
{
}

// ------------------------------------------------------

class Test_InNavigateChatOut_Wait: public Test_InNavigateChatOut_Action
{
public:
  Test_InNavigateChatOut_Wait(int nMSec)
  {
    nDelayMSec_ = nMSec;
  }
};

class Test_InNavigateChatOut_CreateAndConfigureContext: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    {
      Msg_VpView_ContextCreated msg;
      msg.hContext = t->hContext_;
      msg.Send();
    }

    {
      Msg_VpView_ContextVisibility msg;
      msg.hContext = t->hContext_;
      msg.bVisible = 0;
      msg.Send();
    }

    {
      Msg_VpView_ContextPosition msg;
      msg.hContext = t->hContext_;
      msg.nX = t->nLeft_;
      msg.nY = t->nBottom_;
      msg.Send();
    }

    {
      Msg_VpView_ContextSize msg;
      msg.hContext = t->hContext_;
      msg.nWidth = t->nWidth_;
      msg.nHeight = t->nHeight_;
      msg.Send();
    }

    {
      Msg_VpView_ContextVisibility msg;
      msg.hContext = t->hContext_;
      msg.bVisible = 1;
      msg.Send();
    }
  }
};

class Test_InNavigateChatOut_ContextLocationAssigned1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    t->nPhase_ = 1;

    Msg_VpView_ContextLocationAssigned msg;
    msg.hContext = t->hContext_;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ContextDetailsChanged_DocumentUrl: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_ContextDetailsChanged msg;
    msg.hContext = t->hContext_;
    msg.vlKeys.add(Msg_VpView_ContextDetail_DocumentUrl);
    msg.Send();
  }
};

class Test_InNavigateChatOut_ContextDetailsChanged_LocationUrl: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_ContextDetailsChanged msg;
    msg.hContext = t->hContext_;
    msg.vlKeys.add(Msg_VpView_ContextDetail_LocationUrl);
    msg.Send();
  }
};

class Test_InNavigateChatOut_EnterLocationRequested1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_EnterLocationRequested msg;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_LocationContextsChanged1a: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

// VpView_GetLocationContexts
// VpView_SubscribeContextDetail
// VpView_SubscribeLocationDetail
// VpView_GetContextDetail
// VpView_GetLocationDetail

class Test_InNavigateChatOut_EnterLocationBegin1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_EnterLocationBegin msg;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ParticipantsChanged1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_ParticipantsChanged msg;
    msg.hLocation = t->hLocation1_;
    msg.nCount = t->lParticipants_.Count();
    msg.Send();
  }
};

// VpView_GetParticipants
// VpView_SubscribeParticipantDetail
// VpView_GetParticipantDetailString
// VpView_GetParticipantDetailData
// Galileo_IsAnimationDataInStorage
// Galileo_LoadAnimationDataFromStorage

class Test_InNavigateChatOut_EnterLocationComplete1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_EnterLocationComplete msg;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

// VpView_ReplayLocationPublicChat

// CHANGE: Navigate
class Test_InNavigateChatOut_LeaveLocationRequested1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LeaveLocationRequested msg;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_LocationContextsChanged1b: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ContextLocationUnassigned1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    t->nPhase_ = 0;

    Msg_VpView_ContextLocationUnassigned msg;
    msg.hContext = t->hContext_;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ContextLocationAssigned2: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    t->nPhase_ = 2;

    Msg_VpView_ContextLocationAssigned msg;
    msg.hContext = t->hContext_;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_EnterLocationRequested2: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_EnterLocationRequested msg;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_LocationContextsChanged2a: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_VpView_LeaveLocationBegin1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LeaveLocationBegin msg;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ParticipantsChanged: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_ParticipantsChanged msg;
    msg.hLocation = t->hLocation1_;
    msg.nCount = 0;
    msg.Send();
  }
};

class Test_InNavigateChatOut_LeaveLocationComplete1: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LeaveLocationComplete msg;
    msg.hLocation = t->hLocation1_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_EnterLocationBegin2: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_EnterLocationBegin msg;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ParticipantsChanged2a: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_ParticipantsChanged msg;
    msg.hLocation = t->hLocation2_;
    msg.nCount = t->lParticipants_.Count();
    msg.Send();
  }
};

class Test_InNavigateChatOut_EnterLocationComplete2: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_EnterLocationComplete msg;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_Chat2a: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LocationPublicChat msg;
    msg.hLocation = t->hLocation2_;
    msg.hParticipant = t->lParticipants_.Next(0)->Key();
    msg.hChat = Apollo::newHandle();
    msg.sNickname = t->lParticipants_.Next(0)->Value()->sNickname;
    msg.sText = "2a Lorem ";
    msg.nSec = Apollo::TimeValue::getTime().Sec();
    msg.nMicroSec = Apollo::TimeValue::getTime().MicroSec();
    msg.Send();
  }
};

class Test_InNavigateChatOut_Chat2b: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LocationPublicChat msg;
    msg.hLocation = t->hLocation2_;
    msg.hParticipant = t->lParticipants_.Next(0)->Key();
    msg.hChat = Apollo::newHandle();
    msg.sNickname = t->lParticipants_.Next(0)->Value()->sNickname;
    msg.sText = "2b adipisicing elit, sed do eiusmod tempor incididunt ut";
    msg.nSec = Apollo::TimeValue::getTime().Sec();
    msg.nMicroSec = Apollo::TimeValue::getTime().MicroSec();
    msg.Send();
  }
};

class Test_InNavigateChatOut_Chat2c: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LocationPublicChat msg;
    msg.hLocation = t->hLocation2_;
    msg.hParticipant = t->lParticipants_.Next(0)->Key();
    msg.hChat = Apollo::newHandle();
    msg.sNickname = t->lParticipants_.Next(0)->Value()->sNickname;
    msg.sText = "2c labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
    msg.nSec = Apollo::TimeValue::getTime().Sec();
    msg.nMicroSec = Apollo::TimeValue::getTime().MicroSec();
    msg.Send();
  }
};

// OUT: CloseContext
class Test_InNavigateChatOut_LeaveLocationRequested2: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LeaveLocationRequested msg;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_LocationContextsChanged2b: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ContextLocationUnassigned2: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    t->nPhase_ = 0;

    Msg_VpView_ContextLocationUnassigned msg;
    msg.hContext = t->hContext_;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ContextDestroyed: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_ContextDestroyed msg;
    msg.hContext = t->hContext_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_LeaveLocationBegin2: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LeaveLocationBegin msg;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

class Test_InNavigateChatOut_ParticipantsChanged2b: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_ParticipantsChanged msg;
    msg.hLocation = t->hLocation2_;
    msg.nCount = 0;
    msg.Send();
  }
};

class Test_InNavigateChatOut_LeaveLocationComplete2: public Test_InNavigateChatOut_Action
{
public:
  void Execute()
  {
    Msg_VpView_LeaveLocationComplete msg;
    msg.hLocation = t->hLocation2_;
    msg.Send();
  }
};

// ------------------------------------------------------

void Test_InNavigateChatOut::Begin()
{
  hContext_ = Apollo::newHandle();
  hLocation1_ = Apollo::newHandle();
  hLocation2_ = Apollo::newHandle();
  nLeft_ = 100;
  nBottom_ = 600;
  nWidth_ = 1200;
  nHeight_ = 400;
  nPhase_ = 0;

  {
    Test_Participant* p = new Test_Participant();
    if (p) {
      p->sNickname = "Tassadar";
      Apollo::loadFile(Apollo::getAppResourcePath() + "test/tassadar/" + "config.xml", p->sbAvatar);
      p->sAvatarMimetype = "avatar/gif";
      p->sAvatarSource = "IdentityItemUrl=http://ydentiti.org/test/Tassadar/config.xml";
      p->sAvatarUrl = "http://ydentiti.org/test/Tassadar/walk-l.gif";
      p->sOnlineStatus = "";
      p->sMessage = "Hallo";
      p->sPosition = "x=333\ny=0\nz=0";
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
  { Msg_VpView_GetParticipantDetailRef msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailRef, this, ApCallbackPosEarly); }
  { Msg_VpView_ReplayLocationPublicChat msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_ReplayLocationPublicChat, this, ApCallbackPosEarly); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_IsAnimationDataInStorage, this, ApCallbackPosEarly); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_LoadAnimationDataFromStorage, this, ApCallbackPosEarly); }
  { Msg_VpView_GetLocationContexts msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetLocationContexts, this, ApCallbackPosEarly); }
  { Msg_VpView_SubscribeContextDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeContextDetail, this, ApCallbackPosEarly); }
  { Msg_VpView_SubscribeLocationDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeLocationDetail, this, ApCallbackPosEarly); }
  { Msg_VpView_GetContextDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetContextDetail, this, ApCallbackPosEarly); }
  { Msg_VpView_GetLocationDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetLocationDetail, this, ApCallbackPosEarly); }

  // IN: Open Context, Navigate
  AddLast(new Test_InNavigateChatOut_CreateAndConfigureContext());
  AddLast(new Test_InNavigateChatOut_ContextLocationAssigned1());
  AddLast(new Test_InNavigateChatOut_ContextDetailsChanged_DocumentUrl());
  AddLast(new Test_InNavigateChatOut_EnterLocationRequested1());
  AddLast(new Test_InNavigateChatOut_ContextDetailsChanged_LocationUrl());
  AddLast(new Test_InNavigateChatOut_LocationContextsChanged1a());
  AddLast(new Test_InNavigateChatOut_EnterLocationBegin1());
  AddLast(new Test_InNavigateChatOut_ParticipantsChanged1());
  AddLast(new Test_InNavigateChatOut_EnterLocationComplete1());

  AddLast(new Test_InNavigateChatOut_Wait(500));
  AddLast(new Test_InNavigateChatOut_LeaveLocationRequested1());
  AddLast(new Test_InNavigateChatOut_ContextDetailsChanged_LocationUrl());
  AddLast(new Test_InNavigateChatOut_LocationContextsChanged1b());
  AddLast(new Test_InNavigateChatOut_ContextLocationUnassigned1());
  AddLast(new Test_InNavigateChatOut_ContextDetailsChanged_DocumentUrl());
  AddLast(new Test_InNavigateChatOut_ContextLocationAssigned2());
  AddLast(new Test_InNavigateChatOut_ContextDetailsChanged_DocumentUrl());
  AddLast(new Test_InNavigateChatOut_EnterLocationRequested2());
  AddLast(new Test_InNavigateChatOut_ContextDetailsChanged_LocationUrl());
  AddLast(new Test_InNavigateChatOut_LocationContextsChanged2a());
  AddLast(new Test_InNavigateChatOut_ParticipantsChanged());
  AddLast(new Test_InNavigateChatOut_LeaveLocationComplete1());
  AddLast(new Test_InNavigateChatOut_ParticipantsChanged2a());
  AddLast(new Test_InNavigateChatOut_EnterLocationComplete2());
  AddLast(new Test_InNavigateChatOut_Wait(500));
  AddLast(new Test_InNavigateChatOut_Chat2a());

  AddLast(new Test_InNavigateChatOut_Wait(500));
  AddLast(new Test_InNavigateChatOut_Chat2b());
  AddLast(new Test_InNavigateChatOut_Wait(500));
  AddLast(new Test_InNavigateChatOut_Chat2c());
  AddLast(new Test_InNavigateChatOut_Wait(500));
  AddLast(new Test_InNavigateChatOut_LeaveLocationRequested2());
  AddLast(new Test_InNavigateChatOut_ContextDetailsChanged_LocationUrl());
  AddLast(new Test_InNavigateChatOut_LocationContextsChanged2b());

  //AddLast(new Test_InNavigateChatOut_ContextLocationUnassigned2());
  //AddLast(new Test_InNavigateChatOut_ContextDetailsChanged_DocumentUrl());
  //AddLast(new Test_InNavigateChatOut_ContextDestroyed());
  //AddLast(new Test_InNavigateChatOut_LeaveLocationBegin2());
  //AddLast(new Test_InNavigateChatOut_ParticipantsChanged2b());
  //AddLast(new Test_InNavigateChatOut_LeaveLocationComplete2());

  ActionList::Begin();
}

void Test_InNavigateChatOut::End()
{
  { Msg_VpView_GetParticipants msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipants, this); }
  { Msg_VpView_SubscribeParticipantDetail msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeParticipantDetail, this); }
  { Msg_VpView_GetParticipantDetailString msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailString, this); }
  { Msg_VpView_GetParticipantDetailData msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailData, this); }
  { Msg_VpView_GetParticipantDetailRef msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailRef, this); }
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

Test_InNavigateChatOut* gTest_InNavigateChatOut = 0;

static void Test_Arena_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Arena_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Arena_UnitTest_Token", "Starting Test/WebArena"));
  int bTokenEndNow = 0;

  gTest_InNavigateChatOut->Begin();

  if (bTokenEndNow) { Test_Arena_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#endif // AP_TEST_Arena

void Test_Arena_Register()
{
#if defined(AP_TEST_Arena)
  if (Apollo::isLoadedModule("WebArena")) {

    { gTest_InNavigateChatOut = new Test_InNavigateChatOut(); Msg_UnitTest_Register msg; msg.sName = gTest_InNavigateChatOut->getName(); msg.Send(); }
  
    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Arena_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
#endif // AP_TEST_Arena
}

void Test_Arena_Begin()
{
#if defined(AP_TEST_Arena)
  if (Apollo::isLoadedModule("WebArena")) {
  }
#endif // AP_TEST_Arena
}

void Test_Arena_End()
{
#if defined(AP_TEST_Arena)
  if (Apollo::isLoadedModule("WebArena")) {
  }
#endif // AP_TEST_Arena
}
