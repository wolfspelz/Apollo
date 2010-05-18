// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ApLog.h"
#include "FluidModule.h"
#include "Display.h"
#include "Connection.h"
#include "SrpcServer.h"

int FluidModule::addConnection(ApHandle hConnection, Connection* pConnection)
{
  int ok = 1;

  ok = connections_.Set(hConnection, pConnection);

  return ok;
}

int FluidModule::removeConnection(ApHandle hConnection)
{
  int ok = 1;

  ApHandle hDisplay = findDisplayHandleByConnectionHandle(hConnection);
  if (!ApIsHandle(hDisplay)) {
    apLog_Warning((LOG_CHANNEL, "FluidModule::removeConnection", "No display for connection " ApHandleFormat "", ApHandleType(hConnection)));
  } else {

    unsetConnectionDisplayMapping(hConnection, hDisplay);
    int bDone = 0;

    if (!bDone) {
      // Maybe the connection breaks unintentionally
      Display* pDisplay = findDisplay(hDisplay);
      if (pDisplay != 0) {
        pDisplay->onDestroyed();
        if (displays_.Unset(pDisplay->apHandle())) {
          delete pDisplay;
          pDisplay = 0;
        }
        bDone = 1;
      }
    }

    if (!bDone) {
      // Try to find in intentionally destroyed displays
      Display* pDisplay = findDestroyedDisplay(hDisplay);
      if (pDisplay != 0) {
        pDisplay->onDestroyed();
        if (destroyedDisplays_.Unset(hDisplay)) {
          delete pDisplay;
          pDisplay = 0;
        }
        bDone = 1;
      }
    }

    if (!bDone) {
      apLog_Warning((LOG_CHANNEL, "FluidModule::removeConnection", "Unknown display " ApHandleFormat "", ApHandleType(hDisplay)));
    }
  }

  ok = connections_.Unset(hConnection);

  return ok;
}

Display* FluidModule::findDisplay(ApHandle h)
{
  Display* pDisplay = 0; 

  DisplayNode* pNode = displays_.Find(h);
  if (pNode != 0) {
    pDisplay = pNode->Value();
  }

  return pDisplay; 
}

Display* FluidModule::findDestroyedDisplay(ApHandle h)
{
  Display* pDisplay = 0; 

  DisplayNode* pNode = destroyedDisplays_.Find(h);
  if (pNode != 0) {
    pDisplay = pNode->Value();
  }

  return pDisplay; 
}

Display* FluidModule::findDisplayById(const String& sId)
{
  Display* pDisplay = 0; 

  DisplayNode* node = 0;
  for (DisplayIterator iter(displays_); (node = iter.Next()) != 0; ) {
    Display* p = node->Value();
    if (p != 0) {
      if (p->getId() == sId) {
        pDisplay = p;
        break;
      }
    }
  }

  return pDisplay; 
}

Connection* FluidModule::findConnection(ApHandle h)
{
  Connection* pConnection = 0; 

  ConnectionNode* pNode = connections_.Find(h);
  if (pNode != 0) {
    pConnection = pNode->Value();
  }

  return pConnection; 
}

void FluidModule::setConnectionDisplayMapping(ApHandle hConnection, ApHandle hDisplay)
{
  connection2Display_.Set(hConnection, hDisplay);
  display2Connection_.Set(hDisplay, hConnection);
}

void FluidModule::unsetConnectionDisplayMapping(ApHandle hConnection, ApHandle hDisplay)
{
  connection2Display_.Unset(hConnection);
  display2Connection_.Unset(hDisplay);
}

ApHandle FluidModule::findDisplayHandleByConnectionHandle(ApHandle hConnection)
{
  ApHandle hDisplay = ApNoHandle;

  ApHandleTreeNode<ApHandle>* pNode = connection2Display_.Find(hConnection);
  if (pNode != 0) {
    hDisplay = pNode->Value();
  }

  return hDisplay;
}

ApHandle FluidModule::findConnectionHandleByDisplayHandle(ApHandle hDisplay)
{
  ApHandle hConnection = ApNoHandle;

  ApHandleTreeNode<ApHandle>* pNode = display2Connection_.Find(hDisplay);
  if (pNode != 0) {
    hConnection = pNode->Value();
  }

  return hConnection;
}

