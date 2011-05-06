// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgSystem.h"
#include "MsgDB.h"
#include "URL.h"
#include "IdentityModule.h"

ItemAcquisitionTask* IdentityModule::findItemAcquisitionTaskByRequest(const ApHandle& hRequest)
{
  ItemAcquisitionTask* pResult = 0;

  ItemAcquisitionTaskListIterator iter(itemTasks_);
  for (ItemAcquisitionTaskListNode* pNode = 0; pNode = iter.Next(); ) {
    ItemAcquisitionTask* pTask = pNode->Value();
    if (pTask != 0) {
      if (pTask->getRequest() == hRequest) {
        pResult = pTask;
        break;
      }
    }
  }

  return pResult;
}

//----------------------------------------------------------

String IdentityModule::selectItemId(const String& sUrl, const String& sType, const String& sMimeType)
{
  int ok = 1;
  String sResult;
  int nResultOrder = -1;

  Msg_Identity_GetItemIds msgIGII;
  msgIGII.sUrl = sUrl;
  msgIGII.sType = sType;
  ok = msgIGII.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "IdentityModule::selectItemId", "Msg_Identity_GetItemIds failed url=%s type=%s", StringType(msgIGII.sUrl), StringType(msgIGII.sType)));
  } else {
    for (ValueElem* e = 0; (e = msgIGII.vlIds.nextElem(e)) != 0; ) {
      String sId = e->getString();

      // Fetch item
      Msg_Identity_GetItem msgIGI;
      msgIGI.sUrl = sUrl;
      msgIGI.sId = sId;
      ok = msgIGI.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "IdentityModule::selectItemId", "Msg_Identity_GetItem failed url=%s id=%s", StringType(msgIGI.sUrl), StringType(msgIGI.sId)));
      } else {

        int bApplicable = 0;
        if (sMimeType.empty()) {
          bApplicable = 1;
        } else {
          if (msgIGI.sMimeType.empty() || sMimeType == msgIGI.sMimeType) {
            bApplicable = 1;
          }
        }

        if (bApplicable) {
          if (sResult.empty()) {
            sResult = sId;
            nResultOrder = msgIGI.nOrder;
          } else {
            if (msgIGI.nOrder < nResultOrder) {
              sResult = sId;
              nResultOrder = msgIGI.nOrder;
            } else if (msgIGI.nOrder == nResultOrder) {
              if (!msgIGI.sMimeType.empty()) {
                sResult = sId;
              }
            }
          }
        }

      }

    }
  } // msgIGII

  return sResult;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(IdentityModule, System_AfterLoadModules)
{
  AP_UNUSED_ARG(pMsg);

  Msg_DB_Open msg;
  msg.sName = sDb_;
  int ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "IdentityModule::System_AfterLoadModules", "Msg_DB_Open %s failed", StringType(msg.sName)));
  }
}

AP_MSG_HANDLER_METHOD(IdentityModule, System_BeforeUnloadModules)
{
  AP_UNUSED_ARG(pMsg);

  bInShutdown_ = true;

  Msg_DB_Close msg;
  msg.sName = sDb_;
  int ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "IdentityModule::System_BeforeUnloadModules", "Msg_DB_Close %s failed", StringType(msg.sName)));
  }
}

