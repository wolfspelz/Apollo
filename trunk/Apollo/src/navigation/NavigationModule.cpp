// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "NavigationModule.h"
#include "Context.h"

Context* NavigationModule::findContext(const ApHandle& h)
{
  Context* pContext = 0;

  ContextNode* pNode = contexts_.Find(h);
  if (pNode != 0) {
    pContext = pNode->Value();
  }

  return pContext;
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
    } catch (ApException& ex) {
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
    } catch (ApException& ex) {
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

AP_MSG_HANDLER_METHOD(NavigationModule, TcpServer_SrpcRequest)
{
  // Add/fake the hConnection for some messages
  // Will be used by SrpcGate hander below

  String sMethod = pMsg->srpc.getString("Method");
  if (0
    || sMethod == "Navigation_NavigatorHello"
    || sMethod == "Navigation_NavigatorBye"
    || sMethod == "Navigation_ContextOpen"
    || sMethod == "Navigation_ContextNavigate"
    ) {
    pMsg->srpc.set("hConnection", pMsg->hConnection);
  }

  // No apStatus
}

AP_MSG_HANDLER_METHOD(NavigationModule, TcpServer_Disconnected)
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

//---------------------------

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_NavigatorHello)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_NavigatorHello", "conn=" ApHandleFormat "", ApHandleType(pMsg->hConnection)));
  // do nothing
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_NavigatorBye)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_NavigatorBye", "conn=" ApHandleFormat "", ApHandleType(pMsg->hConnection)));
  // do nothing
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_ContextOpen)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_ContextOpen", "conn=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(pMsg->hConnection), ApHandleType(pMsg->hContext)));

  Context* pContext = findContext(pMsg->hContext);
  if (pContext == 0) {
    pContext = createContext(pMsg->hContext);
    if (pContext == 0) { throw ApException("createContext() failed conn=" ApHandleFormat " ctxt=" ApHandleFormat "", ApHandleType(pMsg->hConnection), ApHandleType(pMsg->hContext));}

    associateContextWithConnection(pMsg->hContext, pMsg->hConnection);
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_ContextNavigate)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_ContextNavigate", "conn=" ApHandleFormat " ctxt=" ApHandleFormat ": %s", ApHandleType(pMsg->hConnection), ApHandleType(pMsg->hContext), StringType(pMsg->sUrl)));

  Context* pContext = findContext(pMsg->hContext);
  if (pContext == 0) {
    pContext = createContext(pMsg->hContext);
    if (pContext == 0) { throw ApException("createContext() failed conn=" ApHandleFormat " ctxt=" ApHandleFormat ": %s", ApHandleType(pMsg->hConnection), ApHandleType(pMsg->hContext), StringType(pMsg->sUrl));}
  }

  associateContextWithConnection(pMsg->hContext, pMsg->hConnection);

  pContext->navigate(pMsg->sUrl);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_ContextClose)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_ContextClose", "ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));

  if (!destroyContext(pMsg->hContext)) { throw ApException("createContext() failed ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext));}

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_ContextShow)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_ContextShow", "ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));

  Context* pContext = findContext(pMsg->hContext);
  if (pContext != 0) {
    pContext->show();
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_ContextHide)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_ContextHide", "ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));

  Context* pContext = findContext(pMsg->hContext);
  if (pContext != 0) {
    pContext->hide();
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_ContextPosition)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_ContextPosition", "ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));

  Context* pContext = findContext(pMsg->hContext);
  if (pContext != 0) {
    pContext->position(pMsg->nLeft, pMsg->nBottom);
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_ContextSize)
{
  apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_ContextSize", "ctxt=" ApHandleFormat "", ApHandleType(pMsg->hContext)));

  Context* pContext = findContext(pMsg->hContext);
  if (pContext != 0) {
    pContext->size(pMsg->nWidth, pMsg->nHeight);
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NavigationModule, Navigation_ContextNativeWindow)
{
  if (apLog_IsVerbose) {
    String sSignature;
    for (Apollo::KeyValueElem* e = 0; (e = pMsg->kvSignature.nextElem(e)) != 0; ) {
      if (!sSignature.empty()) { sSignature += " "; }
      sSignature.appendf("%s=%s", StringType(e->getKey()), StringType(e->getString()));
    }
    apLog_Verbose((LOG_CHANNEL, "NavigationModule::Navigation_ContextNativeWindow", "ctxt=" ApHandleFormat " sig: %s", ApHandleType(pMsg->hContext), StringType(sSignature)));
  }

  Context* pContext = findContext(pMsg->hContext);
  if (pContext != 0) {
    pContext->nativeWindow(pMsg->kvSignature);
  }

  pMsg->apStatus = ApMessage::Ok;
}

//---------------------------

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

//---------------------------

void SrpcGate_Navigation_NavigatorHello(ApSRPCMessage* pMsg)
{
  Msg_Navigation_NavigatorHello msg;
  msg.hConnection = pMsg->srpc.getHandle("hConnection");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_NavigatorBye(ApSRPCMessage* pMsg)
{
  Msg_Navigation_NavigatorBye msg;
  msg.hConnection = pMsg->srpc.getHandle("hConnection");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_ContextOpen(ApSRPCMessage* pMsg)
{
  Msg_Navigation_ContextOpen msg;
  msg.hConnection = pMsg->srpc.getHandle("hConnection");
  msg.hContext = pMsg->srpc.getHandle("hContext");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_ContextNavigate(ApSRPCMessage* pMsg)
{
  Msg_Navigation_ContextNavigate msg;
  msg.hConnection = pMsg->srpc.getHandle("hConnection");
  msg.hContext = pMsg->srpc.getHandle("hContext");
  msg.sUrl = pMsg->srpc.getString("sUrl");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_ContextClose(ApSRPCMessage* pMsg)
{
  Msg_Navigation_ContextClose msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_ContextShow(ApSRPCMessage* pMsg)
{
  Msg_Navigation_ContextShow msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_ContextHide(ApSRPCMessage* pMsg)
{
  Msg_Navigation_ContextHide msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_ContextPosition(ApSRPCMessage* pMsg)
{
  Msg_Navigation_ContextPosition msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  msg.nLeft = pMsg->srpc.getInt("nLeft");
  msg.nBottom = pMsg->srpc.getInt("nBottom");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_ContextSize(ApSRPCMessage* pMsg)
{
  Msg_Navigation_ContextSize msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  msg.nWidth = pMsg->srpc.getInt("nWidth");
  msg.nHeight = pMsg->srpc.getInt("nHeight");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Navigation_ContextNativeWindow(ApSRPCMessage* pMsg)
{
  Msg_Navigation_ContextNativeWindow msg;
  msg.hContext = pMsg->srpc.getHandle("hContext");
  pMsg->srpc.getKeyValueList("kvSignature", msg.kvSignature);
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
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

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, TcpServer_SrpcRequest, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, TcpServer_Disconnected, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_NavigatorHello, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_NavigatorBye, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_ContextOpen, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_ContextNavigate, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_ContextClose, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_ContextShow, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_ContextHide, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_ContextPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_ContextSize, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, Navigation_ContextNativeWindow, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, BrowserInfo_Visibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, BrowserInfo_Position, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NavigationModule, BrowserInfo_Size, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(NavigationModule, this);

  srpcGateRegistry_.add("Navigation_NavigatorHello", SrpcGate_Navigation_NavigatorHello);
  srpcGateRegistry_.add("Navigation_NavigatorBye", SrpcGate_Navigation_NavigatorBye);
  srpcGateRegistry_.add("Navigation_ContextOpen", SrpcGate_Navigation_ContextOpen);
  srpcGateRegistry_.add("Navigation_ContextNavigate", SrpcGate_Navigation_ContextNavigate);
  srpcGateRegistry_.add("Navigation_ContextClose", SrpcGate_Navigation_ContextClose);
  srpcGateRegistry_.add("Navigation_ContextShow", SrpcGate_Navigation_ContextShow);
  srpcGateRegistry_.add("Navigation_ContextHide", SrpcGate_Navigation_ContextHide);
  srpcGateRegistry_.add("Navigation_ContextPosition", SrpcGate_Navigation_ContextPosition);
  srpcGateRegistry_.add("Navigation_ContextSize", SrpcGate_Navigation_ContextSize);
  srpcGateRegistry_.add("Navigation_ContextNativeWindow", SrpcGate_Navigation_ContextNativeWindow);

  return ok;
}

void NavigationModule::exit()
{
  AP_UNITTEST_UNHOOK(NavigationModule, this);
  AP_MSG_REGISTRY_FINISH;
}
