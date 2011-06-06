// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgSystem.h"
#include "Local.h"
#include "NavigationModule.h"
#include "SrpcServer.h"
#include "Context.h"
#include "Connection.h"

Context* NavigationModule::findContext(const ApHandle& h)
{
  Context* pContext = 0;

  ContextNode* pNode = contexts_.Find(h);
  if (pNode != 0) {
    pContext = pNode->Value();
  }

  return pContext;
}

Connection* NavigationModule::findConnection(const ApHandle& h)
{
  Connection* pConnection = 0;

  ConnectionNode* pNode = connections_.Find(h);
  if (pNode) {
    pConnection = pNode->Value();
  }

  return pConnection;
}

int NavigationModule::addConnection(const ApHandle& hConnection, Connection* pConnection)
{
  int ok = 1;

  ok = connections_.Set(hConnection, pConnection);

  Msg_Navigation_Connected msg;
  msg.hConnection = hConnection;
  msg.Send();

  return ok;
}

int NavigationModule::removeConnection(const ApHandle& hConnection)
{
  int ok = 1;

  ok = connections_.Unset(hConnection);

  // Connection lost:
  Msg_Navigation_Disconnected msg;
  msg.hConnection = hConnection;
  msg.Send();

  return ok;
}

void NavigationModule::associateContextWithConnection(const ApHandle& hContext, const ApHandle& hConnection)
{
  ApHandle hCurrentlyAssociatedConnection = findConnectionHandleByContextHandle(hContext);
  if (hCurrentlyAssociatedConnection != hConnection) {
    if (ApIsHandle(hCurrentlyAssociatedConnection)) {
      removeContextFromConnection(hContext, hCurrentlyAssociatedConnection);
    }
    addContextToConnection(hContext, hConnection);
  }
}

ApHandle NavigationModule::findConnectionHandleByContextHandle(const ApHandle& hContext)
{
  ApHandle hConnection = ApNoHandle;

  ConnectionContextListNode* pNode = 0;
  for (ConnectionContextListIterator iter(connectionContexts_); (pNode = iter.Next()) != 0; ) {
    if (pNode->Value().Find(hContext)) {
      hConnection = pNode->Key();
      break;
    }
  }

  return hConnection;
}

void NavigationModule::addContextToConnection(const ApHandle& hContext, const ApHandle& hConnection)
{
  ConnectionContextListNode* pNode = connectionContexts_.Find(hConnection);
  if (pNode == 0) {
    ContextHandleList emptyContextHandleList;
    connectionContexts_.Set(hConnection, emptyContextHandleList);
    pNode = connectionContexts_.Find(hConnection);
  }
  if (pNode) {
    pNode->Value().Set(hContext, hContext);
  }
}

void NavigationModule::removeContextFromConnection(const ApHandle& hContext, const ApHandle& hConnection)
{
  ConnectionContextListNode* pNode = connectionContexts_.Find(hConnection);
  if (pNode) {
    pNode->Value().Unset(hContext);
    if (pNode->Value().Count() == 0) {
      connectionContexts_.Unset(hConnection);
    }
  }
}

//----------------------------------------------------------

Context* NavigationModule::createContext(const ApHandle& hContext)
{
  Context* pContext = new Context(hContext);
  if (pContext != 0) {
    contexts_.Set(hContext, pContext);

    try {
      pContext->create();
    } catch (ApException ex) {
      contexts_.Unset(hContext);
      delete pContext;
      pContext = 0;
    }
  }

  return pContext;
}

