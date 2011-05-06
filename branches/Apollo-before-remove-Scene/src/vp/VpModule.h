// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(VpModule_H_INCLUDED)
#define VpModule_H_INCLUDED

#include "vp.h"
#include "ApContainer.h"
#include "MsgSystem.h"
#include "MsgVpi.h"
#include "MsgVp.h"
#include "MsgVpView.h"
#include "MsgProtocol.h"
#include "MsgIdentity.h"
#include "MsgUnitTest.h"
#include "MsgServer.h"
#include "SrpcGateHelper.h"
#include "Context.h"
#include "Location.h"

//#include "MsgXmpp.h"
//#include "MsgTimer.h"

typedef ApHandleTree<Participant*> ParticipantPointerList;
typedef ApHandleTreeNode<Participant*> ParticipantPointerListNode;
typedef ApHandleTreeIterator<Participant*> ParticipantPointerListIterator;

// ---------------------------------

class DisplayProfileParticipantDetail: public Elem
{
public:
  DisplayProfileParticipantDetail(const String& sKey): Elem(sKey) {}
  void setMimeTypes(Apollo::ValueList& vlMimeTypes) { vlMimeTypes_ = vlMimeTypes; }
  Apollo::ValueList& getMimeTypes() { return vlMimeTypes_; }

protected:
  Apollo::ValueList vlMimeTypes_;
};

class DisplayProfile
{
public:
  DisplayProfile(const ApHandle& hAp): hAp_(hAp) {}
  void setParticipantDetail(const String& sKey, Apollo::ValueList& vlMimeTypes);

protected:
  ApHandle hAp_;
  ListT<DisplayProfileParticipantDetail, Elem> lDetails_;

#if defined(AP_TEST)
  friend class VpModuleTester;
#endif
};

// ---------------------------------

typedef ApHandlePointerTree<Context*> ContextList;
typedef ApHandlePointerTreeNode<Context*> ContextListNode;
typedef ApHandlePointerTreeIterator<Context*> ContextListIterator;

typedef ApHandlePointerTree<Location*> LocationList;
typedef ApHandlePointerTreeNode<Location*> LocationListNode;
typedef ApHandlePointerTreeIterator<Location*> LocationListIterator;

// ---------------------------------

class VpModule
{
public:
  VpModule()
    :bInShutdown_(false)
    ,nExternUrlPort_(0)
  {}

  int init();
  void exit();

  String getItemDataExternUrl(const String& sIdentityUrl, const String& sItemId);

public:
  void On_System_AfterLoadModules(Msg_System_AfterLoadModules* pMsg);
  void On_System_BeforeUnloadModules(Msg_System_BeforeUnloadModules* pMsg);

  void On_Vpi_LocationXmlResponse(Msg_Vpi_LocationXmlResponse* pMsg);

  void On_Vp_NavigateContext(Msg_Vp_NavigateContext* pMsg);
  void On_Vp_CloseContext(Msg_Vp_CloseContext* pMsg);

  void On_VpView_ContextVisibility(Msg_VpView_ContextVisibility* pMsg);
  void On_VpView_ContextPosition(Msg_VpView_ContextPosition* pMsg);
  void On_VpView_ContextSize(Msg_VpView_ContextSize* pMsg);

  void On_Vp_CreateContext(Msg_Vp_CreateContext* pMsg);
  void On_Vp_DestroyContext(Msg_Vp_DestroyContext* pMsg);

  void On_Vp_CreateLocation(Msg_Vp_CreateLocation* pMsg);
  void On_Vp_DestroyLocation(Msg_Vp_DestroyLocation* pMsg);

  void On_Vp_AddLocationContext(Msg_Vp_AddLocationContext* pMsg);
  void On_Vp_RemoveLocationContext(Msg_Vp_RemoveLocationContext* pMsg);

  void On_Vp_EnterLocation(Msg_Vp_EnterLocation* pMsg);
  void On_Vp_LeaveLocation(Msg_Vp_LeaveLocation* pMsg);

