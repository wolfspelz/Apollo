// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgVp.h"
#include "MsgProtocol.h"
#include "MsgVpView.h"
#include "Location.h"

Location::Location(const ApHandle& hLocation, const String& sLocationUrl)
:hAp_(hLocation)
,sLocationUrl_(sLocationUrl)
,hRoom_(ApNoHandle)
,nState_(State_NotEntered)
,bEnterAfterProtocolOnline_(0)
,bEnterAfterLeaveComplete_(0)
{
  lThingys_.setLocation(this);
  (void) lThingys_.getOrCreate(Msg_VpView_LocationDetail_LocationUrl);
  (void) lThingys_.getOrCreate(Msg_VpView_LocationDetail_State);
}

//----------------------------------------------------------

int Location::splitLocationUrl(String& sProtocol, String& sRoom)
{
  String sLocation = sLocationUrl_;
  sLocation.nextToken(":", sProtocol);
  sLocation.nextToken(":", sRoom);

  if (sProtocol.empty() || sRoom.empty()) {
    return 0;
  }

  return 1;
}

String& Location::protocol()
{
  if (!sProtocol_){
    (void) splitLocationUrl(sProtocol_, sRoom_);
  }

  return sProtocol_;
}

String& Location::room()
{
  if (!sRoom_){
    (void) splitLocationUrl(sProtocol_, sRoom_);
  }

  return sRoom_;
}

//----------------------------------------------------------

int Location::addContext(const ApHandle& hContext)
{
  int ok = 1;

  {
    Msg_VpView_ContextLocationAssigned msg;
    msg.hContext = hContext;
    msg.hLocation = apHandle();
    msg.Send();
  }

  if (contexts_.Count() == 0){
    Msg_Vp_EnterLocation msg;
    msg.hLocation = hAp_;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Vp_EnterLocation failed for " ApHandleFormat " %s", ApHandlePrintf(hAp_), _sz(sLocationUrl_)));
    }
  }

  contexts_.Set(hContext, hContext);

  {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat ".addContext(" ApHandleFormat ")", ApHandlePrintf(apHandle()), ApHandlePrintf(hContext)));

    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = apHandle();
    msg.Send();
  }

  return ok;
}

int Location::removeContext(const ApHandle& hContext)
{
  int ok = 1;

  if (contexts_.Count() == 1){
    Msg_Vp_LeaveLocation msg;
    msg.hLocation = hAp_;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Vp_LeaveLocation failed for " ApHandleFormat " %s", ApHandlePrintf(hAp_), _sz(sLocationUrl_)));
    } else {
      apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat ".removeContext(" ApHandleFormat ")", ApHandlePrintf(apHandle()), ApHandlePrintf(hContext)));
    }
  }

  contexts_.Unset(hContext);

  {
    Msg_VpView_LocationContextsChanged msg;
    msg.hLocation = apHandle();
    msg.Send();
  }

  {
    Msg_VpView_ContextLocationUnassigned msg;
    msg.hContext = hContext;
    msg.hLocation = apHandle();
    msg.Send();
  }

  return ok;
}

int Location::hasContext(const ApHandle& hContext)
{
  return (contexts_.Find(hContext) != 0);
}

// @throws ApException
void Location::getContexts(Apollo::ValueList& vlContexts)
{
  ContextHandleListIterator iter(contexts_);
  for (ContextHandleListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    vlContexts.add(pNode->Key());
  }
}

int Location::removeAllContexts()
{
  int ok = 0;

  if (contexts_.Count() == 0) {
    ok = 1;
  } else {
    while (contexts_.Count() > 0) {
      ContextHandleListIterator iter(contexts_);
      ContextHandleListNode* pNode = iter.Next();
      if (pNode != 0) {
        ApHandle hContext = pNode->Key();
        ok = removeContext(hContext);
      }
    }
  }
  
  return ok;
}

//----------------------------

