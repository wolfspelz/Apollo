// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "MsgConfig.h"
#include "MsgDB.h"
#include "URL.h"
#include "GalileoModule.h"
#include "GalileoModuleTester.h"
#include "AnimationClient.h"

GalileoModule::GalileoModule()
:bInShutdown_(false)
,nLocalHttpServerPort_(23761)
,bLocalHttpServerActive_(0)
,sDb_(DB_NAME)
{
}

GalileoModule::~GalileoModule()
{
}

int GalileoModule::AnimationIsRequested(const String& sUrl)
{
  int bIsIt = 0;

  if (requestedAnimations_.Find(sUrl) != 0) {
    bIsIt = 1;
  }

  return bIsIt;
}

Item* GalileoModule::GetItem(const ApHandle& hItem)
{
  ItemListNode* pNode = items_.Find(hItem);
  if (pNode == 0) { throw ApException(LOG_CONTEXT, "Item not found: " ApHandleFormat "", ApHandlePrintf(hItem)); }
  Item* pItem = pNode->Value();
  if (pItem == 0) { throw ApException(LOG_CONTEXT, "Node Value() empty: " ApHandleFormat "", ApHandlePrintf(hItem)); }
  return pItem;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_Create)
{
  if (pMsg->sMimeType == "avatar/gif") {
    // Handle it
  } else {
    // Unhandled, no pMsg->apStatus, hope, that someone else likes it
    return;
  }

  if (items_.Find(pMsg->hItem) != 0) { throw ApException(LOG_CONTEXT, "hItem=" ApHandleFormat ", already exists", ApHandlePrintf(pMsg->hItem)); }

  Item* pItem = new Item(pMsg->hItem, this);
  if (pItem == 0) { throw ApException(LOG_CONTEXT, "new Item() failed, hItem=" ApHandleFormat "", ApHandlePrintf(pMsg->hItem)); }

  items_.Set(pMsg->hItem, pItem);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_Destroy)
{
  Item* pItem = GetItem(pMsg->hItem);

  items_.Unset(pMsg->hItem);
  delete pItem;
  pItem = 0;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_Start)
{
  GetItem(pMsg->hItem)->Start();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_Stop)
{
  GetItem(pMsg->hItem)->Stop();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_SetRate)
{
  Item* pItem = GetItem(pMsg->hItem);

  if (pMsg->nMaxRate > 0 && pMsg->nMaxRate < 25) {
    int nDelay = 1000 / pMsg->nMaxRate;
    pItem->SetDelay(nDelay);
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_SetData)
{
  GetItem(pMsg->hItem)->SetData(pMsg->sbData, pMsg->sSourceUrl);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_SetStatus)
{
  GetItem(pMsg->hItem)->SetStatus(pMsg->sStatus);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_SetCondition)
{
  GetItem(pMsg->hItem)->SetCondition(pMsg->sCondition);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_Event)
{
  GetItem(pMsg->hItem)->PlayEvent(pMsg->sEvent);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_Activity)
{
  GetItem(pMsg->hItem)->PlayActivity(pMsg->sActivity);
  pMsg->apStatus = ApMessage::Ok;
}

//AP_MSG_HANDLER_METHOD(GalileoModule, Animation_Static)
//{
//  GetItem(pMsg->hItem)->PlayStatic(pMsg->bState);
//  pMsg->apStatus = ApMessage::Ok;
//}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_GetGroups)
{
  GetItem(pMsg->hItem)->GetGroups(pMsg->vlGroups);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_GetGroupSequences)
{
  GetItem(pMsg->hItem)->GetGroupSequences(pMsg->sGroup, pMsg->vlSequences);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Animation_GetSequenceInfo)
{
  GetItem(pMsg->hItem)->GetSequenceInfo(pMsg->sSequence
                                        ,pMsg->sGroup
                                        ,pMsg->sType
                                        ,pMsg->sCondition
                                        ,pMsg->sSrc
                                        ,pMsg->nProbability
                                        ,pMsg->sIn
                                        ,pMsg->sOut
                                        ,pMsg->nDx
                                        ,pMsg->nDy
                                        ,pMsg->nDuration
                                       );
  pMsg->apStatus = ApMessage::Ok;
}

//AP_MSG_HANDLER_METHOD(GalileoModule, Animation_SetPosition)
//{
//  ItemListNode* pNode = items_.Find(pMsg->hItem);
//  if (pNode == 0) { return; }
//  Item* pItem = pNode->Value();
//
//  pItem->SetPosition(pMsg->nX);
//
//  pMsg->apStatus = ApMessage::Ok;
//}
//
//AP_MSG_HANDLER_METHOD(GalileoModule, Animation_MoveTo)
//{
//  ItemListNode* pNode = items_.Find(pMsg->hItem);
//  if (pNode == 0) { return; }
//  Item* pItem = pNode->Value();
//
//  pItem->MoveTo(pMsg->nX);
//
//  pMsg->apStatus = ApMessage::Ok;
//}
//
//AP_MSG_HANDLER_METHOD(GalileoModule, Animation_GetPosition)
//{
//  ItemListNode* pNode = items_.Find(pMsg->hItem);
//  if (pNode == 0) { return; }
//  Item* pItem = pNode->Value();
//
//  //pItem->GetPosition();
//
//  pMsg->apStatus = ApMessage::Ok;
//}

Item* GalileoModule::GetItemByTimer(const ApHandle& hTimer)
{
  Item* pResult = 0;

  ItemListIterator iter(items_);
  for (ItemListNode* pNode = 0; pNode = iter.Next(); ) {
    Item* pItem = pNode->Value();
    if (pItem != 0) {
      if (pItem->HasTimer(hTimer)) {
        pResult = pItem;
        break;
      }
    }
  }
  
  return pResult;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Galileo_RequestAnimation)
{
  int ok = 0;

  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(pMsg->sUrl)));

  if (!AnimationIsRequested(pMsg->sUrl)) {

    AnimationClient* pClient = new AnimationClient(pMsg->hRequest);
    if (pClient != 0) {
      ok = pClient->Get(pMsg->sUrl);
      if (!ok) {
        apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "pClient->Get() failed, src=%s", _sz(pMsg->sUrl)));
        delete pClient;
        pClient = 0;
      } else {
        AnimationRequest ar(pMsg->sUrl);
        requestedAnimations_.Set(pMsg->sUrl, ar);
      }
    }

  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Galileo_RequestAnimationComplete)
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "%s success=%d len=%d mimetype=%s", _sz(pMsg->sUrl), pMsg->bSuccess, pMsg->sbData.Length(), _sz(pMsg->sMimeType)));

  if (AnimationIsRequested(pMsg->sUrl)) {
    requestedAnimations_.Unset(pMsg->sUrl);

    if (pMsg->bSuccess) {
      ItemListIterator iter(items_);
      for (ItemListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
        Item* pItem = pNode->Value();
        if (pItem) {
          pItem->SetAnimationData(pMsg->sUrl, pMsg->sbData, pMsg->sMimeType);
        }
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(GalileoModule, Galileo_SetStorageName)
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
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Close %s failed", _sz(pMsg->sPreviousName)));
      }
    }

    {
      Msg_DB_Open msg;
      msg.sName = pMsg->sName;
      int ok = msg.Request();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Open %s failed", _sz(pMsg->sName)));
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Galileo_SaveAnimationDataToStorage)
{
  Msg_DB_SetBinary msgDSB;
  msgDSB.sName = sDb_;
  msgDSB.sKey.appendf("%s data", _sz(PrepareDbKey(pMsg->sUrl)));
  msgDSB.sbValue = pMsg->sbData;
  if (!msgDSB.Request()) { throw ApException(LOG_CONTEXT, "Msg_DB_SetBinary failed: db=%s key=%s", _sz(msgDSB.sName), _sz(msgDSB.sKey)); }

  Msg_DB_Set msgDS;
  msgDS.sName = sDb_;
  msgDS.sKey.appendf("%s mimetype", _sz(PrepareDbKey(pMsg->sUrl)));
  msgDS.sValue = pMsg->sMimeType;
  if (!msgDS.Request()) { throw ApException(LOG_CONTEXT, "Msg_DB_Set failed: db=%s key=%s", _sz(msgDS.sName), _sz(msgDS.sKey)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Galileo_IsAnimationDataInStorage)
{
  String sMimeType;

  // Fetch mimetype from DB
  Msg_DB_HasKey msg;
  msg.sName = sDb_;
  msg.sKey.appendf("%s data", _sz(PrepareDbKey(pMsg->sUrl)));
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_DB_HasKey failed: db=%s key=%s", _sz(msg.sName), _sz(msg.sKey)); }

  pMsg->bAvailable = msg.bAvailable;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Galileo_LoadAnimationDataFromStorage)
{
  String sMimeType;

  // Fetch mimetype from DB
  Msg_DB_Get msgDG;
  msgDG.sName = sDb_;
  msgDG.sKey.appendf("%s mimetype", _sz(PrepareDbKey(pMsg->sUrl)));
  if (!msgDG.Request()) { throw ApException(LOG_CONTEXT, "Msg_DB_Get failed: db=%s key=%s", _sz(msgDG.sName), _sz(msgDG.sKey)); }

  // Fetch data from DB
  Msg_DB_GetBinary msgDGB;
  msgDGB.sName = sDb_;
  msgDGB.sKey.appendf("%s data", _sz(PrepareDbKey(pMsg->sUrl)));
  if (!msgDGB.Request()) { throw ApException(LOG_CONTEXT, "Msg_DB_GetBinary failed: db=%s key=%s", _sz(msgDGB.sName), _sz(msgDGB.sKey)); }

  pMsg->sbData = msgDGB.sbValue;
  pMsg->sMimeType = msgDG.sValue;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Galileo_ClearAllStorage)
{
  Msg_Galileo_ExpireAllStorage msg;
  msg.nAge = 0;
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_Galileo_ExpireAllStorage failed: msg.nAge=%d", msg.nAge); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Galileo_ExpireAllStorage)
{
  Msg_DB_DeleteOlderThan msg;
  msg.sName = sDb_;
  msg.nAge = pMsg->nAge;
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_DB_DeleteOlderThan failed: msg.nAge=%d", msg.nAge); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GalileoModule, Timer_Event)
{
  Item* pItem = GetItemByTimer(pMsg->hTimer);
  if (pItem == 0) { return; }

  pItem->OnTimer();
}

AP_MSG_HANDLER_METHOD(GalileoModule, System_AfterLoadModules)
{
  AP_UNUSED_ARG(pMsg);

  Msg_DB_Open msg;
  msg.sName = sDb_;
  int ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Open %s failed", _sz(msg.sName)));
  }
}

AP_MSG_HANDLER_METHOD(GalileoModule, System_BeforeUnloadModules)
{
  AP_UNUSED_ARG(pMsg);

  bInShutdown_ = true;

  Msg_DB_Close msg;
  msg.sName = sDb_;
  int ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Close %s failed", _sz(msg.sName)));
  }
}

AP_MSG_HANDLER_METHOD(GalileoModule, System_RunLevel)
{
  if (0) {
  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
    sLocalHttpServerAddress_ = Apollo::getConfig("Server/HTTP/Address", "localhost");
    nLocalHttpServerPort_ = Apollo::getConfig("Server/HTTP/Port", 23761);
    bLocalHttpServerActive_ = 1;
  }
}

String GalileoModule::GetSequenceDataCacheUrl(ApHandle& hItem, const String& sGroup, const String& sName)
{
  if (bLocalHttpServerActive_) {
    Apollo::UrlBuilder url;
    url.setHost(sLocalHttpServerAddress_);
    url.setPort(nLocalHttpServerPort_);
    url.setPath("/");
    url.setFile(MODULE_NAME);
    url.setQueryParam("cmd", "data");
    url.setQueryParam("id", hItem.toString());
    url.setQueryParam("group", sGroup);
    url.setQueryParam("name", sName);
    return url();
  } else {
    return "";
  }
}

AP_MSG_HANDLER_METHOD(GalileoModule, HttpServer_Request)
{
  #define GalileoModule_HttpServer_Request_sUriPrefix "/" MODULE_NAME

  if (Apollo::getModuleConfig(MODULE_NAME, "HTTP/Enabled", 1) && pMsg->sUri.startsWith(GalileoModule_HttpServer_Request_sUriPrefix)) {

    String sUriPrefix = GalileoModule_HttpServer_Request_sUriPrefix;
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

      Msg_HttpServer_SendResponse msgSHR;

      String sHtml;
      Apollo::UriBuilder baseUri;
      baseUri.setPath("/");
      baseUri.setFile(MODULE_NAME);

      if (0) {
      } else if (sCmd == "menu") {
        Apollo::UriBuilder uri = baseUri;
        uri.setQueryParam("cmd", "cache");
        sHtml.appendf("<a href=\"%s\">List cache</a>", _sz(uri()));

      } else if (sCmd == "cache") {
        ItemListIterator iter(items_);
        for (ItemListNode* pNode = 0; (pNode = iter.Next()) != 0; ) {
          Item* pItem = pNode->Value();
          if (pItem) {
            sHtml.appendf("%s <a href=\"%s\">%s</a>\n", _sz(pItem->apHandle().toString()), _sz(pItem->Src()), _sz(pItem->Src()));
            for (Group* pGroup = 0; (pGroup = pItem->Groups().Next(pGroup)) != 0; ) {
              for (Sequence* pSequence = 0; (pSequence = pGroup->Next(pSequence)) != 0; ) {

                Apollo::UriBuilder sequenceUri = baseUri;
                sequenceUri.setQueryParam("cmd", "data");
                sequenceUri.setQueryParam("id", pItem->apHandle().toString());
                sequenceUri.setQueryParam("group", pGroup->getName());
                sequenceUri.setQueryParam("name", pSequence->getName());
        
                Animation* pAnimation = pSequence->First();
                if (pAnimation) {
                  sHtml.appendf("  <a href=\"%s\">%s</a> %s %s <a href=\"%s\">%s</a>\n" 
                    , _sz(sequenceUri())
                    , _sz(pSequence->getName())
                    , pAnimation->HasData() ? "mem" : ""
                    , pAnimation->HasDataInCache() ? "storage" : ""
                    , _sz(pSequence->Src())
                    , _sz(pSequence->Src())
                    );
                } else {
                  sHtml.appendf("  tsnh: no animation\n");
                }

              }
           }
          }
        }

      } else if (sCmd == "data") {
        String sId = lQuery["id"].getString();
        String sGroup = lQuery["group"].getString();
        String sName = lQuery["name"].getString();

        ApHandle hItem = Apollo::string2Handle(sId);
        if (!ApIsHandle(hItem)) { throw ApException(LOG_CONTEXT, "No handle: id=%s", _sz(sId)); }
        if (!sGroup) { throw ApException(LOG_CONTEXT, "No group"); }
        if (!sName) { throw ApException(LOG_CONTEXT, "No name"); }

        ItemListNode* pItemNode = items_.Find(hItem);
        if (pItemNode == 0) { throw ApException(LOG_CONTEXT, "No item for " ApHandleFormat "", ApHandlePrintf(hItem)); }

        Item* pItem = pItemNode->Value();
        if (pItem == 0) { throw ApException(LOG_CONTEXT, "pItem == 0"); }

        Group* pGroup = pItem->Groups().FindByName(sGroup);
        if (pGroup == 0) { throw ApException(LOG_CONTEXT, "No group for group=%s", _sz(sGroup)); }

        Sequence* pSequence = pGroup->FindByName(sName);
        if (pSequence == 0) { throw ApException(LOG_CONTEXT, "No group for sequence name=%s", _sz(sName)); }

        Animation* pAnimation = pSequence->First();
        if (pAnimation == 0) { throw ApException(LOG_CONTEXT, "No first animation for sequence"); }

        int bLoadedForServer = 0;
        if (!pAnimation->HasData()) { 
          if (pAnimation->HasDataInCache()) {
            pAnimation->GetDataFromCache();
            if (pAnimation->HasData()) {
              bLoadedForServer = 1;
            }
          }
        }

        if (!pAnimation->HasData()) { throw ApException(LOG_CONTEXT, "Data not in memory"); }

        String sMimeType;
        pAnimation->GetAnimationData(msgSHR.sbBody, sMimeType);
        msgSHR.kvHeader.add("Content-type", sMimeType);

        if (bLoadedForServer) {
          pAnimation->FlushData();
        }

      } else {
         throw ApException(LOG_CONTEXT, "Unknown cmd=%s", _sz(sCmd));
      }

      if (!sHtml.empty()) {
        String sData = "<html><head><title>Galileo</title></head><body><pre>" + sHtml + "</pre></body></html>";
        msgSHR.sbBody.SetData(sData);
        msgSHR.kvHeader.add("Content-type", "text/html");
      }

      msgSHR.hConnection = pMsg->hConnection;
      //msgSHR.kvHeader.add("Pragma", "no-cache");
      //msgSHR.kvHeader.add("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
      //msgSHR.kvHeader.add("Expires", "Thu, 19 Nov 1981 08:52:00 GMT");
      msgSHR.kvHeader.add("Expires", Apollo::getNow().operator+=(Apollo::TimeValue(3600, 0)).toRFC2822());
      if (!msgSHR.Request()) { throw ApException(LOG_CONTEXT, "Msg_HttpServer_SendResponse failed: conn=" ApHandleFormat "", ApHandlePrintf(msgSHR.hConnection)); }

      pMsg->Stop();
      pMsg->apStatus = ApMessage::Ok;

    } catch (ApException& ex) {

      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(ex.getText())));

      Msg_HttpServer_SendResponse msgSHR;
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
        { throw ApException(LOG_CONTEXT, "Msg_HttpServer_SendResponse (for error message) failed: conn=" ApHandleFormat "", ApHandlePrintf(msgSHR.hConnection)); }
      } else {
        pMsg->Stop();
        pMsg->apStatus = ApMessage::Ok;
      }
    }

  } // sUriPrefix
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(GalileoModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  GalileoModuleTester::Begin();
}

