// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgDB.h"
#include "Local.h"
#include "ConfigModule.h"

int ConfigModule::setValue(ConfigPlane* pPlane, const String& sKey, const String& sValue)
{
  if (sKey == "Translation/Language") {
    StringTreeNode<String>* pNode = pPlane->stData_.Find(sKey);
    if (pNode != 0) {
      String sPreviousValue = pNode->Value();
      int AP_UNUSED_VARIABLE x = 1;
    }
  }

  if (pPlane) {
    String sTrimmed = sKey;
    sTrimmed.trimWSP();
    if (!sTrimmed.empty()) {
      if (sKey.startsWith("#")) {
        apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "%s:%s=%s", _sz(pPlane->getName()), _sz(sKey), _sz(sValue)));
      } else {
        apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "%s:%s=%s", _sz(pPlane->getName()), _sz(sKey), _sz(sValue)));
      }
      return pPlane->stData_.Set(sKey, sValue);
    }
  }

  return 0;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_SetValue)
{
  int ok = 0;

  if (pMsg->sPath == "Dialog/Theme") {
    int AP_UNUSED_VARIABLE x = 1;
  }

  ConfigPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
  if (pPlane == 0) {
    pPlane = new ConfigPlane(sCurrentPlane_);
    if (pPlane != 0) {
      lPlanes_.Add(pPlane);
    }
  }
  if (pPlane != 0) {
    ok = setValue(pPlane, pMsg->sPath, pMsg->sValue);

    int bSkipDB = 0;
    if (pPlane->getName().startsWith("_")) { bSkipDB = 1; }

    if (bUseDb_ && !bSkipDB) {
      if (ok) {
        if (pPlane->sDbName_) {
          Msg_DB_Set msg;
          msg.sName = pPlane->sDbName_;
          msg.sKey = pMsg->sPath;
          msg.sValue = pMsg->sValue;
          ok = msg.Request();
          if (!ok) {
            apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Set failed name=%s key=%s", _sz(msg.sName), _sz(msg.sKey)));
          }
        }
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_GetValue)
{
  int bHandled = 0;

  if (pMsg->sPath == "System/Debug") {
    int AP_UNUSED_VARIABLE x = 1;
  }

  if (!bHandled) {
    ConfigPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
    if (pPlane != 0) {
      StringTreeNode<String>* pNode = pPlane->stData_.Find(pMsg->sPath);
      if (pNode != 0) {
        pMsg->sValue = pNode->Value();
        bHandled = 1;
      }
    }
  }

  // Pseudo config values:
  if (!bHandled) {
    if (pMsg->sPath == "System/Debug") {
#if defined(_DEBUG)
      pMsg->sValue = "1";
#else
      pMsg->sValue = "0";
#endif
      bHandled = 1;
    }
  }

  if (bHandled) {
    pMsg->apStatus = ApMessage::Ok;
  } else {
    pMsg->apStatus = ApMessage::Error;
  }
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_DeleteValue)
{
  ConfigPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
  List lKeys;
  if (pPlane != 0) {
    StringTreeNode<String>* pNode = 0;
    for (StringTreeIterator<String> iter(pPlane->stData_); (pNode = iter.Next()); ) {
      if (pNode->Key().startsWith(pMsg->sPath)) {
        lKeys.Add(pNode->Key());
      }
    }
    for (Elem* pKey = NULL; (pKey = lKeys.Next(pKey)) != NULL; ){
      pPlane->stData_.Unset(pKey->getName());

      if (bUseDb_) {
        if (pPlane->sDbName_) {
          Msg_DB_Delete msg;
          msg.sName = pPlane->sDbName_;
          msg.sKey = pKey->getName();
          if (!msg.Request()) {
            apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Set failed name=%s key=%s", _sz(msg.sName), _sz(msg.sKey)));
          }
        }
      }
    }
  }  

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_Clear)
{
  ConfigPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
  if (pPlane != 0) {
    lPlanes_.Remove(pPlane);
    delete pPlane;
    pPlane = 0;
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_Load)
{
  int ok = 0;

  if (pMsg->sName.empty()) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Filename empty"));
  } else {

    ConfigPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
    if (pPlane == 0) {
      pPlane = new ConfigPlane(sCurrentPlane_);
      if (pPlane != 0) {
        lPlanes_.Add(pPlane);
      }
    }

    if (pPlane != 0) {

      // Load from file
      String sDir = pMsg->sDir;
      sDir.makeTrailingSlash();

      // Main config file
      {
        String sFile = sDir;
        sFile += pMsg->sName;
        sFile += ".txt";

        String sData;
        if (!Apollo::loadFile(sFile, sData)) {
          apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "fConfig.Load(%s)", _sz(sFile)));
        } else {
          ok = 1;
          pPlane->vlPathNames_.add(sFile);

          List lParams;
          KeyValueLfBlob2List(sData, lParams);
          for (Elem* e = 0; (e = lParams.Next(e)) != 0; ) {
            (void) setValue(pPlane, e->getName(), e->getString());
          }
        }
      }

      // Local config file
      {
        String sFile = sDir;
        sFile += pMsg->sName;
        sFile += ".local.txt";

        String sData;
        if (!Apollo::loadFile(sFile, sData)) {
          apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "fConfig.Load failed(%s)", _sz(sFile)));
        } else {
          ok = 1;
          pPlane->vlPathNames_.add(sFile);

          List lParams;
          KeyValueLfBlob2List(sData, lParams);
          for (Elem* e = 0; (e = lParams.Next(e)) != 0; ) {
            (void) setValue(pPlane, e->getName(), e->getString());
          }
        }
      }

      // Load from DB
      if (bUseDb_) {
        String sDbName = Apollo::getConfig("Db", pMsg->sName);
        if (sDbName) {
          if (Apollo::isLoadedModule("Db")) {

            // Open db and load if any stored
            Msg_DB_Open msgDO;
            msgDO.sName = sDbName;
            if (!msgDO.Request()) {
              apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Open %s failed", _sz(msgDO.sName)));
            } else {

              Msg_DB_GetKeys msgDGK;
              msgDGK.sName = sDbName;
              if (!msgDGK.Request()) {
                apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_GetKeys %s failed", _sz(msgDGK.sName)));
              } else {

                for (Apollo::ValueElem* e = 0; (e = msgDGK.vlKeys.nextElem(e)) != 0; ) {
                  Msg_DB_Get msgDG;
                  msgDG.sName = sDbName;
                  msgDG.sKey = e->getString();
                  if (!msgDG.Request()) {
                    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Get %s failed key=%s", _sz(msgDG.sName), _sz(msgDG.sKey)));
                  } else {
                    (void) setValue(pPlane, e->getString(), msgDG.sValue);
                  }
                } // for keys

                // DB name changed by DB data?
                pPlane->sDbName_ = Apollo::getConfig("Db", sDbName);

              } // msgDGK
            } // msgDO
          } // isLoadedModule DB
        } // sDbName
      } // bUseDb_

    } // pPlane
  } // sName
  
  if (ok) { Msg_Config_Loaded msg; msg.Send(); }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_GetFileNames)
{
  int ok = 0;

  ConfigPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
  if (pPlane != 0) {
    pMsg->vlPathNames = pPlane->vlPathNames_;
    ok = 1;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_SetPlane)
{
  int ok = 1;

  sCurrentPlane_ = pMsg->sPlane;

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_GetPlane)
{
  int ok = 1;

  pMsg->sPlane = sCurrentPlane_;

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_DeletePlane)
{
  int ok = 1;

  ConfigPlane* pPlane = lPlanes_.FindByName(pMsg->sPlane);
  if (pPlane != 0) {

    if (bUseDb_) {
      if (pPlane->sDbName_) {
        Msg_DB_Close msg;
        msg.sName = pPlane->sDbName_;
        if (!msg.Request()) {
          ok = 0;
          apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Msg_DB_Close %s failed", _sz(msg.sName)));
        }
      }
    }

    lPlanes_.Remove(pPlane);
    delete pPlane;
    pPlane = 0;
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

AP_MSG_HANDLER_METHOD(ConfigModule, Config_GetKeys)
{
  int ok = 1;

  ConfigPlane* pPlane = lPlanes_.FindByName(sCurrentPlane_);
  if (pPlane != 0) {
    StringTreeNode<String>* pNode = 0;
    for (StringTreeIterator<String> iter(pPlane->stData_); (pNode = iter.Next()); ) {
      if (pNode->Key().startsWith(pMsg->sPath)) {
        String sPath = pNode->Key().subString(pMsg->sPath.bytes());
        if (!sPath.empty()) {
          sPath.trim("/");
          String sKey;
          sPath.nextToken("/", sKey);
          pMsg->vlKeys.add(sKey);
        }
      }
    }
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

#if defined(AP_TEST)

int Test_Config_SetGet()
{
  int ok = 1;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }

  Apollo::setConfig("Test_Config_SetGet/Int1", 42);
  int n1 = Apollo::getConfig("Test_Config_SetGet/Int1", 43);
  if (n1 != 42) ok = 0;
  int n2 = Apollo::getConfig("Test_Config_SetGet/Int2", 44);
  if (n2 != 44) ok = 0;

  Apollo::setConfig("Test_Config_SetGet/String1", "A");
  String s1 = Apollo::getConfig("Test_Config_SetGet/String1", "B");
  if (s1 != "A") ok = 0;
  String s2 = Apollo::getConfig("Test_Config_SetGet/String2", "C");
  if (s2 != "C") ok = 0;

  { Msg_Config_Clear msg; msg.Request(); }
  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }
  return ok;
}

int Test_Config_Load()
{
  int ok = 1;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }
  {
    Msg_Config_Load msg;
    msg.sDir = Apollo::getModuleResourcePath("Test");
    msg.sName = "testConfig";
    msg.Request();
  }

  { int n = Apollo::getConfig("Test_Config_Load/not-available", 42); if (n != 42) ok = 0; }
  { int n = Apollo::getConfig("Test_Config_Load/String", 43); if (n != 0) ok = 0; }
  { int n = Apollo::getConfig("Test_Config_Load/EmptyString", 44); if (n != 44) ok = 0; }
  { int n = Apollo::getConfig("Test_Config_Load/Int", 45); if (n != 5) ok = 0; }
  { int n = Apollo::getConfig("Test_Config_Load/Int0", 46); if (n != 0) ok = 0; }

  { String s = Apollo::getConfig("Test_Config_Load/not-available", "s42"); if (s != "s42") ok = 0; }
  { String s = Apollo::getConfig("Test_Config_Load/String", "s43"); if (s != "abc") ok = 0; }
  { String s = Apollo::getConfig("Test_Config_Load/EmptyString", "s44"); if (s != "") ok = 0; }
  { String s = Apollo::getConfig("Test_Config_Load/Int", "s45"); if (s != "5") ok = 0; }
  { String s = Apollo::getConfig("Test_Config_Load/Int0", "s46"); if (s != "0") ok = 0; }

  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test/List/A/a", vlKeys); if (vlKeys.length() != 0) { ok = 0; } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test/List/A", vlKeys); if (vlKeys.length() != 1) { ok = 0; } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test/List", vlKeys); if (vlKeys.length() != 3) { ok = 0; } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test/List/C/c", vlKeys); if (vlKeys.length() != 0) { ok = 0; } }

  {
    Apollo::ValueList vlKeys;
    Apollo::getConfigKeys("Test/List", vlKeys);
    Apollo::ValueElem* e = 0;
    e = vlKeys.nextElem(e); if (!e || e->getString() != "A") { ok = 0; }
    e = vlKeys.nextElem(e); if (!e || e->getString() != "B") { ok = 0; }
    e = vlKeys.nextElem(e); if (!e || e->getString() != "C") { ok = 0; }
  }

  { Msg_Config_Clear msg; msg.Request(); }
  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }
  return ok;
}