int Location::enter()
{
  int ok = 1;

  String sRoom = room();
  String sProtocol = protocol();
  if (sProtocol.empty() || sRoom.empty()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "protocol=%s or location=%s empty", _sz(sProtocol), _sz(sRoom)));
  } else {

    int bProtocolIsOnline = 0;
    {
      Msg_Protocol_IsOnline msg;
      msg.sProtocol = sProtocol;
      if (msg.Request()) {
        bProtocolIsOnline = msg.bOnline;
      }
    }

    if (!bProtocolIsOnline) {
      bEnterAfterProtocolOnline_ = 1;
      apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "protocol=%s not online, waiting for online before enter", _sz(sProtocol)));
    } else {

      if (nState_	== State_LeaveRequested) {
        bEnterAfterLeaveComplete_ = 1;
        apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "protocol=%s leave requested, waiting for leave complete before enter", _sz(sProtocol)));
      } else {

        hRoom_ = Apollo::newHandle();
        apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " enter " ApHandleFormat "", ApHandlePrintf(apHandle()), ApHandlePrintf(hRoom_)));

        Msg_Protocol_EnterRoom msg;
        msg.sProtocol = sProtocol;
        msg.sRoom = sRoom;
        msg.hRoom = hRoom_;
        ok = msg.Request();
        if (!ok) {
          apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Protocol_EnterRoom failed, %s", _sz(sLocationUrl_)));
        } else {
          setState(State_EnterRequested);

          Msg_VpView_EnterLocationRequested msg;
          msg.hLocation = apHandle();
          msg.Send();
        }
      }
    }

  }

  return ok;
}

void Location::entering()
{
  setState(State_Entering);

  Msg_VpView_EnterLocationBegin msg;
  msg.hLocation = apHandle();
  msg.Send();
}

void Location::enterComplete()
{
  setState(State_Entered);

  Msg_VpView_EnterLocationComplete msg;
  msg.hLocation = apHandle();
  msg.Send();
}

int Location::leave()
{
  int ok = 1;

  String sRoom = room();
  String sProtocol = protocol();
  if (sProtocol.empty() || sRoom.empty()) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "protocol=%s or location=%s empty", _sz(sProtocol), _sz(sRoom)));
  } else {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " leave " ApHandleFormat "", ApHandlePrintf(apHandle()), ApHandlePrintf(hRoom_)));

    Msg_Protocol_LeaveRoom msg;
    msg.sProtocol = sProtocol;
    msg.hRoom = hRoom_;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Protocol_LeaveRoom failed, %s", _sz(sLocationUrl_)));
    } else {
      setState(State_LeaveRequested);

      Msg_VpView_LeaveLocationRequested msg;
      msg.hLocation = apHandle();
      msg.Send();
    }
  }

  return ok;
}

void Location::leaving()
{
  setState(State_Leaving);

  Msg_VpView_LeaveLocationBegin msg;
  msg.hLocation = apHandle();
  msg.Send();
}

void Location::leaveComplete()
{
  setState(State_NotEntered);
  hRoom_ = ApNoHandle;

  Msg_VpView_LeaveLocationComplete msg;
  msg.hLocation = apHandle();
  msg.Send();

  if (bEnterAfterLeaveComplete_) {
    bEnterAfterLeaveComplete_ = 0;
    enter();
  }
}

//----------------------------------------------------------

int Location::onProtocolOnline()
{
  int ok = 1;

  if (bEnterAfterProtocolOnline_) {
    bEnterAfterProtocolOnline_ = 0;
    ok = enter();
  }

  return ok;
}

int Location::onProtocolOffline()
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Cleaning up %d participants", participants_.Count()));

  State nPreviousState = nState_;

  (void) leaving();

  (void) removeAllParticipants();

  (void) leaveComplete();

  switch (nPreviousState) {
    case State_EnterRequested:
    case State_Entering:
    case State_Entered:
      bEnterAfterProtocolOnline_ = 1;
      break;
    default:;
  }

  return ok;
}

//----------------------------------------------------------

int Location::addParticipant(const ApHandle& hParticipant)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " add " ApHandleFormat "", ApHandlePrintf(apHandle()), ApHandlePrintf(hParticipant)));

  Participant* pParticipant = new Participant(hParticipant, this);
  if (pParticipant != 0) {
    ok = participants_.Set(hParticipant, pParticipant);
  }

  // ---------------
  // These two asyc because:
  // in AP_MSG_HANDLER_METHOD(VpModule, Protocol_ParticipantEntered)
  //   setParticipantLocationMapping() 
  // must be executed before 
  //   pLocation->addParticipant()
  // because in addParticipant() the avatars are created by displays, 
  // which need an active participant with data and location association

  if (ok) {
    ApAsyncMessage<Msg_VpView_ParticipantAdded> msg;
    msg->hLocation = apHandle();
    msg->hParticipant = hParticipant;
    if (getSelfParticipant() == hParticipant) {
      msg->bSelf = 1;
    }
    msg.Post();
  }

  if (ok) {
    ApAsyncMessage<Msg_VpView_ParticipantsChanged> msg;
    msg->hLocation = apHandle();
    msg->nCount = participants_.Count();
    msg.Post();
  }
  // ---------------

  return ok;
}

