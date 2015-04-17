// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgVp.h"
#include "MsgIdentity.h"
#include "MsgHttpServer.h"
#include "Participant.h"
#include "Location.h"
#include "Chat.h"
#include "VpModule.h"

// For breakpointing
#define TEST_BreakOnCertainUser "Tassadar1"

//------------------------------------------------------------------

Participant::Participant(const ApHandle& hParticipant, Location* pLocation)
:hAp_(hParticipant)
,pLocation_(pLocation)
{
  lThingys_.setParticipant(this);

  ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_PublicChat);
  if (pThingy != 0) {
    pThingy->addSubscription();
  }
}

//--------------------------------

void Participant::onProtocolDataBegin()
{
  lThingys_.beginChanges();
}

void Participant::onProtocolNickname(const String& sNickname)
{
  sName_ = sNickname;

  ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_Nickname);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      if (pProvider->getVariant() == ParticipantThingyProvider::Variant_Nickname) {
        ((NicknameParticipantThingyProvider*) pProvider)->setProtocolNickname(sNickname);
      }
    }
  }
}

void Participant::onProtocolStatus(const String& sStatus)
{
  ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_OnlineStatus);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      if (pProvider->getVariant() == ParticipantThingyProvider::Variant_String) {
        ((StringParticipantThingyProvider*) pProvider)->setData(sStatus);
      }
    }
  }
}

void Participant::onProtocolMessage(const String& sMessage)
{
  ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_Message);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      if (pProvider->getVariant() == ParticipantThingyProvider::Variant_String) {
        ((StringParticipantThingyProvider*) pProvider)->setData(sMessage);
      }
    }
  }
}

void Participant::onProtocolJabberId(const String& sJabberId)
{
  if (!sJabberId.empty()) {
    ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_JabberId);
    if (pThingy) {
      ParticipantThingyProvider* pProvider = pThingy->getProvider();
      if (pProvider) {
        if (pProvider->getVariant() == ParticipantThingyProvider::Variant_String) {
          ((StringParticipantThingyProvider*) pProvider)->setData(sJabberId);
        }
      }
    }
  }
}

void Participant::onProtocolMucUser(const String& sJabberId, const String& sAffiliation, const String& sRole)
{
  onProtocolJabberId(sJabberId);

  {
    ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_MucAffiliation);
    if (pThingy) {
      ParticipantThingyProvider* pProvider = pThingy->getProvider();
      if (pProvider) {
        if (pProvider->getVariant() == ParticipantThingyProvider::Variant_String) {
          ((StringParticipantThingyProvider*) pProvider)->setData(sAffiliation);
        }
      }
    }
  }

  {
    ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_MucRole);
    if (pThingy) {
      ParticipantThingyProvider* pProvider = pThingy->getProvider();
      if (pProvider) {
        if (pProvider->getVariant() == ParticipantThingyProvider::Variant_String) {
          ((StringParticipantThingyProvider*) pProvider)->setData(sRole);
        }
      }
    }
  }
}

void Participant::onProtocolIdentity(const String& sId, const String& sSrc, const String& sDigest)
{
  int bSrcChanged = (sIdentitySrc_ != sSrc);
  int bDigestChanged = (sIdentityDigest_ != sDigest);

  sIdentityId_ = sId;
  sIdentitySrc_ = sSrc;
  sIdentityDigest_ = sDigest;

  if (bSrcChanged) {
    // Complete URL changed: mark as changed if available or acquire if not

    lThingys_.setChanged(Msg_VpView_ParticipantDetail_IdentityUrl);
    for (ParticipantThingy* pThingy = 0; (pThingy = lThingys_.Next(pThingy)) != 0; ) {
      ParticipantThingyProvider* pProvider = pThingy->getProvider();
      if (pProvider) {
        if (pProvider->isIdentityBased() && pThingy->isSubscribed()) {
          String sType = ((IdentityItemParticipantThingyProvider*) pProvider)->getItemType();
          if (hasItemDataByType(sType)) {
            #if defined(_DEBUG)
            if (sName_ == TEST_BreakOnCertainUser) {
              int breakhere = 1;
            }
            #endif
            pThingy->setChanged();
          } else {
            acquireItemDataByType(sType);
          }
        }
      }
    }

  } else if (bDigestChanged) {
    // Digest changed: request new container if there are identity based items subscribed

    int bIdentityBasedDetailSubscribed = 0;

    for (ParticipantThingy* pThingy = 0; (pThingy = lThingys_.Next(pThingy)) != 0; ) {
      ParticipantThingyProvider* pProvider = pThingy->getProvider();
      if (pProvider) {
        if (pProvider->isIdentityBased() && pThingy->isSubscribed()) {
          bIdentityBasedDetailSubscribed = 1;
          break;
        }
      }
    }

    if (bIdentityBasedDetailSubscribed) {
      if (!requestIdentityContainer()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "requestIdentityContainer failed part=" ApHandleFormat " url=%s", ApHandlePrintf(hAp_), _sz(sSrc)));
      }
    }

  } // bDigestChanged

  { // Fetch previously needed item data
    Elem* e = 0;
    while (e = lNeededTypes_.First()) {
      (void) acquireItemDataByType(e->getName());
      lNeededTypes_.Remove(e);
      delete e;
      e = 0;
    }
  }

}

