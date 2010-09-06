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
};

typedef ApHandleTree<Test_Participant> Test_ParticipantList;
typedef ApHandleTreeIterator<Test_Participant> Test_ParticipantListIterator;
typedef ApHandleTreeNode<Test_Participant> Test_ParticipantListNode;

class Test_Setup
{
public:
  String Begin();
  String End();

  ApHandle hContext_;
  ApHandle hLocation1_;
  ApHandle hLocation2_;
  int nLeft_;
  int nBottom_;
  int nWidth_;
  int nHeight_;

  Test_ParticipantList pl1_;
};

// ------------------------------------------------------

Test_Setup* gTest_Arena_InChangeOut = 0;

static void Test_Arena_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Arena_UnitTest_TokenEnd", "Finished Test/Arena"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

static String Test_Arena_InChangeOut_Begin()
{
  String s;

  gTest_Arena_InChangeOut = new Test_Setup();
  s = gTest_Arena_InChangeOut->Begin();

  return s;
}

static String Test_Arena_InChangeOut_End()
{
  String s;

  s = gTest_Arena_InChangeOut->End();
  delete gTest_Arena_InChangeOut;
  gTest_Arena_InChangeOut = 0;

  AP_UNITTEST_RESULT(Test_Arena_InChangeOut_End, s.empty(), s);

  return s;
}

// ------------------------------------------------------

static void Test_VpView_GetParticipants(Msg_VpView_GetParticipants* pMsg)
{
  Test_Setup* t = (Test_Setup*) pMsg->Ref();
  Test_ParticipantListIterator iter(t->pl1_);
  for (Test_ParticipantListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    pMsg->vlParticipants.add(pNode->Key());
  }
}

static void Test_VpView_SubscribeParticipantDetail(Msg_VpView_SubscribeParticipantDetail* pMsg)
{
  Test_Setup* t = (Test_Setup*) pMsg->Ref();
  Test_Participant p;
  t->pl1_.Get(pMsg->hParticipant, p);

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

static void Test_VpView_GetParticipantDetailString(Msg_VpView_GetParticipantDetailString* pMsg)
{
  Test_Setup* t = (Test_Setup*) pMsg->Ref();
  Test_Participant p;
  t->pl1_.Get(pMsg->hParticipant, p);

  String sKey = pMsg->sKey;
  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Nickname) {
    pMsg->sValue = p.sNickname;
  } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
    pMsg->sValue = p.sOnlineStatus;
  } else if (sKey == Msg_VpView_ParticipantDetail_Message) {
    pMsg->sValue = p.sMessage;
  } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
    pMsg->sValue = p.sPosition;
  } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {
    pMsg->sValue = p.sCondition;
  } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {
    pMsg->sValue = p.sProfileUrl;
  }
}

static void Test_VpView_GetParticipantDetailData(Msg_VpView_GetParticipantDetailData* pMsg)
{
  Test_Setup* t = (Test_Setup*) pMsg->Ref();
  Test_Participant p;
  t->pl1_.Get(pMsg->hParticipant, p);

  String sKey = pMsg->sKey;
  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    pMsg->sbData = p.sbAvatar;
    pMsg->sMimeType = p.sAvatarMimetype;
    pMsg->sSource = p.sAvatarSource;
  }
}

static void Test_VpView_ReplayLocationPublicChat(Msg_VpView_ReplayLocationPublicChat* pMsg)
{
  Test_Setup* t = (Test_Setup*) pMsg->Ref();

  Apollo::TimeValue tNow = Apollo::getNow();
  Apollo::TimeValue tAge(2, 0);
  Apollo::TimeValue tThen = tNow - tAge;

  Test_ParticipantListIterator iter(t->pl1_);
  for (Test_ParticipantListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    Msg_VpView_LocationPublicChat msg;
    msg.hLocation = pMsg->hLocation;
    msg.hParticipant = pNode->Key();
    msg.hChat = Apollo::newHandle();
    msg.sNickname = pNode->Value().sNickname;
    msg.sText = "Hello World";
    msg.nSec = tThen.Sec();
    msg.nMicroSec = tThen.MicroSec();
    msg.Send();
  }

  //Test_Arena_InChangeOut_End();
}

static void Test_Galileo_IsAnimationDataInStorage(Msg_Galileo_IsAnimationDataInStorage* pMsg)
{
  Test_Setup* t = (Test_Setup*) pMsg->Ref();

  pMsg->bAvailable = 1;

  pMsg->apStatus = ApMessage::Ok;
}

static void Test_Galileo_LoadAnimationDataFromStorage(Msg_Galileo_LoadAnimationDataFromStorage* pMsg)
{
  Test_Setup* t = (Test_Setup*) pMsg->Ref();

  String sFile = String::filenameFile(pMsg->sUrl);
  Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + sFile, pMsg->sbData);
  pMsg->sMimeType = "image/gif";

  pMsg->apStatus = ApMessage::Ok;
}

// ------------------------------------------------------

