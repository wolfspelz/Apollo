// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "DbModule.h"

AP_MSG_HANDLER_METHOD(DbModule, DB_Open)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile != 0) {
    ok = 1;
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "DB file already open: %s %s", _sz(pMsg->sName), _sz(pMsg->sFilePath)));
  } else {
    String sFilePath = pMsg->sFilePath;
    if (sFilePath.empty()) {
      sFilePath = SQLiteFile::pathFromName(pMsg->sName);
    }

    pFile = new SQLiteFile(pMsg->sName, sFilePath);
    if (pFile != 0) {
      ok = pFile->open();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "DB file open failed: %s %s", _sz(pMsg->sName), _sz(pMsg->sFilePath)));
      }
    }

    if (ok) {
      ok = pFile->check();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "check() failed: %s %s", _sz(pMsg->sName), _sz(pMsg->sFilePath)));
      }
    }

    if (ok) {
      ok = pFile->beginTransaction();
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "beginTransaction() failed: %s %s", _sz(pMsg->sName), _sz(pMsg->sFilePath)));
      }
    }

    if (ok) {
      lFiles_.Add(pFile);
    } else {
      delete pFile;
      pFile = 0;
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_Close)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "DB file not found: %s", _sz(pMsg->sName)));
  } else {
    ok = 1;

    ok = pFile->commitTransaction();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "commitTransaction() failed: %s", _sz(pMsg->sName)));
    }

    ok = pFile->close();
    if (!ok) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "DB file close failed: %s", _sz(pMsg->sName)));
    } else {
      lFiles_.Remove(pFile);
      delete pFile;
      pFile = 0;
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_Set)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "DB file not found: %s", _sz(pMsg->sName)));
  } else {
    ok = pFile->setValue(pMsg->sKey, pMsg->sValue, pMsg->nLifeTime);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_SetBinary)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "DB file not found: %s", _sz(pMsg->sName)));
  } else {
    ok = pFile->setValue(pMsg->sKey, pMsg->sbValue.Data(), pMsg->sbValue.Length(), pMsg->nLifeTime);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_HasKey)
{
  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile == 0) { throw ApException(LOG_CONTEXT, "DB file not found: %s", _sz(pMsg->sName)); }
  if (!pFile->hasValue(pMsg->sKey, pMsg->bAvailable)) { throw ApException(LOG_CONTEXT, "pFile->hasValue() failed"); }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_Get)
{
  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile == 0) { throw ApException(LOG_CONTEXT, "DB file not found: %s", _sz(pMsg->sName)); }
  if (!pFile->getValue(pMsg->sKey, pMsg->sValue)) { throw ApException(LOG_CONTEXT, "pFile->getValue() failed"); }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_GetBinary)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "DB file not found: %s", _sz(pMsg->sName)));
  } else {
    ok = pFile->getValue(pMsg->sKey, pMsg->sbValue);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_GetKeys)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "DB file not found: %s", _sz(pMsg->sName)));
  } else {
    ok = pFile->getKeys(pMsg->vlKeys);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_Delete)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile == 0) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "DB file not found: %s", _sz(pMsg->sName)));
  } else {
    ok = pFile->deleteValue(pMsg->sKey);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_DeleteFile)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile != 0) {
    if (pFile->isOpen()) {
      Msg_DB_Close msg;
      msg.sName = pMsg->sName;
      msg.Request();
    }
  }

  String sFilePath = pMsg->sFilePath;
  if (sFilePath.empty()) {
    sFilePath = SQLiteFile::pathFromName(pMsg->sName);
  }

  ok = Msg_File_Delete::_(sFilePath);

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_Clear)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile != 0) {
    ok = pFile->clear();
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_Expire)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile != 0) {
    ok = pFile->expire();
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_DeleteOlderThan)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile != 0) {
    ok = pFile->deleteOlderThan(pMsg->nAge);
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, DB_Sync)
{
  int ok = 0;

  SQLiteFile* pFile = lFiles_.FindByName(pMsg->sName);
  if (pFile != 0) {
    ok = pFile->sync();
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(DbModule, System_10SecTimer)
{
  for (SQLiteFile* pFile = 0; (pFile = lFiles_.Next(pFile)) != 0; ) {
    if (pFile->isChanged()) {
      if (! pFile->sync()) {
        apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "pFile->sync() failed (%s)", _sz(pFile->getFilePath())));
      }
    }
  }
}

AP_MSG_HANDLER_METHOD(DbModule, System_60SecTimer)
{
}

//----------------------------------------------------------

#if defined(AP_TEST)

