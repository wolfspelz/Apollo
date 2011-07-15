// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(DbModule_H_INCLUDED)
#define DbModule_H_INCLUDED

#include "ApModule.h"
#include "MsgUnitTest.h"
#include "MsgDB.h"
#include "MsgSystem.h"
#include "SQLite.h"

class DbModule
{
public:
  DbModule() {}
  virtual ~DbModule() {}

  int init();
  void exit();

  void On_DB_Open(Msg_DB_Open* pMsg);
  void On_DB_Close(Msg_DB_Close* pMsg);
  void On_DB_Set(Msg_DB_Set* pMsg);
  void On_DB_SetBinary(Msg_DB_SetBinary* pMsg);
  void On_DB_HasKey(Msg_DB_HasKey* pMsg);
  void On_DB_Get(Msg_DB_Get* pMsg);
  void On_DB_GetBinary(Msg_DB_GetBinary* pMsg);
  void On_DB_GetKeys(Msg_DB_GetKeys* pMsg);
  void On_DB_Delete(Msg_DB_Delete* pMsg);
  void On_DB_DeleteFile(Msg_DB_DeleteFile* pMsg);
  void On_DB_Clear(Msg_DB_Clear* pMsg);
  void On_DB_Expire(Msg_DB_Expire* pMsg);
  void On_DB_DeleteOlderThan(Msg_DB_DeleteOlderThan* pMsg);
  void On_DB_Sync(Msg_DB_Sync* pMsg);
  void On_System_10SecTimer(Msg_System_10SecTimer* pMsg);
  void On_System_60SecTimer(Msg_System_60SecTimer* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Token(Msg_UnitTest_Token* pMsg);
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

protected:
  ListT<SQLiteFile, Elem> lFiles_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<DbModule> DbModuleInstance;

#endif // DbModule_H_INCLUDED
