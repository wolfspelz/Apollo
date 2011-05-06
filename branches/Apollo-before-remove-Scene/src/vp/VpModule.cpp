// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "URL.h"
#include "MsgSystem.h"
#include "MsgDB.h"
#include "MsgProtocol.h"
#include "VpModule.h"
#include "VpModuleTester.h"
#include "Local.h"

//----------------------------------------------------------

void DisplayProfile::setParticipantDetail(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  DisplayProfileParticipantDetail* pDetail = lDetails_.FindByName(sKey);
  if (!pDetail) {
    pDetail = new DisplayProfileParticipantDetail(sKey);
    if (pDetail) {
      lDetails_.Add(pDetail);
    }
  }
  if (pDetail) {
    pDetail->setMimeTypes(vlMimeTypes);
  }
}

//----------------------------------------------------------

DisplayProfile* VpModule::findProfile(const ApHandle& hProfile)
{
  DisplayProfile* pResult = 0;
  
  participantSubscriptionProfiles_.Get(hProfile, pResult);

  return pResult;
}

//----------------------

Context* VpModule::findContext(const ApHandle& hContext)
{
  Context* pResult = 0;
  
  contexts_.Get(hContext, pResult);

  return pResult;
}

Context* VpModule::findContextByMapping(const ApHandle& hMapping)
{
  Context* pResult = 0;

  ContextListIterator iter(contexts_);
  for (ContextListNode* pNode = 0; pNode = iter.Next(); ) {
    Context* pContext = pNode->Value();
    if (pContext != 0) {
      if (pContext->getMapping() == hMapping) {
        pResult = pContext;
        break;
      }
    }
  }

  return pResult;
}

Location* VpModule::findLocation(const ApHandle& hLocation)
{
  Location* pResult = 0;
  
  locations_.Get(hLocation, pResult);

  return pResult;
}

Location* VpModule::findLocationByUrl(const String& sUrl)
{
  Location* pResult = 0;
  
  LocationListIterator iter(locations_);
  for (LocationListNode* pNode = 0; pNode = iter.Next(); ) {
    Location* pLocation = pNode->Value();
    if (pLocation != 0) {
      if (pLocation->getUrl() == sUrl) {
        pResult = pLocation;
        break;
      }
    }
  }

  return pResult;
}

Location* VpModule::findLocationByContext(const ApHandle& hContext)
{
  Location* pResult = 0;
  
  LocationListIterator iter(locations_);
  for (LocationListNode* pNode = 0; pNode = iter.Next(); ) {
    Location* pLocation = pNode->Value();
    if (pLocation != 0) {
      if (pLocation->hasContext(hContext)) {
        pResult = pLocation;
        break;
      }
    }
  }

  return pResult;
}

Location* VpModule::findLocationByRoom(const ApHandle& hRoom)
{
  Location* pResult = 0;

  LocationListIterator iter(locations_);
  for (LocationListNode* pNode = 0; pNode = iter.Next(); ) {
    Location* pLocation = pNode->Value();
    if (pLocation != 0) {
      if (pLocation->getRoom() == hRoom) {
        pResult = pLocation;
        break;
      }
    }
  }

  return pResult;
}

Participant* VpModule::findParticipant(const ApHandle& hParticipant)
{
  Participant* pResult = 0;

  Location* pLocation = findLocationByParticipant(hParticipant);
  if (pLocation) {
    pResult = pLocation->findParticipant(hParticipant);
  }

  return pResult;
}

Participant* VpModule::findParticipantInRoom(const ApHandle& hRoom, const ApHandle& hParticipant)
{
  Participant* pResult = 0;

  Location* pLocation = findLocationByRoom(hRoom);
  if (pLocation) {
    pResult = pLocation->findParticipant(hParticipant);
  }

  return pResult;
}

ParticipantPointerList VpModule::findParticipantsByIdentityUrl(const String& sUrl)
{
  ParticipantPointerList result;

  Apollo::ValueList vlHandles;
  getParticipantHandlesByIdentity(sUrl, vlHandles);
  for (Apollo::ValueElem* e = 0; (e = vlHandles.nextElem(e)) != 0; ) {
    ApHandle hLocation = findLocationHandleByParticipant(e->getHandle());
    if (ApIsHandle(hLocation)) {
      Location* pLocation = 0;
      if (locations_.Get(hLocation, pLocation)) {
        if (pLocation) {
          Participant* pParticipant = pLocation->findParticipant(e->getHandle());
          if (pParticipant) {
            result.Set(e->getHandle(), pParticipant);
          }
        }
      }
    }
  }

  return result;
}

//----------------------

void VpModule::setParticipantLocationMapping(const ApHandle& hParticipant, const ApHandle& hLocation)
{
  participant2Location_.Set(hParticipant, hLocation);
}

void VpModule::unsetParticipantLocationMapping(const ApHandle& hParticipant, const ApHandle& hLocation)
{
  participant2Location_.Unset(hParticipant);
}

ApHandle VpModule::findLocationHandleByParticipant(const ApHandle& hParticipant)
{
  ApHandleTreeNode<ApHandle>* pNode = participant2Location_.Find(hParticipant);
  if (pNode != 0) {
    return pNode->Value();
  } else {
    return ApNoHandle;
  }
}

Location* VpModule::findLocationByParticipant(const ApHandle& hParticipant)
{
  Location* pLocation = 0;

  ApHandle hLocation = findLocationHandleByParticipant(hParticipant);
  if (ApIsHandle(hLocation)) {
    locations_.Get(hLocation, pLocation);
  }

  return pLocation;
}

//----------------------

void VpModule::setIdentityParticipantMapping(const String& sUrl, const ApHandle& hParticipant)
{
  {
    ApHandleTreeNode<String>* pNode = participant2Identity_.Find(hParticipant);
    if (pNode != 0) {
      if (pNode->Value() != sUrl) {
        String sPreviousUrl = pNode->Value();
        unsetIdentityParticipantMapping(sPreviousUrl, hParticipant);
      }
    }
  }

  participant2Identity_.Set(hParticipant, sUrl);

  {
    StringTreeNode<ApHandleTree<int>>* pNode = identity2Participant_.Find(sUrl);
    if (pNode == 0) {
      ApHandleTree<int> handles;
      handles.Set(hParticipant, 1);
      identity2Participant_.Set(sUrl, handles);
    } else {
      ApHandleTree<int>& handles = pNode->Value();
      handles.Set(hParticipant, 1);
    }
  }
}

