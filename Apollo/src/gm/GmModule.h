// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(GmModule_H_INCLUDED)
#define GmModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
#include "MsgConfig.h"
#include "MsgSystem.h"
#include "MsgTimer.h"
#include "MsgGm.h"
#include "MsgIdentityMgmt.h"
#include "SrpcGateHelper.h"
#include "SrpcClient.h"

class GmModule
{
public:
  GmModule()
    :bActive_(1)
    ,bLoginOnRegister_(0)
    ,nRegisterRequests_(0)
    ,bStartXmppClientOnLogin_(0)
    ,nLoginRequests_(0)
    ,bLoggedIn_(0)
    {}

  int init();
  void exit();

  void On_Gm_Start(Msg_Gm_Start* pMsg);
  void On_Gm_Stop(Msg_Gm_Stop* pMsg);
  void On_Gm_StartXmpp(Msg_Gm_StartXmpp* pMsg);
  void On_Gm_Activate(Msg_Gm_Activate* pMsg);
  void On_Gm_SendRequest(Msg_Gm_SendRequest* pMsg);
  void On_Gm_ReceiveResponse(Msg_Gm_ReceiveResponse* pMsg);
  void On_Gm_ReceiveRequest(Msg_Gm_ReceiveRequest* pMsg);
  void On_Gm_SendResponse(Msg_Gm_SendResponse* pMsg);

  void On_Config_GetValue(Msg_Config_GetValue* pMsg);
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);
  void On_Timer_Event(Msg_Timer_Event* pMsg);
  void On_IdentityMgmt_SetProperty(Msg_IdentityMgmt_SetProperty* pMsg);

  void onRegisterResult(SrpcMessage& srpc);
  void onRegisterError(const String sError);
  void onLoginResult(SrpcMessage& srpc);
  void onLoginError(const String sError);
  void onTranceiverResult(SrpcMessage& srpc, const String& sReference);
  void onTranceiverError(const String sError);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);

  friend class GmModuleTester;
#endif

protected:
  String encrypt(const String& sIn);
  String decrypt(const String& sIn);

  String getInstanceId();
  String getMachineId();

  String getPassword();
  int hasGmLoginData();
  int doRegister();
  int doLogin();
  int doLogout();
  int doXmppStart();

public:
  int bActive_;
  String sMachine_;
  int bLoginOnRegister_;
  int nRegisterRequests_;
  ApHandle hRegisterTimer_;
  int bStartXmppClientOnLogin_;
  int nLoginRequests_;
  ApHandle hLoginTimer_;
  int bLoggedIn_;

  SrpcGateHandlerRegistry srpcGateRegistry_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<GmModule> GmModuleInstance;

//----------------------------------------------------------

class RegisterClient: public SrpcClient
{
public:
  RegisterClient(GmModule* pModule)
    :SrpcClient("RegisterClient", pModule) 
  {}

  virtual void OnResult(SrpcMessage& srpc);
  virtual void OnError(const String sError);
};

class LoginClient: public SrpcClient
{
public:
  LoginClient(GmModule* pModule)
    :SrpcClient("LoginClient", pModule) 
  {}

  virtual void OnResult(SrpcMessage& srpc);
  virtual void OnError(const String sError);
};

class SetPropertyClient: public SrpcClient
{
public:
  SetPropertyClient(GmModule* pModule)
    :SrpcClient("SetPropertyClient", pModule) 
  {}

  virtual void OnResult(SrpcMessage& srpc);
  virtual void OnError(const String sError);
};

class TranceiverClient: public SrpcClient
{
public:
  TranceiverClient(GmModule* pModule, const String& sReference)
    :SrpcClient("TranceiverClient", pModule)
    ,sReference_(sReference)
  {}

  String sReference_;

  virtual void OnResult(SrpcMessage& srpc);
  virtual void OnError(const String sError);
};

#define GmService_Method_Register "Gm.Register"
#define GmService_Method_Login "Gm.Login"
#define GmService_Method_SetProperty "Gm.SetProperty"
#define GmService_Method_ItemApi "Gm.ItemApi"

#endif // GmModule_H_INCLUDED