int Location::removeParticipant(const ApHandle& hParticipant)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " remove " ApHandleFormat "", ApHandlePrintf(apHandle()), ApHandlePrintf(hParticipant)));

  {
    Msg_VpView_ParticipantRemoved msg;
    msg.hLocation = apHandle();
    msg.hParticipant = hParticipant;
    msg.Send();
  }

  Participant* pParticipant = 0;
  if (participants_.Get(hParticipant, pParticipant)) {
    ok = participants_.Unset(hParticipant);
    if (ok) {
      delete pParticipant;
      pParticipant = 0;
    }
  } else {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "participants_.Unset() failed, part=" ApHandleFormat " unknown", ApHandlePrintf(hParticipant)));
  }

  if (ok) {
    Msg_VpView_ParticipantsChanged msg;
    msg.hLocation = apHandle();
    msg.nCount = participants_.Count();
    msg.Send();
  }

  return ok;
}

int Location::removeAllParticipants()
{
  int ok = 0;

  if (participants_.Count() == 0) {
    ok = 1;
  } else {

    while (participants_.Count() > 0) {
      ParticipantListIterator iter(participants_);
      ParticipantListNode* pNode = iter.Next();
      if (pNode != 0) {
        ApHandle hParticipant = pNode->Key();
        ok = removeParticipant(hParticipant);
      }
    }

  }
  
  return ok;
}

Participant* Location::findParticipant(const ApHandle& hParticipant)
{
  Participant* pParticipant = 0;
  
  participants_.Get(hParticipant, pParticipant);

  return pParticipant;
}

void Location::getParticipants(Apollo::ValueList& vlParticipants)
{
  ParticipantListIterator iter(participants_);
  for (ParticipantListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    vlParticipants.add(pNode->Key());
  }
}

//----------------------------------------------------------

void Location::sendPublicChat(const String& sText)
{
  int ok = 1;

  Msg_Protocol_SendPublicChat msg;
  msg.sProtocol = protocol();
  msg.hRoom = hRoom_;
  msg.sText = sText;
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_Protocol_SendPublicChat failed, %s", _sz(sLocationUrl_)); }
}

String Location::getNickname(const ApHandle& hParticipant)
{
  String sNickname;

  Participant* pParticipant = 0;
  participants_.Get(hParticipant, pParticipant);
  if (pParticipant != 0) {
    Apollo::ValueList lMimeTypes;
    String sMimeType;
    pParticipant->getDetailString(Msg_VpView_ParticipantDetail_Nickname, lMimeTypes, sNickname, sMimeType);
  }

  return sNickname;
}

void Location::onReceivePublicChat(const ApHandle& hParticipant, const String& sText, int nSec, int nMicroSec)
{
  ApHandle hChat = Apollo::newHandle();

  Participant* pParticipant = 0;
  participants_.Get(hParticipant, pParticipant);

  if (ApIsHandle(hParticipant) && pParticipant != 0) {
    String sNickname = getNickname(hParticipant);

    int bIsNew = 1;
    Apollo::TimeValue tvStamp;
    if (nSec <= 0) {
      tvStamp = Apollo::getNow();
    } else {
      tvStamp = Apollo::TimeValue(nSec, nMicroSec);
      bIsNew = 0;
    }

    String sChatline;
    {
      Msg_Vp_FilterPublicChat msg;
      msg.hLocation = hAp_;
      msg.hParticipant = hParticipant;
      msg.sText = sText;
      msg.Filter();

      sChatline = msg.sText;
    }

    if (sChatline) {
      Chat* pChat = new Chat(apHandle(), hParticipant, hChat, sNickname, sChatline, tvStamp);
      if (pChat != 0 && pParticipant != 0) {
        //pParticipant->onReceivePublicChat(sChatline);

        if (pParticipant->isSubscribedDetail(Msg_VpView_ParticipantDetail_PublicChat)) {
          pChat->send_VpView_LocationPublicChat(bIsNew);
        }

        // Store the thing
        int nMaxPublicChatLines = Apollo::getModuleConfig(MODULE_NAME, "Location/MaxChatLines", 100);
        if (nMaxPublicChatLines < 0) { nMaxPublicChatLines = 0; }
        if (nMaxPublicChatLines > 0) {
          lChats_.AddFirst(pChat);
        } // nMaxPublicChatLines

        cleanupChat();
      } // pChat
    } // is Chat

  } // pParticipant
}

