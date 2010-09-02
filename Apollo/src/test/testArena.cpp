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

static void Test_VpView_GetParticipants(Msg_VpView_GetParticipants* pMsg)
{
  Test_ParticipantList* pl = (Test_ParticipantList*) pMsg->Ref();
  Test_ParticipantListIterator iter(*pl);
  for (Test_ParticipantListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    pMsg->vlParticipants.add(pNode->Key());
  }
}

static void Test_VpView_SubscribeParticipantDetail(Msg_VpView_SubscribeParticipantDetail* pMsg)
{
  Test_ParticipantList* pl = (Test_ParticipantList*) pMsg->Ref();
  Test_Participant p;
  pl->Get(pMsg->hParticipant, p);

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
  Test_ParticipantList* pl = (Test_ParticipantList*) pMsg->Ref();
  Test_Participant p;
  pl->Get(pMsg->hParticipant, p);

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
  Test_ParticipantList* pl = (Test_ParticipantList*) pMsg->Ref();
  Test_Participant p;
  pl->Get(pMsg->hParticipant, p);

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
  Test_ParticipantList* pl = (Test_ParticipantList*) pMsg->Ref();
  { Msg_VpView_ReplayLocationPublicChat msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_ReplayLocationPublicChat, pl); }
}

static String Test_Arena_InChangeOut()
{
  String s;

  // IN
  ApHandle hContext = Apollo::newHandle();
  ApHandle hLocation1 = Apollo::newHandle();
  ApHandle hLocation2 = Apollo::newHandle();
  int nLeft = 100;
  int nBottom = 400;
  int nWidth = 500;
  int nHeight = 300;
  {
    Msg_VpView_ContextCreated msg;
    msg.hContext = hContext;
    msg.Send();
  }

  {
    Msg_VpView_ContextVisibility msg;
    msg.hContext = hContext;
    msg.bVisible = 0;
    msg.Send();
  }

  {
    Msg_VpView_ContextPosition msg;
    msg.hContext = hContext;
    msg.nX = nLeft;
    msg.nY = nBottom;
    msg.Send();
  }

  {
    Msg_VpView_ContextSize msg;
    msg.hContext = hContext;
    msg.nWidth = nWidth;
    msg.nHeight = nHeight;
    msg.Send();
  }

  {
    Msg_VpView_ContextVisibility msg;
    msg.hContext = hContext;
    msg.bVisible = 1;
    msg.Send();
  }

  {
    Msg_VpView_ContextLocationAssigned msg;
    msg.hContext = hContext;
    msg.hLocation = hLocation1;
    msg.Send();
  }

  {
    Msg_VpView_EnterLocationRequested msg;
    msg.hLocation = hLocation1;
    msg.Send();
  }

  {
    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = hLocation1;
    msg.Send();
  }

  // VpView_GetLocationContexts
  // VpView_SubscribeContextDetail
  // VpView_SubscribeLocationDetail
  // VpView_GetContextDetail
  // VpView_GetLocationDetail

  {
    Msg_VpView_EnterLocationBegin msg;
    msg.hLocation = hLocation1;
    msg.Send();
  }

  Test_ParticipantList pl1;
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
    pl1.Set(Apollo::newHandle(), p);
  }
  { Msg_VpView_GetParticipants msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipants, &pl1, ApCallbackPosEarly); }
  { Msg_VpView_SubscribeParticipantDetail msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeParticipantDetail, &pl1, ApCallbackPosEarly); }
  { Msg_VpView_GetParticipantDetailString msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailString, &pl1, ApCallbackPosEarly); }
  { Msg_VpView_GetParticipantDetailData msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailData, &pl1, ApCallbackPosEarly); }

  {
    Msg_VpView_ParticipantsChanged msg;
    msg.hLocation = hLocation1;
    msg.nCount = 1;
    msg.Send();
  }

  // VpView_GetParticipants
  // VpView_SubscribeParticipantDetail
  // VpView_GetParticipantDetailString
  // VpView_GetParticipantDetailData

  { Msg_VpView_GetParticipants msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipants, &pl1); }
  { Msg_VpView_SubscribeParticipantDetail msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_SubscribeParticipantDetail, &pl1); }
  { Msg_VpView_GetParticipantDetailString msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailString, &pl1); }
  { Msg_VpView_GetParticipantDetailData msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_VpView_GetParticipantDetailData, &pl1); }

  { Msg_VpView_ReplayLocationPublicChat msg; msg.Hook(MODULE_NAME, (ApCallback) Test_VpView_ReplayLocationPublicChat, &pl1, ApCallbackPosEarly); }

  {
    Msg_VpView_EnterLocationComplete msg;
    msg.hLocation = hLocation1;
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

static void Test_Arena_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Arena_UnitTest_TokenEnd", "Finished Test/Arena"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

static void Test_Arena_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Arena_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Arena_UnitTest_Token", "Starting Test/Arena"));
  int bTokenEndNow = 1;

  AP_UNITTEST_EXECUTE(Test_Arena_InChangeOut);

  if (bTokenEndNow) { Test_Arena_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#endif // AP_TEST_Arena

void Test_Arena_Register()
{
#if defined(AP_TEST_Arena)
  if (Apollo::isLoadedModule("Arena")) {
    AP_UNITTEST_REGISTER(Test_Arena_InChangeOut);
  
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