void VpModule::unsetIdentityParticipantMapping(const String& sUrl, const ApHandle& hParticipant)
{
  participant2Identity_.Unset(hParticipant);

  StringTreeNode<ApHandleTree<int>>* pNode = identity2Participant_.Find(sUrl);
  if (pNode != 0) {
    ApHandleTree<int>& handles = pNode->Value();
    handles.Unset(hParticipant);
    if (handles.Count() == 0) {
      identity2Participant_.Unset(sUrl);
    }
  }
}

void VpModule::removeParticipantFromIdentityParticipantMapping(const ApHandle& hParticipant)
{
  {
    ApHandleTreeNode<String>* pNode = participant2Identity_.Find(hParticipant);
    if (pNode != 0) {
      String sPreviousUrl = pNode->Value();
      unsetIdentityParticipantMapping(sPreviousUrl, hParticipant);
    }
  }

  participant2Identity_.Unset(hParticipant);
}

void VpModule::getParticipantHandlesByIdentity(const String& sUrl, Apollo::ValueList& vlHandles)
{
  StringTreeNode<ApHandleTree<int>>* pNode = identity2Participant_.Find(sUrl);
  if (pNode != 0) {
    ApHandleTree<int>& handles = pNode->Value();
    ApHandleTreeNode<int>* pHandleNode = 0;
    for (ApHandleTreeIterator<int> iter(handles); (pHandleNode = iter.Next()) != 0; ) {
      vlHandles.add(pHandleNode->Key());
    }
  }
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(VpModule, System_AfterLoadModules)
{
  AP_UNUSED_ARG(pMsg);

  Msg_DB_Open msg;
  msg.sName = DB_NAME;
  int ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "VpModule::System_AfterLoadModules", "Msg_DB_Open %s failed", StringType(msg.sName)));
  }

  sExternUrlAddress_ = Apollo::getConfig("Server/HTTP/Address", "localhost");
  nExternUrlPort_ = Apollo::getConfig("Server/HTTP/Port", 23761);
}