String Test_Config_DBOverride()
{
  String s;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }
  {
    Msg_Config_Load msg;
    msg.sDir = Apollo::getModuleResourcePath("Test");
    msg.sName = "testConfig";
    msg.Request();
  }

  // Check values from DB
  { int n = Apollo::getConfig("Test_Config_DB/not-available", 42); if (n != 42) { s.from(__LINE__); } }
  { int n = Apollo::getConfig("Test_Config_DB/String", 43); if (n != 0) { s.from(__LINE__); } }
  { int n = Apollo::getConfig("Test_Config_DB/EmptyString", 44); if (n != 44) { s.from(__LINE__); } }
  { int n = Apollo::getConfig("Test_Config_DB/Int", 45); if (n != 5) { s.from(__LINE__); } }
  { int n = Apollo::getConfig("Test_Config_DB/Int0", 46); if (n != 0) { s.from(__LINE__); } }

  { String s = Apollo::getConfig("Test_Config_DB/not-available", "s42"); if (s != "s42") { s.from(__LINE__); } }
  { String s = Apollo::getConfig("Test_Config_DB/String", "s43"); if (s != "def") { s.from(__LINE__); } }
  { String s = Apollo::getConfig("Test_Config_DB/EmptyString", "s44"); if (s != "") { s.from(__LINE__); } }
  { String s = Apollo::getConfig("Test_Config_DB/Int", "s45"); if (s != "5") { s.from(__LINE__); } }
  { String s = Apollo::getConfig("Test_Config_DB/Int0", "s46"); if (s != "0") { s.from(__LINE__); } }

  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test_Config_DB/List/D/d", vlKeys); if (vlKeys.length() != 0) { s.from(__LINE__); } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test_Config_DB/List/D", vlKeys); if (vlKeys.length() != 1) { s.from(__LINE__); } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test_Config_DB/List", vlKeys); if (vlKeys.length() != 3) { s.from(__LINE__); } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test_Config_DB/List/F/f", vlKeys); if (vlKeys.length() != 0) { s.from(__LINE__); } }

  {
    Apollo::ValueList vlKeys;
    Apollo::getConfigKeys("Test_Config_DB/List", vlKeys);
    Apollo::ValueElem* e = 0;
    e = vlKeys.nextElem(e); if (!e || e->getString() != "D") { s.from(__LINE__); }
    e = vlKeys.nextElem(e); if (!e || e->getString() != "E") { s.from(__LINE__); }
    e = vlKeys.nextElem(e); if (!e || e->getString() != "F") { s.from(__LINE__); }
  }

  // Check values from DB which override txt file
  { int n = Apollo::getConfig("Test_Config_DBOverride/String", 43); if (n != 0) { s.from(__LINE__); } }
  { String s = Apollo::getConfig("Test_Config_DBOverride/String", "s43"); if (s != "ghi") { s.from(__LINE__); } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test_Config_DBOverride/List/G/g", vlKeys); if (vlKeys.length() != 0) { s.from(__LINE__); } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test_Config_DBOverride/List/G", vlKeys); if (vlKeys.length() != 1) { s.from(__LINE__); } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test_Config_DBOverride/List", vlKeys); if (vlKeys.length() != 3) { s.from(__LINE__); } }
  { Apollo::ValueList vlKeys; Apollo::getConfigKeys("Test_Config_DBOverride/List/I/i", vlKeys); if (vlKeys.length() != 0) { s.from(__LINE__); } }
  {
    Apollo::ValueList vlKeys;
    Apollo::getConfigKeys("Test_Config_DBOverride/List", vlKeys);
    Apollo::ValueElem* e = 0;
    e = vlKeys.nextElem(e); if (e->getString() != "G") { s.from(__LINE__); }
    e = vlKeys.nextElem(e); if (e->getString() != "H") { s.from(__LINE__); }
    e = vlKeys.nextElem(e); if (e->getString() != "I") { s.from(__LINE__); }
  }

  { Msg_Config_Clear msg; msg.Request(); }
  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }

  return s;
}