void Participant::onProtocolPosition(Apollo::KeyValueList& kvParams)
{
  String sPosition = kvParams.toString();

  ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_Position);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      if (pProvider->getVariant() == ParticipantThingyProvider::Variant_KeyValue) {
        ((KeyValueParticipantThingyProvider*) pProvider)->setData(kvParams);
      }
    }
  }
}

void Participant::onProtocolCondition(Apollo::KeyValueList& kvParams)
{
  String sCondition = kvParams.toString();

  ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_Condition);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      if (pProvider->getVariant() == ParticipantThingyProvider::Variant_KeyValue) {
        ((KeyValueParticipantThingyProvider*) pProvider)->setData(kvParams);
      }
    }
  }
}

void Participant::onProtocolFirebatFeatures(const String& sFeatures)
{
  ParticipantThingy* pThingy = lThingys_.getOrCreate(Msg_VpView_ParticipantDetail_FirebatFeatures);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      if (pProvider->getVariant() == ParticipantThingyProvider::Variant_String) {
        ((StringParticipantThingyProvider*) pProvider)->setData(sFeatures);
      }
    }
  }
}

void Participant::onProtocolDataEnd()
{
  lThingys_.endChanges();
}

//----------------------

void Participant::onIdentityContainerBegin(const String& sUrl)
{
  #if defined(_DEBUG)
  if (sName_ == TEST_BreakOnCertainUser) {
    int breakhere = 1;
  }
  #endif
  lThingys_.beginChanges();
}

void Participant::onIdentityItemAdded(const String& sUrl, const String& sId)
{
}

void Participant::onIdentityItemChanged(const String& sUrl, const String& sId)
{
  int ok = 1;

  String sType;

  Msg_Identity_GetItem msgIGI;
  msgIGI.sUrl = getIdentitySrc();
  msgIGI.sId = sId;
  ok = msgIGI.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsItemDataAvailable failed part=" ApHandleFormat " url=%s id=%s", ApHandlePrintf(apHandle()), _sz(msgIGI.sUrl), _sz(msgIGI.sId)));
  } else {
    sType = msgIGI.sType;
  }

  if (sType) {
    for (ParticipantThingy* pThingy = 0; (pThingy = lThingys_.Next(pThingy)) != 0; ) {
      ParticipantThingyProvider* pProvider = pThingy->getProvider();
      if (pProvider) {
        if (pProvider->isIdentityBased() && pThingy->isSubscribed()) {
          if (pProvider->isBasedOnItemType(sType)) {
            if (hasItemDataById(sId)) {
              #if defined(_DEBUG)
              if (sName_ == TEST_BreakOnCertainUser) {
                int breakhere = 1;
              }
              #endif
              pThingy->setChanged();
            } else {
              needItemDataByType(sType);
            }
            break;
          }
        }
      }
    }
  }

}

void Participant::onIdentityItemRemoved(const String& sUrl, const String& sId)
{
}

void Participant::onIdentityContainerEnd(const String& sUrl)
{
  #if defined(_DEBUG)
  if (sName_ == TEST_BreakOnCertainUser) {
    int breakhere = 1;
  }
  #endif
  adjustThingyVariants();
  lThingys_.endChanges();
}

//----------------------------

