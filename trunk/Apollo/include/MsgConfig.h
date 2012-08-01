// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgConfig_h_INCLUDED)
#define MsgConfig_h_INCLUDED

#include "ApMessage.h"

class Msg_Config_GetValue: public ApRequestMessage
{
public:
  Msg_Config_GetValue() : ApRequestMessage("Config_GetValue") {}
  ApIN String sPath;
  ApOUT String sValue;
};

class Msg_Config_SetValue: public ApRequestMessage
{
public:
  Msg_Config_SetValue() : ApRequestMessage("Config_SetValue") {}
  ApIN String sPath;
  ApIN String sValue;
};

class Msg_Config_DeleteValue: public ApRequestMessage
{
public:
  Msg_Config_DeleteValue() : ApRequestMessage("Config_DeleteValue") {}
  ApIN String sPath;
};

// Clears the current config plane in memory
class Msg_Config_Clear: public ApRequestMessage
{
public:
  Msg_Config_Clear() : ApRequestMessage("Config_Clear") {}
  static int _() { Msg_Config_Clear msg; return msg.Request(); }
};

// Load file into the current config plane
class Msg_Config_Load: public ApRequestMessage
{
public:
  Msg_Config_Load() : ApRequestMessage("Config_Load") {}
  ApIN String sDir;
  ApIN String sName;
};

class Msg_Config_Loaded: public ApNotificationMessage
{
public:
  Msg_Config_Loaded() : ApNotificationMessage("Config_Loaded") {};
};

class Msg_Config_GetFileNames: public ApRequestMessage
{
public:
  Msg_Config_GetFileNames() : ApRequestMessage("Config_GetFileNames") {}
  ApOUT Apollo::ValueList vlPathNames;
};

class Msg_Config_SetPlane: public ApRequestMessage
{
public:
  Msg_Config_SetPlane() : ApRequestMessage("Config_SetPlane") {}
  static int _(const String& sPlane) { Msg_Config_SetPlane msg; msg.sPlane = sPlane; return msg.Request(); }
  ApIN String sPlane;
};

class Msg_Config_GetPlane: public ApRequestMessage
{
public:
  Msg_Config_GetPlane() : ApRequestMessage("Config_GetPlane") {}
  ApOUT String sPlane;

  static String _()
  {
    Msg_Config_GetPlane msg;
    msg.Request();
    return msg.sPlane;
  }
};

class Msg_Config_DeletePlane: public ApRequestMessage
{
public:
  Msg_Config_DeletePlane() : ApRequestMessage("Config_DeletePlane") {}
  ApOUT String sPlane;
};

class Msg_Config_GetKeys: public ApRequestMessage
{
public:
  Msg_Config_GetKeys() : ApRequestMessage("Config_GetKeys") {}
  ApIN String sPath;
  ApOUT Apollo::ValueList vlKeys;
};

#endif // !defined(MsgConfig_h_INCLUDED)