//---------------------------

AP_MSG_HANDLER_METHOD(FluidModule, Fluid_Create)
{
  int ok = 1;

  if (pServer_ == 0) {
    pServer_ = new SrpcServer();
    if (pServer_ != 0) {
      ok = pServer_->Start(Apollo::getModuleConfig(MODULE_NAME, "Server/Address", "localhost"), Apollo::getModuleConfig(MODULE_NAME, "Server/Port", 23762));
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "FluidModule::On_Fluid_Create", "Display " ApHandleFormat " SrpcServer::Start() failed", ApHandleType(pMsg->hDisplay)));
      }
    }
  }

  if (ok) {
    String sArgs = pMsg->sArgs;
    String sId; 
    sId.appendf("" ApHandleFormat "", ApHandleType(pMsg->hDisplay));
    sArgs.appendf(" -host localhost -port %d -id %s", Apollo::getModuleConfig(MODULE_NAME, "Server/Port", 23762), StringType(sId));
    if (pMsg->bVisible) {
      sArgs.appendf(" -visible 1");
    }
    Display* pDisplay = new Display(pMsg->hDisplay, sId, pMsg->sPathname, sArgs);
    if (pDisplay != 0) {
      ok = pDisplay->start();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, "FluidModule::On_Fluid_Create", "Display " ApHandleFormat " pDisplay->start() failed for %s", ApHandleType(pMsg->hDisplay), StringType(pMsg->sPathname)));
        delete pDisplay;
        pDisplay = 0;
      } else {
        displays_.Set(pMsg->hDisplay, pDisplay);
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(FluidModule, Fluid_Destroy)
{
  int ok = 1;

  Display* pDisplay = findDisplay(pMsg->hDisplay);
  if (pDisplay != 0) {
    ok = pDisplay->stop();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "FluidModule::On_Fluid_Destroy", "Display " ApHandleFormat " pDisplay->stop() failed for %s", ApHandleType(pMsg->hDisplay), StringType(pDisplay->getPathname())));
    }

    displays_.Unset(pMsg->hDisplay);
    destroyedDisplays_.Set(pMsg->hDisplay, pDisplay);
    //delete pDisplay;
    //pDisplay = 0;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(FluidModule, Fluid_Send)
{
  int ok = 0;

  Connection* pConnection = findConnection(pMsg->hConnection);
  if (pConnection != 0) {

    String sMsg = pMsg->srpc.toString();

    if (!sMsg.empty()) {
      sMsg += "\n";

      if (apLog_IsVerbose) {
        apLog_Verbose((LOG_CHANNEL, "FluidModule::On_Fluid_Send", "" ApHandleFormat " %s", ApHandleType(pMsg->hConnection), StringType(sMsg)));
      }

      ok = pConnection->DataOut((unsigned char*) sMsg.c_str(), sMsg.bytes());
      if (!ok ) {
        apLog_Error((LOG_CHANNEL, "FluidModule::On_Fluid_Send", "Connection " ApHandleFormat " DataOut() failed", ApHandleType(pMsg->hConnection)));
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(FluidModule, Fluid_Receive)
{
  int ok = 1;

  if (apLog_IsVerbose) {
    String sMsg;
    for (Elem* e = 0; (e = pMsg->srpc.Next(e)) != 0; ) {
      if (!sMsg.empty()) { sMsg += " "; }
      sMsg.appendf("%s=%s", StringType(e->getName()), StringType(e->getString()));
    }
    if (apLog_IsVerbose) {
      apLog_Verbose((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "" ApHandleFormat " %s", ApHandleType(pMsg->hConnection), StringType(sMsg)));
    }
  }

  int bDone = 0;

  if (!bDone) {
    String sApType = pMsg->srpc.getString("ApType");
    if (!sApType.empty()) {
      bDone = 1;

      ApHandle hDisplay = findDisplayHandleByConnectionHandle(pMsg->hConnection);
      if (!ApIsHandle(hDisplay)) {
        apLog_Warning((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Unknown connection " ApHandleFormat "", ApHandleType(pMsg->hConnection)));
      } else {
        Msg_Fluid_HostCall msg(sApType);
        msg.hDisplay = hDisplay;

        // Shuffle params to other message without copying data. Original message looses the data.
        pMsg->srpc >> msg.srpc;

        (void) msg.Call();

        if (! msg.response.getString("Status").empty()) {
          // if the response has been filled, then send it back to the display
          Msg_Fluid_Send msgFS;
          msgFS.hConnection = pMsg->hConnection;
          // Shuffle response params without copying data. Original message looses the data.
          msg.response >> msgFS.srpc;

          ok = msgFS.Request();
          if (!ok) {
            apLog_Error((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Msg_Fluid_Send response failed " ApHandleFormat "", ApHandleType(pMsg->hConnection)));
          }
        }

      } // hDisplay
    } // sApType
  } // if (!bDone)

  String sMethod = pMsg->srpc.getString("Method");

  if (!bDone) {
    if (sMethod == Fluid_SrpcMethod_Hello) {
      bDone = 1;

      String sId = pMsg->srpc.getString("sId");
      Display* pDisplay = findDisplayById(sId);
      if (pDisplay == 0) {
        apLog_Warning((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Unknown display id=", StringType(sId)));
      } else {
        Connection* pConnection = findConnection(pMsg->hConnection);
        if (pConnection != 0) {
          setConnectionDisplayMapping(pConnection->apHandle(), pDisplay->apHandle());
          apLog_Info((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Display connection " ApHandleFormat " assigned to display " ApHandleFormat "", ApHandleType(pConnection->apHandle()), ApHandleType(pDisplay->apHandle())));
        }

        pDisplay->onCreated(pMsg->hConnection);

        Msg_Fluid_Send msg;
        msg.hConnection = pMsg->hConnection;
        msg.srpc.setString("Method", Fluid_SrpcMethod_Hello);
        int ok = msg.Request();
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Msg_Fluid_Send() failed"));
        }
      } // pDisplay != 0
    } // sMethod == Fluid_SrpcMethod_Hello
  } // if (!bDone)

  if (!bDone) {
    bDone = 1;

    ApHandle hDisplay = findDisplayHandleByConnectionHandle(pMsg->hConnection);
    if (!ApIsHandle(hDisplay)) {
      apLog_Warning((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Unknown connection " ApHandleFormat "", ApHandleType(pMsg->hConnection)));
    } else {
      Display* pDisplay = findDisplay(hDisplay);
      if (pDisplay == 0) {
        apLog_Warning((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Unknown display " ApHandleFormat "", ApHandleType(hDisplay)));
      } else {

        if (0) {
        } else if (sMethod == Fluid_SrpcMethod_Loaded) {
          pDisplay->onLoaded();

        } else if (sMethod == Fluid_SrpcMethod_BeforeUnload) {
          pDisplay->onBeforeUnload();

        } else if (sMethod == Fluid_SrpcMethod_Unloaded) {
          pDisplay->onUnloaded();

        } else if (sMethod == Fluid_SrpcMethod_Log) {
          int nLevel = pMsg->srpc.getInt("nLevel");
          String sContext = pMsg->srpc.getString("sContext");
          String sMessage = pMsg->srpc.getString("sMessage");
          Apollo::sendLog(apLog_Level2Mask(nLevel), LOG_CHANNEL, sContext, sMessage);

        } else if (sMethod == Fluid_SrpcMethod_GetConfig) {
          String sModule = pMsg->srpc.getString("sModule");
          String sKey = pMsg->srpc.getString("sKey");
          String sValue = Apollo::getModuleConfig(sModule, sKey, "");

          Msg_Fluid_Send msg;
          msg.hConnection = pMsg->hConnection;
          msg.srpc.createResponse(pMsg->srpc);
          msg.srpc.setString("sValue", sValue);
          ok = msg.Request();
          if (!ok) {
            apLog_Error((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Msg_Fluid_Send() failed"));
          }

        } else if (sMethod == Fluid_SrpcMethod_SetConfig) {
          String sModule = pMsg->srpc.getString("sModule");
          String sKey = pMsg->srpc.getString("sKey");
          String sValue = pMsg->srpc.getString("sValue");
          Apollo::setModuleConfig(sModule, sKey, sValue);

        } else {
          String sMsg;
          for (Elem* e = 0; (e = pMsg->srpc.Next(e)) != 0; ) {
            if (!sMsg.empty()) { sMsg += " "; }
            sMsg.appendf("%s=%s", StringType(e->getName()), StringType(e->getString()));
          }
          apLog_Warning((LOG_CHANNEL, "FluidModule::On_Fluid_Receive", "Unhandled message on connection " ApHandleFormat ": %s", ApHandleType(pMsg->hConnection), StringType(sMsg)));
        }

      } // pDisplay
    } // hDisplay
  } // if (!bDone)

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(FluidModule, Fluid_DisplayCall)
{
  int ok = 0;

  ApHandle hConnection = findConnectionHandleByDisplayHandle(pMsg->hDisplay);
  if (!ApIsHandle(hConnection)) {
    apLog_Warning((LOG_CHANNEL, "FluidModule::On_Fluid_DisplayCall", "Unknown display " ApHandleFormat "", ApHandleType(pMsg->hDisplay)));
  } else {
    Msg_Fluid_Send msg;
    msg.hConnection = hConnection;
    // Shuffle params to other message without copying data. Original message looses the data.
    pMsg->srpc >> msg.srpc;
    ok = msg.Request();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "FluidModule::On_Fluid_DisplayCall", "Msg_Fluid_Send failed display=" ApHandleFormat " conn=" ApHandleFormat "", ApHandleType(pMsg->hDisplay), ApHandleType(hConnection)));
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

#if defined(AP_TEST)

String Test_Fluid()
{
  String s;
  int ok = 1;
  /*
  ApHandle hDisplay = Apollo::newHandle();
  if (ok) {
    Msg_Fluid_Create msg;
    msg.hDisplay = hDisplay;
    msg.sPathname = "C:\\Wolf\\Projekte\\Apollo\\Co &de\\Apollo\\fl\\ApolloLib\\SimpleHTML\\SimpleHTML.exe";
    msg.sArgs = "-url \"file:C:\\Wolf\\Projekte\\Apollo\\Co &de\\Apollo\\fl\\ApolloLib\\html\\test.html\"";
    ok = msg.Request();
    if (!ok) {
      s.appendf("Msg_Fluid_Create failed");
    }
  }
  */
  /*
  ApHandle hDisplay = Apollo::newHandle();
  if (ok) {
    Msg_Fluid_Create msg;
    msg.hDisplay = hDisplay;
    msg.sPathname = "C:\\Wolf\\Projekte\\Apollo\\Co &de\\Apollo\\fl\\ApolloLib\\ApolloDisplayTest\\ApolloDisplayTest.exe";
    ok = msg.Request();
    if (!ok) {
      s.appendf("Msg_Fluid_Create failed");
    }
  }
  */
  /*
  if (ok) {
    Msg_Fluid_Destroy msg;
    msg.hDisplay = hDisplay;
    ok = msg.Request();
    if (!ok) {
      s.appendf("Msg_Fluid_Destroy failed");
    }
  }
  */
  return s;
}

AP_MSG_HANDLER_METHOD(FluidModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Fluid", 0)) {
    AP_UNITTEST_REGISTER(Test_Fluid);
  }
}

AP_MSG_HANDLER_METHOD(FluidModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Fluid", 0)) {
    AP_UNITTEST_EXECUTE(Test_Fluid);
  }
}

AP_MSG_HANDLER_METHOD(FluidModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

//----------------------------------------------------------

int FluidModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, FluidModule, Fluid_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, FluidModule, Fluid_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, FluidModule, Fluid_Send, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, FluidModule, Fluid_Receive, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, FluidModule, Fluid_DisplayCall, this, ApCallbackPosNormal);

  return ok;
}

void FluidModule::exit()
{
  if (pServer_ != 0) {
    pServer_->Stop();
    delete pServer_;
    pServer_ = 0;
  }

  AP_UNITTEST_UNHOOK(FluidModule, FluidModuleInstance::Get());
  AP_MSG_REGISTRY_FINISH;
}

#endif // #if defined(AP_TEST)