void Participant::onIdentityItemDataAvailable(const String& sUrl, const String& sId, const String& sType, const String& sMimeType, const String& sSrc, int nSize)
{
  #if defined(_DEBUG)
  if (sName_ == TEST_BreakOnCertainUser) {
    int breakhere = 1;
  }
  #endif
  lThingys_.beginChanges();

  if (0) {
  } else if (sType == Msg_Identity_ItemType_Properties) {

    adjustThingyVariants();

    // Notify changed properties
    for (ParticipantThingy* pThingy = 0; (pThingy = lThingys_.Next(pThingy)) != 0; ) {
      ParticipantThingyProvider* pProvider = pThingy->getProvider();
      if (pProvider) {
        if (pProvider->isPropertyBased()) {

          Msg_Identity_GetProperty msg;
          msg.sUrl = sIdentitySrc_;
          msg.sKey = pThingy->getName();
          if (!msg.Request()) {
            apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetProperty failed part=" ApHandleFormat " url=%s key=%s", ApHandlePrintf(hAp_), _sz(msg.sUrl), _sz(msg.sKey)));
          } else {
            String sValue = ((IdentityPropertyParticipantThingyProvider*) pProvider)->getReferenceData();
            #if defined(_DEBUG)
            if (sName_ == TEST_BreakOnCertainUser) {
              int breakhere = 1;
            }
            #endif
            if (sValue != msg.sValue) {
              ((IdentityPropertyParticipantThingyProvider*) pProvider)->setReferenceData(msg.sValue);
              pThingy->setChanged();
            }
          }

        }
      }
    }

  } else {

    for (ParticipantThingy* pThingy = 0; (pThingy = lThingys_.Next(pThingy)) != 0; ) {
      ParticipantThingyProvider* pProvider = pThingy->getProvider();
      if (pProvider) {
        if (pProvider->isIdentityBased() && pProvider->isBasedOnItemType(sType)) {
          #if defined(_DEBUG)
          if (sName_ == TEST_BreakOnCertainUser) {
            int breakhere = 1;
          }
          #endif
          pThingy->setChanged();
        }
      }
    }

  } // sType

  #if defined(_DEBUG)
  if (sName_ == TEST_BreakOnCertainUser) {
    int breakhere = 1;
  }
  #endif
  lThingys_.endChanges();
}

//----------------------------------------------------------

int Participant::hasItemDataById(const String& sId)
{
  int ok = 0;
  int bAvailable = 0;

  if (sIdentitySrc_) {
    Msg_Identity_IsItemDataAvailable msgIIIDA;
    msgIIIDA.sUrl = sIdentitySrc_;
    msgIIIDA.sId = sId;
    ok = msgIIIDA.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsItemDataAvailable failed part=" ApHandleFormat " url=%s id=%s", ApHandlePrintf(hAp_), _sz(msgIIIDA.sUrl), _sz(msgIIIDA.sId)));
    } else {

      if (msgIIIDA.bAvailable) {
        bAvailable = 1;
      }

    }
  } // sIdentitySrc_

  return bAvailable;
}

int Participant::hasItemDataByType(const String& sType)
{
  int ok = 0;
  int bAvailable = 0;

  if (sIdentitySrc_) {
    Msg_Identity_IsContainerAvailable msgIICA;
    msgIICA.sUrl = sIdentitySrc_;
    msgIICA.sDigest = sIdentityDigest_;
    ok = msgIICA.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsContainerAvailable failed part=" ApHandleFormat " url=%s", ApHandlePrintf(hAp_), _sz(msgIICA.sUrl)));
    } else {
      if (msgIICA.bAvailable) {

        Msg_Identity_GetItemIds msgIGII;
        msgIGII.sUrl = sIdentitySrc_;
        msgIGII.sType = sType;
        ok = msgIGII.Request();
        if (!ok) {
          apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetItemIds failed url=%s type=%s", _sz(msgIGII.sUrl), _sz(msgIGII.sType)));
        } else {
          String sId = msgIGII.vlIds.atIndex(0, "");
          if (sId) {

            bAvailable = hasItemDataById(sId);

          } // sId
        } // msgIGII
      } // msgIICA.bAvailable
    } // msgIICA
  } // sIdentitySrc_

  return bAvailable;
}

int Participant::needItemDataByType(const String& sType)
{
  int ok = 0;

  if (sIdentitySrc_) {
    ok = acquireItemDataByType(sType);

  } else {
    // Save the need until the identity URL is available
    if (! lNeededTypes_.FindByName(sType)) {
      lNeededTypes_.Add(sType);
    }

    ok = 1;
  }

  return ok;
}

int Participant::acquireItemDataByType(const String& sType)
{
  int ok = 0;

  if (sIdentitySrc_) {
    Msg_Identity_AcquireItemData msg;
    msg.hAcquisition = Apollo::newHandle();
    msg.sUrl = sIdentitySrc_;
    msg.sDigest = sIdentityDigest_;
    msg.sType = sType;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_AcquireItemData failed part=" ApHandleFormat " url=%s type=%s", ApHandlePrintf(hAp_), _sz(msg.sUrl), _sz(msg.sType)));
    }
  }

  return ok;
}