AP_MSG_HANDLER_METHOD(GalileoModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  GalileoModuleTester::Execute();
}

AP_MSG_HANDLER_METHOD(GalileoModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  GalileoModuleTester::End();
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

//#include "ximagif.h"
//#include "Image.h"

int GalileoModule::Init()
{
  int ok = 1;

  //CxImageGIF img;
  //Buffer sbSource;
  //Apollo::loadFile("test/tassadar/" + "walk-l.gif", sbSource);
  //CxMemFile mfSource(sbSource.Data(), sbSource.Length());
  //img.Decode(&mfSource);

  //CxImage img;
  //String sFilename = "test/tassadar/" + "walk-l.gif";
  //img.Load(sFilename);

  //Msg_Animation_Frame msg;
  //msg.iFrame.Allocate(img.GetWidth(), img.GetHeight());
  //CxMemFile mfDest((BYTE*) msg.iFrame.Pixels(), msg.iFrame.Size());
  //img.Encode2RGBA(&mfDest);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_Start, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_Stop, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_SetRate, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_SetData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_SetStatus, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_SetCondition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_Event, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_Activity, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_Static, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_GetGroups, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_GetGroupSequences, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_GetSequenceInfo, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_SetPosition, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_MoveTo, this, ApCallbackPosNormal);
  //AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Animation_GetPosition, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Galileo_RequestAnimation, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Galileo_RequestAnimationComplete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Galileo_SetStorageName, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Galileo_SaveAnimationDataToStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Galileo_IsAnimationDataInStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Galileo_LoadAnimationDataFromStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Galileo_ClearAllStorage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Galileo_ExpireAllStorage, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, Timer_Event, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, System_AfterLoadModules, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, System_BeforeUnloadModules, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, System_RunLevel, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GalileoModule, HttpServer_Request, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(GalileoModule, this);

  return ok;
}

void GalileoModule::Exit()
{
  AP_UNITTEST_UNHOOK(GalileoModule, this);
  AP_MSG_REGISTRY_FINISH;
}
