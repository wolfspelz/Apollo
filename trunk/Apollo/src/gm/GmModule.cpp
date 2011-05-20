// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Crypto.h"
#include "MsgConfig.h"
#include "MsgNet.h"
#include "MsgCore.h"
#include "MsgXmpp.h"
#include "SrpcMessage.h"
#include "Local.h"
#include "GmModule.h"
#include "GmModuleTester.h"

//----------------------------------------------------------

void RegisterClient::OnResult(SrpcMessage& srpc)
{
  String sPassword = srpc.getString("sPassword");
  if (!sPassword.empty()) {
    srpc.set("sPassword", "********");
  }
  apLog_Info((MODULE_NAME, "RegisterClient::OnResult", "%s", StringType(srpc.toString())));
  srpc.set("sPassword", sPassword);

  if (pModule_) {
    pModule_->onRegisterResult(srpc);
  }
}

void RegisterClient::OnError(const String sError)
{
  apLog_Warning((MODULE_NAME, "RegisterClient::OnError", "%s", StringType(sError)));

  if (pModule_) {
    pModule_->onRegisterError(sError);
  }
}

//----------------------------------------------------------

void LoginClient::OnResult(SrpcMessage& srpc)
{
  apLog_Info((MODULE_NAME, "LoginClient::OnResult", "%s", StringType(srpc.toString())));

  if (pModule_) {
    pModule_->onLoginResult(srpc);
  }
}

void LoginClient::OnError(const String sError)
{
  apLog_Warning((MODULE_NAME, "LoginClient::OnError", "%s", StringType(sError)));

  if (pModule_) {
    pModule_->onLoginError(sError);
  }
}

//----------------------------------------------------------

void SetPropertyClient::OnResult(SrpcMessage& srpc)
{
  apLog_Info((MODULE_NAME, "SetPropertyClient::OnResult", "%s", StringType(srpc.toString())));

  //if (pModule_) {
  //  pModule_->onLoginResult(srpc);
  //}
}

void SetPropertyClient::OnError(const String sError)
{
  apLog_Warning((MODULE_NAME, "SetPropertyClient::OnError", "%s", StringType(sError)));

  //if (pModule_) {
  //  pModule_->onLoginError(sError);
  //}
}

//----------------------------------------------------------

//String GmModule::encrypt(const String& sIn)
//{
//  String sOut;
//
//  String sDummy = Apollo::getRandomString(20);
//  String sKey = Apollo::getRandomString(20);
//
//  Apollo::Crypto c;
//  c.SetData((unsigned char*) (const char*) sIn, sIn.bytes());
//
//  Buffer b;
//  c.encryptBlowfish(sKey, b);
//
//  String sCryptedBase64;
//  b.encodeBinhex(sCryptedBase64);
//
//  sOut = sDummy.subString(0, 3) + sKey + sDummy.subString(3, 17) + sCryptedBase64;
//
//  return sOut;
//}
//
//String GmModule::decrypt(const String& sIn)
//{
//  String sOut;
//
//  String sKey = sIn.subString(3, 20);
//  String sData = sIn.subString(40);
//
//  Buffer b;
//  b.decodeBinhex(sData);
//
//  Apollo::Crypto c;
//  c.decryptBlowfish(sKey, b);
//
//  c.GetString(sOut);
//
//  return sOut;
//}

String GmModule::encrypt(const String& sIn)
{
  Apollo::Crypto data;
  data.SetData((unsigned char*) (const char*) sIn, sIn.bytes());

  Buffer b;
  if (!data.encryptWithLoginCredentials(b)) {
    apLog_Error((LOG_CHANNEL, "GmModule::encrypt", "encryptWithLoginCredentials() failed"));
  }

  String sCryptedBase64;
  b.encodeBase64(sCryptedBase64);

  return sCryptedBase64;
}

String GmModule::decrypt(const String& sIn)
{
  Buffer b;
  b.decodeBase64(sIn);

  Apollo::Crypto data;
  if (!data.decryptWithLoginCredentials(b)) {
    apLog_Error((LOG_CHANNEL, "GmModule::encrypt", "decryptWithLoginCredentials() failed"));
  }

  String sDecrypted;
  data.GetString(sDecrypted);

  return sDecrypted;
}

//-----------------------------