int Participant::requestIdentityContainer()
{
  int ok = 0;

  // This should really check if we already tried and failed
  if (sIdentitySrc_) {
    Msg_Identity_RequestContainer msg;
    msg.hRequest = Apollo::newHandle();
    msg.sUrl = sIdentitySrc_;
    msg.sDigest = sIdentityDigest_;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_RequestContainer failed url=%s", _sz(msg.sUrl)));
    }
  }

  return ok;
}

int Participant::requestIdentityItem(const String sId)
{
  int ok = 0;

  // This should really check if we already tried and failed
  if (sIdentitySrc_) {
    Msg_Identity_GetItem msgIGI;
    msgIGI.sUrl = sIdentitySrc_;
    msgIGI.sId = sId;
    if (!msgIGI.Request()) {
      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetItem failed url=%s id=%s", _sz(msgIGI.sUrl), _sz(msgIGI.sId)));
    } else {

      Msg_Identity_RequestItem msgIRI;
      msgIRI.hRequest = Apollo::newHandle();
      msgIRI.sUrl = sIdentitySrc_;
      msgIRI.sId = sId;
      msgIRI.sSrc = msgIGI.sSrc;
      msgIRI.sDigest = msgIGI.sDigest;
      ok = msgIRI.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_RequestItem failed url=%s id=%s", _sz(msgIRI.sUrl), _sz(msgIRI.sId)));
      }
    }
  }

  return ok;
}

//----------------------------------------------------------

int Participant::isDetailAvailable(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  int bAvailable = 0;

  ParticipantThingy* pThingy = lThingys_.FindByName(sKey);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      bAvailable = pProvider->hasData();
    }
  }

  return bAvailable;
}

int Participant::getDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes, String& sValue, String& sMimeType)
{
  int ok = 0;

  ParticipantThingy* pThingy = lThingys_.FindByName(sKey);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      pProvider->getString(sValue, sMimeType);
      ok = 1;
    }
  }

  return ok;
}

int Participant::getDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes, Buffer& sbData, String& sMimeType, String& sSource)
{
  int ok = 0;

  ParticipantThingy* pThingy = lThingys_.FindByName(sKey);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      pProvider->getData(sbData, sMimeType, sSource);
      ok = 1;
    }
  }

  return ok;
}

int Participant::getDetailUrl(const String& sKey, Apollo::ValueList& vlMimeTypes, String& sUrl, String& sMimeType)
{
  int ok = 0;

  ParticipantThingy* pThingy = lThingys_.FindByName(sKey);
  if (pThingy) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      if (pProvider->isIdentityBased()) {
        ((IdentityItemParticipantThingyProvider*) pProvider)->getDataRef(sUrl, sMimeType);
        ok = 1;
      }
    }
  }

  return ok;
}

int Participant::subscribeDetail(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  int ok = 0;

  ok = addSubscription(sKey, vlMimeTypes);

  return ok;
}

int Participant::unsubscribeDetail(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  int ok = 1;

  ok = removeSubscription(sKey, vlMimeTypes);

  return ok;
}

int Participant::isSubscribedDetail(const String& sKey)
{
  int bSubscribed = 0;

  ParticipantThingy* pThingy = lThingys_.FindByName(sKey);
  if (pThingy) {
    bSubscribed = pThingy->isSubscribed();
  }

  return bSubscribed;
}

int Participant::addSubscription(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  int ok = 1;
  AP_UNUSED_ARG(vlMimeTypes);

  ParticipantThingy* pThingy = lThingys_.getOrCreate(sKey);
  if (pThingy != 0) {
    pThingy->addSubscription();
  }

  return ok;
}

int Participant::removeSubscription(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  int ok = 1;
  AP_UNUSED_ARG(vlMimeTypes);

  ParticipantThingy* pThingy = lThingys_.getOrCreate(sKey);
  if (pThingy != 0) {
    pThingy->removeSubscription();
  }

  return ok;
}

//----------------------------------------------------------

void IdentityUrlParticipantThingyProvider::getString(String& sValue, String& sMimeType)
{
  if (pParticipant_) {
    sValue = pParticipant_->getIdentitySrc();
  }
}

//---------------------

int StringParticipantThingyProvider::hasData()
{
  return bHasData_;
}

void StringParticipantThingyProvider::setData(const String& sValue)
{
  bHasData_ = 1;

  if (sValue_ != sValue) {
    if (pThingy_) { pThingy_->setChanged(); }
  }

  sValue_ = sValue;
}

