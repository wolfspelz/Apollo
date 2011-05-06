// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgCore_h_INCLUDED)
#define MsgCore_h_INCLUDED

#include "ApMessage.h"

// -------------------------------------------------------------------
// Msg definitions

// -> core
class APOLLO_API Msg_Core_LoadModule: public ApRequestMessage
{
public:
  Msg_Core_LoadModule() : ApRequestMessage("Core_LoadModule") {}
  ApIN String sDllPath;
  ApIN String sModuleName;
};

// -> core
class APOLLO_API Msg_Core_UnloadModule: public ApRequestMessage
{
public:
  Msg_Core_UnloadModule() : ApRequestMessage("Core_UnloadModule") {}
  ApIN String sModuleName;
};

class Msg_Core_ModulePackageName : public ApRequestMessage
{
public:
  Msg_Core_ModulePackageName() : ApRequestMessage("Core_ModulePackageName") {}
public:
  ApIN  String sModuleName;
  ApOUT String sPackageName;
};

// -> core
class APOLLO_API Msg_Core_IsLoadedModule: public ApRequestMessage
{
public:
  Msg_Core_IsLoadedModule() : ApRequestMessage("Core_IsLoadedModule"), bLoaded(0) {}
  ApIN String sModuleName;
  ApOUT int bLoaded;
};

// -> core
class APOLLO_API Msg_Core_GetModuleInfo: public ApRequestMessage
{
public:
  Msg_Core_GetModuleInfo() : ApRequestMessage("Core_GetModuleInfo") {}
  ApIN String sModuleName;
  ApOUT Apollo::KeyValueList kvInfo;
};

// -> core
// List is in load order, module names in Elem.Name
class APOLLO_API Msg_Core_GetLoadedModules: public ApRequestMessage
{
public:
  Msg_Core_GetLoadedModules() : ApRequestMessage("Core_GetLoadedModules") {}
  ApOUT Apollo::ValueList vlModules;
};

// core -> 
class APOLLO_API Msg_Core_BeforeModuleLoaded: public ApNotificationMessage
{
public:
  Msg_Core_BeforeModuleLoaded() : ApNotificationMessage("Core_BeforeModuleLoaded") {}
public:
  ApIN String sModuleName;
};

// core -> 
class APOLLO_API Msg_Core_ModuleLoaded: public ApNotificationMessage
{
public:
  Msg_Core_ModuleLoaded() : ApNotificationMessage("Core_ModuleLoaded") {}
  ApIN String sShortName;
  ApIN String sDLLPath;
};

// core -> 
class APOLLO_API Msg_Core_BeforeModuleUnloaded : public ApNotificationMessage
{
public:
  Msg_Core_BeforeModuleUnloaded() : ApNotificationMessage("Core_BeforeModuleUnloaded") {}
public:
  ApIN String sModuleName;
};

// core -> 
class APOLLO_API Msg_Core_ModuleUnloaded: public ApNotificationMessage
{
public:
  Msg_Core_ModuleUnloaded() : ApNotificationMessage("Core_ModuleUnloaded") {}
  ApIN String sShortName;
};

// -------------------------------

// -> core
class APOLLO_API Msg_Core_Hook: public ApRequestMessage
{
public:
  Msg_Core_Hook() : ApRequestMessage("Core_Hook"), fnHandler(0), nRef(0), nPosition(0) {}
  ApIN String sCallName;
  ApIN String sModuleName;
  ApIN ApCallback fnHandler;
  ApIN ApCallbackRef nRef;
  ApIN ApCallbackPos nPosition;
};

// -> core
class APOLLO_API Msg_Core_Unhook: public ApRequestMessage
{
public:
  Msg_Core_Unhook() : ApRequestMessage("Core_Unhook"), fnHandler(0), nRef(0) {}
  ApIN String sCallName;
  ApIN String sModuleName;
  ApIN ApCallback fnHandler;
  ApIN ApCallbackRef nRef;
};

#endif // !defined(MsgCore_h_INCLUDED)