//----------------------------
// Download

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_RequestContainer)
{
  int ok = 0;
  
  ContainerRequest* pRequest = lContainerRequests_.FindByName(pMsg->sUrl);
  if (pRequest != 0) {
    pRequest->addHandle(pMsg->hRequest);
    apLog_Info((LOG_CHANNEL, "IdentityModule::Identity_RequestContainer", "Ignored, because already in progress, src=%s", StringType(pMsg->sUrl)));
    ok = 1;

  } else {

    ContainerClient* pClient = new ContainerClient(pMsg->hRequest, pMsg->sDigest);
    if (pClient != 0) {
      ok = pClient->Get(pMsg->sUrl);
      if (!ok) {
        apLog_Verbose((LOG_CHANNEL, "IdentityModule::Identity_RequestContainer", "pClient->Get() failed, src=%s", StringType(pMsg->sUrl)));
        delete pClient;
        pClient = 0;
      } else {
        pRequest = lContainerRequests_.Add(pMsg->sUrl);
        if (pRequest != 0) {
          pRequest->addHandle(pMsg->hRequest);
        }
      }
    }

  }
  
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_ReceiveContainer)
{
  int ok = 1;

  int bSuccess = 0;
  String sError;

  if (pMsg->sbData.Length() == 0) {
    sError.appendf("No data: %s", StringType(pMsg->sComment));
  } else {
    if (ok) {
      Msg_Identity_SetContainer msg;
      msg.sUrl = pMsg->sUrl;
      msg.sDigest = pMsg->sDigest;
      pMsg->sbData.GetString(msg.sData);
      ok = msg.Request();
      if (!ok) {
        sError.appendf("Msg_Identity_SetContainer failed: %s, src=%s ", StringType(pMsg->sComment), StringType(pMsg->sUrl));
        apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ReceiveContainer", sError));
      } else {
        bSuccess = 1;
      }
    }

    if (ok) {
      Msg_Identity_SaveContainerToStorage msg;
      msg.sUrl = pMsg->sUrl;
      ok = msg.Request();
      if (!ok) {
        sError.appendf("Msg_Identity_SaveContainerToStorage failed: src=%s", StringType(pMsg->sUrl));
        apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ReceiveContainer", sError));
      }
    }
  } // pMsg->sbData.Length()

  ContainerRequest* pRequest = lContainerRequests_.FindByName(pMsg->sUrl);
  if (pRequest != 0) {
    for (Apollo::ValueElem* e = 0; (e = pRequest->getHandles().nextElem(e)) != 0; ) {
      ApHandle hRequest = e->getHandle();

      {
        ItemAcquisitionTask* pTask = findItemAcquisitionTaskByRequest(hRequest);
        if (pTask != 0) {
          // Someone needed an item not only the container
          int bAcquisitionFinished = 1;
          int bAcquisitionSuccess = 0;

          if (bSuccess) {
            // Check if the data is already here, otherwise request the data

            String sId = selectItemId(pTask->getUrl(), pTask->getType(), pTask->getMIMEType());

            if (!sId.empty()) {
              Msg_Identity_IsItemDataAvailable msgIIIDA;
              msgIIIDA.sUrl = pTask->getUrl();
              msgIIIDA.sId = sId;
              ok = msgIIIDA.Request();
              if (!ok) {
                apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ReceiveContainer", "Msg_Identity_IsItemDataAvailable failed url=%s id=%s", StringType(msgIIIDA.sUrl), StringType(msgIIIDA.sId)));
              } else {

                if (msgIIIDA.bAvailable) {
                  bAcquisitionSuccess = 1;
                } else {

                  // Fetch item
                  Msg_Identity_GetItem msgIGI;
                  msgIGI.sUrl = pTask->getUrl();
                  msgIGI.sId = sId;
                  ok = msgIGI.Request();
                  if (!ok) {
                    apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ReceiveContainer", "Msg_Identity_GetItem failed url=%s id=%s", StringType(msgIGI.sUrl), StringType(msgIGI.sId)));
                  } else {

                    // Fetch item data 
                    pTask->setRequest(Apollo::newHandle());

                    // This should really check if we already tried and failed
                    Msg_Identity_RequestItem msgIRI;
                    msgIRI.hRequest = pTask->getRequest();
                    msgIRI.sUrl = pTask->getUrl();
                    msgIRI.sId = sId;
                    msgIRI.sSrc = msgIGI.sSrc;
                    msgIRI.sDigest = msgIGI.sDigest;
                    ok = msgIRI.Request();
                    if (!ok) {
                      apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ReceiveContainer", "Msg_Identity_RequestItem failed url=%s id=%s", StringType(msgIRI.sUrl), StringType(msgIRI.sId)));
                    } else {
                      bAcquisitionFinished = 0;
                    }

                  } // msgIGI
                } // msgIIIDA.bAvailable
              } // msgIIIDA
            } // sId
          } // bSuccess

          if (bAcquisitionFinished) {
            Msg_Identity_AcquireItemDataComplete pMsgIAIDC;
            pMsgIAIDC.hAcquisition = pTask->apHandle();
            pMsgIAIDC.sUrl = pTask->getUrl();
            pMsgIAIDC.sType = pTask->getType();
            pMsgIAIDC.bSuccess = bAcquisitionSuccess;
            pMsgIAIDC.Send();

            if (itemTasks_.Unset(pTask->apHandle())) {
              delete pTask;
              pTask = 0;
            }
          }
        }
      }

      {
        Msg_Identity_RequestContainerComplete msg;
        msg.hRequest = hRequest;
        msg.sUrl = pMsg->sUrl;
        msg.sDigest = pMsg->sDigest;
        msg.bSuccess = bSuccess;
        if (!bSuccess) {
          msg.sComment = sError;
        }
        pMsg->sbData.GetString(msg.sXml);
        msg.Send();
      }

    } // for getHandles()

    lContainerRequests_.Remove(pRequest);
    delete pRequest;
    pRequest = 0;
  } // pRequest

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_SetContainer)
{
  int ok = 0;

  Container* pContainer = 0;
  if (!data_.Get(pMsg->sUrl, pContainer)) {
    pContainer = new Container(pMsg->sUrl);
  }

  if (pContainer != 0) {
    ok = pContainer->set(pMsg->sData, pMsg->sDigest);
    if (!ok) {
      pMsg->sComment.appendf("pContainer->parse() failed src=%s", StringType(pMsg->sUrl));
      apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_SetContainer", pMsg->sComment));
      delete pContainer;
      pContainer = 0;
    } else {
      data_.Set(pMsg->sUrl, pContainer);
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_SaveContainerToStorage)
{
  int ok = 1;

  Container* pContainer = 0;
  if (!data_.Get(pMsg->sUrl, pContainer)) {
    apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_SaveContainerToStorage", "data_.Get() failed: src=%s", StringType(pMsg->sUrl)));
    ok = 0;
  }

  if (ok && pContainer != 0) {
    Msg_DB_Set msg;
    msg.sName = sDb_;
    msg.sKey.appendf("%s data", StringType(prepareDbKey(pMsg->sUrl)));
    msg.sValue = pContainer->getData();
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "IdentityModule, Identity_SaveContainerToStorage", "Msg_DB_Set failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)));
    }
  }

  if (ok && pContainer != 0) {
    Msg_DB_Set msg;
    msg.sName = sDb_;
    msg.sKey.appendf("%s digest", StringType(prepareDbKey(pMsg->sUrl)));
    msg.sValue = pContainer->getDigest();
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_SaveContainerToStorage", "Msg_DB_Set failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_LoadContainerFromStorage)
{
  int ok = 0;

  Container* pContainer = 0;
  if (data_.Get(pMsg->sUrl, pContainer)) {
    // Already in cache
    ok = 1;
  } else {

    String sDigest;
    {
      // Fetch digest from DB
      Msg_DB_Get msg;
      msg.sName = sDb_;
      msg.sKey.appendf("%s digest", StringType(prepareDbKey(pMsg->sUrl)));
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_LoadContainerFromStorage", "Msg_DB_Get failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)));
      } else {
        sDigest = msg.sValue;
      }
    }

    if (!sDigest.empty()) {
      // Fetch data from DB
      Msg_DB_Get msg;
      msg.sName = sDb_;
      msg.sKey.appendf("%s data", StringType(prepareDbKey(pMsg->sUrl)));
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_LoadContainerFromStorage", "Msg_DB_Get failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)));
      } else {
        Msg_Identity_SetContainer msgCCI;
        msgCCI.sUrl = pMsg->sUrl;
        msgCCI.sDigest = sDigest;
        msgCCI.sData = msg.sValue;
        ok = msgCCI.Request();
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_LoadContainerFromStorage", "Msg_Identity_SetContainer failed: src=%s", StringType(pMsg->sUrl)));
        }
      }
    }

  } // !Already in cache

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_IsContainerAvailable)
{
  int ok = 1;

  pMsg->bAvailable = 0;

  Msg_Identity_LoadContainerFromStorage msg;
  msg.sUrl = pMsg->sUrl;
  if (!msg.Request()) {
    // Could not be loaded: no error
    apLog_Verbose((LOG_CHANNEL, "IdentityModule::Identity_IsContainerAvailable", "Msg_Identity_LoadContainerFromStorage failed: src=%s", StringType(pMsg->sUrl)));
  } else {
    Container* pContainer = 0;
    if (data_.Get(pMsg->sUrl, pContainer)) {
      if (pContainer->getDigest() == pMsg->sDigest) {
        pMsg->bAvailable = 1;
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_DeleteContainer)
{
  int ok = 1;

  if (ok) {
    Msg_Identity_DeleteContainerFromStorage msg;
    msg.sUrl = pMsg->sUrl;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_DeleteContainer", "Msg_Identity_DeleteContainerFromStorage failed: url=%s", StringType(msg.sUrl)));
    }
  }

  if (ok) {
    Msg_Identity_DeleteContainerFromMemory msg;
    msg.sUrl = pMsg->sUrl;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_DeleteContainer", "Msg_Identity_DeleteContainerFromMemory failed: url=%s", StringType(msg.sUrl)));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_DeleteContainerFromMemory)
{
  int ok = 0;

  Container* pContainer = 0;
  if (data_.Get(pMsg->sUrl, pContainer)) {
    if (data_.Unset(pMsg->sUrl)) {
      delete pContainer;
      pContainer = 0;
    }
    ok = 1;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_DeleteContainerFromStorage)
{
  int ok = 1;

  if (ok) {
    Msg_DB_Delete msg;
    msg.sName = sDb_;
    msg.sKey.appendf("%s data", StringType(prepareDbKey(pMsg->sUrl)));
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_DeleteContainerFromStorage", "Msg_DB_Delete failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)));
    }
  }

  if (ok) {
    Msg_DB_Delete msg;
    msg.sName = sDb_;
    msg.sKey.appendf("%s digest", StringType(prepareDbKey(pMsg->sUrl)));
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_DeleteContainerFromStorage", "Msg_DB_Delete failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------
// Identity evaluation progress

//----------------------------
// Getter

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_HasProperty)
{
  int ok = 0;

  Msg_Identity_LoadContainerFromStorage msg;
  msg.sUrl = pMsg->sUrl;
  if (!msg.Request()) {
    // Could not be loaded
    apLog_Warning((LOG_CHANNEL, "IdentityModule::Identity_HasProperty", "Msg_Identity_LoadContainerFromStorage failed: src=%s", StringType(pMsg->sUrl)));
  } else {
    Container* pContainer = 0;
    if (data_.Get(pMsg->sUrl, pContainer)) {
      ok = 1;
      pMsg->bIsProperty = pContainer->hasProperty(pMsg->sKey);
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_GetProperty)
{
  int ok = 0;

  Msg_Identity_LoadContainerFromStorage msg;
  msg.sUrl = pMsg->sUrl;
  if (!msg.Request()) {
    // Could not be loaded
    apLog_Warning((LOG_CHANNEL, "IdentityModule::Identity_GetProperty", "Msg_Identity_LoadContainerFromStorage failed: src=%s", StringType(pMsg->sUrl)));
  } else {
    Container* pContainer = 0;
    if (data_.Get(pMsg->sUrl, pContainer)) {
      ok = 1;
      pMsg->sValue = pContainer->getProperty(pMsg->sKey);
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_GetItemIds)
{
  int ok = 0;

  Msg_Identity_LoadContainerFromStorage msg;
  msg.sUrl = pMsg->sUrl;
  if (!msg.Request()) {
    // Could not be loaded
    apLog_Warning((LOG_CHANNEL, "IdentityModule::Identity_GetItemIds", "On_Identity_GetItemIds failed: src=%s", StringType(pMsg->sUrl)));
  } else {
    Container* pContainer = 0;
    if (data_.Get(pMsg->sUrl, pContainer)) {
      if (pMsg->sType.empty() && pMsg->vlMimeTypes.length() == 0 && pMsg->nMax == 0) {
        ok = pContainer->getItemIds(pMsg->vlIds);      
      } else {
        ok = pContainer->getItemIds(pMsg->sType, pMsg->vlMimeTypes, pMsg->nMax, pMsg->vlIds);      
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------
// Items

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_GetItem)
{
  int ok = 0;

  Msg_Identity_LoadContainerFromStorage msg;
  msg.sUrl = pMsg->sUrl;
  if (!msg.Request()) {
    // Could not be loaded
    apLog_Warning((LOG_CHANNEL, "IdentityModule::Identity_GetItem", "On_Identity_GetItemIds failed: src=%s", StringType(pMsg->sUrl)));
  } else {
    Container* pContainer = 0;
    if (data_.Get(pMsg->sUrl, pContainer)) {

      List lAttributes;
      ok = pContainer->getItem(pMsg->sId, lAttributes);
      for (Elem* e = 0; (e = lAttributes.Next(e)) != 0; ) {
        String sKey = e->getName();
        if (0) {
        } else if (sKey == "digest") {
          pMsg->sDigest = e->getString();
        } else if (sKey == "contenttype" || sKey == "type") {
          pMsg->sType = e->getString();
        } else if (sKey == "mimetype") {
          pMsg->sMimeType = e->getString();
        } else if (sKey == "order") {
          pMsg->nOrder = String::atoi(e->getString());
        } else if (sKey == "size") {
          pMsg->nSize = String::atoi(e->getString());
        } else if (sKey == "src") {
          pMsg->sSrc = e->getString();
        }
      }

    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_IsItemDataAvailable)
{
  int ok = 0;

  pMsg->bAvailable = 0;

  Msg_Identity_LoadContainerFromStorage msg;
  msg.sUrl = pMsg->sUrl;
  if (!msg.Request()) {
    // Could not be loaded
    apLog_Warning((LOG_CHANNEL, "IdentityModule::Identity_IsItemDataAvailable", "Msg_Identity_LoadContainerFromStorage failed: url=%s", StringType(pMsg->sUrl)));
  } else {
    Container* pContainer = 0;
    if (data_.Get(pMsg->sUrl, pContainer)) {
      ok = 1;
      pMsg->bAvailable = pContainer->hasItemData(pMsg->sId);

      if (!pMsg->bAvailable) {
        Msg_Identity_LoadItemDataFromStorage msg;
        msg.sUrl = pMsg->sUrl;
        msg.sId = pMsg->sId;
        if (!msg.Request()) {
          // not in db, ok, shit happnz
        } else {
          pMsg->bAvailable = pContainer->hasItemData(pMsg->sId);
        }
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_GetItemData)
{
  int ok = 0;

  Msg_Identity_LoadContainerFromStorage msg;
  msg.sUrl = pMsg->sUrl;
  if (!msg.Request()) {
    // Could not be loaded
    apLog_Warning((LOG_CHANNEL, "IdentityModule::Identity_GetItemData", "Msg_Identity_LoadContainerFromStorage failed: url=%s id=%s", StringType(pMsg->sUrl), StringType(pMsg->sId)));
  } else {
    Container* pContainer = 0;
    if (data_.Get(pMsg->sUrl, pContainer)) {
      int bGetIt = 0;

      if (pContainer->hasItemData(pMsg->sId)) {
        bGetIt = 1;
      } else {
        Msg_Identity_LoadItemDataFromStorage msg;
        msg.sUrl = pMsg->sUrl;
        msg.sId = pMsg->sId;
        if (!msg.Request()) {
          // not in db, ok, shit happnz
        } else {
          if (!pContainer->hasItemData(pMsg->sId)) {
            apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_GetItemData", "pContainer->hasItemData() failed although just loaded: url=%s id=%s", StringType(pMsg->sUrl), StringType(pMsg->sId)));
          } else {
            bGetIt = 1;
          }
        }
      }

      if (bGetIt) {
        ok = pContainer->getItemData(pMsg->sId, pMsg->sMimeType, pMsg->sbData);
      }

    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_RequestItem)
{
  int ok = 0;
  
  ItemRequest* pRequest = lItemRequests_.FindByName(pMsg->sSrc);
  if (pRequest != 0) {
    pRequest->addHandle(pMsg->hRequest);
    apLog_Info((LOG_CHANNEL, "IdentityModule::Identity_RequestItem", "Ignored, because already in progress, src=%s", StringType(pMsg->sUrl)));
    ok = 1;

  } else {

    ItemClient* pClient = new ItemClient(pMsg->hRequest, pMsg->sUrl, pMsg->sId, pMsg->sDigest);
    if (pClient != 0) {
      ok = pClient->Get(pMsg->sSrc);
      if (!ok) {
        apLog_Verbose((LOG_CHANNEL, "IdentityModule::Identity_RequestItem", "pClient->Get() failed, src=%s", StringType(pMsg->sSrc)));
        delete pClient;
        pClient = 0;
      } else {
        pRequest = lItemRequests_.Add(pMsg->sSrc);
        if (pRequest != 0) {
          pRequest->addHandle(pMsg->hRequest);
        }
      }
    }
  }
  
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_ReceiveItem)
{
  int ok = 1;

  int bSuccess = 0;
  String sError;

  if (pMsg->sbData.Length() == 0) {
    sError = "No data";
  } else {
    if (ok) {
      Msg_Identity_SetItemData msg;
      msg.sUrl = pMsg->sUrl;
      msg.sId = pMsg->sId;
      msg.sMimeType = pMsg->sMimeType;
      msg.sbData = pMsg->sbData;
      ok = msg.Request();
      if (!ok) {
        sError.appendf("Msg_Identity_SetItemData failed: %s, url=%s id=%s src=%s ", StringType(pMsg->sComment), StringType(pMsg->sUrl), StringType(pMsg->sId), StringType(pMsg->sSrc));
        apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ReceiveItem", sError));
      } else {
        bSuccess = 1;
      }
    }

    if (ok) {
      Msg_Identity_SaveItemDataToStorage msg;
      msg.sUrl = pMsg->sUrl;
      msg.sId = pMsg->sId;
      ok = msg.Request();
      if (!ok) {
        sError.appendf("Msg_Identity_SaveItemDataToStorage failed: src=%s", StringType(pMsg->sUrl));
        apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ReceiveItem", sError));
      }
    }
  } // pMsg->sbData.Length()

  ItemRequest* pRequest = lItemRequests_.FindByName(pMsg->sSrc);
  if (pRequest != 0) {
    for (Apollo::ValueElem* e = 0; (e = pRequest->getHandles().nextElem(e)) != 0; ) {
      ApHandle hRequest = e->getHandle();

      {
        ItemAcquisitionTask* pTask = findItemAcquisitionTaskByRequest(hRequest);
        if (pTask != 0) {
          // Someone needed an item not only the container
          int bAcquisitionFinished = 1;
          int bAcquisitionSuccess = 0;

          if (bSuccess) {
            // Check if the data is already here, otherwise request the data

            String sId = selectItemId(pTask->getUrl(), pTask->getType(), pTask->getMIMEType());

            if (!sId.empty()) {
              Msg_Identity_IsItemDataAvailable msgIIIDA;
              msgIIIDA.sUrl = pTask->getUrl();
              msgIIIDA.sId = sId;
              ok = msgIIIDA.Request();
              if (!ok) {
                apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ReceiveItem", "Msg_Identity_IsItemDataAvailable failed url=%s id=%s", StringType(msgIIIDA.sUrl), StringType(msgIIIDA.sId)));
              } else {

                if (msgIIIDA.bAvailable) {
                  bAcquisitionSuccess = 1;
                }
              } // msgIIIDA
            } // sId
          } // bSuccess

          if (bAcquisitionFinished) {
            Msg_Identity_AcquireItemDataComplete pMsgIAIDC;
            pMsgIAIDC.hAcquisition = pTask->apHandle();
            pMsgIAIDC.sUrl = pTask->getUrl();
            pMsgIAIDC.sType = pTask->getType();
            pMsgIAIDC.bSuccess = bAcquisitionSuccess;
            pMsgIAIDC.Send();

            if (itemTasks_.Unset(pTask->apHandle())) {
              delete pTask;
              pTask = 0;
            }
          }
        }
      }

      {
        Msg_Identity_RequestItemComplete msg;
        msg.hRequest = hRequest;
        msg.sUrl = pMsg->sUrl;
        msg.sId = pMsg->sId;
        msg.sDigest = pMsg->sDigest;
        msg.bSuccess = bSuccess;
        if (!bSuccess) {
          msg.sComment = sError;
        }
        msg.sbData = pMsg->sbData;
        msg.Send();
      }

    } // for getHandles()
    lItemRequests_.Remove(pRequest);
    delete pRequest;
    pRequest = 0;
  } // pRequest

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_SetItemData)
{
  int ok = 0;

  Msg_Identity_LoadContainerFromStorage msg;
  msg.sUrl = pMsg->sUrl;
  if (!msg.Request()) {
    // Could not be loaded
    apLog_Warning((LOG_CHANNEL, "IdentityModule::Identity_GetItemData", "On_Identity_GetItemIds failed: src=%s", StringType(pMsg->sUrl)));
  } else {
    Container* pContainer = 0;
    if (data_.Get(pMsg->sUrl, pContainer)) {
      ok = pContainer->setItemData(pMsg->sId, pMsg->sMimeType, pMsg->sbData);
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_SaveItemDataToStorage)
{
  Container* pContainer = 0;
  if (!data_.Get(pMsg->sUrl, pContainer)) { throw ApException("data_.Get() failed: src=%s", StringType(pMsg->sUrl)); }

  String sSrc = pContainer->getItemAttribute(pMsg->sId, "src");
  if (!sSrc) { throw ApException("pContainer->getItemAttribute: no src for url=%s id=%s", StringType(pMsg->sUrl), StringType(pMsg->sId)); }

  String sMimeType;

  Msg_DB_SetBinary msgDSB;
  msgDSB.sName = sDb_;
  msgDSB.sKey.appendf("%s data", StringType(prepareDbKey(sSrc)));
  pContainer->getItemData(pMsg->sId, sMimeType, msgDSB.sbValue);
  if (!msgDSB.Request()) { throw ApException("Msg_DB_SetBinary failed: db=%s key=%s", StringType(msgDSB.sName), StringType(msgDSB.sKey)); }

  Msg_DB_Set msgDS;
  msgDS.sName = sDb_;
  msgDS.sKey.appendf("%s mimetype", StringType(prepareDbKey(sSrc)));
  msgDS.sValue = sMimeType;
  if (!msgDS.Request()) { throw ApException("Msg_DB_Set failed: db=%s key=%s", StringType(msgDS.sName), StringType(msgDS.sKey)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_LoadItemDataFromStorage)
{
  Container* pContainer = 0;
  if (!data_.Get(pMsg->sUrl, pContainer)) { throw ApException("data_.Get() failed: src=%s", StringType(pMsg->sUrl)); }

  String sSrc = pContainer->getItemAttribute(pMsg->sId, "src");
  if (!sSrc) { throw ApException("pContainer->getItemAttribute: no src for url=%s id=%s", StringType(pMsg->sUrl), StringType(pMsg->sId)); }

  String sMimeType;

  // Fetch mimetype from DB
  Msg_DB_Get msgDG;
  msgDG.sName = sDb_;
  msgDG.sKey.appendf("%s mimetype", StringType(prepareDbKey(sSrc)));
  if (!msgDG.Request()) { throw ApException("Msg_DB_Get failed: db=%s key=%s", StringType(msgDG.sName), StringType(msgDG.sKey)); }

  sMimeType = msgDG.sValue;
  if (sMimeType) { // No MimeType, then do not fetch the data

    // Fetch data from DB
    Msg_DB_GetBinary msgDGB;
    msgDGB.sName = sDb_;
    msgDGB.sKey.appendf("%s data", StringType(prepareDbKey(sSrc)));
    if (!msgDGB.Request()) { throw ApException("Msg_DB_GetBinary failed: db=%s key=%s", StringType(msgDGB.sName), StringType(msgDGB.sKey)); }

    Msg_Identity_SetItemData msgISID;
    msgISID.sUrl = pMsg->sUrl;
    msgISID.sId = pMsg->sId;
    msgISID.sMimeType = sMimeType;
    msgISID.sbData = msgDGB.sbValue;
    if (!msgISID.Request()) { throw ApException("Msg_Identity_SetItemData failed: url=%s id=%s", StringType(msgISID.sUrl), StringType(msgISID.sId)); }

  } // sMimeType

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_DeleteItemData)
{
  Msg_Identity_DeleteItemDataFromStorage msgIDIDFS;
  msgIDIDFS.sUrl = pMsg->sUrl;
  msgIDIDFS.sId = pMsg->sId;
  if (!msgIDIDFS.Request()) { throw ApException("Msg_Identity_DeleteItemDataFromStorage failed: url=%s", StringType(msgIDIDFS.sUrl)); }

  Msg_Identity_DeleteItemDataFromMemory msgIDIDFM;
  msgIDIDFM.sUrl = pMsg->sUrl;
  msgIDIDFM.sId = pMsg->sId;
  if (!msgIDIDFM.Request()) { throw ApException("Msg_Identity_DeleteItemDataFromMemory failed: url=%s", StringType(msgIDIDFM.sUrl)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_DeleteItemDataFromMemory)
{
  int ok = 0;

  Container* pContainer = 0;
  if (!data_.Get(pMsg->sUrl, pContainer)) {
    // Tolerate if no container: nothing to do
    ok = 1;
    apLog_Warning((LOG_CHANNEL, "IdentityModule::Identity_SaveContainerToStorage", "data_.Get() failed: src=%s", StringType(pMsg->sUrl)));
  } else {
    ok = pContainer->deleteItemData(pMsg->sId);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_DeleteItemDataFromStorage)
{
  Container* pContainer = 0;
  if (!data_.Get(pMsg->sUrl, pContainer)) { throw ApException("data_.Get() failed: src=%s", StringType(pMsg->sUrl)); }

  String sSrc = pContainer->getItemAttribute(pMsg->sId, "src");
  if (!sSrc) { throw ApException("pContainer->getItemAttribute: no src for url=%s id=%s", StringType(pMsg->sUrl), StringType(pMsg->sId)); }

  {
    Msg_DB_Delete msg;
    msg.sName = sDb_;
    msg.sKey.appendf("%s data", StringType(prepareDbKey(sSrc)));
    if (!msg.Request()) { throw ApException("Msg_DB_Delete failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)); }
  }

  {
    Msg_DB_Delete msg;
    msg.sName = sDb_;
    msg.sKey.appendf("%s mimetype", StringType(prepareDbKey(sSrc)));
    if (!msg.Request()) { throw ApException("Msg_DB_Delete failed: db=%s key=%s", StringType(msg.sName), StringType(msg.sKey)); }
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_AcquireItemData)
{
  int ok = 0;

  int bAcquisitionFinished = 1;
  int bAcquisitionSuccess = 0;

  Msg_Identity_IsContainerAvailable msgIICA;
  msgIICA.sUrl = pMsg->sUrl;
  msgIICA.sDigest = pMsg->sDigest;
  ok = msgIICA.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_AcquireItemData", "Msg_Identity_IsContainerAvailable failed url=%s", StringType(msgIICA.sUrl)));
  } else {

    if (!msgIICA.bAvailable) {

      // Fetch container and prepare for item data fetch if necessary
      ItemAcquisitionTask* pTask = new ItemAcquisitionTask(pMsg->hAcquisition, pMsg->sUrl, pMsg->sType, "");
      if (pTask != 0) {
        pTask->setRequest(Apollo::newHandle());
        itemTasks_.Set(pTask->apHandle(), pTask);

        // This should really check if we already tried and failed
        Msg_Identity_RequestContainer msgIRC;
        msgIRC.hRequest = pTask->getRequest();
        msgIRC.sUrl = pMsg->sUrl;
        msgIRC.sDigest = pMsg->sDigest;
        ok = msgIRC.Request();
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_AcquireItemData", "Msg_Identity_RequestContainer failed url=%s", StringType(msgIRC.sUrl)));
          if (itemTasks_.Unset(pTask->apHandle())) {
            delete pTask;
            pTask = 0;
          }
        } else {
          bAcquisitionFinished = 0;
        }
      }

    } else {

      String sId = selectItemId(pMsg->sUrl, pMsg->sType, pMsg->sMimeType);
      if (!sId.empty()) {

        Msg_Identity_IsItemDataAvailable msgIIIDA;
        msgIIIDA.sUrl = pMsg->sUrl;
        msgIIIDA.sId = sId;
        ok = msgIIIDA.Request();
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_AcquireItemData", "Msg_Identity_IsItemDataAvailable failed url=%s id=%s", StringType(msgIIIDA.sUrl), StringType(msgIIIDA.sId)));
        } else {

          if (msgIIIDA.bAvailable) {
            bAcquisitionSuccess = 1;
          } else {

            // Fetch item
            Msg_Identity_GetItem msgIGI;
            msgIGI.sUrl = pMsg->sUrl;
            msgIGI.sId = sId;
            ok = msgIGI.Request();
            if (!ok) {
              apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_AcquireItemData", "Msg_Identity_GetItem failed url=%s id=%s", StringType(msgIGI.sUrl), StringType(msgIGI.sId)));
            } else {

              // This should really check if we already tried and failed
              Msg_Identity_RequestItem msgIRI;
              msgIRI.hRequest = Apollo::newHandle();
              msgIRI.sUrl = pMsg->sUrl;
              msgIRI.sId = sId;
              msgIRI.sSrc = msgIGI.sSrc;
              msgIRI.sDigest = msgIGI.sDigest;
              ok = msgIRI.Request();
              if (!ok) {
                apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_AcquireItemData", "Msg_Identity_GetItem failed url=%s id=%s", StringType(msgIRI.sUrl), StringType(msgIRI.sId)));
              } else {
                bAcquisitionFinished = 0;
              }

            } // msgIGI
          } // !msgIIIDA.bAvailable
        } // msgIIIDA
      } // sId
    } // msgIICA.bAvailable
  } // msgIICA

  if (bAcquisitionFinished) {
    ApAsyncMessage<Msg_Identity_AcquireItemDataComplete> msgIAIDC;
    msgIAIDC->hAcquisition = pMsg->hAcquisition;
    msgIAIDC->sUrl = pMsg->sUrl;
    msgIAIDC->sType = pMsg->sType;
    msgIAIDC->bSuccess = bAcquisitionSuccess;
    msgIAIDC.Post();
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------
// Memory cache

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_ClearAllCache)
{
  int ok = 1;

  Msg_Identity_ExpireAllCache msg;
  msg.nAge = 0;
  ok = msg.Request();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_ExpireAllCache)
{
  int ok = 1;

  List lDeleteKeys;

  time_t tLimit = Apollo::getNow().Sec() - pMsg->nAge;

  IdentityIterator iter(data_);
  for (IdentityNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
    Container* pContainer = pNode->Value();
    if (pContainer != 0) {
      if (pContainer->getAccessTime() <= tLimit) {
        lDeleteKeys.Add(pNode->Key());
      }
    }
  }

  for (Elem* e = 0; (e = lDeleteKeys.Next(e)) != 0; ) {
    Container* pContainer = 0;
    if (data_.Get(e->getName(), pContainer)) {
      if (data_.Unset(e->getName())) {
        delete pContainer;
        pContainer = 0;
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------
// Persistent storage

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_ClearAllStorage)
{
  int ok = 1;

  Msg_Identity_ExpireAllStorage msg;
  msg.nAge = 0;
  ok = msg.Request();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_ExpireAllStorage)
{
  int ok = 0;

  Msg_DB_DeleteOlderThan msg;
  msg.sName = sDb_;
  msg.nAge = pMsg->nAge;
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_ExpireAllStorage", "Msg_DB_DeleteOlderThan failed"));
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Identity_SetStorageName)
{
  int ok = 1;

  pMsg->sPreviousName = sDb_;
  sDb_ = pMsg->sName;

  if (pMsg->sName != pMsg->sPreviousName) {

    {
      Msg_DB_Close msg;
      msg.sName = pMsg->sPreviousName;
      int ok = msg.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_SetStorageName", "Msg_DB_Close %s failed", StringType(pMsg->sPreviousName)));
      }
    }

    {
      Msg_DB_Open msg;
      msg.sName = pMsg->sName;
      int ok = msg.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "IdentityModule::Identity_SetStorageName", "Msg_DB_Open %s failed", StringType(pMsg->sName)));
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(IdentityModule, Server_HttpRequest)
{
  #define IdentityModule_Server_HttpRequest_sUriPrefix "/" MODULE_NAME

  if (Apollo::getModuleConfig(MODULE_NAME, "HTTP/Enabled", 1) && pMsg->sUri.startsWith(IdentityModule_Server_HttpRequest_sUriPrefix)) {
    String sUriPrefix = IdentityModule_Server_HttpRequest_sUriPrefix;
    try {
      String sQuery = pMsg->sUri;
      String sBase; sQuery.nextToken("?", sBase);

      String sCmd;
      List lQuery;
      KeyValueBlob2List(sQuery, lQuery, "&", "=", "");
      for (Elem* e = 0; (e = lQuery.Next(e)) != 0; ) {
        String sKey = e->getName();
        String sValue = e->getString();
        sKey.unescape(String::EscapeURL);
        sValue.unescape(String::EscapeURL);
        e->setName(sKey);
        e->setString(sValue);
      }
      sCmd = lQuery["cmd"].getString();
      if (sCmd.empty()) { sCmd = "menu"; }

      Msg_Server_HttpResponse msgSHR;

      String sHtml;
      Apollo::UriBuilder baseUri;
      baseUri.setPath("/");
      baseUri.setFile(MODULE_NAME);

      if (0) {
      } else if (sCmd == "menu") {
        Apollo::UriBuilder uri = baseUri;
        uri.setQueryParam("cmd", "cache");
        sHtml.appendf("<a href=\"%s\">List cache</a>", StringType(uri()));

      } else if (sCmd == "cache") {
        IdentityIterator iter(data_);
        for (IdentityNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
          Container* pContainer = pNode->Value();
          if (pContainer) {
            Apollo::UriBuilder containerUri = baseUri;
            containerUri.setQueryParam("cmd", "data");
            containerUri.setQueryParam("url", pContainer->getUrl());
            sHtml.appendf("<a href=\"%s\">%s</a>", StringType(containerUri()), StringType(pContainer->getUrl()));

            Apollo::ValueList vlIds;
            if (pContainer->getItemIds(vlIds)) {
              for (Apollo::ValueElem* e = 0; (e = vlIds.nextElem(e)) != 0; ) {
                String sItemId = e->getString();
                if (pContainer->hasItemData(sItemId)) {
                  Apollo::UriBuilder itemUri = containerUri;
                  itemUri.setQueryParam("id", sItemId);
                  sHtml.appendf(" <a href=\"%s\">%s</a>", StringType(itemUri()), StringType(sItemId));
                } else {
                  if (!pContainer->getItemAttribute(sItemId, "src").empty()) {
                    sHtml.appendf(" <a href=\"%s\">%s</a>", StringType(pContainer->getItemAttribute(sItemId, "src")), StringType(sItemId));
                  } else {
                    sHtml.appendf(" %s", StringType(sItemId));
                  }
                }
              }
            }
            sHtml += "\n";
          }
        }

      } else if (sCmd == "data") {
        String sUrl;
        String sId;
        sUrl = lQuery["url"].getString();
        sId = lQuery["id"].getString();

        // Return identity data or item data
        if (sUrl.empty()) { throw ApException("url empty: uri=%s", StringType(sUrl), StringType(pMsg->sUri)); }

        Container* pContainer = 0;
        if (!data_.Get(sUrl, pContainer)) { throw ApException("Not found: url=%s", StringType(sUrl)); }

        if (sId.empty()) {
          msgSHR.sbBody.SetData(pContainer->getData());
          msgSHR.kvHeader.add("Content-type", "text/xml");
        } else {
          String sMimeType;
          if (!pContainer->getItemData(sId, sMimeType, msgSHR.sbBody)) { throw ApException("Not found: url=%s: id=%s", StringType(sUrl), StringType(sId)); }
          msgSHR.kvHeader.add("Content-type", sMimeType);
        }
      } else {
         throw ApException("Unknown cmd=%s", StringType(sCmd));
      }

      if (!sHtml.empty()) {
        String sData = "<html><head><title>Identity</title></head><body><pre>" + sHtml + "</pre></body></html>";
        msgSHR.sbBody.SetData(sData);
        msgSHR.kvHeader.add("Content-type", "text/html");
      }

      msgSHR.hConnection = pMsg->hConnection;
      msgSHR.kvHeader.add("Pragma", "no-cache");
      msgSHR.kvHeader.add("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
      msgSHR.kvHeader.add("Expires", "Thu, 19 Nov 1981 08:52:00 GMT");
      if (!msgSHR.Request()) { throw ApException("Msg_Server_HttpResponse failed: conn=" ApHandleFormat "", ApHandleType(msgSHR.hConnection)); }

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

  } // sUriPrefix
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(IdentityModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  IdentityModuleTester::begin();
}

AP_MSG_HANDLER_METHOD(IdentityModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  IdentityModuleTester::execute();
}

AP_MSG_HANDLER_METHOD(IdentityModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  IdentityModuleTester::end();
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int IdentityModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, System_AfterLoadModules, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, System_BeforeUnloadModules, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_RequestContainer, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_ReceiveContainer, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_SetContainer, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_SaveContainerToStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_LoadContainerFromStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_IsContainerAvailable, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_DeleteContainer, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_DeleteContainerFromMemory, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_DeleteContainerFromStorage, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_HasProperty, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_GetProperty, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_GetItemIds, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_GetItem, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_IsItemDataAvailable, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_GetItemData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_RequestItem, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_ReceiveItem, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_SetItemData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_SaveItemDataToStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_LoadItemDataFromStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_DeleteItemData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_DeleteItemDataFromMemory, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_DeleteItemDataFromStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_AcquireItemData, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_ClearAllCache, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_ExpireAllCache, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_ClearAllStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_ExpireAllStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Identity_SetStorageName, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, Server_HttpRequest, this, ApCallbackPosNormal);

#if defined(AP_TEST)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, UnitTest_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, UnitTest_Execute, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, IdentityModule, UnitTest_End, this, ApCallbackPosNormal);
#endif

  return ok;
}

void IdentityModule::exit()
{
  AP_MSG_REGISTRY_FINISH;
}