void StringParticipantThingyProvider::getString(String& sValue, String& sMimeType)
{
  sValue = sValue_;
}

//---------------------

int KeyValueParticipantThingyProvider::hasData()
{
  return bHasData_;
}

void KeyValueParticipantThingyProvider::setData(Apollo::KeyValueList& kvList)
{
  int bChanged = 0;
  bHasData_ = 1;

  if (kvList_.length() != kvList.length()) {
    bChanged = 1;
  }

  if (!bChanged) {
    Apollo::KeyValueElem* f = 0;
    for (Apollo::KeyValueElem* e = 0; (e = kvList.nextElem(e)) != 0; ) {
      f = kvList_.nextElem(f);
      if (!f) {
        bChanged = 1;
        break;
      } else {
        if (e->getKey() != f->getKey()) {
          bChanged = 1;
          break;
        }
        if (e->getString() != f->getString()) {
          bChanged = 1;
          break;
        }
      }
    }
  }

  kvList_ = kvList;

  if (bChanged) {
    if (pThingy_) { pThingy_->setChanged(); }
  }
}

void KeyValueParticipantThingyProvider::getString(String& sValue, String& sMimeType)
{
  sValue = kvList_.toString();
  sMimeType = "apollo/KeyValueList";
}

//---------------------

String IdentityItemParticipantThingyProvider::getItemType()
{
  String sItemType;

  if (pThingy_) { sItemType = pThingy_->getName(); }

  return sItemType;
}

void IdentityItemParticipantThingyProvider::getDataCore(Buffer& sbData, int& bDataValid, String& sMimeType, String& sLocalUrl, String& sOriginalUrl)
{
  String sType = getItemType();

  if (pParticipant_ != 0) {
    if (pParticipant_->getIdentitySrc()) {

      // Check, if container is available

      Msg_Identity_IsContainerAvailable msgIICA;
      msgIICA.sUrl = pParticipant_->getIdentitySrc();
      msgIICA.sDigest = pParticipant_->getIdentityDigest();
      if (!msgIICA.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsContainerAvailable failed part=" ApHandleFormat " url=%s", ApHandlePrintf(pParticipant_->apHandle()), _sz(msgIICA.sUrl)));
      } else {
        if (msgIICA.bAvailable) {

          Msg_Identity_GetItemIds msgIGII;
          msgIGII.sUrl = pParticipant_->getIdentitySrc();
          msgIGII.sType = sType;
          msgIGII.nMax = 1;
          if (!msgIGII.Request()) {
            apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetItemIds failed url=%s type=%s", _sz(msgIGII.sUrl), _sz(msgIGII.sType)));
          } else {
            String sId = msgIGII.vlIds.atIndex(0, "");
            if (sId) {

              // Check, if the data is available
              // If not then request it for later

              Msg_Identity_IsItemDataAvailable msgIIDA;
              msgIIDA.sUrl = pParticipant_->getIdentitySrc();
              msgIIDA.sId = sId;
              if (!msgIIDA.Request()) { 
                apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsItemDataAvailable failed: url=%s or id=%s empty", _sz(msgIIDA.sUrl), _sz(msgIIDA.sId)));
              } else {
                if (!msgIIDA.bAvailable) {
                  if (!pParticipant_->requestIdentityItem(sId)) {
                    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "pParticipant_->requestIdentityItem() failed id=%s", _sz(sId)));
                  }
                } // msgIIDA.bAvailable
              } // msgIIDA

              Msg_Identity_GetItem msgIGI;
              msgIGI.sUrl = pParticipant_->getIdentitySrc();
              msgIGI.sId = sId;
              if (!msgIGI.Request()) {
                apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetItem failed url=%s id=%s", _sz(msgIGI.sUrl), _sz(msgIGI.sId)));
              } else {
                sMimeType = msgIGI.sMimeType;
                sOriginalUrl = msgIGI.sSrc;
              }

              // Return the internal URL. If the data is not available until the internal URL is 
              // requested, then the internal HTTP server will redirect to the original item src URL
              sLocalUrl = VpModuleInstance::Get()->getItemDataExternUrl(pParticipant_->getIdentitySrc(), sId);

              if (msgIIDA.bAvailable) {
                Msg_Identity_GetItemData msgIGID;
                msgIGID.sUrl = pParticipant_->getIdentitySrc();
                msgIGID.sId = sId;
                if (!msgIGID.Request()) {
                  // Not an error, item data just not available, maybe already requested
                  //apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetItemData failed: url=%s or id=%s empty", _sz(msgIGID.sUrl), _sz(msgIGID.sId)));
                } else {

                  sbData = msgIGID.sbData;
                  sMimeType = msgIGID.sMimeType;
                  bDataValid = 1;

                } // msgIGID
              } // msgIIDA.bAvailable

            } // sId
          } // msgIGII
        } // msgIICA.bAvailable
      } // msgIICA
    } // pParticipant_->getIdentitySrc
  } // pParticipant_
}
  