  void On_Protocol_Online(Msg_Protocol_Online* pMsg);
  void On_Protocol_Offline(Msg_Protocol_Offline* pMsg);

  void On_Protocol_EnteringRoom(Msg_Protocol_EnteringRoom* pMsg);
  void On_Protocol_EnterRoomComplete(Msg_Protocol_EnterRoomComplete* pMsg);
  void On_Protocol_LeavingRoom(Msg_Protocol_LeavingRoom* pMsg);
  void On_Protocol_LeaveRoomComplete(Msg_Protocol_LeaveRoomComplete* pMsg);

  void On_Protocol_ParticipantEntered(Msg_Protocol_ParticipantEntered* pMsg);
  void On_Protocol_ParticipantLeft(Msg_Protocol_ParticipantLeft* pMsg);
  void On_Protocol_ParticipantSelf(Msg_Protocol_ParticipantSelf* pMsg);
  void On_Protocol_Participant_Begin(Msg_Protocol_Participant_Begin* pMsg);
  void On_Protocol_Participant_Status(Msg_Protocol_Participant_Status* pMsg);
  void On_Protocol_Participant_StatusMessage(Msg_Protocol_Participant_StatusMessage* pMsg);
  void On_Protocol_Participant_JabberId(Msg_Protocol_Participant_JabberId* pMsg);
  void On_Protocol_Participant_MucUser(Msg_Protocol_Participant_MucUser* pMsg);
  void On_Protocol_Participant_VpIdentity(Msg_Protocol_Participant_VpIdentity* pMsg);
  void On_Protocol_Participant_Position(Msg_Protocol_Participant_Position* pMsg);
  void On_Protocol_Participant_Condition(Msg_Protocol_Participant_Condition* pMsg);
  void On_Protocol_Participant_FirebatFeatures(Msg_Protocol_Participant_FirebatFeatures* pMsg);
  void On_Protocol_Participant_End(Msg_Protocol_Participant_End* pMsg);

  void On_Protocol_Room_GetIdentity(Msg_Protocol_Room_GetIdentity* pMsg);
  void On_Protocol_Room_GetPosition(Msg_Protocol_Room_GetPosition* pMsg);
  void On_Protocol_Room_GetCondition(Msg_Protocol_Room_GetCondition* pMsg);
  void On_Protocol_ReceivePublicChat(Msg_Protocol_ReceivePublicChat* pMsg);

  void On_Identity_ContainerBegin(Msg_Identity_ContainerBegin* pMsg);
  void On_Identity_ItemAdded(Msg_Identity_ItemAdded* pMsg);
  void On_Identity_ItemChanged(Msg_Identity_ItemChanged* pMsg);
  void On_Identity_ItemDataAvailable(Msg_Identity_ItemDataAvailable* pMsg);
  void On_Identity_ItemRemoved(Msg_Identity_ItemRemoved* pMsg);
  void On_Identity_ContainerEnd(Msg_Identity_ContainerEnd* pMsg);

  void On_VpView_GetLocations(Msg_VpView_GetLocations* pMsg);
  void On_VpView_GetLocationDetail(Msg_VpView_GetLocationDetail* pMsg);
  void On_VpView_SubscribeLocationDetail(Msg_VpView_SubscribeLocationDetail* pMsg);
  void On_VpView_UnsubscribeLocationDetail(Msg_VpView_UnsubscribeLocationDetail* pMsg);
  void On_VpView_GetLocationContexts(Msg_VpView_GetLocationContexts* pMsg);
  void On_VpView_SubscribeContextDetail(Msg_VpView_SubscribeContextDetail* pMsg);
  void On_VpView_UnsubscribeContextDetail(Msg_VpView_UnsubscribeContextDetail* pMsg);
  void On_VpView_GetContextDetail(Msg_VpView_GetContextDetail* pMsg);
  void On_VpView_GetParticipants(Msg_VpView_GetParticipants* pMsg);
  void On_VpView_GetParticipantDetailString(Msg_VpView_GetParticipantDetailString* pMsg);
  void On_VpView_GetParticipantDetailData(Msg_VpView_GetParticipantDetailData* pMsg);
  void On_VpView_GetParticipantDetailRef(Msg_VpView_GetParticipantDetailRef* pMsg);
  void On_VpView_SubscribeParticipantDetail(Msg_VpView_SubscribeParticipantDetail* pMsg);
  void On_VpView_UnsubscribeParticipantDetail(Msg_VpView_UnsubscribeParticipantDetail* pMsg);
  void On_VpView_Profile_Create(Msg_VpView_Profile_Create* pMsg);
  void On_VpView_Profile_SetParticipantDetail(Msg_VpView_Profile_SetParticipantDetail* pMsg);
  void On_VpView_Profile_GetParticipantDetails(Msg_VpView_Profile_GetParticipantDetails* pMsg);
  void On_VpView_Profile_SubscribeParticipantDetails(Msg_VpView_Profile_SubscribeParticipantDetails* pMsg);
  void On_VpView_Profile_UnsubscribeParticipantDetails(Msg_VpView_Profile_UnsubscribeParticipantDetails* pMsg);
  void On_VpView_Profile_Delete(Msg_VpView_Profile_Delete* pMsg);