int Test_Config_Delete()
{
  int ok = 1;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }

  Apollo::setConfig("Test_Config_Delete/root", 42);
  Apollo::setConfig("Test_Config_Delete/keep", 43);
  Apollo::setConfig("Test_Config_Delete/root/delete/this", 44);
  Apollo::setConfig("Test_Config_Delete/root/delete/me", 45);
  Apollo::setConfig("Test_Config_Delete/root/and_me", 46);

  Apollo::deleteConfig("Test_Config_Delete/root");

  {
    Apollo::KeyValueList kvKeys;
    Apollo::getConfigKeys("Test_Config_Delete", kvKeys);
    if (ok) { ok = (kvKeys.findString("keep") != NULL); }
    if (ok) { ok = (kvKeys.findString("root") == NULL); }
    if (ok) { ok = (kvKeys.length() == 1); }
  }

  {
    Apollo::KeyValueList kvKeys;
    Apollo::getConfigKeys("Test_Config_Delete/root", kvKeys);
    if (ok) { ok = (kvKeys.findString("delete") == NULL); }
    if (ok) { ok = (kvKeys.findString("and_me") == NULL); }
    if (ok) { ok = (kvKeys.length() == 0); }
  }

  {
    Apollo::KeyValueList kvKeys;
    Apollo::getConfigKeys("Test_Config_Delete/root/delete", kvKeys);
    if (ok) { ok = (kvKeys.findString("this") == NULL); }
    if (ok) { ok = (kvKeys.findString("me") == NULL); }
    if (ok) { ok = (kvKeys.length() == 0); } 
  }

  { Msg_Config_Clear msg; msg.Request(); }
  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }

  return ok;
}