int IdentityItemParticipantThingyProvider::hasData()
{
  int ok = 0;
  int bAvailable = 0;

  String sType = getItemType();

  if (pParticipant_ != 0) {
    if (pParticipant_->getIdentitySrc()) {

      // Check, if container is available

      Msg_Identity_IsContainerAvailable msgIICA;
      msgIICA.sUrl = pParticipant_->getIdentitySrc();
      msgIICA.sDigest = pParticipant_->getIdentityDigest();
      ok = msgIICA.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsContainerAvailable failed part=" ApHandleFormat " url=%s", ApHandlePrintf(pParticipant_->apHandle()), _sz(msgIICA.sUrl)));
      } else {
        if (msgIICA.bAvailable) {

          Msg_Identity_GetItemIds msgIGII;
          msgIGII.sUrl = pParticipant_->getIdentitySrc();
          msgIGII.sType = sType;
          msgIGII.nMax = 1;
          ok = msgIGII.Request();
          if (!ok) {
            apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetItemIds failed url=%s type=%s", _sz(msgIGII.sUrl), _sz(msgIGII.sType)));
          } else {
            String sId = msgIGII.vlIds.atIndex(0, "");
            if (sId) {

              // Check, if the data is available

              Msg_Identity_IsItemDataAvailable msgIIDA;
              msgIIDA.sUrl = pParticipant_->getIdentitySrc();
              msgIIDA.sId = sId;
              if (!msgIIDA.Request()) { 
                apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsItemDataAvailable failed: url=%s or id=%s empty", _sz(msgIIDA.sUrl), _sz(msgIIDA.sId)));
              } else {
                bAvailable = msgIIDA.bAvailable;

              } // msgIIDA
            } // sId
          } // msgIGII
        } // msgIICA.bAvailable
      } // msgIICA
    } // pParticipant_->getIdentitySrc
  } // pParticipant_

  return bAvailable;
}

void IdentityItemParticipantThingyProvider::getString(String& sValue, String& sMimeType)
{
  String sLocalUrl;
  String sOriginalUrl;
  Buffer sbData;
  int bDataValid = 0;
  getDataCore(sbData, bDataValid, sMimeType, sLocalUrl, sOriginalUrl);
  sbData.GetString(sValue);
}

void IdentityItemParticipantThingyProvider::getData(Buffer& sbData, String& sMimeType, String& sSource)
{
  String sLocalUrl;
  String sOriginalUrl;
  int bDataValid = 0;
  getDataCore(sbData, bDataValid, sMimeType, sLocalUrl, sOriginalUrl);
  if (sOriginalUrl) {
    sSource = Msg_VpView_ParticipantDetail_SourcePrefix_IdentityItemUrl;
    sSource += Msg_VpView_ParticipantDetail_SourceSeparator;
    sSource += sOriginalUrl;
  }
}

void IdentityItemParticipantThingyProvider::getDataRef(String& sUrl, String& sMimeType)
{
  String sOriginalUrl;
  Buffer sbData;
  int bDataValid = 0;
  getDataCore(sbData, bDataValid, sMimeType, sUrl, sOriginalUrl);
}

void IdentityItemParticipantThingyProvider::needData()
{
  if (pParticipant_ != 0) {
    if (!pParticipant_->hasItemDataByType(getItemType())) {
      pParticipant_->needItemDataByType(getItemType());
    }
  }
}

void IdentityItemParticipantThingyProvider::onSubscribe()
{
  if (pThingy_) {
    if (!pThingy_->isSubscribed()) {
      needData();
    } // ! was subscribed
  }
}

//---------------------

