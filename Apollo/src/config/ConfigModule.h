// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ConfigModule_H_INCLUDED)
#define ConfigModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
#include "MsgConfig.h"

class ConfigPlane: public Elem
{
public:
  ConfigPlane(const char* szId)
    :Elem(szId)
  {}

public:
  String sDbName_;
  Apollo::ValueList vlPathNames_;
  StringTree<String> stData_;
};

class ConfigModule
{
public:
  ConfigModule()
    :sCurrentPlane_("default")
    ,bUseDb_(1)
  {}

  int init();
  void exit();

  void On_Config_SetValue(Msg_Config_SetValue* pMsg);
  void On_Config_GetValue(Msg_Config_GetValue* pMsg);
  void On_Config_DeleteValue(Msg_Config_DeleteValue* pMsg);
  void On_Config_Clear(Msg_Config_Clear* pMsg);
  void On_Config_Load(Msg_Config_Load* pMsg);
  void On_Config_GetFileNames(Msg_Config_GetFileNames* pMsg);
  void On_Config_SetPlane(Msg_Config_SetPlane* pMsg);
  void On_Config_GetPlane(Msg_Config_GetPlane* pMsg);
  void On_Config_DeletePlane(Msg_Config_DeletePlane* pMsg);
  void On_Config_GetKeys(Msg_Config_GetKeys* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Token(Msg_UnitTest_Token* pMsg);
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

protected:
  int setValue(ConfigPlane* pPlane, const String& sKey, const String& sValue);

protected:
  String sCurrentPlane_;
  ListT<ConfigPlane, Elem> lPlanes_;
  int bUseDb_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<ConfigModule> ConfigModuleInstance;

#endif // ConfigModule_H_INCLUDED