AP_MSG_HANDLER_METHOD(VpModule, System_BeforeUnloadModules)
{
  AP_UNUSED_ARG(pMsg);

  bInShutdown_ = true;

  Msg_DB_Close msg;
  msg.sName = DB_NAME;
  int ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "VpModule::System_BeforeUnloadModules", "Msg_DB_Close %s failed", StringType(msg.sName)));
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Vpi_LocationXmlResponse)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vpi_LocationXmlResponse", "url=%s ", StringType(pMsg->sDocumentUrl)));

  Context* pContext = findContextByMapping(pMsg->hRequest);

  if (pContext) {
    if (!pMsg->bSuccess) {
      apLog_Warning((LOG_CHANNEL, "VpModule::Vpi_LocationXmlResponse", "Msg_Vpi_LocationXmlResponse not successful for %s", StringType(pMsg->sDocumentUrl)));
    } else {
      ok = pContext->setLocationXml(pMsg->sLocationXml);
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "VpModule::Vpi_LocationXmlResponse", "pContext->setLocationXml() failed for %s", StringType(pContext->getDocumentUrl())));
      } else {

        Location* pNewLocation = findLocationByUrl(pContext->getLocationUrl());
        Location* pOldLocation = findLocationByContext(pContext->apHandle());
        if (pNewLocation == 0) {
          ApHandle hLocation = Apollo::newHandle();

          Msg_Vp_CreateLocation msg;
          msg.hLocation = hLocation;
          msg.sLocationUrl = pContext->getLocationUrl();
          ok = msg.Request();
          if (!ok) {
            apLog_Error((LOG_CHANNEL, "VpModule::Vpi_LocationXmlResponse", "Msg_Vp_CreateLocation failed for loc=%s url=%s", StringType(pContext->getLocationUrl()), StringType(pContext->getDocumentUrl())));
          } else {
            locations_.Get(hLocation, pNewLocation);
          }
        }

        if (pNewLocation == 0) {
          apLog_Error((LOG_CHANNEL, "VpModule::Vpi_LocationXmlResponse", "No location for loc=%s url=%s", StringType(pContext->getLocationUrl()), StringType(pContext->getDocumentUrl())));
        } else {

          if (pNewLocation == pOldLocation) {
            // context remains at location
          } else {
            // move context to new location

            if (pOldLocation != 0) {
              Msg_Vp_RemoveLocationContext msg;
              msg.hLocation = pOldLocation->apHandle();
              msg.hContext = pContext->apHandle();
              ok = msg.Request();
              if (!ok) {
                apLog_Error((LOG_CHANNEL, "VpModule::Vpi_LocationXmlResponse", "Msg_Vp_RemoveLocationContext failed for loc=%s url=%s", StringType(pContext->getLocationUrl()), StringType(pContext->getDocumentUrl())));
              }
            }

            {
              Msg_Vp_AddLocationContext msg;
              msg.hLocation = pNewLocation->apHandle();
              msg.hContext = pContext->apHandle();
              ok = msg.Request();
              if (!ok) {
                apLog_Error((LOG_CHANNEL, "VpModule::Vpi_LocationXmlResponse", "Msg_Vp_AddLocationContext failed for loc=%s url=%s", StringType(pContext->getLocationUrl()), StringType(pContext->getDocumentUrl())));
              }
            }

          } // pNewLocation != pOldLocation

        } // pNewLocation != 0

      } // pContext->setLocationXml
    } // bSuccess
  } // pContext
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_NavigateContext)
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_NavigateContext", "ctxt=" ApHandleFormat " url=%s", ApHandleType(pMsg->hContext), StringType(pMsg->sUrl)));

  Context* pContext = 0;
  if (contexts_.Get(pMsg->hContext, pContext)) {
    // xists
  } else {
    Msg_Vp_CreateContext msg;
    msg.hContext = pMsg->hContext;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Vp_NavigateContext", "Msg_Vp_CreateContext failed for ctxt=" ApHandleFormat " url=%s", ApHandleType(pMsg->hContext), StringType(pMsg->sUrl)));
    } else {
      contexts_.Get(pMsg->hContext, pContext);
    }
  }

  if (pContext != 0) {
    ok = pContext->navigate(pMsg->sUrl); // resolves async
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Vp_NavigateContext", "pUrlContext->changeUrl failed for ctxt=" ApHandleFormat " url=%s", ApHandleType(pMsg->hContext), StringType(pMsg->sUrl)));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_CloseContext)
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_CloseContext", "ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));

  Context* pContext = 0;
  if (contexts_.Get(pMsg->hContext, pContext)) {

    Msg_Vp_DestroyContext msg;
    msg.hContext = pMsg->hContext;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Vpi_LocationXmlResponse", "Msg_Vp_DestroyContext failed for ctxt=" ApHandleFormat " previousurl=%s", ApHandleType(pMsg->hContext), StringType(pContext->getDocumentUrl())));
    }

  } else {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_CloseContext", "unknown ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_ContextVisibility)
{
  int ok = 1;

  Context* pContext = 0;
  if (contexts_.Get(pMsg->hContext, pContext)) {
    pContext->setVisibility(pMsg->bVisible);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_ContextPosition)
{
  int ok = 1;

  Context* pContext = 0;
  if (contexts_.Get(pMsg->hContext, pContext)) {
    pContext->setPosition(pMsg->nX, pMsg->nY);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_ContextSize)
{
  int ok = 1;

  Context* pContext = 0;
  if (contexts_.Get(pMsg->hContext, pContext)) {
    pContext->setSize(pMsg->nWidth, pMsg->nHeight);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_CreateContext)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_CreateContext", "ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));

  Context* pContext = 0;
  if (contexts_.Get(pMsg->hContext, pContext)) {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_CreateContext", "Context already exists, ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
  } else {
    pContext = new Context(pMsg->hContext);
    if (pContext != 0) {
      contexts_.Set(pMsg->hContext, pContext);
      ok = 1;
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_DestroyContext)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_DestroyContext", "ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));

  Context* pContext = 0;
  if (contexts_.Get(pMsg->hContext, pContext)) {

    // Remove destroyed context from all locations (should be only one)
    LocationListIterator iter(locations_);
    for (LocationListNode* pNode = 0; pNode = iter.Next(); ) {
      Location* pLocation = pNode->Value();
      if (pLocation != 0) {
        if (pLocation->hasContext(pMsg->hContext)) {
          int iok = pLocation->removeContext(pMsg->hContext);
          if (!iok) {
            apLog_Error((LOG_CHANNEL, "VpModule::Vp_DestroyContext", "pLocation->removeContext() failed, ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
          }
        }
      }
    }

    if (contexts_.Unset(pMsg->hContext)) {
      delete pContext;
      pContext = 0;
    }
    ok = 1;
  } else {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_DestroyContext", "contexts_.Get() failed, ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_CreateLocation)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_CreateLocation", "loc=" ApHandleFormat "", ApHandleType(pMsg->hLocation)));

  Location* pLocation = 0;
  if (locations_.Get(pMsg->hLocation, pLocation)) {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_CreateLocation", "Location already exists, loc=" ApHandleFormat " url=%s", ApHandleType(pMsg->hLocation), StringType(pMsg->sLocationUrl)));
  } else {
    pLocation = new Location(pMsg->hLocation, pMsg->sLocationUrl);
    if (pLocation != 0) {
      locations_.Set(pMsg->hLocation, pLocation);
      ok = 1;

      Msg_VpView_LocationsChanged msg;
      msg.Send();
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_DestroyLocation)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_DestroyLocation", "loc=" ApHandleFormat "", ApHandleType(pMsg->hLocation)));

  Location* pLocation = 0;
  if (locations_.Get(pMsg->hLocation, pLocation)) {

    // Remove all contexts from destroyed location
    ok = pLocation->removeAllContexts();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Vp_DestroyLocation", "pLocation->removeAllContexts() failed, loc=" ApHandleFormat "", ApHandleType(pMsg->hLocation)));
    }

    if (locations_.Unset(pMsg->hLocation)) {
      delete pLocation;
      pLocation = 0;
    }
    ok = 1;

    Msg_VpView_LocationsChanged msg;
    msg.Send();
  } else {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_DestroyLocation", "locations_.Get() failed, loc=" ApHandleFormat "", ApHandleType(pMsg->hLocation)));
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_AddLocationContext)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_AddLocationContext", "loc=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(pMsg->hLocation), ApHandleType(pMsg->hContext)));

  Location* pLocation = 0;
  locations_.Get(pMsg->hLocation, pLocation);

  if (pLocation == 0) {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_AddLocationContext", "loc=" ApHandleFormat " invalid", ApHandleType(pMsg->hLocation)));
  } else {
    ok = pLocation->addContext(pMsg->hContext);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Vp_AddLocationContext", "pLocation->addContext() failed loc=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(pMsg->hLocation), ApHandleType(pMsg->hContext)));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_RemoveLocationContext)
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_RemoveLocationContext", "loc=" ApHandleFormat "", ApHandleType(pMsg->hLocation)));

  Location* pLocation = 0;  
  locations_.Get(pMsg->hLocation, pLocation);

  if (pLocation == 0) {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_RemoveLocationContext", "loc=" ApHandleFormat " invalid", ApHandleType(pMsg->hLocation)));
  } else {
    ok = pLocation->removeContext(pMsg->hContext);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Vp_RemoveLocationContext", "pLocation->removeContext() failed loc=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(pMsg->hLocation), ApHandleType(pMsg->hContext)));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_EnterLocation)
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_EnterLocation", "" ApHandleFormat "", ApHandleType(pMsg->hLocation)));

  Location* pLocation = 0;  
  locations_.Get(pMsg->hLocation, pLocation);

  if (pLocation == 0) {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_EnterLocation", "loc=" ApHandleFormat " invalid", ApHandleType(pMsg->hLocation)));
  } else {

    ok = pLocation->enter();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Vp_EnterLocation", "pLocation->enter failed, %s", StringType(pLocation->getUrl())));
    }

  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_LeaveLocation)
{
  int ok = 1;
  apLog_Verbose((LOG_CHANNEL, "VpModule::Vp_LeaveLocation", "" ApHandleFormat "", ApHandleType(pMsg->hLocation)));

  Location* pLocation = 0;  
  locations_.Get(pMsg->hLocation, pLocation);

  if (pLocation == 0) {
    apLog_Error((LOG_CHANNEL, "VpModule::Vp_LeaveLocation", "loc=" ApHandleFormat " invalid", ApHandleType(pMsg->hLocation)));
  } else {
    ok = pLocation->leave();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Vp_LeaveLocation", "pLocation->leave failed, %s", StringType(pLocation->getUrl())));
    }

  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Online)
{
  lOnlineProtocols_[pMsg->sProtocol] = 1;

  if (locations_.Count() > 0) {
    apLog_Info((LOG_CHANNEL, "VpModule::Protocol_Online", "re-enter %d locations", locations_.Count()));

    LocationListIterator iter(locations_);
    for (LocationListNode* pNode = 0; pNode = iter.Next(); ) {
      Location* pLocation = pNode->Value();
      if (pLocation != 0) {
        (void) pLocation->onProtocolOnline();
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Offline)
{
  lOnlineProtocols_[pMsg->sProtocol] = 0;

  if (locations_.Count() > 0) {
    apLog_Info((LOG_CHANNEL, "VpModule::Protocol_Offline", "Setting %d locations to wait for online", locations_.Count()));

    LocationListIterator iter(locations_);
    for (LocationListNode* pNode = 0; pNode = iter.Next(); ) {
      Location* pLocation = pNode->Value();
      if (pLocation != 0) {
        (void) pLocation->onProtocolOffline();
      }
    }
  }
}

//----------------------

AP_MSG_HANDLER_METHOD(VpModule, Protocol_EnteringRoom)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    pLocation->entering();
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_EnterRoomComplete)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    pLocation->enterComplete();
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_LeavingRoom)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    pLocation->leaving();
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_LeaveRoomComplete)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    pLocation->leaveComplete();

    // Destroy only if no enter in leaveComplete due to bEnterAfterLeaveComplete_
    if (pLocation->getState() == Location::State_NotEntered) {
      Msg_Vp_DestroyLocation msg;
      msg.hLocation = pLocation->apHandle();
      if (!msg.Request()) { throw ApException("Msg_Vp_DestroyLocation failed loc=" ApHandleFormat "", ApHandleType(msg.hLocation)); }
    }
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_ParticipantEntered)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    int ok = pLocation->addParticipant(pMsg->hParticipant);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Protocol_ParticipantEntered", "" ApHandleFormat "->addParticipant(" ApHandleFormat ") failed", ApHandleType(pLocation->apHandle()), ApHandleType(pMsg->hParticipant)));
    } else {
      setParticipantLocationMapping(pMsg->hParticipant, pLocation->apHandle());
    }

    if (!pMsg->sNickname.empty()) {
      Participant* pParticipant = pLocation->findParticipant(pMsg->hParticipant);
      if (pParticipant != 0) {
        pParticipant->onProtocolDataBegin();
        pParticipant->onProtocolNickname(pMsg->sNickname);
        pParticipant->onProtocolDataEnd();
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_ParticipantLeft)
{
  removeParticipantFromIdentityParticipantMapping(pMsg->hParticipant);

  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    int ok = pLocation->removeParticipant(pMsg->hParticipant);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VpModule::Protocol_ParticipantLeft", "" ApHandleFormat "->removeParticipant(" ApHandleFormat ") failed", ApHandleType(pLocation->apHandle()), ApHandleType(pMsg->hParticipant)));
    } else {
      unsetParticipantLocationMapping(pMsg->hParticipant, pLocation->apHandle());
    }
  } else {
    apLog_Warning((LOG_CHANNEL, "VpModule::Protocol_ParticipantLeft", "No location for room=" ApHandleFormat "", ApHandleType(pMsg->hRoom)));
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_ParticipantSelf)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    pLocation->setSelfParticipant(pMsg->hParticipant);
  }
}