void Location::onFilterPublicChat(const ApHandle& hParticipant, String& sText)
{
  String sLine = sText;
  String sCmd;
  sLine.nextToken(" ", sCmd);

  if (0) {
  } else if (sCmd == "/do") {
    sLine.trimWSP();

    Msg_VpView_LocationPublicAction msg;
    msg.hLocation = hAp_;
    msg.hParticipant = hParticipant;
    msg.sAction = sLine;
    msg.Send();

    // Translate sText
    sText = "*" + getNickname(hParticipant) + " " + sLine;
  }
}

void Location::replayChats(int nMaxAge, int nMaxLines, int nMaxData)
{
  cleanupChat();

  int nLines = 0;
  int nChars = 0;
  time_t tNow = Apollo::getNow().Sec();

  ChatList lChatOut;

  {
    Chat* pChat = 0;
    int bDone = 0;
    while ((pChat = lChats_.First()) != 0 && !bDone) {
      nLines++;
      nChars += pChat->getText().chars();
      int tTime = pChat->getTime().Sec();

      if (nMaxLines > 0 && nLines > nMaxLines) {
        bDone = 1;
      }
      if (nMaxData > 0 && nChars > nMaxData) {
        bDone = 1;
      }
      if (nMaxAge > 0 && tNow - tTime > nMaxAge) {
        bDone = 1;
      }

      if (!bDone)  {
        lChats_.Remove(pChat);
        lChatOut.AddFirst(pChat);
      }
    }
  }

  {
    Chat* pChat = 0;
    while ((pChat = lChatOut.First()) != 0) {
      pChat->send_VpView_LocationPublicChat(0);
      lChatOut.Remove(pChat);
      lChats_.AddFirst(pChat);
    }
  }
}

void Location::cleanupChat()
{
  int nMaxPublicChatLines = Apollo::getModuleConfig(MODULE_NAME, "Location/MaxChatLines", 100);
  if (nMaxPublicChatLines < 0) { nMaxPublicChatLines = 0; }
  int nMaxChatLineAge = Apollo::getModuleConfig(MODULE_NAME, "Location/MaxChatLineAge", 100);
  if (nMaxChatLineAge < 0) { nMaxChatLineAge = 0; }

  {
    while (lChats_.length() > nMaxPublicChatLines) {
      Chat* pOldest = lChats_.Last();
      if (pOldest) {
        lChats_.Remove(pOldest);
        delete pOldest;
        pOldest = 0;
      }
    } // while lChats_.length
  }

  {
    int bDone = 0;
    Apollo::TimeValue tvNow = Apollo::getNow();
    Apollo::TimeValue tvMin;
    tvMin = tvNow - Apollo::TimeValue(nMaxChatLineAge, 0);
    while (!bDone) {
      bDone = 1;
      Chat* pOldest = lChats_.Last();
      if (pOldest) {
        if (pOldest->getTime() < tvMin) {
          lChats_.Remove(pOldest);
          delete pOldest;
          pOldest = 0;
          bDone = 0;
        }
      }
    } // while lChats_.length
  }
}

//----------------------------------------------------------

String Location::state2String(State nState)
{
  switch (nState) {
  case State_NotEntered: return "NotEntered";
  case State_EnterRequested: return "EnterRequested";
  case State_Entering: return "Entering";
  case State_Entered: return "Entered";
  case State_LeaveRequested: return "LeaveRequested";
  case State_Leaving: return "Leaving";
  default: return "Unknown";
  }
}

void Location::setState(State nState)
{
  if (nState_ != nState) {
    lThingys_.beginChanges();
    nState_ = nState;
    lThingys_.setChanged(Msg_VpView_LocationDetail_State);
    lThingys_.endChanges();
  }
}

//----------------------------------------------------------

// @throws ApException
void Location::getDetailString(const String& sKey, String& sValue, String& sMimeType)
{
  LocationThingy* pThingy = lThingys_.getOrCreate(sKey);
  if (!pThingy) {
    throw ApException(LOG_CONTEXT, "unknown key=%s", _sz(sKey));
  } else {
    LocationThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      pProvider->getString(sValue, sMimeType);
    }
  }
}

int Location::subscribeDetail(const String& sKey, String& sValue, String& sMimeType)
{
  int ok = 0;

  ok = addSubscription(sKey);

  (void) getDetailString(sKey, sValue, sMimeType);

  return ok;
}

int Location::unsubscribeDetail(const String& sKey)
{
  int ok = 1;

  ok = removeSubscription(sKey);

  return ok;
}