String GmModule::getInstanceId()
{
  String sInstance = Apollo::getModuleConfig(MODULE_NAME, "Instance", "");

  if (sInstance.empty()) {
    sInstance = Apollo::getRandomString(20);
    Apollo::setModuleConfig(MODULE_NAME, "Instance", sInstance);
  }

  return sInstance;
}

String GmModule::getMachineId()
{
  String sMachine = sMachine_;

  if (sMachine.empty()) {
    sMachine = Apollo::getMachineId();
    sMachine_ = sMachine;
  }

  return sMachine;
}

String GmModule::getPassword()
{
  String sPassword;

  if (sPassword.empty()) {
    String sEncryptedPassword = Apollo::getModuleConfig(MODULE_NAME, "EncryptedPassword", "");
    if (sEncryptedPassword) {
      sPassword = decrypt(sEncryptedPassword);
    }
  }

  if (sPassword.empty()) {
    sPassword = Apollo::getModuleConfig(MODULE_NAME, "Password", "");
  }

  return sPassword;
}

int GmModule::hasGmLoginData()
{
  int bLoginDataAvailable = 0;

  String sUser = Apollo::getModuleConfig(MODULE_NAME, "User", "");
  String sPassword = getPassword();

  if (!sUser.empty() && !sPassword.empty()) {
    bLoginDataAvailable = 1;
  }

  return bLoginDataAvailable;
}

//----------------------------------------------------------

int GmModule::doXmppStart()
{
  int ok = 0;

  Msg_Gm_StartXmpp msg;
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "GmModule::doXmppStart", "Msg_Gm_StartXmpp failed"));
  }

  return ok;
}

void GmModule::onRegisterResult(SrpcMessage& srpc)
{
  String sUser = srpc.getString("User");
  String sPassword = srpc.getString("Password");

  apLog_Verbose((LOG_CHANNEL, "GmModule::onRegisterResult", "Success user=%s", StringType(sUser)));

  Apollo::setModuleConfig(MODULE_NAME, "User", sUser);
  Apollo::setModuleConfig(MODULE_NAME, "EncryptedPassword", encrypt(sPassword));

  if (bLoginOnRegister_) {
    if (!doLogin()) {
      apLog_Error((LOG_CHANNEL, "GmModule::onRegisterResult", "doLogin() failed"));
    } else {
      bLoginOnRegister_ = 0;
    }
  }
}

void GmModule::onRegisterError(const String sError)
{
  apLog_Warning((LOG_CHANNEL, "GmModule::onRegisterError", "%s", StringType(sError)));

  if (nRegisterRequests_ >= Apollo::getModuleConfig(MODULE_NAME, "Register/MaxRetries", 3)) {
    apLog_Error((LOG_CHANNEL, "GmModule::onRegisterError", "Register/MaxRetries=%d exceeded, giving up", nRegisterRequests_));
  } else {
    if (ApIsHandle(hRegisterTimer_)) {
      apLog_Error((LOG_CHANNEL, "GmModule::onRegisterError", "Skipping timer, because already active"));
    } else {
      int nDelay = Apollo::getModuleConfig(MODULE_NAME, "Register/RetryDelay", 10) + Apollo::getRandom(Apollo::getModuleConfig(MODULE_NAME, "Register/RetryRandomDelay", 10));
      apLog_Info((LOG_CHANNEL, "GmModule::onRegisterError", "Scheduling timer in %d sec", nDelay));
      hRegisterTimer_ = Apollo::startTimeout(nDelay, 0);
    }
  }
}