//----------------------

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_Begin)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolDataBegin();
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_Status)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolStatus(pMsg->sStatus);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_StatusMessage)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolMessage(pMsg->sMessage);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_JabberId)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolJabberId(pMsg->sJabberId);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_MucUser)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolMucUser(pMsg->sJabberId, pMsg->sAffiliation, pMsg->sRole);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_VpIdentity)
{
  setIdentityParticipantMapping(pMsg->sSrc, pMsg->hParticipant);

  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolIdentity(pMsg->sId, pMsg->sSrc, pMsg->sDigest);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_Position)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolPosition(pMsg->kvParams);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_Condition)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolCondition(pMsg->kvParams);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_FirebatFeatures)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolFirebatFeatures(pMsg->sFeatures);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Participant_End)
{
  Participant* pParticipant = findParticipantInRoom(pMsg->hRoom, pMsg->hParticipant);
  if (pParticipant) {
    pParticipant->onProtocolDataEnd();
  }
}

//----------------------

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Room_GetIdentity)
{
  pMsg->sUrl = Apollo::getModuleConfig(MODULE_NAME, "Identity/Url", "");
  pMsg->sDigest = Apollo::getModuleConfig(MODULE_NAME, "Identity/Digest", "");
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Room_GetPosition)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    pLocation->onGetPosition(pMsg->kvParams);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Protocol_Room_GetCondition)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    pLocation->onGetCondition(pMsg->kvParams);
  }
}

//----------------------

AP_MSG_HANDLER_METHOD(VpModule, Protocol_ReceivePublicChat)
{
  Location* pLocation = findLocationByRoom(pMsg->hRoom);
  if (pLocation) {
    pLocation->onReceivePublicChat(pMsg->hParticipant, pMsg->sText, pMsg->nSec, pMsg->nMicroSec);
  }
}

//----------------------