int Location::addSubscription(const String& sKey)
{
  int ok = 1;

  Thingy* pThingy = lThingys_.getOrCreate(sKey);
  if (pThingy != 0) {
    pThingy->addSubscription();
  }

  return ok;
}

int Location::removeSubscription(const String& sKey)
{
  int ok = 1;

  Thingy* pThingy = lThingys_.FindByName(sKey);
  if (pThingy != 0) {
    pThingy->removeSubscription();
  }

  return ok;
}

//----------------------------------------------------------

void LocationUrlLocationThingyProvider::getString(String& sValue, String& sMimeType)
{
  if (pLocation_) {
    sValue = pLocation_->getUrl();
  }
}

void StateLocationThingyProvider::getString(String& sValue, String& sMimeType)
{
  if (pLocation_) {
    sValue = pLocation_->state2String(pLocation_->getState());
  }
}

//----------------------------

ThingyProvider* LocationThingyList::newProvider(const String& sKey)
{
  LocationThingyProvider* pProvider = 0;

  if (0) {
  } else if (sKey == Msg_VpView_LocationDetail_LocationUrl) { pProvider = new LocationUrlLocationThingyProvider();
  } else if (sKey == Msg_VpView_LocationDetail_State) { pProvider = new StateLocationThingyProvider();
  } else {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "unknown key=%s", _sz(sKey)));
  }

  if (pProvider) {
    pProvider->setLocation(pLocation_);
  }

  return pProvider;
}

void LocationThingyList::sendChanges(Apollo::ValueList& vlChanges)
{
  Msg_VpView_LocationDetailsChanged msg;
  msg.hLocation = pLocation_->apHandle();
  msg.vlKeys = vlChanges;
  msg.Send();
}

//----------------------------------------------------------

// @throws ApException
void Location::setPosition(Apollo::KeyValueList& kvParams)
{
  kvPosition_ = kvParams;

  String sX = kvParams[Msg_Vp_Position_X].getString();
  String sY = kvPosition_[Msg_Vp_Position_Y].getString();
  String sZ = kvPosition_[Msg_Vp_Position_Z].getString();
  if (sX) { Apollo::setModuleConfig(MODULE_NAME, "Location/DefaultX", sX); }
  if (sY) { Apollo::setModuleConfig(MODULE_NAME, "Location/DefaultY", sY); }
  if (sZ) { Apollo::setModuleConfig(MODULE_NAME, "Location/DefaultZ", sZ); }

  {
    Msg_Protocol_Room_BeginState msg;
    msg.sProtocol = protocol();
    msg.hRoom = getRoom();
    msg.Send();
  }

  {
    Msg_Protocol_Room_SetPosition msg;
    msg.sProtocol = protocol();
    msg.hRoom = getRoom();
    msg.kvParams = kvParams;
    msg.Send();
  }

  {
    Msg_Protocol_Room_CommitState msg;
    msg.sProtocol = protocol();
    msg.hRoom = getRoom();
    msg.Send();
  }
}

void Location::onGetPosition(Apollo::KeyValueList& kvParams)
{
  String sX = kvPosition_[Msg_Vp_Position_X].getString();
  if (!sX) { kvPosition_[Msg_Vp_Position_X].setString(Apollo::getModuleConfig(MODULE_NAME, "Location/DefaultX", "200")); }

  String sY = kvPosition_[Msg_Vp_Position_Y].getString();
  if (!sY) { kvPosition_[Msg_Vp_Position_Y].setString(Apollo::getModuleConfig(MODULE_NAME, "Location/DefaultY", "0")); }

  String sZ = kvPosition_[Msg_Vp_Position_Z].getString();
  if (!sZ) { kvPosition_[Msg_Vp_Position_Z].setString(Apollo::getModuleConfig(MODULE_NAME, "Location/DefaultZ", "0")); }

  kvParams = kvPosition_;
}

// @throws ApException
void Location::setCondition(Apollo::KeyValueList& kvParams)
{
  kvCondition_ = kvParams;

  {
    Msg_Protocol_Room_BeginState msg;
    msg.sProtocol = protocol();
    msg.hRoom = getRoom();
    msg.Send();
  }

  {
    Msg_Protocol_Room_SetCondition msg;
    msg.sProtocol = protocol();
    msg.hRoom = getRoom();
    msg.kvParams = kvParams;
    msg.Send();
  }

  {
    Msg_Protocol_Room_CommitState msg;
    msg.sProtocol = protocol();
    msg.hRoom = getRoom();
    msg.Send();
  }
}

void Location::onGetCondition(Apollo::KeyValueList& kvParams)
{
  kvParams = kvCondition_;
}