String Test_Setup::Begin()
{
  String s;

  hContext_ = Apollo::newHandle();
  hLocation1_ = Apollo::newHandle();
  hLocation2_ = Apollo::newHandle();
  nLeft_ = 100;
  nBottom_ = 400;
  nWidth_ = 500;
  nHeight_ = 300;

  {
    Test_Participant p;
    p.sNickname = "Tassadar";
    Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + "config.xml", p.sbAvatar);
    p.sAvatarMimetype = "avatar/gif";
    p.sAvatarSource = "IdentityItemUrl=http://ydentiti.org/test/Tassadar/config.xml";
    p.sOnlineStatus = "";
    p.sMessage = "Hallo";
    p.sPosition;
    p.sCondition;
    p.sProfileUrl;
    pl1_.Set(Apollo::newHandle(), p);
  }

  { Msg_VpView_GetParticipants msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipants, this, ApCallbackPosEarly); }
  { Msg_VpView_SubscribeParticipantDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeParticipantDetail, this, ApCallbackPosEarly); }
  { Msg_VpView_GetParticipantDetailString msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailString, this, ApCallbackPosEarly); }
  { Msg_VpView_GetParticipantDetailData msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailData, this, ApCallbackPosEarly); }
  { Msg_VpView_ReplayLocationPublicChat msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_ReplayLocationPublicChat, this, ApCallbackPosEarly); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_IsAnimationDataInStorage, this, ApCallbackPosEarly); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_LoadAnimationDataFromStorage, this, ApCallbackPosEarly); }

  // IN
  {
    Msg_VpView_ContextCreated msg;
    msg.hContext = hContext_;
    msg.Send();
  }

  {
    Msg_VpView_ContextVisibility msg;
    msg.hContext = hContext_;
    msg.bVisible = 0;
    msg.Send();
  }

  {
    Msg_VpView_ContextPosition msg;
    msg.hContext = hContext_;
    msg.nX = nLeft_;
    msg.nY = nBottom_;
    msg.Send();
  }

  {
    Msg_VpView_ContextSize msg;
    msg.hContext = hContext_;
    msg.nWidth = nWidth_;
    msg.nHeight = nHeight_;
    msg.Send();
  }

  {
    Msg_VpView_ContextVisibility msg;
    msg.hContext = hContext_;
    msg.bVisible = 1;
    msg.Send();
  }

  {
    Msg_VpView_ContextLocationAssigned msg;
    msg.hContext = hContext_;
    msg.hLocation = hLocation1_;
    msg.Send();
  }

  {
    Msg_VpView_EnterLocationRequested msg;
    msg.hLocation = hLocation1_;
    msg.Send();
  }

  {
    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = hLocation1_;
    msg.Send();
  }

  // VpView_GetLocationContexts
  // VpView_SubscribeContextDetail
  // VpView_SubscribeLocationDetail
  // VpView_GetContextDetail
  // VpView_GetLocationDetail

  {
    Msg_VpView_EnterLocationBegin msg;
    msg.hLocation = hLocation1_;
    msg.Send();
  }

  {
    Msg_VpView_ParticipantsChanged msg;
    msg.hLocation = hLocation1_;
    msg.nCount = 1;
    msg.Send();
  }

  // VpView_GetParticipants
  // VpView_SubscribeParticipantDetail
  // VpView_GetParticipantDetailString
  // VpView_GetParticipantDetailData
  // Galileo_IsAnimationDataInStorage
  // Galileo_LoadAnimationDataFromStorage

  {
    Msg_VpView_EnterLocationComplete msg;
    msg.hLocation = hLocation1_;
    msg.Send();
  }

  // VpView_ReplayLocationPublicChat

  /*
  // CHANGE
  {
    Msg_VpView_ContextVisibility msg;// 1
  }

  {
    Msg_VpView_LeaveLocationRequested msg;
  }

  {
    Msg_VpView_LocationContextsChanged msg;
  }

  {
    Msg_VpView_ContextLocationUnassigned msg;
  }

  {
    Msg_VpView_ContextLocationAssigned msg;
  }

  {
    Msg_VpView_EnterLocationRequested msg;
  }

  {
    Msg_VpView_LocationContextsChanged msg;
  }

  {
    Msg_VpView_LeaveLocationBegin msg;
  }

  {
    Msg_VpView_ParticipantsChanged msg;
  }

  {
    Msg_VpView_LeaveLocationComplete msg;
  }

  {
    Msg_VpView_EnterLocationBegin msg;
  }

  {
    Msg_VpView_ParticipantsChanged msg;
  }

  {
    Msg_VpView_EnterLocationComplete msg;
  }

  // OUT
  {
    Msg_VpView_LeaveLocationRequested msg;
  }

  {
    Msg_VpView_LocationContextsChanged msg;
  }

  {
    Msg_VpView_ContextLocationUnassigned msg;
  }

  {
    Msg_VpView_ContextDestroyed msg;
  }

  {
    Msg_VpView_LeaveLocationBegin msg;
  }

  {
    Msg_VpView_ParticipantsChanged msg;
  }

  {
    Msg_VpView_LeaveLocationComplete msg;
  }
  */

  return s;
}

String Test_Setup::End()
{
  String s;

  { Msg_VpView_GetParticipants msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipants, this); }
  { Msg_VpView_SubscribeParticipantDetail msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeParticipantDetail, this); }
  { Msg_VpView_GetParticipantDetailString msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailString, this); }
  { Msg_VpView_GetParticipantDetailData msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailData, this); }
  { Msg_VpView_ReplayLocationPublicChat msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_ReplayLocationPublicChat, this); }
  { Msg_Galileo_IsAnimationDataInStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_IsAnimationDataInStorage, this); }
  { Msg_Galileo_LoadAnimationDataFromStorage msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_LoadAnimationDataFromStorage, this); }

  Test_Arena_UnitTest_TokenEnd();

  return s;
}

// ------------------------------------------------------

static void Test_Arena_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Arena_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Arena_UnitTest_Token", "Starting Test/Arena"));
  int bTokenEndNow = 0;

  AP_UNITTEST_EXECUTE(Test_Arena_InChangeOut_Begin);

  if (bTokenEndNow) { Test_Arena_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#endif // AP_TEST_Arena

void Test_Arena_Register()
{
#if defined(AP_TEST_Arena)
  if (Apollo::isLoadedModule("Arena")) {
    AP_UNITTEST_REGISTER(Test_Arena_InChangeOut_Begin);
    AP_UNITTEST_REGISTER(Test_Arena_InChangeOut_End);
  
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