  void On_VpView_ReplayLocationPublicChat(Msg_VpView_ReplayLocationPublicChat* pMsg);

  void On_Server_HttpRequest(Msg_Server_HttpRequest* pMsg);

  //void On_Xmpp_DataIn(Msg_Xmpp_DataIn* pMsg);
  //void On_Timer_Event(Msg_Timer_Event* pMsg);

  void On_Vp_SendPublicChat(Msg_Vp_SendPublicChat* pMsg);
  void On_Vp_SendPosition(Msg_Vp_SendPosition* pMsg);
  void On_Vp_SendCondition(Msg_Vp_SendCondition* pMsg);

  AP_UNITTEST_DECLAREHOOK();

protected:
  Context* findContext(const ApHandle& hContext);
  Context* findContextByMapping(const ApHandle& hMapping);
  Location* findLocation(const ApHandle& hLocation);
  Location* findLocationByUrl(const String& sUrl);
  Location* findLocationByContext(const ApHandle& hContext);
  Location* findLocationByRoom(const ApHandle& hRoom);
  Location* findLocationByParticipant(const ApHandle& hParticipant);
  ApHandle findLocationHandleByParticipant(const ApHandle& hParticipant);

  Participant* findParticipant(const ApHandle& hParticipant);
  Participant* findParticipantInRoom(const ApHandle& hRoom, const ApHandle& hParticipant);
  ParticipantPointerList findParticipantsByIdentityUrl(const String& sUrl);

  DisplayProfile* findProfile(const ApHandle& hProfile);

  void setParticipantLocationMapping(const ApHandle& hParticipant, const ApHandle& hLocation);
  void unsetParticipantLocationMapping(const ApHandle& hParticipant, const ApHandle& hLocation);

  void setIdentityParticipantMapping(const String& sUrl, const ApHandle& hParticipant);
  void unsetIdentityParticipantMapping(const String& sUrl, const ApHandle& hParticipant);
  void removeParticipantFromIdentityParticipantMapping(const ApHandle& hParticipant);
  void getParticipantHandlesByIdentity(const String& sUrl, Apollo::ValueList& vlHandles);

protected:
  bool bInShutdown_;
  String sExternUrlAddress_;
  int nExternUrlPort_;

  ContextList contexts_;
  LocationList locations_;
  ApHandlePointerTree<DisplayProfile*> participantSubscriptionProfiles_;
  List lOnlineProtocols_;
  ApHandleTree<ApHandle> participant2Location_;

  ApHandleTree<String> participant2Identity_;
  StringTree<ApHandleTree<int>> identity2Participant_;

  Apollo::SrpcGateHandlerRegistry srpcGateRegistry_;

  AP_MSG_REGISTRY_DECLARE;

#if defined(AP_TEST)
  friend class VpModuleTester;
#endif
};

typedef ApModuleSingleton<VpModule> VpModuleInstance;

// ---------------------------------

#endif // VpModule_H_INCLUDED