static String Test_Db_String()
{
  String s;

  int bSyncToDisk = 1;

  //{ Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }
  { Msg_DB_Open msg; msg.sName = "test"; msg.Request(); }

  String sString = "Hallo Welt string";

  if (!s) {
    Msg_DB_Set msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    msg.sValue = sString;
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_Get msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    String sValue = msg.sValue;
    if (!s) { if (sValue != sString) { s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }

  { Msg_DB_Close msg; msg.sName = "test"; msg.Request(); }
//  { Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }

  return s;
}

static String Test_Db_Binary()
{
  String s;

  int bSyncToDisk = 1;

  //{ Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }
  { Msg_DB_Open msg; msg.sName = "test"; msg.Request(); }

  String sBinary = "Hallo Welt binary";

  // Binary
  if (!s) {
    Msg_DB_SetBinary msg;
    msg.sName = "test";
    msg.sKey = "test-binary";
    msg.sbValue.SetData((const unsigned char*) sBinary.c_str(), sBinary.bytes());
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_Get msg;
    msg.sName = "test";
    msg.sKey = "test-binary";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    String sValue = msg.sValue;
    if (!s) { if (sValue != sBinary) { s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }
  // Delete binary
  if (!s) {
    Msg_DB_Delete msg;
    msg.sName = "test";
    msg.sKey = "test-binary";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_Get msg;
    msg.sName = "test";
    msg.sKey = "test-binary";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) { if (msg.sValue != "") { s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }

  { Msg_DB_Close msg; msg.sName = "test"; msg.Request(); }
//  { Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }

  return s;
}

static String Test_Db_Bulk()
{
  String s;

  int bSyncToDisk = 1;

  //{ Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }
  { Msg_DB_Open msg; msg.sName = "test"; msg.Request(); }

  // Bulk data
  Buffer sbFile;
  Msg_File_Load::_(Apollo::getAppResourcePath() + "ApCore.dll", sbFile);
  Buffer sbFile2;
  Msg_File_Load::_(Apollo::getAppResourcePath() + "test/test1.png", sbFile2);

  if (!s) {
    Msg_DB_SetBinary msg;
    msg.sName = "test";
    msg.sKey = "test-bulk";
    msg.sbValue.SetData(sbFile.Data(), sbFile.Length());
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_GetBinary msg;
    msg.sName = "test";
    msg.sKey = "test-bulk";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) {
      int bEqual = (msg.sbValue.Length() == sbFile.Length());
      if (bEqual) { bEqual = !memcmp(sbFile.Data(), msg.sbValue.Data(), sbFile.Length()); }
      if (!bEqual) {
        s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey));
      }
    }
  }

  { Msg_DB_Close msg; msg.sName = "test"; msg.Request(); }
//  { Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }

  return s;
}

static String Test_Db_UpdateString()
{
  String s;

  int bSyncToDisk = 1;

  //{ Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }
  { Msg_DB_Open msg; msg.sName = "test"; msg.Request(); }

  String sString = "Hallo Welt string";
  String sString2 = "Hallo Welt string 2";

  if (!s) {
    Msg_DB_Set msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    msg.sValue = sString;
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_Get msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    String sValue = msg.sValue;
    if (!s) { if (sValue != sString) { s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }
  // Update string
  if (!s) {
    Msg_DB_Set msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    msg.sValue = sString2;
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_Get msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) { if (msg.sValue != sString2) { s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }
  // Delete string
  if (!s) {
    Msg_DB_Delete msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_Get msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) { if (msg.sValue != "") { s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }

  { Msg_DB_Close msg; msg.sName = "test"; msg.Request(); }
//  { Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }

  return s;
}

static String Test_Db_UpdateBulk()
{
  String s;

  int bSyncToDisk = 1;

  //{ Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }
  { Msg_DB_Open msg; msg.sName = "test"; msg.Request(); }

  // Bulk data
  Buffer sbFile;
  Msg_File_Load::_(Apollo::getAppResourcePath() + "ApCore.dll", sbFile);
  Buffer sbFile2;
  Msg_File_Load::_(Apollo::getAppResourcePath() + "test/test1.png", sbFile2);

  if (!s) {
    Msg_DB_SetBinary msg;
    msg.sName = "test";
    msg.sKey = "test-bulk";
    msg.sbValue.SetData(sbFile.Data(), sbFile.Length());
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_GetBinary msg;
    msg.sName = "test";
    msg.sKey = "test-bulk";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) {
      int bEqual = (msg.sbValue.Length() == sbFile.Length());
      if (bEqual) { bEqual = !memcmp(sbFile.Data(), msg.sbValue.Data(), sbFile.Length()); }
      if (!bEqual) {
        s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey));
      }
    }
  }
  // Update bulk binary
  if (!s) {
    Msg_DB_SetBinary msg;
    msg.sName = "test";
    msg.sKey = "test-bulk";
    msg.sbValue.SetData(sbFile2.Data(), sbFile2.Length());
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  if (!s) {
    Msg_DB_GetBinary msg;
    msg.sName = "test";
    msg.sKey = "test-bulk";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) {
      int bEqual = (msg.sbValue.Length() == sbFile2.Length());
      if (bEqual) { bEqual = !memcmp(sbFile2.Data(), msg.sbValue.Data(), sbFile2.Length()); }
      if (!bEqual) {
        s.appendf("%s %s %s wrong value", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey));
      }
    }
  }
  // Delete bulk
  if (!s) {
    Msg_DB_Delete msg;
    msg.sName = "test";
    msg.sKey = "test-bulk";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }

  { Msg_DB_Close msg; msg.sName = "test"; msg.Request(); }
//  { Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }

  return s;
}

static String Test_Db_HasKey()
{
  String s;

  int bSyncToDisk = 1;

  //{ Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }
  { Msg_DB_Open msg; msg.sName = "test"; msg.Request(); }

  // String
  if (!s) {
    Msg_DB_Set msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    msg.sValue = "Hello World";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  // Check available
  if (!s) {
    Msg_DB_HasKey msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) { if (msg.bAvailable != 1) { s.appendf("%s %s %s expected available", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }
  // Delete string
  if (!s) {
    Msg_DB_Delete msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (bSyncToDisk) { Msg_DB_Sync msg; msg.sName = "test"; msg.Request(); }
  }
  // Check available
  if (!s) {
    Msg_DB_HasKey msg;
    msg.sName = "test";
    msg.sKey = "test-string";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) { if (msg.bAvailable != 0) { s.appendf("%s %s %s expected unavailable", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }

  // Check availablity of not existing key
  if (!s) {
    Msg_DB_HasKey msg;
    msg.sName = "test";
    msg.sKey = "test-string-unavailable";
    if (!msg.Request()) { s.appendf("%s %s %s failed", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); }
    if (!s) { if (msg.bAvailable != 0) { s.appendf("%s %s %s expected unavailable", _sz(msg.getName()), _sz(msg.sName), _sz(msg.sKey)); } }
  }

  { Msg_DB_Close msg; msg.sName = "test"; msg.Request(); }
//  { Msg_DB_DeleteFile msg; msg.sName = "test"; msg.Request(); }

  return s;
}

//--------------------------

void Test_Db_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Finishing Test/Db"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

AP_MSG_HANDLER_METHOD(DbModule, UnitTest_Token)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(DbModule, UnitTest_Token), this); }
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Starting Test/Db"));
  int bTokenEndNow = 1;

  if (Apollo::getConfig("Test/Db", 0)) {
    AP_UNITTEST_EXECUTE(Test_Db_String);
    AP_UNITTEST_EXECUTE(Test_Db_Binary);
    AP_UNITTEST_EXECUTE(Test_Db_Bulk);
    AP_UNITTEST_EXECUTE(Test_Db_UpdateString);
    AP_UNITTEST_EXECUTE(Test_Db_UpdateBulk);
    AP_UNITTEST_EXECUTE(Test_Db_HasKey);
  }

  if (bTokenEndNow) { Test_Db_UnitTest_TokenEnd(); }
}

AP_MSG_HANDLER_METHOD(DbModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Db", 0)) {
    AP_UNITTEST_REGISTER(Test_Db_String);
    AP_UNITTEST_REGISTER(Test_Db_Binary);
    AP_UNITTEST_REGISTER(Test_Db_Bulk);
    AP_UNITTEST_REGISTER(Test_Db_UpdateString);
    AP_UNITTEST_REGISTER(Test_Db_UpdateBulk);
    AP_UNITTEST_REGISTER(Test_Db_HasKey);

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(DbModule, UnitTest_Token), this, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
  }
}

AP_MSG_HANDLER_METHOD(DbModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Db", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(DbModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int DbModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_Open, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_Close, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_Set, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_SetBinary, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_HasKey, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_Get, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_GetBinary, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_GetKeys, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_Delete, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_DeleteFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_Clear, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_Expire, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_DeleteOlderThan, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, DB_Sync, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, System_10SecTimer, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, System_60SecTimer, this, ApCallbackPosNormal);

#if defined(AP_TEST)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, UnitTest_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, UnitTest_Execute, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DbModule, UnitTest_End, this, ApCallbackPosNormal);
#endif

  return ok;
}

void DbModule::exit()
{
  AP_MSG_REGISTRY_FINISH;
}
