// ============================================================================
//
// Apollo
//
// ============================================================================

#include "MsgConfig.h"
#include "MsgFile.h"
#include "ApLog.h"
#include "ApOS.h"

#include "HttpManager.h"
#include "NetModule.h"
#include "URL.h"

//----------------------------------------------------------

void HTTPRequestTask::ExecuteHTTP()
{
  int ok = 1;

  Apollo::URL url(sUrl_);
  int bSSL = 0;
  String sError;
  
  apLog_Verbose((LOG_CHANNEL, "HTTPRequestTask::ExecuteHTTP", ApHandleFormat " Fetching url: %s...", ApHandleType(hAp_), StringType(sUrl_)));
    
  if (url.protocol().empty() || url.host().empty() || url.portnum() == 0 || url.uri().empty()) {
    sError.appendf("URL part missing: protocol=%s host=%s port=%d uri=%s", StringType(url.protocol()), StringType(url.host()), StringType(url.portnum()), StringType(url.uri()));
    apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
    ok = 0;
  } else {
    if (0) {
    } else if (url.protocol() == "http") {
      bSSL = 0;
    } else if (url.protocol() == "https") {
      bSSL = 1;
    } else {
      sError.appendf("Protocol=%s not supported: url=%s", StringType(url.protocol()), StringType(url));
      apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
      ok = 0;
    }
  }

  if (! (sMethod_ == "GET" || sMethod_ == "POST") ) {
    sError.appendf("Method=%s not supported, url=%s", StringType(sMethod_), StringType(url));
    apLog_Warning((LOG_CHANNEL, "HTTPRequestTask::Execute", "%s", StringType(sError)));
    ok = 0;
  }

  ok = NetOS::HTTP_PerformRequest(ok, url, sMethod_, bSSL, sError, this);
  if (ok) {
    apLog_Verbose((LOG_CHANNEL, "HTTPRequestTask::ExecuteHTTP", ApHandleFormat " ... Success", ApHandleType(hAp_)));
  } else {
    apLog_Verbose((LOG_CHANNEL, "HTTPRequestTask::ExecuteHTTP", ApHandleFormat " ... Error: %s", ApHandleType(hAp_), StringType(sError)));
  }
}

void HTTPRequestTask::ExecuteFile(String& sDataFilename)
{
  int ok = 1;
  String sError;

  if (ok) {
    xFile fData(sDataFilename);
    if (!fData.Load()) {
      ok = 0;
      sError.appendf("f.Load(%s) failed", StringType(sDataFilename));
    } else {

      {
        ApAsyncMessage<Msg_Net_HTTP_Connected> msg;
        msg->hClient = hAp_;
        msg.Post();
      }

      Apollo::Sleep(50);

      String sHeader;
      String sHeaderFilename = sDataFilename;
      sHeaderFilename += ".http";
      xFile fHeader(sHeaderFilename);
      if (fHeader.Load()) {
        fHeader.GetData(sHeader);
      }

      {
        ApAsyncMessage<Msg_Net_HTTP_Header> msg;
        msg->hClient = hAp_;
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
          msg->hClient = hAp_;
          msg->sbData.SetData(sbData.Data() + nSize, nFragment);
          msg.Post();
        }

        Apollo::Sleep(50);
        nSize += nFragment;
      }

    }
  }

  if (!ok) {
    ApAsyncMessage<Msg_Net_HTTP_Failed> msg;
    msg->hClient = hAp_;
    msg->sMessage = sError;
    msg.Post();
  }

  {
    ApAsyncMessage<Msg_Net_HTTP_Closed> msg;
    msg->hClient = hAp_;
    msg.Post();
  }
}