String Test_Config_Persist()
{
  String s;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = "testPersist"; msg.Request(); }

  {
    Msg_Config_Load msg;
    msg.sDir = Apollo::getModuleResourcePath("Test");
    msg.sName = "testPersist";
    msg.Request();
  }

  if (!s) { String sValue = Apollo::getConfig("Test_Config_Persist/String", "default"); if (sValue != "A") { s = String::from(__LINE__); }; }

  Apollo::setConfig("Test_Config_Persist/String", "B");
  Apollo::setConfig("Test_Config_Persist/EmptyString", "");
  Apollo::setConfig("Test_Config_Persist/Int", 5);
  Apollo::setConfig("Test_Config_Persist/Int0", 0);
  Apollo::setConfig("Test_Config_Persist/DeleteMe", "C");

  if (!s) { String sValue = Apollo::getConfig("Test_Config_Persist/String", "default"); if (sValue != "B") { s = String::from(__LINE__); }; }
  if (!s) { String sValue = Apollo::getConfig("Test_Config_Persist/DeleteMe", "default"); if (sValue != "C") { s = String::from(__LINE__); }; }

  Apollo::deleteConfig("Test_Config_Persist/DeleteMe");
  Apollo::deleteConfig("Test_Config_Persist/NotExists");

  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }
  { Msg_Config_DeletePlane msg; msg.sPlane = "test"; msg.Request(); }

  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }

  {
    Msg_Config_Load msg;
    msg.sDir = Apollo::getModuleResourcePath("Test");
    msg.sName = "testPersist";
    msg.Request();
  }

  if (!s) { String sValue = Apollo::getConfig("Test_Config_Persist/String", "default"); if (sValue != "B") { s = "Did not get the value written before"; }; }
  if (!s) { String sValue = Apollo::getConfig("Test_Config_Persist/EmptyString", "default"); if (sValue != "") { s = String::from(__LINE__); }; }
  if (!s) { int nValue = Apollo::getConfig("Test_Config_Persist/Int", 42); if (nValue != 5) { s = String::from(__LINE__); }; }
  if (!s) { int nValue = Apollo::getConfig("Test_Config_Persist/Int0", 43); if (nValue != 0) { s = String::from(__LINE__); }; }
  if (!s) { String sValue = Apollo::getConfig("Test_Config_Persist/DeleteMe", "default"); if (sValue != "default") { s = String::from(__LINE__); }; }

  { Msg_DB_DeleteFile msg; msg.sName = "testPersist"; msg.Request(); } // Before reset plane, because of Db/BaseFolder
  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }
  { Msg_Config_DeletePlane msg; msg.sPlane = "test"; msg.Request(); }

  return s;
}

