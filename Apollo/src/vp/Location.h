// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Location_H_INCLUDED)
#define Location_H_INCLUDED

#include "ApContainer.h"
#include "Participant.h"
#include "Chat.h"

class Location;
class LocationThingyProvider;

class LocationThingy: public Thingy
{
public:
  LocationThingyProvider* getProvider() { return (LocationThingyProvider*) pProvider_; }
};

class LocationThingyListBase: public ThingyList<LocationThingy>
{
public:
  void setLocation(Location* pLocation) { pLocation_ = pLocation; }

protected:
  Location* pLocation_;
};

class LocationThingyList: public LocationThingyListBase
{
public:
  ThingyProvider* newProvider(const String& sKey);
  void sendChanges(Apollo::ValueList& vlChanges);
};

//------------------------------

class LocationThingyProvider: public ThingyProvider
{
public:
  void setLocation(Location* pLocation) { pLocation_ = pLocation; }
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource) {} // shut up, string only

protected:
  Location* pLocation_;
};

//------------------------------

class StateLocationThingyProvider: public LocationThingyProvider
{
public:
  void getString(String& sValue, String& sMimeType);
};

//------------------------------

class LocationUrlLocationThingyProvider: public LocationThingyProvider
{
public:
  void getString(String& sValue, String& sMimeType);
};

//------------------------------------------------------------

typedef ApHandleTree<ApHandle> ContextHandleList;
typedef ApHandleTreeNode<ApHandle> ContextHandleListNode;
typedef ApHandleTreeIterator<ApHandle> ContextHandleListIterator;

typedef ApHandlePointerTree<Participant*> ParticipantList;
typedef ApHandlePointerTreeNode<Participant*> ParticipantListNode;
typedef ApHandlePointerTreeIterator<Participant*> ParticipantListIterator;

typedef ListT<Chat, Elem> ChatList;

class Location
{
public:
  Location(ApHandle hLocation, const String& sLocationUrl);

  typedef enum _State { State_NotEntered
    ,State_EnterRequested
    ,State_Entering
    ,State_Entered
    ,State_LeaveRequested
    ,State_Leaving
  } State;

  ApHandle apHandle() { return hAp_; }
  String& getUrl() { return sLocationUrl_; }
  ApHandle getRoom() { return hRoom_; }
  State getState() { return nState_; };

  int addContext(ApHandle hContext);
  int removeContext(ApHandle hContext);
  int hasContext(ApHandle hContext);
  void getContexts(Apollo::ValueList& vlContexts);
  int removeAllContexts();

  int enter();
  int leave();
  void entering();
  void leaving();
  void enterComplete();
  void leaveComplete();

  int onProtocolOnline();
  int onProtocolOffline();

  int addParticipant(ApHandle hParticipant);
  int removeParticipant(ApHandle hParticipant);
  int removeAllParticipants();
  Participant* findParticipant(ApHandle hParticipant);
  void getParticipants(Apollo::ValueList& vlParticipants);

  void setSelfParticipant(ApHandle hParticipant) { hSelf_ = hParticipant; }
  ApHandle getSelfParticipant() { return hSelf_; }

  void sendPublicChat(const String& sText);
  void onReceivePublicChat(ApHandle hParticipant, const String& sText, int nSec, int nMicroSec);
  void replayChats(int nMaxAge, int nMaxLines, int nMaxData);
  void cleanupChat();

  void getDetailString(const String& sKey, String& sValue, String& sMimeType);
  int subscribeDetail(const String& sKey, String& sValue, String& sMimeType);
  int unsubscribeDetail(const String& sKey);
  int addSubscription(const String& sKey);
  int removeSubscription(const String& sKey);

  void setPosition(Apollo::KeyValueList& kvParams);
  void onGetPosition(Apollo::KeyValueList& kvParams);
  void setCondition(Apollo::KeyValueList& kvParams);
  void onGetCondition(Apollo::KeyValueList& kvParams);

protected:
  int splitLocationUrl(String& sProtocol, String& sRoom);
  String& protocol();
  String& room();

  friend class StateLocationThingyProvider;
  String state2String(State nState);
  void setState(State nState);

protected:
  ApHandle hAp_;
  String sLocationUrl_;
  ApHandle hRoom_;
  ApHandle hSelf_;
  State nState_;
  String sProtocol_;
  String sRoom_;
  int bEnterAfterProtocolOnline_;
  int bEnterAfterLeaveComplete_;

  ContextHandleList contexts_;
  ParticipantList participants_;
  ChatList lChats_;

  Apollo::KeyValueList kvPosition_;
  Apollo::KeyValueList kvCondition_;

  LocationThingyList lThingys_;
};

#endif // Location_H_INCLUDED