void HTTPRequestTask::Execute()
{
  //int ok = 1;

  NetModuleInstance::Get()->oHttp_.SetCurrentTask(this);

  String sDataFilename;
  if (!sIndexFile_.empty()) {
    xFile fIndex(sIndexFile_);
    if (fIndex.Load()) {
      String sIndex;
      fIndex.GetData(sIndex);
      List lDataFiles;
      KeyValueBlob2List(sIndex, lDataFiles, "\r\n", " ", "");
      Elem* e = lDataFiles.FindByName(sUrl_);
      if (e != 0) {
        sDataFilename = e->getString();
      } else {
        int x = 1;
        AP_UNUSED_ARG(x);
      }
    }
  }

  if (!sDataFilename.empty()) {
    ExecuteFile(sDataFilename);
  } else {

    if (!NetModuleInstance::Get()->oHttp_.sHttpLog_.empty()) {
      String s;
      s.appendf("[URL] %s\n", StringType(sUrl_));

      // Async because we are in a thread
      ApAsyncMessage<Msg_File_Append> msg;
      msg->sPathName = NetModuleInstance::Get()->oHttp_.sHttpLog_;
      msg->nFlags = Msg_File_Flag_CreatePath;
      msg->sbData.SetData(s);
      msg.Post();
    }

    ExecuteHTTP();
  }

  NetModuleInstance::Get()->oHttp_.SetCurrentTask(0);
}

//----------------------------------------------------------

int HttpManager::Init(const char* szClientName)
{
  int ok = 1;
  String sClientName = szClientName;
  ok = NetOS::HTTP_Manager_Init(sClientName);

  sHttpLog_ = Apollo::getModuleConfig(MODULE_NAME, "/HTTP/LogFile", "");
  if (!sHttpLog_.empty()) {
    String s = "[Begin] " + Apollo::TimeValue::getTime().toString() + "\n";
    Apollo::appendFile(NetModuleInstance::Get()->oHttp_.sHttpLog_, s);
  }

  return ok;
}

void HttpManager::Exit()
{
  if (!sHttpLog_.empty()) {
    String s = "[End] " + Apollo::TimeValue::getTime().toString() + "\n";
    Apollo::appendFile(NetModuleInstance::Get()->oHttp_.sHttpLog_, s);
  }
  (void) NetOS::HTTP_Manager_Shutdown(*this);
}

HTTPRequestTask* HttpManager::GetCurrentTask()
{
  return pCurrentTask_;
}

void HttpManager::SetCurrentTask(HTTPRequestTask* t)
{
  Apollo::MutexGuard g(&oCurrentTaskMutex_);

  pCurrentTask_ = t;
}

int HttpManager::CancelRequest(ApHandle h)
{
  int ok = 0;

  {
    Apollo::MutexGuard g(&oCurrentTaskMutex_);

    if (pCurrentTask_ != 0) {
      if (pCurrentTask_->hAp_ == h) {
        NetOS::HTTP_InterruptTaskAndCloseConnection(pCurrentTask_);
        ok = 1;
      }
    }

    {
      HTTPRequestTask* pTask = 0;
      while ((pTask = (HTTPRequestTask*) lTasks_.Next(pTask)) != 0) {
        if (pTask->hAp_ == h) {
          break;
        }
      }
      if (pTask != 0) {
        if (pTask->GetHandle() != NULL) {
          apLog_Error((LOG_CHANNEL, "HttpManager::CancelRequest", "pTask->hConnect != NULL: " ApHandleFormat "", ApHandleType(pTask->hAp_)));
          AP_DEBUG_BREAK();
        }
        lTasks_.Delete(pTask);
        pTask = 0;
        ok = 1;
      }
    }

  } // MutexGuard

  if (ok) {
    Msg_Net_HTTP_Closed msg;
    msg.hClient = h;
    msg.Send();
  }

  return ok;
}

int HttpManager::CancelAllRequests()
{
  int ok = 1;

  {
    Apollo::MutexGuard g(&lTasks_.Mutex());

    HTTPRequestTask* pTask = 0;
    while ((pTask = (HTTPRequestTask*) lTasks_.First()) != 0) {
      lTasks_.Remove(pTask);

      Msg_Net_HTTP_Closed msg;
      msg.hClient = pTask->hAp_;
      msg.Send();

      delete pTask;
      pTask = 0;
    }
  }

  {
    Apollo::MutexGuard g(&oCurrentTaskMutex_);

    if (pCurrentTask_ != 0) {
      CancelRequest(pCurrentTask_->hAp_);
    }
  }

  return ok;
}

void HttpManager::CloseAll()
{
  Apollo::MutexGuard g(&oCurrentTaskMutex_);

  if (pCurrentTask_ != 0) {
    CancelRequest(pCurrentTask_->hAp_);
  }

  bFinished_ = 1;
}