int GmModule::doRegister()
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "GmModule::doRegister", ""));

  if (nRegisterRequests_ > Apollo::getModuleConfig(MODULE_NAME, "Register/MaxRetries", 3)) {
    apLog_Error((LOG_CHANNEL, "GmModule::doRegister", "Register/MaxRetries=%d exceeded, giving up", nRegisterRequests_));
  } else {

    String sUrl = Apollo::getModuleConfig(MODULE_NAME, "Srpc/Url", "");

    if (sUrl.empty()) {
      apLog_Error((LOG_CHANNEL, "GmModule::doRegister", "Missing Srpc/Url"));
    } else {

      SrpcMessage srpc;
      srpc.set("Method", GmService_Method_Register);
      srpc.set("Token", Apollo::getModuleConfig(MODULE_NAME, "Srpc/ApiToken", "8uzxXXZTAmHcni6tK3t"));
      srpc.set("User", Apollo::getRandomString(20));
      srpc.set("Password", Apollo::getRandomString(20));
      
      if (Apollo::getModuleConfig(MODULE_NAME, "Register/UseLoginAsNickname", 1)) {
        String sNickname = Apollo::getUserLoginName();
        if (!sNickname.empty()) {
          srpc.set("Nickname", sNickname);
        }
      }

      RegisterClient* pClient = new RegisterClient(this);
      if (pClient != 0) {
        ok = pClient->Post(sUrl, srpc);
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "GmModule::doRegister", "RegisterClient::Post(%s) failed", StringType(sUrl)));
        } else {
          nRegisterRequests_++;
        }
      }
    } // sUrl.empty
  } // nRegisterRequests_

  return ok;
}

void GmModule::onLoginResult(SrpcMessage& srpc)
{
  apLog_Verbose((LOG_CHANNEL, "GmModule::onLoginResult", "Success"));
  bLoggedIn_ = 1;

  int nCnt = 0;
  int bDone = 0;
  while (!bDone) {
    String sKey; sKey.appendf("%d:Command", nCnt);
    String sValue = srpc.getString(sKey);
    if (sValue.empty()) {
      bDone = 1;
    } else {
      Apollo::KeyValueList kvCommand;
      srpc.getKeyValueList(sKey, kvCommand);

      ApSRPCMessage msg("SrpcGate");
      for (Apollo::KeyValueElem* e = 0; (e = kvCommand.nextElem(e)) != 0; ) {
        msg.srpc.set(e->getKey(), e->getString());
      }
      apLog_Info((LOG_CHANNEL, "GmModule::onLoginResult", "Execute %s", StringType(msg.srpc.toString())));
      msg.Call();
      nCnt++;
    }
  }
}

void GmModule::onLoginError(const String sError)
{
  apLog_Warning((LOG_CHANNEL, "GmModule::onLoginError", "%s", StringType(sError)));

  if (nLoginRequests_ >= Apollo::getModuleConfig(MODULE_NAME, "Login/MaxRetries", 1000000000)) {
    apLog_Error((LOG_CHANNEL, "GmModule::onLoginError", "Login/MaxRetries=%d exceeded, giving up", nLoginRequests_));
  } else {
    if (ApIsHandle(hLoginTimer_)) {
      apLog_Error((LOG_CHANNEL, "GmModule::onLoginError", "Skipping timer, because already active"));
    } else {
      int nDelay = Apollo::getModuleConfig(MODULE_NAME, "Login/RetryDelay", 30) + Apollo::getRandom(Apollo::getModuleConfig(MODULE_NAME, "Login/RetryRandomDelay", 30));
      apLog_Info((LOG_CHANNEL, "GmModule::onLoginError", "Scheduling timer in %d sec", nDelay));
      hLoginTimer_ = Apollo::startTimeout(nDelay, 0);
    }
  }
}

int GmModule::doLogin()
{
  int ok = 0;
  apLog_Verbose((LOG_CHANNEL, "GmModule::doLogin", ""));

  if (!hasGmLoginData()) {
    bLoginOnRegister_ = 1;
    ok = doRegister();
  } else {

    if (nLoginRequests_ > Apollo::getModuleConfig(MODULE_NAME, "Login/MaxRetries", 1000000000)) {
      apLog_Error((LOG_CHANNEL, "GmModule::doLogin", "Login/MaxRetries=%d exceeded, giving up", nLoginRequests_));
    } else {
      String sUrl = Apollo::getModuleConfig(MODULE_NAME, "Srpc/Url", "");

      if (sUrl.empty()) {
        apLog_Error((LOG_CHANNEL, "GmModule::doLogin", "Missing Srpc/Url"));
      } else {

        Apollo::KeyValueList kvClientInfo;
        Msg_Core_GetLoadedModules msgCGLM;
        if (msgCGLM.Request()) {
          for (Apollo::ValueElem* e = 0; (e = msgCGLM.vlModules.nextElem(e)) != 0; ) {
            Msg_Core_GetModuleInfo msgCGMI;
            msgCGMI.sModuleName = e->getString();
            if (msgCGMI.Request()) {
              Apollo::KeyValueElem* e = msgCGMI.kvInfo.find("Version");
              if (e) {
                kvClientInfo.add(msgCGMI.sModuleName, e->getString());
              } else {
                kvClientInfo.add(msgCGMI.sModuleName, "");
              }
            }
          }
        }

        kvClientInfo.add("Instance", getInstanceId());
        kvClientInfo.add("Machine", getMachineId());

        SrpcMessage srpc;
        srpc.set("Method", GmService_Method_Login);
        srpc.set("Token", Apollo::getModuleConfig(MODULE_NAME, "Srpc/ApiToken", "8uzxXXZTAmHcni6tK3t"));
        srpc.set("User", Apollo::getModuleConfig(MODULE_NAME, "User", ""));
        srpc.set("Password", getPassword());
        srpc.set("Client", kvClientInfo.toString());

        LoginClient* pClient = new LoginClient(this);
        if (pClient != 0) {
          ok = pClient->Post(sUrl, srpc);
          if (!ok) {
            apLog_Error((LOG_CHANNEL, "GmModule::doLogin", "LoginClient::Post(%s) failed", StringType(sUrl)));
          } else {
            nLoginRequests_++;
          }
        }
      } // sUrl.empty
    } // nLoginRequests_
  }

  return ok;
}