AP_MSG_HANDLER_METHOD(VpModule, Identity_ContainerBegin)
{
  ParticipantPointerList list = findParticipantsByIdentityUrl(pMsg->sUrl);
  ParticipantPointerListIterator iter(list);
  for (ParticipantPointerListNode* pNode = 0; pNode = iter.Next(); ) {
    Participant* pParticipant = pNode->Value();
    if (pParticipant != 0) {
      pParticipant->onIdentityContainerBegin(pMsg->sUrl);
    }
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Identity_ItemAdded)
{
  ParticipantPointerList list = findParticipantsByIdentityUrl(pMsg->sUrl);
  ParticipantPointerListIterator iter(list);
  for (ParticipantPointerListNode* pNode = 0; pNode = iter.Next(); ) {
    Participant* pParticipant = pNode->Value();
    if (pParticipant != 0) {
      pParticipant->onIdentityItemAdded(pMsg->sUrl, pMsg->sId);
    }
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Identity_ItemChanged)
{
  ParticipantPointerList list = findParticipantsByIdentityUrl(pMsg->sUrl);
  ParticipantPointerListIterator iter(list);
  for (ParticipantPointerListNode* pNode = 0; pNode = iter.Next(); ) {
    Participant* pParticipant = pNode->Value();
    if (pParticipant != 0) {
      pParticipant->onIdentityItemChanged(pMsg->sUrl, pMsg->sId);
    }
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Identity_ItemRemoved)
{
  ParticipantPointerList list = findParticipantsByIdentityUrl(pMsg->sUrl);
  ParticipantPointerListIterator iter(list);
  for (ParticipantPointerListNode* pNode = 0; pNode = iter.Next(); ) {
    Participant* pParticipant = pNode->Value();
    if (pParticipant != 0) {
      pParticipant->onIdentityItemRemoved(pMsg->sUrl, pMsg->sId);
    }
  }
}

AP_MSG_HANDLER_METHOD(VpModule, Identity_ContainerEnd)
{
  ParticipantPointerList list = findParticipantsByIdentityUrl(pMsg->sUrl);
  ParticipantPointerListIterator iter(list);
  for (ParticipantPointerListNode* pNode = 0; pNode = iter.Next(); ) {
    Participant* pParticipant = pNode->Value();
    if (pParticipant != 0) {
      pParticipant->onIdentityContainerEnd(pMsg->sUrl);
    }
  }
}

//----------------------

AP_MSG_HANDLER_METHOD(VpModule, Identity_ItemDataAvailable)
{
  ParticipantPointerList list = findParticipantsByIdentityUrl(pMsg->sUrl);
  ParticipantPointerListIterator iter(list);
  for (ParticipantPointerListNode* pNode = 0; pNode = iter.Next(); ) {
    Participant* pParticipant = pNode->Value();
    if (pParticipant != 0) {
      pParticipant->onIdentityItemDataAvailable(pMsg->sUrl, pMsg->sId, pMsg->sType, pMsg->sMimeType, pMsg->sSrc, pMsg->nSize);
    }
  }
}

//----------------------

AP_MSG_HANDLER_METHOD(VpModule, VpView_GetLocations)
{
  LocationListIterator iter(locations_);
  for (LocationListNode* pNode = 0; pNode = iter.Next(); ) {
    Location* pLocation = pNode->Value();
    if (pLocation != 0) {
      pMsg->vlLocations.add(pLocation->apHandle());
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_GetLocationDetail)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) {
    pLocation->getDetailString(pMsg->sKey, pMsg->sValue, pMsg->sMimeType);
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_SubscribeLocationDetail)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) {
    if (!pLocation->subscribeDetail(pMsg->sKey, pMsg->sValue, pMsg->sMimeType)) { throw ApException("pLocation->subscribeDetail() failed: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hLocation), StringType(pMsg->sKey)); }
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_UnsubscribeLocationDetail)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) { 
    if (!pLocation->unsubscribeDetail(pMsg->sKey)) { throw ApException("pLocation->unsubscribeDetail() failed: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hLocation), StringType(pMsg->sKey)); }
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_GetLocationContexts)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) {
    pLocation->getContexts(pMsg->vlContexts);
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_GetContextDetail)
{
  Context* pContext = findContext(pMsg->hContext);
  if (pContext) {
    pContext->getDetailString(pMsg->sKey, pMsg->sValue, pMsg->sMimeType);
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_SubscribeContextDetail)
{
  Context* pContext = findContext(pMsg->hContext);
  if (pContext) {
    if (!pContext->subscribeDetail(pMsg->sKey, pMsg->sValue, pMsg->sMimeType)) { throw ApException("pContext->subscribeDetail() failed: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hContext), StringType(pMsg->sKey)); }
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_UnsubscribeContextDetail)
{
  Context* pContext = findContext(pMsg->hContext);
  if (pContext) { 
    if (!pContext->unsubscribeDetail(pMsg->sKey)) { throw ApException("pContext->unsubscribeDetail() failed: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hContext), StringType(pMsg->sKey)); }
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_GetParticipants)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) {
    pLocation->getParticipants(pMsg->vlParticipants);
    pMsg->hSelf = pLocation->getSelfParticipant();
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_ReplayLocationPublicChat)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) {
    pLocation->replayChats(pMsg->nMaxAge, pMsg->nMaxLines, pMsg->nMaxData);
  }
  pMsg->apStatus = ApMessage::Ok;
}

//-----------------------------

AP_MSG_HANDLER_METHOD(VpModule, VpView_GetParticipantDetailString)
{
  Participant* pParticipant = findParticipant(pMsg->hParticipant);
  if (pParticipant) {
    if (!pParticipant->getDetailString(pMsg->sKey, pMsg->vlMimeTypes, pMsg->sValue, pMsg->sMimeType)) { throw ApException("Not available: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hParticipant), StringType(pMsg->sKey)); }
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_GetParticipantDetailData)
{
  Participant* pParticipant = findParticipant(pMsg->hParticipant);
  if (pParticipant) {
    if (!pParticipant->getDetailData(pMsg->sKey, pMsg->vlMimeTypes, pMsg->sbData, pMsg->sMimeType, pMsg->sSource)) { throw ApException("Not available: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hParticipant), StringType(pMsg->sKey)); }
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_GetParticipantDetailRef)
{
  Participant* pParticipant = findParticipant(pMsg->hParticipant);
  if (pParticipant) {
    if (!pParticipant->getDetailRef(pMsg->sKey, pMsg->vlMimeTypes, pMsg->sUrl, pMsg->sMimeType)) { throw ApException("Not available: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hParticipant), StringType(pMsg->sKey)); }
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_SubscribeParticipantDetail)
{
  Participant* pParticipant = findParticipant(pMsg->hParticipant);
  if (pParticipant) {
    if (!pParticipant->subscribeDetail(pMsg->sKey, pMsg->vlMimeTypes)) { throw ApException("pParticipant->subscribeDetail() failed: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hParticipant), StringType(pMsg->sKey)); }
    pMsg->bAvailable = pParticipant->isDetailAvailable(pMsg->sKey, pMsg->vlMimeTypes);
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_UnsubscribeParticipantDetail)
{
  Participant* pParticipant = findParticipant(pMsg->hParticipant);
  if (pParticipant) { 
    if (!pParticipant->unsubscribeDetail(pMsg->sKey, pMsg->vlMimeTypes)) { throw ApException("pParticipant->unsubscribeDetail() failed: part=" ApHandleFormat " key=%s", ApHandleType(pMsg->hParticipant), StringType(pMsg->sKey)); }
  }
  pMsg->apStatus = ApMessage::Ok;
}

//-----------------------------

AP_MSG_HANDLER_METHOD(VpModule, VpView_Profile_Create)
{
  DisplayProfile* pProfile = 0;
  if (participantSubscriptionProfiles_.Get(pMsg->hProfile, pProfile)) {
    throw ApException("profiles_.Get() profile=" ApHandleFormat " already exists", ApHandleType(pMsg->hProfile));
  }

  pProfile = new DisplayProfile(pMsg->hProfile);
  if (pProfile) {
    participantSubscriptionProfiles_.Set(pMsg->hProfile, pProfile);
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_Profile_SetParticipantDetail)
{
  DisplayProfile* pProfile = findProfile(pMsg->hProfile);
  if (pProfile) {
    pProfile->setParticipantDetail(pMsg->sKey, pMsg->vlMimeTypes);
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_Profile_GetParticipantDetails)
{
  //pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_Profile_SubscribeParticipantDetails)
{
  //pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_Profile_UnsubscribeParticipantDetails)
{
  //pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, VpView_Profile_Delete)
{
  DisplayProfile* pProfile = findProfile(pMsg->hProfile);
  if (pProfile) {
    if (participantSubscriptionProfiles_.Unset(pMsg->hProfile)) {
      delete pProfile;
      pProfile = 0;
    }
  }

  pMsg->apStatus = ApMessage::Ok;
}

//-----------------------------

AP_MSG_HANDLER_METHOD(VpModule, Vp_SendPublicChat)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) { pLocation->sendPublicChat(pMsg->sText); }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_SendPosition)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) { pLocation->setPosition(pMsg->kvParams); }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(VpModule, Vp_SendCondition)
{
  Location* pLocation = findLocation(pMsg->hLocation);
  if (pLocation) { pLocation->setCondition(pMsg->kvParams); }
  pMsg->apStatus = ApMessage::Ok;
}

//-----------------------------

#define VpModule_Server_HttpRequest_sUriPrefix "/" MODULE_NAME

String VpModule::getItemDataExternUrl(const String& sIdentityUrl, const String& sItemId)
{
  //String sUrl;
  //sUrl.appendf("http://%s:%d" VpModule_Server_HttpRequest_sUriPrefix "?%s|%s|%s", StringType(sExternUrlAddress_), nExternUrlPort_, StringType(sIdentityUrl), StringType(sItemId), StringType(Apollo::getUniqueId()));
  //return sUrl;

  Apollo::UrlBuilder url;
  url.setHost(sExternUrlAddress_);
  url.setPort(nExternUrlPort_);
  url.setPath("/");
  url.setFile(MODULE_NAME);
  String sQuery; sQuery.appendf("%s|%s|%s", StringType(sIdentityUrl), StringType(sItemId), StringType(Apollo::getUniqueId()));
  url.setQuery(sQuery);
  return url();
}

AP_MSG_HANDLER_METHOD(VpModule, Server_HttpRequest)
{
  if (Apollo::getModuleConfig(MODULE_NAME, "HTTP/Enabled", 1) && pMsg->sUri.startsWith(VpModule_Server_HttpRequest_sUriPrefix)) {
    try {
      String sQuery = pMsg->sUri;
      String sBase; sQuery.nextToken("?", sBase);
      sQuery.unescape(String::EscapeURL);

      String sUrl;
      String sId;
      sQuery.nextToken("|", sUrl);
      sQuery.nextToken("|", sId);

      if (sUrl.empty() || sId.empty()) { throw ApException("url=%s or id=%s empty: uri=%s", StringType(sUrl), StringType(sId), StringType(pMsg->sUri)); }

      Msg_Identity_IsItemDataAvailable msgIIDA;
      msgIIDA.sUrl = sUrl;
      msgIIDA.sId = sId;
      if (!msgIIDA.Request()) { throw ApException("Msg_Identity_IsItemDataAvailable failed: url=%s id=%s", StringType(sUrl), StringType(sId)); }
      if (msgIIDA.bAvailable) {

        // Return data
        Msg_Identity_GetItemData msgIGID;
        msgIGID.sUrl = sUrl;
        msgIGID.sId = sId;
        if (!msgIGID.Request()) { throw ApException("Msg_Identity_GetItemData failed: url=%s id=%s", StringType(sUrl), StringType(sId)); }

        Msg_Server_HttpResponse msgSHR;
        msgSHR.hConnection = pMsg->hConnection;
        msgSHR.kvHeader.add("Content-type", msgIGID.sMimeType);
        msgSHR.kvHeader.add("Pragma", "no-cache");
        msgSHR.kvHeader.add("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
        msgSHR.kvHeader.add("Expires", "Thu, 19 Nov 1981 08:52:00 GMT");
        msgSHR.sbBody = msgIGID.sbData;
        if (!msgSHR.Request()) { throw ApException("Msg_Server_HttpResponse failed: conn=" ApHandleFormat "", ApHandleType(msgSHR.hConnection)); }

      } else {

        // Return item src URL
        Msg_Identity_GetItem msgIGI;
        msgIGI.sUrl = sUrl;
        msgIGI.sId = sId;
        if (!msgIGI.Request()) { throw ApException("Msg_Identity_GetItem failed: url=%s id=%s", StringType(sUrl), StringType(sId)); }

        Msg_Server_HttpResponse msgSHR;
        msgSHR.hConnection = pMsg->hConnection;
        msgSHR.nStatus = 302;
        msgSHR.sMessage = "Found";
        msgSHR.kvHeader.add("Location", msgIGI.sSrc);
        if (!msgSHR.Request()) { throw ApException("Msg_Server_HttpResponse failed: conn=" ApHandleFormat "", ApHandleType(msgSHR.hConnection)); }

      }

      pMsg->Stop();
      pMsg->apStatus = ApMessage::Ok;

    } catch (ApException& ex) {

      Msg_Server_HttpResponse msgSHR;
      msgSHR.hConnection = pMsg->hConnection;
      msgSHR.nStatus = 404;
      msgSHR.sMessage = "Not Found";
      msgSHR.kvHeader.add("Content-type", "text/plain");
      msgSHR.kvHeader.add("Pragma", "no-cache");
      msgSHR.kvHeader.add("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
      msgSHR.kvHeader.add("Expires", "Thu, 19 Nov 1981 08:52:00 GMT");
      String sBody = ex.getText();
      msgSHR.sbBody.SetData(sBody);
      if (!msgSHR.Request()) {
        { throw ApException("Msg_Server_HttpResponse (for error message) failed: conn=" ApHandleFormat "", ApHandleType(msgSHR.hConnection)); }
      } else {
        pMsg->Stop();
        pMsg->apStatus = ApMessage::Ok;
      }
    }

  }
}

//static ApHandle g_hTimer;
//static String g_sData;
//static ApHandle g_hClient;

//AP_MSG_HANDLER_METHOD(VpModule, Xmpp_DataIn)
//{
//  String sData;
//  pMsg->sbData.GetString(sData);
//  if (sData.contains("unavailable") && !ApIsHandle(g_hTimer)) {
//    pMsg->apStatus = ApMessage::Ok;
//    g_sData = sData;
//    g_hTimer = Apollo::startTimeout(10, 0);
//    g_hClient = pMsg->hClient;
//  }
//}
//
//AP_MSG_HANDLER_METHOD(VpModule, Timer_Event)
//{
//  if (pMsg->hTimer == g_hTimer) {
//    Msg_Xmpp_DataIn msg;
//    msg.sbData.SetData(g_sData);
//    msg.hClient = g_hClient;
//    msg.Request();
//  }
//}

//----------------------------------------------------------

void SrpcGate_VpView_GetLocations(ApSRPCMessage* pMsg)
{
  Msg_VpView_GetLocations msg;
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setList("vlLocations", msg.vlLocations);
  }
}

void SrpcGate_VpView_GetLocationDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_GetLocationDetail msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  msg.sKey = pMsg->srpc.getString("sKey");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setString("sKey", msg.sKey);
    pMsg->response.setString("sValue", msg.sValue);
    pMsg->response.setString("sMimeType", msg.sMimeType);
  }
}

void SrpcGate_VpView_SubscribeLocationDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_SubscribeLocationDetail msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  msg.sKey = pMsg->srpc.getString("sKey");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_UnsubscribeLocationDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_UnsubscribeLocationDetail msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  msg.sKey = pMsg->srpc.getString("sKey");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_GetLocationContexts(ApSRPCMessage* pMsg)
{
  Msg_VpView_GetLocationContexts msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setList("vlContexts", msg.vlContexts);
  }
}

void SrpcGate_VpView_GetContextDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_GetContextDetail msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  msg.sKey = pMsg->srpc.getString("sKey");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setString("sKey", msg.sKey);
    pMsg->response.setString("sValue", msg.sValue);
    pMsg->response.setString("sMimeType", msg.sMimeType);
  }
}

void SrpcGate_VpView_SubscribeContextDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_SubscribeContextDetail msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  msg.sKey = pMsg->srpc.getString("sKey");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_UnsubscribeContextDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_UnsubscribeContextDetail msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  msg.sKey = pMsg->srpc.getString("sKey");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_GetParticipants(ApSRPCMessage* pMsg)
{
  Msg_VpView_GetParticipants msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setList("vlParticipants", msg.vlParticipants);
    pMsg->response.setString("hSelf", ApIsHandle(msg.hSelf) ? msg.hSelf.toString() : "");
  }
}

void SrpcGate_VpView_GetParticipantDetailString(ApSRPCMessage* pMsg)
{
  Msg_VpView_GetParticipantDetailString msg;
  msg.hParticipant = pMsg->srpc.getHandle("hParticipant");
  msg.sKey = pMsg->srpc.getString("sKey");
  pMsg->srpc.getValueList("vlMimeTypes", msg.vlMimeTypes);
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setString("sKey", msg.sKey);
    pMsg->response.setString("sValue", msg.sValue);
    pMsg->response.setString("sMimeType", msg.sMimeType);
  }
}

void SrpcGate_VpView_GetParticipantDetailRef(ApSRPCMessage* pMsg)
{
  Msg_VpView_GetParticipantDetailRef msg;
  msg.hParticipant = pMsg->srpc.getHandle("hParticipant");
  msg.sKey = pMsg->srpc.getString("sKey");
  pMsg->srpc.getValueList("vlMimeTypes", msg.vlMimeTypes);
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setString("sKey", msg.sKey);
    pMsg->response.setString("sUrl", msg.sUrl);
    pMsg->response.setString("sMimeType", msg.sMimeType);
  }
}

void SrpcGate_VpView_SubscribeParticipantDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_SubscribeParticipantDetail msg;
  msg.hParticipant = pMsg->srpc.getHandle("hParticipant");
  msg.sKey = pMsg->srpc.getString("sKey");
  pMsg->srpc.getValueList("vlMimeTypes", msg.vlMimeTypes);
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setInt("bAvailable", msg.bAvailable);
  }
}

void SrpcGate_VpView_UnsubscribeParticipantDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_UnsubscribeParticipantDetail msg;
  msg.hParticipant = pMsg->srpc.getHandle("hParticipant");
  msg.sKey = pMsg->srpc.getString("sKey");
  pMsg->srpc.getValueList("vlMimeTypes", msg.vlMimeTypes);
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

//---------------------------

void SrpcGate_VpView_Profile_Create(ApSRPCMessage* pMsg)
{
  Msg_VpView_Profile_Create msg;
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_Profile_SetParticipantDetail(ApSRPCMessage* pMsg)
{
  Msg_VpView_Profile_SetParticipantDetail msg;
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  msg.sKey = pMsg->srpc.getString("sKey");
  pMsg->srpc.getValueList("vlMimeTypes", msg.vlMimeTypes);
  msg.bByRef = pMsg->srpc.getInt("bByRef");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_Profile_GetParticipantDetails(ApSRPCMessage* pMsg)
{
  Msg_VpView_Profile_GetParticipantDetails msg;
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_Profile_SubscribeParticipantDetails(ApSRPCMessage* pMsg)
{
  Msg_VpView_Profile_SubscribeParticipantDetails msg;
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_Profile_UnsubscribeParticipantDetails(ApSRPCMessage* pMsg)
{
  Msg_VpView_Profile_UnsubscribeParticipantDetails msg;
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_VpView_Profile_Delete(ApSRPCMessage* pMsg)
{
  Msg_VpView_Profile_Delete msg;
  msg.hProfile = pMsg->srpc.getHandle("hProfile");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

//---------------------------

void SrpcGate_VpView_ReplayLocationPublicChat(ApSRPCMessage* pMsg)
{
  Msg_VpView_ReplayLocationPublicChat msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  msg.nMaxAge = pMsg->srpc.getInt("nMaxAge");
  msg.nMaxLines = pMsg->srpc.getInt("nMaxLines");
  msg.nMaxData = pMsg->srpc.getInt("nMaxData");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

//---------------------------

void SrpcGate_Vp_SendPublicChat(ApSRPCMessage* pMsg)
{
  Msg_Vp_SendPublicChat msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  msg.sText = pMsg->srpc.getString("sText");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

//---------------------------

void SrpcGate_Vp_SendPosition(ApSRPCMessage* pMsg)
{
  Msg_Vp_SendPosition msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  pMsg->srpc.getKeyValueList("kvParams", msg.kvParams);
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_Vp_SendCondition(ApSRPCMessage* pMsg)
{
  Msg_Vp_SendCondition msg;
  msg.hLocation = pMsg->srpc.getHandle("hLocation");
  pMsg->srpc.getKeyValueList("kvParams", msg.kvParams);
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(VpModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Vp", 0)) {
    AP_UNITTEST_REGISTER(VpModuleTester::test_Identity2ParticipantMapping);
    AP_UNITTEST_REGISTER(VpModuleTester::test_DisplayProfile);
    AP_UNITTEST_REGISTER(VpModuleTester::test_ItemDataExternUrl);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Vp", 0)) {
    AP_UNITTEST_EXECUTE(VpModuleTester::test_Identity2ParticipantMapping);
    AP_UNITTEST_EXECUTE(VpModuleTester::test_DisplayProfile);
    AP_UNITTEST_EXECUTE(VpModuleTester::test_ItemDataExternUrl);
  }
}

AP_MSG_HANDLER_METHOD(VpModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int VpModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, System_AfterLoadModules, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, System_BeforeUnloadModules, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vpi_LocationXmlResponse, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_NavigateContext, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_CloseContext, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_ContextVisibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_ContextPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_ContextSize, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_CreateContext, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_DestroyContext, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_CreateLocation, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_DestroyLocation, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_AddLocationContext, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_RemoveLocationContext, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_EnterLocation, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_LeaveLocation, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Online, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Offline, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_EnteringRoom, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_EnterRoomComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_LeavingRoom, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_LeaveRoomComplete, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_ParticipantEntered, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_ParticipantLeft, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_ParticipantSelf, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_Status, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_StatusMessage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_JabberId, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_MucUser, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_VpIdentity, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_Position, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_Condition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_FirebatFeatures, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Participant_End, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Room_GetIdentity, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Room_GetPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_Room_GetCondition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Protocol_ReceivePublicChat, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Identity_ContainerBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Identity_ItemAdded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Identity_ItemChanged, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Identity_ItemDataAvailable, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Identity_ItemRemoved, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Identity_ContainerEnd, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_GetLocations, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_GetLocationDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_SubscribeLocationDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_UnsubscribeLocationDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_GetLocationContexts, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_GetContextDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_SubscribeContextDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_UnsubscribeContextDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_GetParticipants, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_GetParticipantDetailString, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_GetParticipantDetailData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_GetParticipantDetailRef, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_SubscribeParticipantDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_UnsubscribeParticipantDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_Profile_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_Profile_SetParticipantDetail, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_Profile_GetParticipantDetails, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_Profile_SubscribeParticipantDetails, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_Profile_UnsubscribeParticipantDetails, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_Profile_Delete, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, VpView_ReplayLocationPublicChat, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Server_HttpRequest, this, ApCallbackPosNormal);

  // For Debugging
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Xmpp_DataIn, this, ApCallbackPosEarly);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Timer_Event, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_SendPublicChat, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_SendPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, Vp_SendCondition, this, ApCallbackPosNormal);

#if defined(AP_TEST)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, UnitTest_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, UnitTest_Execute, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, VpModule, UnitTest_End, this, ApCallbackPosNormal);
#endif

  srpcGateRegistry_.add("VpView_GetLocations", SrpcGate_VpView_GetLocations);
  srpcGateRegistry_.add("VpView_GetLocationDetail", SrpcGate_VpView_GetLocationDetail);
  srpcGateRegistry_.add("VpView_SubscribeLocationDetail", SrpcGate_VpView_SubscribeLocationDetail);
  srpcGateRegistry_.add("VpView_UnsubscribeLocationDetail", SrpcGate_VpView_UnsubscribeLocationDetail);
  srpcGateRegistry_.add("VpView_GetLocationContexts", SrpcGate_VpView_GetLocationContexts);
  srpcGateRegistry_.add("VpView_GetContextDetail", SrpcGate_VpView_GetContextDetail);
  srpcGateRegistry_.add("VpView_SubscribeContextDetail", SrpcGate_VpView_SubscribeContextDetail);
  srpcGateRegistry_.add("VpView_UnsubscribeContextDetail", SrpcGate_VpView_UnsubscribeContextDetail);
  srpcGateRegistry_.add("VpView_GetParticipants", SrpcGate_VpView_GetParticipants);
  srpcGateRegistry_.add("VpView_GetParticipantDetailString", SrpcGate_VpView_GetParticipantDetailString);
  srpcGateRegistry_.add("VpView_GetParticipantDetailRef", SrpcGate_VpView_GetParticipantDetailRef);
  srpcGateRegistry_.add("VpView_SubscribeParticipantDetail", SrpcGate_VpView_SubscribeParticipantDetail);
  srpcGateRegistry_.add("VpView_UnsubscribeParticipantDetail", SrpcGate_VpView_UnsubscribeParticipantDetail);
  srpcGateRegistry_.add("VpView_Profile_Create", SrpcGate_VpView_Profile_Create);
  srpcGateRegistry_.add("VpView_Profile_SetParticipantDetail", SrpcGate_VpView_Profile_SetParticipantDetail);
  srpcGateRegistry_.add("VpView_Profile_GetParticipantDetails", SrpcGate_VpView_Profile_GetParticipantDetails);
  srpcGateRegistry_.add("VpView_Profile_SubscribeParticipantDetails", SrpcGate_VpView_Profile_SubscribeParticipantDetails);
  srpcGateRegistry_.add("VpView_Profile_UnsubscribeParticipantDetails", SrpcGate_VpView_Profile_UnsubscribeParticipantDetails);
  srpcGateRegistry_.add("VpView_Profile_Delete", SrpcGate_VpView_Profile_Delete);

  srpcGateRegistry_.add("VpView_ReplayLocationPublicChat", SrpcGate_VpView_ReplayLocationPublicChat);

  srpcGateRegistry_.add("Vp_SendPublicChat", SrpcGate_Vp_SendPublicChat);
  srpcGateRegistry_.add("Vp_SendPosition", SrpcGate_Vp_SendPosition);
  srpcGateRegistry_.add("Vp_SendCondition", SrpcGate_Vp_SendCondition);

  return ok;
}

void VpModule::exit()
{
  srpcGateRegistry_.finish();

  AP_MSG_REGISTRY_FINISH;
}