void IdentityPropertyParticipantThingyProvider::getIdentityProperty(const String& sKey, String& sValue)
{
  int ok = 0;

  if (pParticipant_ != 0) {
    if (pParticipant_->getIdentitySrc()) {

      //acquireItemDataByType(sType);

      Msg_Identity_IsContainerAvailable msgIICA;
      msgIICA.sUrl = pParticipant_->getIdentitySrc();
      msgIICA.sDigest = pParticipant_->getIdentityDigest();
      ok = msgIICA.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsContainerAvailable failed part=" ApHandleFormat " url=%s", ApHandlePrintf(pParticipant_->apHandle()), _sz(msgIICA.sUrl)));
      } else {
        if (msgIICA.bAvailable) {

          Msg_Identity_GetItemIds msgIGII;
          msgIGII.sUrl = pParticipant_->getIdentitySrc();
          msgIGII.sType = Msg_Identity_ItemType_Properties;
          msgIGII.nMax = 1;
          ok = msgIGII.Request();
          if (!ok) {
            apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetItemIds failed url=%s type=%s", _sz(msgIGII.sUrl), _sz(msgIGII.sType)));
          } else {
            if (msgIGII.vlIds.length() > 0) {

              Msg_Identity_GetProperty msg;
              msg.sUrl = pParticipant_->getIdentitySrc();
              msg.sKey = sKey;
              if (!msg.Request()) {
                apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetProperty failed part=" ApHandleFormat " url=%s key=%s", ApHandlePrintf(pParticipant_->apHandle()), _sz(msg.sUrl), _sz(msg.sKey)));
              } else {
                if (!msg.sValue.empty()) {
                  sValue = msg.sValue;
                  ok = 1;
                }
              }

            } // if vlIds > 0
          } // msgIGII
        } // msgIICA.bAvailable
      } // msgIICA
    } // pParticipant_->getIdentitySrc
  } // pParticipant_
}

void IdentityPropertyParticipantThingyProvider::getString(String& sValue, String& sMimeType)
{
  if (pThingy_) { getIdentityProperty(pThingy_->getName(), sValue); }
}

String IdentityPropertyParticipantThingyProvider::getItemType()
{ 
  return "properties";
}

//---------------------

void NicknameParticipantThingyProvider::setProtocolNickname(const String& sNickname)
{
  String sIdentityNickname;
  getIdentityProperty("Nickname", sIdentityNickname);

  if (!sIdentityNickname) {
    if (sProtocolNickname_ != sNickname) {
      if (pThingy_) { pThingy_->setChanged(); }
    }
  }

  sProtocolNickname_ = sNickname;
  setReferenceData(sProtocolNickname_);
}

int NicknameParticipantThingyProvider::hasData()
{
  int bHas = base::hasData();
  if (!bHas) {
    bHas = !sProtocolNickname_.empty();
  }
  return bHas;
}

void NicknameParticipantThingyProvider::getString(String& sValue, String& sMimeType)
{
  getIdentityProperty("Nickname", sValue);

  if (!sValue) {
    sValue = sProtocolNickname_;// + "_proto";
  }

  if (!sValue) {
    if (pParticipant_) {
      sValue = pParticipant_->apHandle().toString();
    }
  }

  if (!sValue) {
    sValue = "unknown";
  }
}

//---------------------------------------------

void Participant::adjustThingyVariants()
{
  for (ParticipantThingy* pThingy = 0; (pThingy = lThingys_.Next(pThingy)) != 0; ) {
    ParticipantThingyProvider* pProvider = pThingy->getProvider();
    if (pProvider) {
      ParticipantThingyProvider::Variant nVariant = lThingys_.getVariantByKey(pThingy->getName());
      if (nVariant != pProvider->getVariant()) {
        changeThingyVariant(pThingy, nVariant);
      }
    }
  }
}

void Participant::changeThingyVariant(ParticipantThingy* pThingy, ParticipantThingyProvider::Variant nVariant)
{
  ParticipantThingyProvider* pNewProvider = (ParticipantThingyProvider*) lThingys_.newProvider(pThingy->getName());
  if (pNewProvider) {
    pThingy->setProvider(pNewProvider);

    if (pThingy->isSubscribed()) {

      // Because onSubscribe() checks if isSubscribed() and won't re-subscribe.
      // OK, this is a hack. 
      pThingy->pretendNotSubscribed(1);

      pNewProvider->onSubscribe();
      pThingy->pretendNotSubscribed(0);
    }

    if (pNewProvider->hasData()) {
      pThingy->setChanged();
    }
  }
}

//----------------------------------------------------------