int GmModule::doLogout()
{
  int ok = 1;
  return ok;
}

/*
int GmModule::hasXmppLoginData()
{
  int bLoginDataAvailable = 0;

  String sJabberId = Apollo::getConfig("Xmpp/Connection/JabberId", "");
  String sJabberPw = Apollo::getConfig("Xmpp/Connection/Password", "");

  if (!sJabberId.empty() && !sJabberPw.empty()) {
    bLoginDataAvailable = 1;
  }

  return bLoginDataAvailable;
}

*/
//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(GmModule, Gm_Start)
{
  if (!bActive_) { pMsg->apStatus = ApMessage::Ok; return;}

  int ok = 0;

  ok = doLogin();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "GmModule::Gm_Start", "doLogin() failed"));
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(GmModule, Gm_Stop)
{
  if (!bActive_) { pMsg->apStatus = ApMessage::Ok; return;}

  int ok = 0;

  ok = doLogout();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "GmModule::Gm_Stop", "doLogout() failed"));
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(GmModule, Gm_StartXmpp)
{
  if (!bActive_) { pMsg->apStatus = ApMessage::Ok; return;}

  int ok = 0;

  Msg_Xmpp_StartClient msg;
  msg.hClient = Apollo::newHandle();
  msg.sJabberId = Apollo::getModuleConfig(MODULE_NAME, "Xmpp/JabberId", "");
  msg.sPassword = Apollo::getModuleConfig(MODULE_NAME, "Xmpp/Password", "");
  msg.sResource = Apollo::getModuleConfig(MODULE_NAME, "Xmpp/Resource", "");
  ok = msg.Request();
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "GmModule::Gm_StartXmpp", "Msg_Xmpp_StartClient failed"));
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(GmModule, Config_GetValue)
{
  if (bActive_) {
    if (0) {
    } else if (pMsg->sPath == "Xmpp/StartOnRunLevelNormal") {
      bStartXmppClientOnLogin_ = 1;
      pMsg->sValue = "0";
      pMsg->Stop();
      pMsg->apStatus = ApMessage::Ok;

    } else if (pMsg->sPath == "Xmpp/xx") {
    }
  }
}

AP_MSG_HANDLER_METHOD(GmModule, System_RunLevel)
{
  if (0) {
  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {

    int bActive = Apollo::getModuleConfig(MODULE_NAME, "Active", 1);
    if (bActive_ != bActive) {
      Msg_Gm_Activate msg;
      msg.bActive = bActive;
      msg.Request();
    }

    if (bActive_) {
      if (Apollo::getModuleConfig(MODULE_NAME, "StartOnRunLevelNormal", 1)) {
        Msg_Gm_Start msg;
        int ok = msg.Request();
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "GmModule::System_RunLevel", "Msg_Gm_Start failed"));
        }
      }
    } // bActive_

  } else if (pMsg->sLevel == Msg_System_RunLevel_Shutdown) {

    if (bActive_) {
      if (bLoggedIn_) {
        Msg_Gm_Stop msg;
        int ok = msg.Request();
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "GmModule::System_RunLevel", "Msg_Gm_Stop failed"));
        }
      }
    } // bActive_

  }
}

