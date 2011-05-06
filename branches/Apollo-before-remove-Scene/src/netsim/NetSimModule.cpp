// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "NetSimModule.h"
#include "Local.h"
/*
void NetSimModule::closeConnection()
{
  Msg_Net_TCP_Closed msg;
  msg.hConnection = hConnection_;
  msg.Send();

  bConnected_ = 0;
  hConnection_ = ApNoHandle;
}

void NetSimModule::sendData(const unsigned char* pData, size_t nLen)
{
  Msg_Net_TCP_DataOut msg;
  msg.hConnection = hConnection_;
  msg.sbData.SetData(pData, nLen);
  msg.Request();
}
*/
void NetSimModule::writeHttpLog(const String& s)
{
  String sLogFile = Apollo::getModuleConfig(MODULE_NAME, "HTTP/LogFile", "");
  if (sLogFile) {
    String sData = s + "\n";
    Apollo::appendFile(sLogFile, sData);
  }
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(NetSimModule, Net_IsOnline)
{
  int ok = 1;

  pMsg->bIsOnline = 1;

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, Net_DNS_Resolve)
{
  //pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, Net_TCP_Connect)
{
  int ok = 0;
/*
  if (pMsg->sHost == (const char*) Apollo::getModuleConfig(MODULE_NAME, "Address", "") && pMsg->nPort == Apollo::getModuleConfig(MODULE_NAME, "Port", 5222)) {
    apLog_Verbose((LOG_CHANNEL, "NetSimModule::On_Net_TCP_Connect", "Handling TCP connect to %s %d " ApHandleFormat "", StringType(pMsg->sHost), pMsg->nPort, ApHandleType(hConnection_)));

    pMsg->Stop();

    bConnected_ = 1;
    hConnection_ = pMsg->hConnection;

    Msg_Net_TCP_Connected msg;
    msg.hConnection = hConnection_;
    msg.Send();
  }
*/
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, Net_TCP_DataOut)
{
  int ok = 0;
/*
  if (pMsg->hConnection == hConnection_) {
    //
  }
*/
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, Net_TCP_Close)
{
  int ok = 0;
  pMsg->Stop();
/*
  if (pMsg->hConnection == hConnection_) {
    apLog_Verbose((LOG_CHANNEL, "NetSimModule::On_Net_TCP_Close", "Handling TCP close " ApHandleFormat "", ApHandleType(hConnection_)));

    closeConnection();
  }
*/
  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, Net_HTTP_Request)
{
  int ok = 1;
  pMsg->Stop();
  apLog_Verbose((LOG_CHANNEL, "NetSimModule::Net_HTTP_Request", "%s url=%s", StringType(pMsg->sMethod), StringType(pMsg->sUrl)));

  {
    String s;
    s.appendf("[%s] %s", StringType(pMsg->sMethod), StringType(pMsg->sUrl));
    writeHttpLog(s);
  }

  Http* pHttp = new Http(pMsg->hClient, pMsg->sMethod, pMsg->sUrl, pMsg->kvHeader, pMsg->sbBody);
  if (pHttp != 0) {
    http_.Set(pMsg->hClient, pHttp);

    ApAsyncMessage<Msg_NetSim_ContinueHttp> msg;
    msg->hClient = pMsg->hClient;
    msg.Post();
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, Net_HTTP_Cancel)
{
  int ok = 0;
  pMsg->Stop();

  Http* pHttp = 0;
  if (http_.Get(pMsg->hClient, pHttp)) {

    ApAsyncMessage<Msg_Net_HTTP_Closed> msg;
    msg->hClient = pHttp->hClient_;
    msg.Post();

    if (http_.Unset(pMsg->hClient)) {
      delete pHttp;
      pHttp = 0;
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, Net_HTTP_CancelAll)
{
  int ok = 0;
  pMsg->Stop();

  ok = 1; // do nothing

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//---------------------------

AP_MSG_HANDLER_METHOD(NetSimModule, Net_Online)
{
  pMsg->Stop();
}

AP_MSG_HANDLER_METHOD(NetSimModule, Net_Offline)
{
  pMsg->Stop();
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(NetSimModule, NetSim_Engage)
{
  int ok = 1;

  simMsgRegistry_.add(MODULE_NAME, "Net_IsOnline", (ApCallback) NetSimModule_On_Net_IsOnline, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  simMsgRegistry_.add(MODULE_NAME, "Net_DNS_Resolve", (ApCallback) NetSimModule_On_Net_DNS_Resolve, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  simMsgRegistry_.add(MODULE_NAME, "Net_TCP_Close", (ApCallback) NetSimModule_On_Net_TCP_Close, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  simMsgRegistry_.add(MODULE_NAME, "Net_TCP_DataOut", (ApCallback) NetSimModule_On_Net_TCP_DataOut, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  simMsgRegistry_.add(MODULE_NAME, "Net_TCP_Connect", (ApCallback) NetSimModule_On_Net_TCP_Connect, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  simMsgRegistry_.add(MODULE_NAME, "Net_HTTP_Request", (ApCallback) NetSimModule_On_Net_HTTP_Request, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  simMsgRegistry_.add(MODULE_NAME, "Net_HTTP_Cancel", (ApCallback) NetSimModule_On_Net_HTTP_Cancel, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  simMsgRegistry_.add(MODULE_NAME, "Net_HTTP_CancelAll", (ApCallback) NetSimModule_On_Net_HTTP_CancelAll, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));

  //simMsgRegistry_.add(MODULE_NAME, "Net_Online", (ApCallback) NetSimModule_On_Net_Online, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));
  simMsgRegistry_.add(MODULE_NAME, "Net_Offline", (ApCallback) NetSimModule_On_Net_Offline, this, Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME));

  {
    Msg_Net_Online msg;
    msg.Send();
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, NetSim_Disengage)
{
  int ok = 1;

  simMsgRegistry_.finish();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, NetSim_ContinueHttp)
{
  int ok = 1;

  Http* pHttp = 0;
  if (http_.Get(pMsg->hClient, pHttp)) {

    try {
      String sIndexFile = Apollo::getModuleConfig(MODULE_NAME, "HTTP/IndexFile", "");
      if (!sIndexFile) { throw ApException("No index file name"); }

      xFile fIndex(sIndexFile);
      if (!fIndex.Load()) { throw ApException("f.Load(%s) failed", StringType(sIndexFile)); }

      String sIndex;
      fIndex.GetData(sIndex);
      List lDataFiles;
      KeyValueBlob2List(sIndex, lDataFiles, "\r\n", " ", "");
      Elem* e = lDataFiles.FindByName(pHttp->sUrl_);
      if (!e) { throw ApException("No data file for url=%s", StringType(pHttp->sUrl_)); }
      String sDataFilename = e->getString();
      if (!sDataFilename) { throw ApException("No data file for url=%s", StringType(pHttp->sUrl_)); }

      String sDataFilepath = Apollo::getModuleConfig(MODULE_NAME, "HTTP/DataFilesBase", "");
      sDataFilepath += String::filenamePathSeparator();
      sDataFilepath += sDataFilename;
      xFile fData(sDataFilepath);
      if (!fData.Load()) { throw ApException("f.Load(%s) failed", StringType(sDataFilepath)); }

      String sHeaderFilename = sDataFilepath;
      sHeaderFilename += ".http";
      xFile fHeader(sHeaderFilename);
      if (!fHeader.Load()) { throw ApException("f.Load(%s) failed", StringType(sHeaderFilename)); }

      {
        ApAsyncMessage<Msg_Net_HTTP_Connected> msg;
        msg->hClient = pHttp->hClient_;
        msg.Post();
      }

      Apollo::Sleep(50);

      String sHeader;
      fHeader.GetData(sHeader);
      {
        ApAsyncMessage<Msg_Net_HTTP_Header> msg;
        msg->hClient = pHttp->hClient_;
        msg->nStatus = 200;
        List lHeader;
        KeyValueLfBlob2List(sHeader, lHeader);
        msg->kvHeader.fromList(lHeader);
        msg.Post();
      }

      Apollo::Sleep(50);

      Buffer sbData;
      fData.GetData(sbData);

      size_t nSize = 0;
      while (nSize < sbData.Length()) {
        int nFragment = sbData.Length() - nSize;
        if (nFragment > 50000) {
          nFragment = 50000;
        }

        {
          ApAsyncMessage<Msg_Net_HTTP_DataIn> msg;
          msg->hClient = pHttp->hClient_;
          msg->sbData.SetData(sbData.Data() + nSize, nFragment);
          msg.Post();
        }

        Apollo::Sleep(50);
        nSize += nFragment;
      }

    } catch (ApException& ex) {
      apLog_Error((LOG_CHANNEL, "NetSimModule::Net_HTTP_Request", "%s url=%s", StringType(ex.getText()), StringType(pHttp->sUrl_)));

      ApAsyncMessage<Msg_Net_HTTP_Failed> msg;
      msg->hClient = pHttp->hClient_;
      msg->sMessage = ex.getText();
      msg.Post();

      ok = 0;
    }

    {
      ApAsyncMessage<Msg_Net_HTTP_Closed> msg;
      msg->hClient = pHttp->hClient_;
      msg.Post();
    }

    if (http_.Unset(pMsg->hClient)) {
      delete pHttp;
      pHttp = 0;
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NetSimModule, MainLoop_EventLoopBegin)
{
  writeHttpLog("[Begin]");

  Msg_NetSim_Engage msg;
  msg.Request();
}

AP_MSG_HANDLER_METHOD(NetSimModule, MainLoop_EventLoopBeforeEnd)
{
  Msg_NetSim_Disengage msg;
  msg.Request();

  writeHttpLog("[End]");
}

//----------------------------------------------------------

int NetSimModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NetSimModule, NetSim_Engage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NetSimModule, NetSim_Disengage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NetSimModule, NetSim_ContinueHttp, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NetSimModule, MainLoop_EventLoopBegin, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NetSimModule, MainLoop_EventLoopBeforeEnd, this, ApCallbackPosLate);

  return ok;
}

void NetSimModule::exit()
{
  AP_MSG_REGISTRY_FINISH;
}
