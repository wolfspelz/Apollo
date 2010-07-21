// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgDb_h_INCLUDED)
#define MsgDb_h_INCLUDED

#include "ApMessage.h"

// -> db module
class Msg_DB_Open: public ApRequestMessage
{
public:
  Msg_DB_Open() : ApRequestMessage("DB_Open") {}
public:
  ApIN String sName;
  ApIN String sFilePath;
};

class Msg_DB_Close: public ApRequestMessage
{
public:
  Msg_DB_Close() : ApRequestMessage("DB_Close") {}
public:
  ApIN String sName;
};

class Msg_DB_Set: public ApRequestMessage
{
public:
  Msg_DB_Set() : ApRequestMessage("DB_Set"), nLifeTime(0) {}
public:
  ApIN String sName;
  ApIN String sKey;
  ApIN String sValue;
  ApIN int nLifeTime;
};

class Msg_DB_SetBinary: public ApRequestMessage
{
public:
  Msg_DB_SetBinary() : ApRequestMessage("DB_SetBinary"), nLifeTime(0) {}
public:
  ApIN String sName;
  ApIN String sKey;
  ApIN Buffer sbValue;
  ApIN int nLifeTime;
};

class Msg_DB_HasKey: public ApRequestMessage
{
public:
  Msg_DB_HasKey() : ApRequestMessage("DB_HasKey"), bAvailable(0) {}
public:
  ApIN String sName;
  ApIN String sKey;
  ApOUT int bAvailable;
};

class Msg_DB_Get: public ApRequestMessage
{
public:
  Msg_DB_Get() : ApRequestMessage("DB_Get") {}
public:
  ApIN String sName;
  ApIN String sKey;
  ApOUT String sValue;
};

class Msg_DB_GetBinary: public ApRequestMessage
{
public:
  Msg_DB_GetBinary() : ApRequestMessage("DB_GetBinary") {}
public:
  ApIN String sName;
  ApIN String sKey;
  ApOUT Buffer sbValue;
};

class Msg_DB_GetKeys: public ApRequestMessage
{
public:
  Msg_DB_GetKeys() : ApRequestMessage("DB_GetKeys") {}
public:
  ApIN String sName;
  ApOUT Apollo::ValueList vlKeys;
};

class Msg_DB_Delete: public ApRequestMessage
{
public:
  Msg_DB_Delete() : ApRequestMessage("DB_Delete") {}
public:
  ApIN String sName;
  ApIN String sKey;
};

class Msg_DB_DeleteFile: public ApRequestMessage
{
public:
  Msg_DB_DeleteFile() : ApRequestMessage("DB_DeleteFile") {}
public:
  ApIN String sName;
  ApIN String sFilePath;
};

class Msg_DB_Clear: public ApRequestMessage
{
public:
  Msg_DB_Clear() : ApRequestMessage("DB_Clear") {}
public:
  ApIN String sName;
};

class Msg_DB_Expire: public ApRequestMessage
{
public:
  Msg_DB_Expire() : ApRequestMessage("DB_Expire") {}
public:
  ApIN String sName;
};

class Msg_DB_DeleteOlderThan: public ApRequestMessage
{
public:
  Msg_DB_DeleteOlderThan() : ApRequestMessage("DB_DeleteOlderThan"), nAge(0) {}
public:
  ApIN String sName;
  ApIN int nAge;
};

class Msg_DB_Sync: public ApRequestMessage
{
public:
  Msg_DB_Sync() : ApRequestMessage("DB_Sync") {}
public:
  ApIN String sName;
};

#endif