AP_MSG_HANDLER_METHOD(GmModule, Timer_Event)
{
  if (pMsg->hTimer == hRegisterTimer_) {
    hRegisterTimer_ = ApNoHandle;
    if (!doRegister()) {
      apLog_Error((LOG_CHANNEL, "GmModule::Timer_Event", "doRegister() failed"));
    }
  }

  if (pMsg->hTimer == hLoginTimer_) {
    hLoginTimer_ = ApNoHandle;
    if (!doLogin()) {
      apLog_Error((LOG_CHANNEL, "GmModule::Timer_Event", "doLogin() failed"));
    }
  }
}

AP_MSG_HANDLER_METHOD(GmModule, Gm_Activate)
{
  bActive_ = pMsg->bActive;
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(GmModule, IdentityMgmt_SetProperty)
{
  if (!bActive_) { pMsg->apStatus = ApMessage::Ok; return; }

  int ok = 0;

  if (!hasGmLoginData()) { throw ApException("Missing login data"); }
  String sUrl = Apollo::getModuleConfig(MODULE_NAME, "Srpc/Url", "");
  if (sUrl.empty()) { throw ApException("Missing Srpc/Url"); }

  SrpcMessage srpc;
  srpc.set("Method", GmService_Method_SetProperty);
  srpc.set("Token", Apollo::getModuleConfig(MODULE_NAME, "Srpc/ApiToken", "8uzxXXZTAmHcni6tK3t"));
  srpc.set("User", Apollo::getModuleConfig(MODULE_NAME, "User", ""));
  srpc.set("Password", getPassword());
  srpc.set("Key", pMsg->sKey);
  srpc.set("Value", pMsg->sValue);

  SetPropertyClient* pClient = new SetPropertyClient(this);
  if (pClient != 0) {
    ok = pClient->Post(sUrl, srpc);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "GmModule::IdentityMgmt_SetProperty", "SetPropertyClient::Post(%s) failed", StringType(sUrl)));
    }
  }

  if (pMsg->sKey == "Nickname") {
    Apollo::setModuleConfig("Xmpp", "Room/Nickname", pMsg->sValue);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

void SrpcGate_Gm_Start(ApSRPCMessage* pMsg)
{
  Msg_Gm_Start msg;
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_Gm_Stop(ApSRPCMessage* pMsg)
{
  Msg_Gm_Stop msg;
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_Gm_StartXmpp(ApSRPCMessage* pMsg)
{
  Msg_Gm_StartXmpp msg;
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_IdentityMgmt_SetProperty(ApSRPCMessage* pMsg)
{
  Msg_IdentityMgmt_SetProperty msg;
  msg.sKey = pMsg->srpc.getString("sKey");
  msg.sValue = pMsg->srpc.getString("sValue");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(GmModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  GmModuleTester::begin();
}

AP_MSG_HANDLER_METHOD(GmModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  GmModuleTester::execute();
}

AP_MSG_HANDLER_METHOD(GmModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  GmModuleTester::end();
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int GmModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, Gm_Start, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, Gm_Stop, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, Gm_StartXmpp, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, Config_GetValue, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, System_RunLevel, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, Timer_Event, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, Gm_Activate, this, ApCallbackPosEarly);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, IdentityMgmt_SetProperty, this, ApCallbackPosEarly);

  #if defined(AP_TEST)
    AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, UnitTest_Begin, this, ApCallbackPosNormal);
    AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, UnitTest_Execute, this, ApCallbackPosNormal);
    AP_MSG_REGISTRY_ADD(MODULE_NAME, GmModule, UnitTest_End, this, ApCallbackPosNormal);
  #endif

  srpcGateRegistry_.add("Gm_Start", SrpcGate_Gm_Start);
  srpcGateRegistry_.add("Gm_Stop", SrpcGate_Gm_Stop);
  srpcGateRegistry_.add("Gm_StartXmpp", SrpcGate_Gm_StartXmpp);
  srpcGateRegistry_.add("IdentityMgmt_SetProperty", SrpcGate_IdentityMgmt_SetProperty);

  return ok;
}

void GmModule::exit()
{
  AP_MSG_REGISTRY_FINISH;
}