ParticipantThingyProvider::Variant ParticipantThingyList::getVariantByKey(const String& sKey)
{
  ParticipantThingyProvider::Variant nVariant = ParticipantThingyProvider::Variant_Unknown;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Nickname) { nVariant = ParticipantThingyProvider::Variant_Nickname;
  } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) { nVariant = ParticipantThingyProvider::Variant_String;
  } else if (sKey == Msg_VpView_ParticipantDetail_Position) { nVariant = ParticipantThingyProvider::Variant_KeyValue;
  } else if (sKey == Msg_VpView_ParticipantDetail_Condition) { nVariant = ParticipantThingyProvider::Variant_KeyValue;
  } else if (sKey == Msg_VpView_ParticipantDetail_Message) { nVariant = ParticipantThingyProvider::Variant_String;
  } else if (sKey == Msg_VpView_ParticipantDetail_JabberId) { nVariant = ParticipantThingyProvider::Variant_String;
  } else if (sKey == Msg_VpView_ParticipantDetail_MucAffiliation) { nVariant = ParticipantThingyProvider::Variant_String;
  } else if (sKey == Msg_VpView_ParticipantDetail_MucRole) { nVariant = ParticipantThingyProvider::Variant_String;
  } else if (sKey == Msg_VpView_ParticipantDetail_FirebatFeatures) { nVariant = ParticipantThingyProvider::Variant_String;
  } else {

    if (pParticipant_ != 0 && !pParticipant_->getIdentitySrc().empty()) {
      int bIsProperty = 0;
      int bIsItem = 0;

      Msg_Identity_IsContainerAvailable msgIICA;
      msgIICA.sUrl = pParticipant_->getIdentitySrc();
      msgIICA.sDigest = pParticipant_->getIdentityDigest();
      if (!msgIICA.Request()) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_IsContainerAvailable failed part=" ApHandleFormat " url=%s", ApHandlePrintf(pParticipant_->apHandle()), _sz(msgIICA.sUrl)));
      } else {
        if (msgIICA.bAvailable) {

          if (nVariant == ParticipantThingyProvider::Variant_Unknown) {
            Msg_Identity_HasProperty msgIHP;
            msgIHP.sUrl = pParticipant_->getIdentitySrc();
            msgIHP.sKey = sKey;
            if (!msgIHP.Request()) {
              apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_HasProperty failed part=" ApHandleFormat " url=%s", ApHandlePrintf(pParticipant_->apHandle()), _sz(msgIHP.sUrl)));
            } else {
              if (msgIHP.bIsProperty) {
                bIsProperty = 1;
              }
            } // msgIHP
          } // !bDataSrcKnown

          if (nVariant == ParticipantThingyProvider::Variant_Unknown) {
            Msg_Identity_GetItemIds msgIGII;
            msgIGII.sUrl = pParticipant_->getIdentitySrc();
            msgIGII.sType = sKey;
            if (!msgIGII.Request()) {
              apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_Identity_GetItemIds failed part=" ApHandleFormat " url=%s", ApHandlePrintf(pParticipant_->apHandle()), _sz(msgIGII.sUrl)));
            } else {
              if (msgIGII.vlIds.length() > 0) {
                bIsItem = 1;
              }
            } // msgIHP
          } // !bDataSrcKnown

        } // msgIICA.bAvailable
      } // msgIICA

      if (bIsProperty) {
        nVariant = ParticipantThingyProvider::Variant_IdentityProperty;
      } else if (bIsItem) {
        nVariant = ParticipantThingyProvider::Variant_IdentityItem;
      }
    } // pParticipant_
  } // sKey

  return nVariant;
}

ThingyProvider* ParticipantThingyList::newProvider(const String& sKey)
{
  ParticipantThingyProvider* pProvider = 0;

  ParticipantThingyProvider::Variant nVariant = getVariantByKey(sKey);
  switch (nVariant) {
    case ParticipantThingyProvider::Variant_String: pProvider = new StringParticipantThingyProvider(); break;
    case ParticipantThingyProvider::Variant_KeyValue: pProvider = new KeyValueParticipantThingyProvider(); break;
    case ParticipantThingyProvider::Variant_IdentityItem: pProvider = new IdentityItemParticipantThingyProvider(); break;
    case ParticipantThingyProvider::Variant_IdentityProperty: pProvider = new IdentityPropertyParticipantThingyProvider(); break;
    case ParticipantThingyProvider::Variant_Nickname: pProvider = new NicknameParticipantThingyProvider(); break;
    default: pProvider = new UnknownParticipantThingyProvider(); break;
  }

  if (pProvider) {
    pProvider->setParticipant(pParticipant_);
  }

  return pProvider;
}

void ParticipantThingyList::sendChanges(Apollo::ValueList& vlChanges)
{
  Location* pLocation = pParticipant_->getLocation();
  if (pLocation) {
    Msg_VpView_ParticipantDetailsChanged msg;
    msg.hLocation = pLocation->apHandle();
    msg.hParticipant = pParticipant_->apHandle();
    msg.vlKeys = vlChanges;
    msg.Send();
  }
}