//--------------------------

void Test_Config_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Finishing Test/Config"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

AP_MSG_HANDLER_METHOD(ConfigModule, UnitTest_Token)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ConfigModule, UnitTest_Token), this); }
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "Starting Test/Config"));
  int bTokenEndNow = 1;

  if (Apollo::getConfig("Test/Config", 0)) {
    AP_UNITTEST_EXECUTE1(Test_Config_SetGet);
    AP_UNITTEST_EXECUTE1(Test_Config_Load);
    AP_UNITTEST_EXECUTE1(Test_Config_Delete);
    if (bUseDb_) {
      AP_UNITTEST_EXECUTE(Test_Config_DBOverride);
      AP_UNITTEST_EXECUTE(Test_Config_Persist);
    }

  }

  if (bTokenEndNow) { Test_Config_UnitTest_TokenEnd(); }
}

AP_MSG_HANDLER_METHOD(ConfigModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Config", 0)) {
    AP_UNITTEST_REGISTER(Test_Config_SetGet);
    AP_UNITTEST_REGISTER(Test_Config_Load);
    AP_UNITTEST_REGISTER(Test_Config_Delete);
    if (bUseDb_) {
      AP_UNITTEST_REGISTER(Test_Config_DBOverride);
      AP_UNITTEST_REGISTER(Test_Config_Persist);
    }

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ConfigModule, UnitTest_Token), this, Apollo::modulePos(ApCallbackPosNormal, MODULE_NAME)); }
  }
}

AP_MSG_HANDLER_METHOD(ConfigModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Config", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(ConfigModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int ConfigModule::init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_SetValue, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_GetValue, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_DeleteValue, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_Clear, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_Load, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_GetFileNames, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_SetPlane, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_GetPlane, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_GetKeys, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, Config_DeletePlane, this, ApCallbackPosNormal);

#if defined(AP_TEST)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, UnitTest_Begin, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, UnitTest_Execute, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, ConfigModule, UnitTest_End, this, ApCallbackPosNormal);
#endif

  return ok;
}

void ConfigModule::exit()
{
  AP_MSG_REGISTRY_FINISH;
}