int NavigationModule::destroyContext(const ApHandle& hContext)
{
  int ok = 0;

  Context* pContext = findContext(hContext);
  if (pContext == 0) {
    apLog_Warning((LOG_CHANNEL, "NavigationModule::destroyContext", "Unknown context " ApHandleFormat "", ApHandleType(hContext)));
  } else {
    try {
      pContext->destroy();
      ok = 1;
    } catch (ApException ex) {
      apLog_Error((LOG_CHANNEL, "NavigationModule::destroyContext", "pContext->close() failed: %s", StringType(ex.getText())));
    }

    ApHandle hConnection = findConnectionHandleByContextHandle(hContext);
    if (ApIsHandle(hConnection)) {
      removeContextFromConnection(hContext, hConnection);
    }

    contexts_.Unset(hContext);

    delete pContext;
    pContext = 0;
  }

  return ok;
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(NavigationModule, MainLoop_EventLoopBegin)
{
  if (pServer_ == 0) {
    pServer_ = new SrpcServer();
    if (pServer_ != 0) {
      int ok = pServer_->Start(Apollo::getModuleConfig(MODULE_NAME, "Server/Address", "localhost"), Apollo::getModuleConfig(MODULE_NAME, "Server/Port", 23763));
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "NavigationModule::MainLoop_EventLoopBegin", "SrpcServer::Start() failed"));
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(NavigationModule, MainLoop_EventLoopBeforeEnd)
{
  if (pServer_ != 0) {
    pServer_->Stop();
    delete pServer_;
    pServer_ = 0;
  }
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_Connected)
{
  // do nothing
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_Disconnected)
{
  // Lost connection -> cleanup associated contexts
  ApHandle hConnection = pMsg->hConnection;

  // Construct list of contexts to be closed
  ContextHandleList closeContexts;
  ConnectionContextListNode* pConnectionContextListNode = 0;
  for (ConnectionContextListIterator connectionContextListIterator(connectionContexts_); (pConnectionContextListNode = connectionContextListIterator.Next()) != 0; ) {
    if (pConnectionContextListNode->Key() == hConnection) {
      closeContexts = pConnectionContextListNode->Value();
      break;
    }
  }

  if (closeContexts.Count() == 0) {
    apLog_Verbose((LOG_CHANNEL, "NavigationModule::removeConnection", "conn=" ApHandleFormat " no associated contexts", ApHandleType(hConnection)));
  } else {
    apLog_Verbose((LOG_CHANNEL, "NavigationModule::removeConnection", "Closing %d associated contexts of conn=" ApHandleFormat "", closeContexts.Count(), ApHandleType(hConnection)));
  }

  // Send Vp_CloseContext for all associated contexts
  ContextHandleListNode* pContextHandleListNode = 0;
  for (ContextHandleListIterator contextHandleListIterator(closeContexts); (pContextHandleListNode = contextHandleListIterator.Next()) != 0; ) {
    ApHandle hContext = pContextHandleListNode->Key();
    destroyContext(hContext);
  } // for all Contexts

  // Cleanup, should do nothing, because destroyContext does removeContextFromConnection
  // and the last context also removes the connection in connectionContexts_
  (void) connectionContexts_.Unset(hConnection);
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_Receive)
{
  int ok = 1;

  if (apLog_IsVerbose) {
    String sMsg;
    for (Elem* e = 0; (e = pMsg->srpc.Next(e)) != 0; ) {
      if (!sMsg.empty()) { sMsg += " "; }
      sMsg.appendf("%s=%s", StringType(e->getName()), StringType(e->getString()));
    }
    if (apLog_IsVerbose) {
      apLog_Verbose((LOG_CHANNEL, "NavigationModule::On_Navigation_Receive", "" ApHandleFormat " %s", ApHandleType(pMsg->hConnection), StringType(sMsg)));
    }
  }

  String sMethod = pMsg->srpc.getString("Method");
  Apollo::SrpcMessage response;

  if (0) {
  } else if (sMethod == Navigation_SrpcMethod_Connect) {
    response.createResponse(pMsg->srpc);

  } else if (sMethod == Navigation_SrpcMethod_GetHandle) {
    Msg_System_GetHandle msg;
    if (!msg.Request()) {
      response.createError(pMsg->srpc, msg.sComment);
    } else {
      response.createResponse(pMsg->srpc);
      response.set("h", msg.h);
    }

  } else if (sMethod == Navigation_SrpcMethod_Navigate || sMethod == Navigation_SrpcMethod_Open) {
    ApHandle hContext = pMsg->srpc.getHandle("hContext");
    Context* pContext = findContext(hContext);
    if (pContext == 0) {
      pContext = createContext(hContext);
      if (pContext == 0) {
        String sError; sError.appendf("createContext() failed " ApHandleFormat "", ApHandleType(hContext));
        response.createError(pMsg->srpc,sError);
      } else {
        // Create response later
      }
    }

  } else if (sMethod == Navigation_SrpcMethod_Close) {
    ApHandle hContext = pMsg->srpc.getHandle("hContext");
    if (!destroyContext(hContext)) {
      String sError; sError.appendf("destroyContext() failed " ApHandleFormat "", ApHandleType(hContext));
      response.createError(pMsg->srpc,sError);
    } else {
      response.createResponse(pMsg->srpc);
    }

  } // sMethod

  if (response.getString("Status").empty()) {
    ApHandle hContext = pMsg->srpc.getHandle("hContext");
    if (!ApIsHandle(hContext)) {
      apLog_Warning((LOG_CHANNEL, "NavigationModule::On_Navigation_Receive", "No context " ApHandleFormat "", ApHandleType(hContext)));
      response.createError(pMsg->srpc, "No context");
    } else {

      associateContextWithConnection(hContext, pMsg->hConnection);

      Context* pContext = findContext(hContext);
      if (pContext == 0) {
        apLog_Warning((LOG_CHANNEL, "NavigationModule::On_Navigation_Receive", "Unknown context " ApHandleFormat "", ApHandleType(hContext)));
        String sError; sError.appendf("Unknown context " ApHandleFormat "", ApHandleType(hContext));
        response.createError(pMsg->srpc, sError);

      } else {

        if (0) {
        } else if (sMethod == Navigation_SrpcMethod_Navigate) {
          String sUrl = pMsg->srpc.getString("sUrl");
          try {
            pContext->navigate(sUrl);
            response.createResponse(pMsg->srpc);
          } catch (ApException ex) {
            response.createError(pMsg->srpc, ex.getText());
          }

        } else if (sMethod == Navigation_SrpcMethod_Open) {
          response.createResponse(pMsg->srpc);

        } else if (sMethod == Navigation_SrpcMethod_Attach) {
          response.createResponse(pMsg->srpc);

        } else if (sMethod == Navigation_SrpcMethod_NativeWindow) {
          Apollo::KeyValueList kvSignature;
          pMsg->srpc.getKeyValueList("kvSignature", kvSignature);
          try {
            pContext->nativeWindow(kvSignature);
            response.createResponse(pMsg->srpc);
          } catch (ApException ex) {
            response.createError(pMsg->srpc, ex.getText());
          }

        } else if (sMethod == Navigation_SrpcMethod_Show) {
          try {
            pContext->show();
            response.createResponse(pMsg->srpc);
          } catch (ApException ex) {
            response.createError(pMsg->srpc, ex.getText());
          }

        } else if (sMethod == Navigation_SrpcMethod_Hide) {
          try {
            pContext->hide();
            response.createResponse(pMsg->srpc);
          } catch (ApException ex) {
            response.createError(pMsg->srpc, ex.getText());
          }

        } else if (sMethod == Navigation_SrpcMethod_Position) {
          String sLeft = pMsg->srpc.getString("nLeft");
          String sBottom = pMsg->srpc.getString("nBottom");
          try {
            pContext->position(String::atoi(sLeft), String::atoi(sBottom));
            response.createResponse(pMsg->srpc);
          } catch (ApException ex) {
            response.createError(pMsg->srpc, ex.getText());
          }

        } else if (sMethod == Navigation_SrpcMethod_Size) {
          String sWidth = pMsg->srpc.getString("nWidth");
          String sHeight = pMsg->srpc.getString("nHeight");
          try {
            pContext->size(String::atoi(sWidth), String::atoi(sHeight));
            response.createResponse(pMsg->srpc);
          } catch (ApException ex) {
            response.createError(pMsg->srpc, ex.getText());
          }

        } else {
          String sMsg;
          for (Elem* e = 0; (e = pMsg->srpc.Next(e)) != 0; ) {
            if (!sMsg.empty()) { sMsg += " "; }
            sMsg.appendf("%s=%s", StringType(e->getName()), StringType(e->getString()));
          }
          apLog_Warning((LOG_CHANNEL, "NavigationModule::On_Navigation_Receive", "Unhandled message on connection " ApHandleFormat ": %s", ApHandleType(pMsg->hConnection), StringType(sMsg)));

        } // sMethod

      } // pContext
    } // hContext
  } // bDone

  if (!response.getString("Status").empty()) {
    // if the response has been filled, then send it back to the navigator
    Msg_Navigation_Send msgNS;
    msgNS.hConnection = pMsg->hConnection;
    // Shuffle response params without copying data. Original message looses the data.
    response >> msgNS.srpc;

    ok = msgNS.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "NavigationModule::On_Navigation_Receive", "Msg_Navigation_Send response failed " ApHandleFormat "", ApHandleType(pMsg->hConnection)));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_Send)
{
  int ok = 1;

  Connection* pConnection = findConnection(pMsg->hConnection);
  if (pConnection != 0) {

    String sMsg = pMsg->srpc.toString();

    if (!sMsg.empty()) {
      sMsg += "\n";

      if (apLog_IsVerbose) {
        apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_Send", "" ApHandleFormat " %s", ApHandleType(pMsg->hConnection), StringType(sMsg)));
      }

      ok = pConnection->DataOut((unsigned char*) sMsg.c_str(), sMsg.bytes());
      if (!ok ) {
        apLog_Error((LOG_CHANNEL, "NavigationModule::Navigation_Send", "Connection " ApHandleFormat " DataOut() failed", ApHandleType(pMsg->hConnection)));
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NavigationModule, BrowserInfo_Visibility)
{
  int ok = 1;

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NavigationModule, BrowserInfo_Position)
{
  int ok = 1;

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(NavigationModule, BrowserInfo_Size)
{
  int ok = 1;

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

#if defined(AP_TEST)

String NavigationModuleTester::associateContextWithConnection()
{
  String s;

  {
    NavigationModule m;
    ApHandle conn1 = Apollo::newHandle();
    ApHandle conn2 = Apollo::newHandle();
    ApHandle ctxt1 = Apollo::newHandle();
    m.associateContextWithConnection(ctxt1, conn1);
    m.associateContextWithConnection(ctxt1, conn2);
    if (m.findConnectionHandleByContextHandle(ctxt1) != conn2) { s = "ctxt1 not associated with conn2"; }
    if (m.connectionContexts_.Count() != 1) { s = "Should have 1 connection with contexts"; }
    if (m.connectionContexts_.Find(conn1) != 0) { s = "conn1 should not have a context list"; }
    if (m.connectionContexts_.Find(conn2)->Value().Count() != 1) { s = "conn2 should have 1 contexts"; }
  }

  {
    NavigationModule m;
    ApHandle conn1 = Apollo::newHandle();
    ApHandle conn2 = Apollo::newHandle();
    ApHandle conn3 = Apollo::newHandle();
    ApHandle ctxt1 = Apollo::newHandle();
    ApHandle ctxt2 = Apollo::newHandle();
    ApHandle ctxt3 = Apollo::newHandle();
    ApHandle ctxt4 = Apollo::newHandle();
    ApHandle ctxt5 = Apollo::newHandle();
    ApHandle ctxt6 = Apollo::newHandle();
    m.associateContextWithConnection(ctxt1, conn1);
    m.associateContextWithConnection(ctxt2, conn1);
    m.associateContextWithConnection(ctxt5, conn1);
    m.associateContextWithConnection(ctxt6, conn1);
    m.associateContextWithConnection(ctxt4, conn1);
    m.associateContextWithConnection(ctxt3, conn1);
    m.associateContextWithConnection(ctxt5, conn2);
    m.associateContextWithConnection(ctxt1, conn2);
    m.associateContextWithConnection(ctxt2, conn3);
    m.associateContextWithConnection(ctxt3, conn3);
    m.associateContextWithConnection(ctxt6, conn3);
    m.associateContextWithConnection(ctxt4, conn2);
    m.associateContextWithConnection(ctxt5, conn1);
    m.associateContextWithConnection(ctxt1, conn1);
    m.associateContextWithConnection(ctxt4, conn1);
    m.associateContextWithConnection(ctxt2, conn2);
    m.associateContextWithConnection(ctxt3, conn2);
    m.associateContextWithConnection(ctxt6, conn2);
    m.associateContextWithConnection(ctxt1, conn2);
    m.associateContextWithConnection(ctxt2, conn3);
    m.associateContextWithConnection(ctxt5, conn3);
    m.associateContextWithConnection(ctxt6, conn3);
    m.associateContextWithConnection(ctxt4, conn3);
    m.associateContextWithConnection(ctxt3, conn3);
    if (m.findConnectionHandleByContextHandle(ctxt1) != conn2) { s = "ctxt1 not associated with conn2"; }
    if (m.connectionContexts_.Count() != 2) { s = "Should have 2 connection with contexts"; }
    if (m.connectionContexts_.Find(conn1) != 0) { s = "conn1 should not have a context list"; }
    if (m.connectionContexts_.Find(conn2)->Value().Count() != 1) { s = "conn2 should have 1 contexts"; }
    if (m.connectionContexts_.Find(conn3)->Value().Count() != 5) { s = "conn3 should have 5 contexts"; }
  }

  return s;
}

AP_MSG_HANDLER_METHOD(NavigationModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Navigation", 0)) {
    AP_UNITTEST_REGISTER(NavigationModuleTester::associateContextWithConnection);
  }
}

AP_MSG_HANDLER_METHOD(NavigationModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Navigation", 0)) {
    AP_UNITTEST_EXECUTE(NavigationModuleTester::associateContextWithConnection);
  }
}

AP_MSG_HANDLER_METHOD(NavigationModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int NavigationModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, MainLoop_EventLoopBegin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, MainLoop_EventLoopBeforeEnd, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_Connected, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_Disconnected, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_Receive, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_Send, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, BrowserInfo_Visibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, BrowserInfo_Position, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, BrowserInfo_Size, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(NavigationModule, this);

  return ok;
}

void NavigationModule::exit()
{
  AP_UNITTEST_UNHOOK(NavigationModule, this);
  AP_MSG_REGISTRY_FINISH;
}
