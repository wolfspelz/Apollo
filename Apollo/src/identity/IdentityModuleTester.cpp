// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgDB.h"
#include "IdentityModule.h"
#include "IdentityModuleTester.h"
#include "Local.h"
#include "MsgNet.h"

#if defined(AP_TEST)

static int g_bTest_Online = 0;

static void Test_Identity_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Identity_UnitTest_TokenEnd", "Finished Test/Identity"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

static void Test_Identity_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Identity_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Identity_UnitTest_Token", "Starting Test/Identity"));
  int bTokenEndNow = 1;

  AP_UNITTEST_EXECUTE(IdentityModuleTester::test_CacheBasic);
  AP_UNITTEST_EXECUTE(IdentityModuleTester::test_CacheStorage);
  AP_UNITTEST_EXECUTE(IdentityModuleTester::test_GetProperty);
  AP_UNITTEST_EXECUTE(IdentityModuleTester::test_GetItem);
  AP_UNITTEST_EXECUTE(IdentityModuleTester::test_GetItemData);
  AP_UNITTEST_EXECUTE(IdentityModuleTester::test_Change);
  AP_UNITTEST_EXECUTE(IdentityModuleTester::test_selectItemId);

  if (g_bTest_Online) {
    bTokenEndNow = 0;
    AP_UNITTEST_EXECUTE(IdentityModuleTester::test_RequestContainer_Start);
  }

  if (bTokenEndNow) { Test_Identity_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------
#define Test_Identity_DB "IdentityTest"
#define Test_Identity_Basic_URL "http://ydentiti.org/test/Planta/identity.xml"
#define Test_Identity_Basic_Digest "1"
#define Test_Identity_Basic_Data \
"<?xml version='1.0' encoding='UTF-8'?>\n" \
"<identity xmlns='http://schema.bluehands.de/digest-container' digest='1'>\n" \
"<item id='properties' contenttype='properties' digest='1' encoding='plain' mimetype='text/plain' order='1'><![CDATA[Nickname=Planta Veloci" "\xC3\xA4" "\n" \
"ProfilePage=http://wolfspelz.de\n" \
"NicknameLink=http://www.virtual-presence.org\n" \
"]]></item>\n" \
"<item id='avatar' contenttype='avatar' digest='1' src='http://ydentiti.org/test/Planta/still.gif' order='1' size='1961' />\n" \
"</identity>\n"
//"<item id='avatar2' contenttype='avatar2' digest='1' src='http://ydentiti.org/test/Planta/avatar.xml' mimetype='avatar/gif' order='1'/>\n" \

String IdentityModuleTester::test_CacheBasic()
{
  String s;

  String sOrigDb;
  { Msg_Identity_SetStorageName msg; msg.sName = Test_Identity_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }

  if (!s) {
    Msg_Identity_IsContainerAvailable msg;
    msg.sUrl = Test_Identity_Basic_URL;
    msg.sDigest = Test_Identity_Basic_Digest;
    if (!msg.Request()) {
      s = "Msg_Identity_IsContainerAvailable failed";
    } else {
      if (msg.bAvailable) {
        s.appendf("%d: bAvailable=%d, expected bAvailable=0", __LINE__, msg.bAvailable);
      }
    }
  }

  if (!s) {
    Msg_Identity_SetContainer msg;
    msg.sUrl = Test_Identity_Basic_URL;
    msg.sDigest = Test_Identity_Basic_Digest;
    msg.sData = Test_Identity_Basic_Data;
    if (!msg.Request()) {
      s = "Msg_Identity_SetContainer failed";
    } else {
      Container* pContainer = 0;
      if (!IdentityModuleInstance::Get()->data_.Get(Test_Identity_Basic_URL, pContainer)) {
        s = "missing data in cache";
      } else {
        if (pContainer->getDigest() != Test_Identity_Basic_Digest) {
          s = "wrong digest";
        }
      }
    }
  }

  if (!s) {
    Msg_Identity_IsContainerAvailable msg;
    msg.sUrl = Test_Identity_Basic_URL;
    msg.sDigest = Test_Identity_Basic_Digest;
    if (!msg.Request()) {
      s = "Msg_Identity_IsContainerAvailable failed";
    } else {
      if (!msg.bAvailable) {
        s.appendf("%d: bAvailable=%d, expected bAvailable=1", __LINE__, msg.bAvailable);
      }
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Identity_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Identity_DB; (void) msg.Request(); }

  return s;
}

//----------------------

#define Test_Identity_CacheStorage_URL Test_Identity_Basic_URL
#define Test_Identity_CacheStorage_Digest Test_Identity_Basic_Digest
#define Test_Identity_CacheStorage_Data Test_Identity_Basic_Data

String IdentityModuleTester::test_CacheStorage()
{
  String s;

  String sOrigDb;
  { Msg_Identity_SetStorageName msg; msg.sName = Test_Identity_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }

  if (!s) {
    Msg_Identity_SetContainer msg;
    msg.sUrl = Test_Identity_CacheStorage_URL;
    msg.sDigest = Test_Identity_CacheStorage_Digest;
    msg.sData = Test_Identity_CacheStorage_Data;
    if (!msg.Request()) {
      s = "Msg_Identity_SetContainer failed";
    } else {
      Container* pContainer = 0;
      if (!IdentityModuleInstance::Get()->data_.Get(Test_Identity_CacheStorage_URL, pContainer)) {
        s = "missing data in cache";
      }
    }
  }

  if (!s) {
    Msg_Identity_SaveContainerToStorage msg;
    msg.sUrl = Test_Identity_CacheStorage_URL;
    if (!msg.Request()) {
      s = "Msg_Identity_SaveContainerToStorage failed";
    }
  }

  { Msg_DB_Sync msg; msg.sName = IdentityModuleInstance::Get()->sDb_; msg.Request(); }
  
  if (!s) {
    Msg_Identity_DeleteContainerFromMemory msg;
    msg.sUrl = Test_Identity_CacheStorage_URL;
    if (!msg.Request()) {
      s = "Msg_Identity_DeleteContainerFromMemory failed";
    } else {
      Container* pContainer = 0;
      if (IdentityModuleInstance::Get()->data_.Get(Test_Identity_CacheStorage_URL, pContainer)) {
        s = "data is in cache, expected not to be in cache";
      }
    }
  }

  if (!s) {
    Msg_Identity_LoadContainerFromStorage msg;
    msg.sUrl = Test_Identity_CacheStorage_URL;
    if (!msg.Request()) {
      s = "Msg_Identity_LoadContainerFromStorage failed";
    } else {
      Container* pContainer = 0;
      if (!IdentityModuleInstance::Get()->data_.Get(Test_Identity_CacheStorage_URL, pContainer)) {
        s = "missing data in cache";
      } else {
        if (pContainer->getDigest() != Test_Identity_CacheStorage_Digest) {
          s = "wrong digest";
        } else {
          if (pContainer->getUrl() != Test_Identity_CacheStorage_URL) {
            s = "wrong src";
          } else {
            if (!pContainer->isValid()) {
              s = "data invalid";
            }
          }
        }
      }
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }
  
  { Msg_DB_Sync msg; msg.sName = IdentityModuleInstance::Get()->sDb_; msg.Request(); }

  if (!s) {
    Msg_Identity_LoadContainerFromStorage msg;
    msg.sUrl = Test_Identity_CacheStorage_URL;
    if (msg.Request()) {
      s = "Msg_Identity_LoadContainerFromStorage succeeded unexpectedly";
    }
  }

  { Msg_Identity_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Identity_DB; (void) msg.Request(); }

  return s;
}

//----------------------

#define Test_Identity_GetProperty_URL Test_Identity_Basic_URL
#define Test_Identity_GetProperty_Digest Test_Identity_Basic_Digest
#define Test_Identity_GetProperty_Data Test_Identity_Basic_Data
#define Test_Identity_GetProperty_Key "Nickname"
#define Test_Identity_GetProperty_Value "Planta Veloci" "\xC3\xA4"

String IdentityModuleTester::test_GetProperty()
{
  String s;

  String sOrigDb;
  { Msg_Identity_SetStorageName msg; msg.sName = Test_Identity_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }

  if (!s) {
    Msg_Identity_SetContainer msg;
    msg.sUrl = Test_Identity_GetProperty_URL;
    msg.sDigest = Test_Identity_GetProperty_Digest;
    msg.sData = Test_Identity_GetProperty_Data;
    if (!msg.Request()) {
      s = "Msg_Identity_SetContainer failed";
    } else {
      Container* pContainer = 0;
      if (!IdentityModuleInstance::Get()->data_.Get(Test_Identity_GetProperty_URL, pContainer)) {
        s = "missing data in cache";
      } else {
        if (pContainer->getDigest() != Test_Identity_GetProperty_Digest) {
          s = "wrong digest";
        }
      }
    }
  }

  if (!s) {
    Msg_Identity_HasProperty msg;
    msg.sUrl = Test_Identity_GetProperty_URL;
    msg.sKey = Test_Identity_GetProperty_Key;
    if (!msg.Request()) {
      s = "Msg_Identity_HasProperty failed";
    } else {
      if (!msg.bIsProperty) {
        s.appendf("got=%d, expected=1", msg.bIsProperty);
      }
    }
  }

  if (!s) {
    Msg_Identity_HasProperty msg;
    msg.sUrl = Test_Identity_GetProperty_URL;
    msg.sKey = "no-property";
    if (!msg.Request()) {
      s = "Msg_Identity_HasProperty failed";
    } else {
      if (msg.bIsProperty) {
        s.appendf("got=%d, expected=0", msg.bIsProperty);
      }
    }
  }

  if (!s) {
    Msg_Identity_GetProperty msg;
    msg.sUrl = Test_Identity_GetProperty_URL;
    msg.sKey = Test_Identity_GetProperty_Key;
    if (!msg.Request()) {
      s = "Msg_Identity_GetProperty failed";
    } else {
      if (msg.sValue != Test_Identity_GetProperty_Value) {
        s.appendf("got %s=%s, expected=%s", StringType(Test_Identity_GetProperty_Key), StringType(msg.sValue), StringType(Test_Identity_GetProperty_Value));
      }
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Identity_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Identity_DB; (void) msg.Request(); }

  return s;
}

//----------------------

#define Test_Identity_GetItem_URL Test_Identity_Basic_URL
#define Test_Identity_GetItem_Digest Test_Identity_Basic_Digest
#define Test_Identity_GetItem_Data Test_Identity_Basic_Data
#define Test_Identity_GetItem_ItemId "avatar"
#define Test_Identity_GetItem_ItemType "avatar"
#define Test_Identity_GetItem_ItemDigest "1"
#define Test_Identity_GetItem_ItemOrder 1
#define Test_Identity_GetItem_ItemSize 1961
#define Test_Identity_GetItem_ItemSrc "http://ydentiti.org/test/Planta/still.gif"
#define Test_Identity_GetItem_ItemMimeType "image/gif"

String IdentityModuleTester::test_GetItem()
{
  String s;

  String sOrigDb;
  { Msg_Identity_SetStorageName msg; msg.sName = Test_Identity_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }

  if (!s) {
    Msg_Identity_SetContainer msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sDigest = Test_Identity_GetItem_Digest;
    msg.sData = Test_Identity_GetItem_Data;
    if (!msg.Request()) {
      s = "Msg_Identity_SetContainer failed";
    } else {
      Container* pContainer = 0;
      if (!IdentityModuleInstance::Get()->data_.Get(Test_Identity_GetItem_URL, pContainer)) {
        s = "missing data in cache";
      } else {
        if (pContainer->getDigest() != Test_Identity_GetItem_Digest) {
          s = "wrong digest";
        }
      }
    }
  }

  if (!s) {
    Msg_Identity_SaveContainerToStorage msg;
    msg.sUrl = Test_Identity_CacheStorage_URL;
    if (!msg.Request()) {
      s = "Msg_Identity_SaveContainerToStorage failed";
    }
  }

  if (!s) {
    Msg_Identity_GetItemIds msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sType = "";
    if (!msg.Request()) {
      s = "Msg_Identity_GetItemIds failed";
    } else {
      ValueList vlExpected; vlExpected.add("avatar"); vlExpected.add("properties"); 
      s = Test_CompareLists("Msg_Identity_GetItemIds", msg.vlIds, vlExpected, Test_CompareLists_Flag_IgnoreOrder);
    }
  }

  if (!s) {
    Msg_Identity_GetItemIds msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sType = Test_Identity_GetItem_ItemType;
    if (!msg.Request()) {
      s = "Msg_Identity_GetItemIds with Type failed";
    } else {
      ValueList vlExpected; vlExpected.add("avatar"); 
      s = Test_CompareLists("Msg_Identity_GetItemIds with Type", msg.vlIds, vlExpected);
    }
  }

  if (!s) {
    Msg_Identity_GetItem msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s = "Msg_Identity_GetItem failed";
    } else {
      if (!s) { if (msg.sType != Test_Identity_GetItem_ItemType) { s.appendf("Msg_Identity_GetItem: got %s=%s", StringType("sType"), StringType(msg.sType)); } }
      if (!s) { if (msg.sDigest != Test_Identity_GetItem_ItemDigest) { s.appendf("Msg_Identity_GetItem: got %s=%s", StringType("sDigest"), StringType(msg.sDigest)); } }
      if (!s) { if (msg.nOrder != Test_Identity_GetItem_ItemOrder) { s.appendf("Msg_Identity_GetItem: got %s=%d", StringType("nOrder"), StringType(msg.nOrder)); } }
      if (!s) { if (msg.nSize != Test_Identity_GetItem_ItemSize) { s.appendf("Msg_Identity_GetItem: got %s=%d", StringType("nSize"), StringType(msg.nSize)); } }
      if (!s) { if (msg.sSrc != Test_Identity_GetItem_ItemSrc) { s.appendf("Msg_Identity_GetItem: got %s=%s", StringType("sSrc"), StringType(msg.sSrc)); } }
    }
  }

  if (!s) {
    Msg_Identity_IsItemDataAvailable msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s = "Msg_Identity_IsItemDataAvailable failed";
    } else {
      if (msg.bAvailable) {
        s.appendf("bAvailable=%d, expected bAvailable=0", msg.bAvailable);
      }
    }
  }

  Buffer sbImage;
  if (!Apollo::loadFile(Apollo::getAppResourcePath() + "test/test1.png", sbImage)) {
    s = "Apollo::loadFile(test/test1.png) failed";
  }

  if (!s) {
    Msg_Identity_SetItemData msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    msg.sMimeType = Test_Identity_GetItem_ItemMimeType;
    msg.sbData = sbImage;
    if (!msg.Request()) {
      s = "Msg_Identity_SetItemData failed";
    }
  }

  if (!s) {
    Msg_Identity_IsItemDataAvailable msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s.appendf("Msg_Identity_IsItemDataAvailable (%d) failed", __LINE__);
    } else {
      if (!msg.bAvailable) {
        s.appendf("%d: bAvailable=%d, expected bAvailable=1", __LINE__, msg.bAvailable);
      }
    }
  }

  if (!s) {
    Msg_Identity_GetItemData msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s = "Msg_Identity_GetItemData failed";
    } else {
      if (msg.sMimeType != Test_Identity_GetItem_ItemMimeType) {
        s.appendf("got mimetype=%s expected=%s", StringType(msg.sMimeType), StringType(Test_Identity_GetItem_ItemMimeType));
      } else {
        if (msg.sbData.Length() != sbImage.Length()) {
          s.appendf("got data length=%d expected=%d", msg.sbData.Length(), sbImage.Length());
        }
      }
    }
  }

  if (!s) {
    Msg_Identity_SaveItemDataToStorage msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s = "Msg_Identity_SaveItemDataToStorage failed";
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }

  if (!s) {
    Msg_Identity_IsItemDataAvailable msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s.appendf("Msg_Identity_IsItemDataAvailable (%d) failed", __LINE__);
    } else {
      if (!msg.bAvailable) {
        s.appendf("(%d) bAvailable=%d, expected bAvailable=1", __LINE__, msg.bAvailable);
      }
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }

  if (!s) {
    Msg_Identity_GetItemData msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s.appendf("Msg_Identity_GetItemData (%d) failed", __LINE__);
    } else {
      if (msg.sMimeType != Test_Identity_GetItem_ItemMimeType) {
        s.appendf("(%d) got mimetype=%s expected=%s", __LINE__, StringType(msg.sMimeType), StringType(Test_Identity_GetItem_ItemMimeType));
      } else {
        if (msg.sbData.Length() != sbImage.Length()) {
          s.appendf("(%d) got data length=%d expected=%d", __LINE__, msg.sbData.Length(), sbImage.Length());
        }
      }
    }
  }

  if (!s) {
    Msg_Identity_DeleteItemData msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s = "Msg_Identity_DeleteItemData failed";
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }

  if (!s) {
    Msg_Identity_IsItemDataAvailable msg;
    msg.sUrl = Test_Identity_GetItem_URL;
    msg.sId = Test_Identity_GetItem_ItemId;
    if (!msg.Request()) {
      s.appendf("Msg_Identity_IsItemDataAvailable (%d) failed", __LINE__);
    } else {
      if (msg.bAvailable) {
        s.appendf("(%d) bAvailable=%d, expected bAvailable=0", __LINE__, msg.bAvailable);
      }
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Identity_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Identity_DB; (void) msg.Request(); }

  return s;
}

//----------------------

#define Test_Identity_GetItemData_URL Test_Identity_Basic_URL
#define Test_Identity_GetItemData_Digest Test_Identity_Basic_Digest
#define Test_Identity_GetItemData_Data \
      "<?xml version='1.0' encoding='UTF-8'?>\n" \
      "<identity digest='1'>\n" \
      "<item id='inline_plain' contenttype='inline_plain' digest='1'>Hello World</item>\n" \
      "<item id='inline_base64' contenttype='inline_base64' digest='1' encoding='base64'>SGVsbG8gV29ybGQ=</item>\n" \
      "</identity>"
#define Test_Identity_GetItemData_ItemId_Plain "inline_plain"
#define Test_Identity_GetItemData_ItemId_Base64 "inline_base64"
#define Test_Identity_GetItemData_ItemData "Hello World"

String IdentityModuleTester::test_GetItemData()
{
  String s;

  String sOrigDb;
  { Msg_Identity_SetStorageName msg; msg.sName = Test_Identity_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }

  if (!s) {
    Msg_Identity_SetContainer msg;
    msg.sUrl = Test_Identity_GetItemData_URL;
    msg.sDigest = Test_Identity_GetItemData_Digest;
    msg.sData = Test_Identity_GetItemData_Data;
    if (!msg.Request()) {
      s = "Msg_Identity_SetContainer failed";
    } else {
      Container* pContainer = 0;
      if (!IdentityModuleInstance::Get()->data_.Get(Test_Identity_GetItemData_URL, pContainer)) {
        s = "missing data in cache";
      } else {
        if (pContainer->getDigest() != Test_Identity_GetItemData_Digest) {
          s = "wrong digest";
        }
      }
    }
  }

  if (!s) {
    Msg_Identity_GetItemData msg;
    msg.sUrl = Test_Identity_GetItemData_URL;
    msg.sId = Test_Identity_GetItemData_ItemId_Plain;
    if (!msg.Request()) {
      s = "Msg_Identity_GetItemData (Plain) failed";
    } else {
      String sData;
      msg.sbData.GetString(sData);
      if (sData != Test_Identity_GetItemData_ItemData) {
        s.appendf("(Plain) got=%s expected=%s", StringType(sData), StringType(Test_Identity_GetItemData_ItemData));
      }
    }
  }

  if (!s) {
    Msg_Identity_GetItemData msg;
    msg.sUrl = Test_Identity_GetItemData_URL;
    msg.sId = Test_Identity_GetItemData_ItemId_Base64;
    if (!msg.Request()) {
      s = "Msg_Identity_GetItemData (Base64) failed";
    } else {
      String sData;
      msg.sbData.GetString(sData);
      if (sData != Test_Identity_GetItemData_ItemData) {
        s.appendf("(Base64) got=%s expected=%s", StringType(sData), StringType(Test_Identity_GetItemData_ItemData));
      }
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Identity_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Identity_DB; (void) msg.Request(); }

  return s;
}

//----------------------

#define Test_Identity_Change_URL "http://ydentiti.org/test/Change/container1.xml"
#define Test_Identity_Change_ItemMimeType "image/png"
#define Test_Identity_Change_Digest1 "1"
#define Test_Identity_Change_Data1_item4_URL "http://ydentiti.org/test/Change/item4.gif"
#define Test_Identity_Change_Data1 \
      "<?xml version='1.0' encoding='UTF-8'?>\n" \
      "<identity digest='1'>\n" \
      "<item id='item1' contenttype='contenttype1' digest='digest1'>Content1</item>\n" \
      "<item id='item2' contenttype='contenttype2' digest='digest2'>Content2</item>\n" \
      "<item id='item3' contenttype='contenttype3' digest='digest3'>Content3</item>\n" \
      "<item id='item4' contenttype='contenttype4' digest='digest4' src='" Test_Identity_Change_Data1_item4_URL "'></item>\n" \
      "</identity>"

#define Test_Identity_Change_Digest2 "2"
#define Test_Identity_Change_Data1_item4a_URL "http://ydentiti.org/test/Change/item5.gif"
#define Test_Identity_Change_Data1_item5_URL "http://ydentiti.org/test/Change/item4a.gif"
#define Test_Identity_Change_Data2 \
      "<?xml version='1.0' encoding='UTF-8'?>\n" \
      "<identity digest='1'>\n" \
      "<item id='item6' contenttype='contenttype6' digest='digest6'>Content6</item>\n" \
      "<item id='item5' contenttype='contenttype5' digest='digest5' src='" Test_Identity_Change_Data1_item5_URL "'></item>\n" \
      "<item id='item4' contenttype='contenttype4' digest='digest4a' src='" Test_Identity_Change_Data1_item4a_URL "'></item>\n" \
      "<item id='item3' contenttype='contenttype3' digest='digest3a'>Content3a</item>\n" \
      "<item id='item2' contenttype='contenttype2' digest='digest2'>ContentZ</item>\n" \
      "</identity>"

int g_nTest_Identity_Change_ContainerBegin = 0;
int g_nTest_Identity_Change_ItemAdded = 0;
int g_nTest_Identity_Change_ItemChanged = 0;
int g_nTest_Identity_Change_ItemRemoved = 0;
int g_nTest_Identity_Change_ContainerEnd = 0;

int g_nTest_Identity_Change_Sequence1Ok = 0;
int g_nTest_Identity_Change_Sequence2Ok = 0;
int g_nTest_Identity_Change_SequenceNum = 0;
String g_sTest_Identity_Change_SequenceErr;

static int Test_Identity_Change_ContainerBegin(Msg_Identity_ContainerBegin* pMsg)
{
  if (pMsg->sUrl == Test_Identity_Change_URL) { 
    g_nTest_Identity_Change_ContainerBegin += 1;
  }
  return 1;
}
static int Test_Identity_Change_ItemAdded(Msg_Identity_ItemAdded* pMsg)
{
  if (pMsg->sUrl == Test_Identity_Change_URL) {
    g_nTest_Identity_Change_ItemAdded += 1;
  }
  return 1;
}
static int Test_Identity_Change_ItemChanged(Msg_Identity_ItemChanged* pMsg)
{
  if (pMsg->sUrl == Test_Identity_Change_URL)
  {
    g_nTest_Identity_Change_ItemChanged += 1;
  }
  return 1;
}
static int Test_Identity_Change_ItemRemoved(Msg_Identity_ItemRemoved* pMsg)
{
  if (pMsg->sUrl == Test_Identity_Change_URL) {
    g_nTest_Identity_Change_ItemRemoved += 1;
  }
  return 1;
}
static int Test_Identity_Change_ContainerEnd(Msg_Identity_ContainerEnd* pMsg)
{
  if (pMsg->sUrl == Test_Identity_Change_URL) {
    g_nTest_Identity_Change_ContainerEnd += 1;

    g_nTest_Identity_Change_SequenceNum++;

    if (g_nTest_Identity_Change_SequenceNum == 1) {
      if (0
        || g_nTest_Identity_Change_ContainerBegin != 1
        || g_nTest_Identity_Change_ItemAdded != 4
        || g_nTest_Identity_Change_ItemChanged != 0
        || g_nTest_Identity_Change_ItemRemoved != 0
        || g_nTest_Identity_Change_ContainerEnd != 1
        ) {
        g_sTest_Identity_Change_SequenceErr = "wrong callback count (1)";
      }
    }

    if (g_nTest_Identity_Change_SequenceNum == 2) {
      if (0
          || g_nTest_Identity_Change_ContainerBegin != 2
          || g_nTest_Identity_Change_ItemAdded != 6
          || g_nTest_Identity_Change_ItemChanged != 2
          || g_nTest_Identity_Change_ItemRemoved != 1
          || g_nTest_Identity_Change_ContainerEnd != 2
        ) {
        g_sTest_Identity_Change_SequenceErr = "wrong callback count (2)";
      }
    }

    if (g_nTest_Identity_Change_SequenceNum == 2) {
      AP_UNITTEST_RESULT(IdentityModuleTester::test_Change_Sequence, g_sTest_Identity_Change_SequenceErr.empty(), g_sTest_Identity_Change_SequenceErr);

      { Msg_Identity_ContainerBegin msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Identity_Change_ContainerBegin, 0); }
      { Msg_Identity_ItemAdded msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Identity_Change_ItemAdded, 0); }
      { Msg_Identity_ItemChanged msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Identity_Change_ItemChanged, 0); }
      { Msg_Identity_ItemRemoved msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Identity_Change_ItemRemoved, 0); }
      { Msg_Identity_ContainerEnd msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Identity_Change_ContainerEnd, 0); }
    }

  } // sUrl == Test_Identity_Change_URL

  return 1;
}

String IdentityModuleTester::test_Change()
{
  String s;

  String sOrigDb;
  { Msg_Identity_SetStorageName msg; msg.sName = Test_Identity_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }

  { Msg_Identity_ContainerBegin msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Identity_Change_ContainerBegin, 0, ApCallbackPosNormal); }  
  { Msg_Identity_ItemAdded msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Identity_Change_ItemAdded, 0, ApCallbackPosNormal); }  
  { Msg_Identity_ItemChanged msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Identity_Change_ItemChanged, 0, ApCallbackPosNormal); }  
  { Msg_Identity_ItemRemoved msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Identity_Change_ItemRemoved, 0, ApCallbackPosNormal); }  
  { Msg_Identity_ContainerEnd msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Identity_Change_ContainerEnd, 0, ApCallbackPosNormal); }  

  if (!s) {
    Msg_Identity_SetContainer msg;
    msg.sUrl = Test_Identity_Change_URL;
    msg.sDigest = Test_Identity_Change_Digest1;
    msg.sData = Test_Identity_Change_Data1;
    if (!msg.Request()) {
      s = "Msg_Identity_SetContainer failed";
    }
  }

  Buffer sbImage;
  if (!Apollo::loadFile(Apollo::getAppResourcePath() + "test/test1.png", sbImage)) {
    s = "Apollo::loadFile(test/test1.png) failed";
  }

  if (!s) {
    Msg_Identity_SetItemData msg;
    msg.sUrl = Test_Identity_Change_URL;
    msg.sId = "item4";
    msg.sMimeType = Test_Identity_Change_ItemMimeType;
    msg.sbData = sbImage;
    if (!msg.Request()) {
      s = "Msg_Identity_SetItemData failed";
    }
  }

  if (!s) {
    Msg_Identity_IsItemDataAvailable msg;
    msg.sUrl = Test_Identity_Change_URL;
    msg.sId = "item4";
    if (!msg.Request()) {
      s = "Msg_Identity_IsItemDataAvailable failed";
    } else {
      if (!msg.bAvailable) {
        s.appendf("(%d) bAvailable=%d, expected bAvailable=1", __LINE__, msg.bAvailable);
      }
    }
  }

  if (!s) {
    Msg_Identity_GetItemIds msg;
    msg.sUrl = Test_Identity_Change_URL;
    msg.sType = "";
    if (!msg.Request()) {
      s = "Msg_Identity_GetItemIds failed";
    } else {
      ValueList vlExpected; vlExpected.add("item1"); vlExpected.add("item2"); vlExpected.add("item3"); vlExpected.add("item4"); 
      s = Test_CompareLists("Msg_Identity_GetItemIds", msg.vlIds, vlExpected, Test_CompareLists_Flag_IgnoreOrder);
    }
  }

  if (!s) {
    Msg_Identity_SetContainer msg;
    msg.sUrl = Test_Identity_Change_URL;
    msg.sDigest = Test_Identity_Change_Digest2;
    msg.sData = Test_Identity_Change_Data2;
    if (!msg.Request()) {
      s.appendf("Msg_Identity_SetContainer (%d) failed", __LINE__);
    }
  }

  if (!s) {
    Msg_Identity_IsItemDataAvailable msg;
    msg.sUrl = Test_Identity_Change_URL;
    msg.sId = "item4";
    if (!msg.Request()) {
      s.appendf("Msg_Identity_IsItemDataAvailable (%d) failed", __LINE__);
    } else {
      if (msg.bAvailable) {
        s.appendf("(%d) bAvailable=%d, expected bAvailable=0", __LINE__, msg.bAvailable);
      }
    }
  }

  if (!s) {
    Msg_Identity_GetItemIds msg;
    msg.sUrl = Test_Identity_Change_URL;
    msg.sType = "";
    if (!msg.Request()) {
      s.appendf("Msg_Identity_GetItemIds (%d) failed", __LINE__);
    } else {
      ValueList vlExpected; vlExpected.add("item2"); vlExpected.add("item3"); vlExpected.add("item4"); vlExpected.add("item5"); vlExpected.add("item6"); 
      s = Test_CompareLists("Msg_Identity_GetItemIds (2)", msg.vlIds, vlExpected, Test_CompareLists_Flag_IgnoreOrder);
    }
  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }
  { Msg_Identity_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Identity_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Identity_DB; (void) msg.Request(); }

  return s;
}

//----------------------

class Test_Identity_selectItemId_Scenario: public Elem
{
public:
  Test_Identity_selectItemId_Scenario(const char* szName, const char* szUrl, const char* szDigest, const char* szData, const char* szType, const char* szMimeType, const char* szId)
    :Elem(szName)
    ,sUrl(szUrl)
    ,sDigest(szDigest)
    ,sData(szData)
    ,sType(szType)
    ,sMimeType(szMimeType)
    ,sId(szId)
  {}
  Test_Identity_selectItemId_Scenario() {}

  String sUrl;
  String sDigest;
  String sData;
  String sType;
  String sMimeType;
  String sId;
};

class Test_Identity_selectItemId_ScenarioList: public ListT<Test_Identity_selectItemId_Scenario, Elem>
{
public:
  void add(const char* szName, const char* szUrl, const char* szDigest, const char* szData, const char* szType, const char* szMimeType, const char* szId);
};

void Test_Identity_selectItemId_ScenarioList::add(const char* szName, const char* szUrl, const char* szDigest, const char* szData, const char* szType, const char* szMimeType, const char* szId)
{
  Test_Identity_selectItemId_Scenario* e = new Test_Identity_selectItemId_Scenario(szName, szUrl, szDigest, szData, szType, szMimeType, szId);
  if (e != 0) {
    AddLast(e);
  }
}

String IdentityModuleTester::test_selectItemId()
{
  String s;

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }

  Test_Identity_selectItemId_ScenarioList l;

  l.add("basic"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1'>Content1</item>\n" \
     "</identity>"
    ,"type1"
    ,""
    ,"item1"
    );

  l.add("basic mimetype"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1' mimetype='mimetype1'>Content1</item>\n" \
     "</identity>"
    ,"type1"
    ,"mimetype1"
    ,"item1"
    );

  l.add("2"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1'>Content1</item>\n" \
     "<item id='item2' contenttype='type2' digest='digest2'>Content2</item>\n" \
     "</identity>"
    ,"type1"
    ,"mimetype1"
    ,"item1"
    );

  l.add("2b"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item2' contenttype='type2' digest='digest2'>Content2</item>\n" \
     "<item id='item1' contenttype='type1' digest='digest1'>Content1</item>\n" \
     "</identity>"
    ,"type1"
    ,"mimetype1"
    ,"item1"
    );

  l.add("2 mimetype"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1' mimetype='mimetype1'>Content1</item>\n" \
     "<item id='item2' contenttype='type2' digest='digest2' mimetype='mimetype2'>Content2</item>\n" \
     "</identity>"
    ,"type1"
    ,"mimetype1"
    ,"item1"
    );

  l.add("2 w&w/o mimetype"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1'>Content1</item>\n" \
     "<item id='item2' contenttype='type1' digest='digest2' mimetype='mimetype2'>Content2</item>\n" \
     "</identity>"
    ,"type1"
    ,"mimetype3"
    ,"item1"
    );

  l.add("2 w&w/o mimetype 2"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1'>Content1</item>\n" \
     "<item id='item2' contenttype='type1' digest='digest2' mimetype='mimetype2'>Content2</item>\n" \
     "</identity>"
    ,"type1"
    ,"mimetype2"
    ,"item2"
    );

  l.add("order"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1' order='2'>Content1</item>\n" \
     "<item id='item2' contenttype='type1' digest='digest2' order='1'>Content2</item>\n" \
     "</identity>"
    ,"type1"
    ,""
    ,"item2"
    );

  l.add("order none"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1'>Content1</item>\n" \
     "<item id='item2' contenttype='type1' digest='digest2' order='1'>Content2</item>\n" \
     "</identity>"
    ,"type1"
    ,""
    ,"item1"
    );

  l.add("order none b"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item2' contenttype='type1' digest='digest2' order='1'>Content2</item>\n" \
     "<item id='item1' contenttype='type1' digest='digest1'>Content1</item>\n" \
     "</identity>"
    ,"type1"
    ,""
    ,"item1"
    );

  l.add("order 5"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1' order='2'>Content1</item>\n" \
     "<item id='item2' contenttype='type1' digest='digest2' order='3'>Content2</item>\n" \
     "<item id='item3' contenttype='type1' digest='digest1' order='1'>Content1</item>\n" \
     "<item id='item4' contenttype='type1' digest='digest1' order='4'>Content1</item>\n" \
     "<item id='item5' contenttype='type1' digest='digest1' order='1'>Content1</item>\n" \
     "</identity>"
    ,"type1"
    ,""
    ,"item3"
    );

  l.add("order 5 none"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1' order='2'>Content1</item>\n" \
     "<item id='item2' contenttype='type1' digest='digest2' order='3'>Content2</item>\n" \
     "<item id='item3' contenttype='type1' digest='digest1' order='1'>Content1</item>\n" \
     "<item id='item4' contenttype='type1' digest='digest1' order='4'>Content1</item>\n" \
     "<item id='item5' contenttype='type1' digest='digest1'>Content1</item>\n" \
     "</identity>"
    ,"type1"
    ,""
    ,"item5"
    );

  l.add("type 5"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1' order='2'>Content1</item>\n" \
     "<item id='item2' contenttype='type2' digest='digest2' order='3'>Content2</item>\n" \
     "<item id='item3' contenttype='type1' digest='digest1' order='1'>Content1</item>\n" \
     "<item id='item4' contenttype='type1' digest='digest1' order='4'>Content1</item>\n" \
     "<item id='item5' contenttype='type2' digest='digest1' order='1'>Content1</item>\n" \
     "</identity>"
    ,"type1"
    ,""
    ,"item3"
    );

  l.add("type 5b"
    ,"http://ydentiti.org/test/selectItemId/container1.xml"
    ,"digest1"
    ,"<?xml version='1.0' encoding='UTF-8'?>\n" \
     "<identity digest='1'>\n" \
     "<item id='item1' contenttype='type1' digest='digest1' order='2'>Content1</item>\n" \
     "<item id='item2' contenttype='type2' digest='digest2' order='3'>Content2</item>\n" \
     "<item id='item3' contenttype='type2' digest='digest1' order='1'>Content1</item>\n" \
     "<item id='item4' contenttype='type1' digest='digest1' order='4'>Content1</item>\n" \
     "<item id='item5' contenttype='type1' digest='digest1' order='1'>Content1</item>\n" \
     "</identity>"
    ,"type1"
    ,""
    ,"item5"
    );

  for (Test_Identity_selectItemId_Scenario* e = 0; (e = l.Next(e)) != 0 && !s; ) {

    { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }

    if (!s) {
      Msg_Identity_SetContainer msg;
      msg.sUrl = e->sUrl;
      msg.sDigest = e->sDigest;
      msg.sData = e->sData;
      if (!msg.Request()) {
        s.appendf("Msg_Identity_SetContainer failed: %s", StringType(e->getName()));
      }
    }

    if (!s) {
      String sId = IdentityModuleInstance::Get()->selectItemId(e->sUrl, e->sType, e->sMimeType);
      if (sId != e->sId) {
        s.appendf("%s: (selectItemId) %s!=%s", StringType(e->getName()), StringType(sId), StringType(e->sId));
      }
    }

    if (!s) {
      Msg_Identity_GetItemIds msg;
      msg.sUrl = e->sUrl;
      msg.sType = e->sType;
      if (!e->sMimeType.empty()) {
        msg.vlMimeTypes.add(e->sMimeType);
      }
      msg.nMax = 1;
      if (!msg.Request()) {
        s.appendf("%s: (Msg_Identity_GetItemIds) Msg_Identity_GetItemIds failed", StringType(e->getName()));
      } else {
        ValueElem* ve = msg.vlIds.nextElem(0);
        if (ve == 0) {
          s.appendf("%s: (Msg_Identity_GetItemIds) missing result", StringType(e->getName()));
        } else {
          if (ve->getString() != e->sId) {
            s.appendf("%s: (Msg_Identity_GetItemIds) %s!=%s", StringType(e->getName()), StringType(ve->getString()), StringType(e->sId));
          }
        }
      }
    }

  }

  { Msg_Identity_ClearAllCache msg; (void) msg.Request(); }

  return s;
}

//----------------------

static ApHandle g_hTest_Identity_RequestContainer = ApNoHandle;
static ApHandle g_hTest_Identity_RequestContainer2 = ApNoHandle;
static int g_nTest_Identity_RequestContainerCount = 0;
#define Test_Identity_RequestContainer_URL "http://ydentiti.org/test/Tassadar/identity.xml"
#define Test_Identity_RequestContainer_Digest "1"
#define Test_Identity_RequestContainer_Property_Key "Nickname"
#define Test_Identity_RequestContainer_Property_Value "Tassadar(Original)"

static ApHandle g_hTest_Identity_RequestItem = ApNoHandle;
static ApHandle g_hTest_Identity_RequestItem2 = ApNoHandle;
static int g_nTest_Identity_RequestItemCount = 0;
#define Test_Identity_RequestItem_ItemId "avatar"
#define Test_Identity_RequestItem_ItemDigest "f5603fbe13bc4189c1503f4d9362e04ac92f5a46"

static int Test_Identity_RequestItemComplete(Msg_Identity_RequestContainerComplete* pMsg)
{
  String s;

  if (pMsg->hRequest == g_hTest_Identity_RequestItem || pMsg->hRequest == g_hTest_Identity_RequestItem2) {
    g_nTest_Identity_RequestItemCount++;

    if (!pMsg->bSuccess) {
      s.appendf("Identity item request failed: %s", StringType(pMsg->sComment));
    } else {

    }
  }

  if (s) {
    AP_UNITTEST_FAILED(IdentityModuleTester::test_RequestItem_End, s);
  } else {
    if (g_nTest_Identity_RequestItemCount == 2) {
      { Msg_Identity_RequestItemComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Identity_RequestItemComplete, 0); }
      AP_UNITTEST_SUCCESS(IdentityModuleTester::test_RequestItem_End);

      Test_Identity_UnitTest_TokenEnd();
    }
  }

  return 1;
}

String IdentityModuleTester::test_RequestItem_Start()
{
  String s;

  { Msg_Identity_RequestItemComplete msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Identity_RequestItemComplete, 0, ApCallbackPosNormal); }  

  if (!s) {
    Msg_Identity_DeleteItemData msg;
    msg.sUrl = Test_Identity_RequestContainer_URL;
    msg.sId = Test_Identity_RequestItem_ItemId;
    (void) msg.Request();
  }

  if (!s) {
    Msg_Identity_IsItemDataAvailable msg;
    msg.sUrl = Test_Identity_RequestContainer_URL;
    msg.sId = Test_Identity_RequestItem_ItemId;
    if (!msg.Request()) {
      s = "Msg_Identity_IsItemDataAvailable failed";
    } else {
      if (msg.bAvailable) {
        s.appendf("bAvailable=%d, expected bAvailable=0", msg.bAvailable);
      }
    }
  }

  String sSrc;
  String sDigest;
  if (!s) {
    Msg_Identity_GetItem msg;
    msg.sUrl = Test_Identity_RequestContainer_URL;
    msg.sId = Test_Identity_RequestItem_ItemId;
    if (!msg.Request()) {
      s = "Msg_Identity_GetItem failed";
    } else {
      sSrc = msg.sSrc;
      sDigest = msg.sDigest;
    }
  }

  if (!s) {
    Msg_Identity_RequestItem msg;
    msg.hRequest = g_hTest_Identity_RequestItem = Apollo::newHandle();
    msg.sUrl = Test_Identity_RequestContainer_URL;
    msg.sId = Test_Identity_RequestItem_ItemId;
    msg.sSrc = sSrc;
    msg.sDigest = sDigest;
    if (!msg.Request()) {
      s = "Msg_Identity_RequestItem failed";
    }
  }

  if (!s) {
    Msg_Identity_RequestItem msg;
    msg.hRequest = g_hTest_Identity_RequestItem2 = Apollo::newHandle();
    msg.sUrl = Test_Identity_RequestContainer_URL;
    msg.sId = Test_Identity_RequestItem_ItemId;
    msg.sSrc = sSrc;
    msg.sDigest = sDigest;
    if (!msg.Request()) {
      s.appendf("Msg_Identity_RequestItem (%d) failed", __LINE__);
    }
  }

  return s;
}

//----------------------

static int Test_Identity_RequestContainerComplete(Msg_Identity_RequestContainerComplete* pMsg)
{
  String s;

  if (pMsg->hRequest == g_hTest_Identity_RequestContainer || pMsg->hRequest == g_hTest_Identity_RequestContainer2) {
    g_nTest_Identity_RequestContainerCount++;

    if (!pMsg->bSuccess) {
      s.appendf("Identity request failed: %s", StringType(pMsg->sComment));
    } else {

      if (pMsg->sXml.empty()) {
        s = "No Xml received";
      } else {

        {
          Msg_Identity_GetProperty msg;
          msg.sUrl = Test_Identity_RequestContainer_URL;
          msg.sKey = Test_Identity_RequestContainer_Property_Key;
          if (!msg.Request()) {
            s = "Msg_Identity_GetProperty failed";
          } else {
            if (msg.sValue != Test_Identity_RequestContainer_Property_Value) {
              s.appendf("Msg_Identity_GetProperty: got=%s expected=%s", StringType(msg.sValue), StringType(Test_Identity_RequestContainer_Property_Value));
            }
          }
        }

      }
    }

  }

  if (s) {
    AP_UNITTEST_FAILED(IdentityModuleTester::test_RequestContainer_End, s);
  } else {
    if (g_nTest_Identity_RequestContainerCount == 2) {
      { Msg_Identity_RequestContainerComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) Test_Identity_RequestContainerComplete, 0); }
      AP_UNITTEST_SUCCESS(IdentityModuleTester::test_RequestContainer_End);
      AP_UNITTEST_EXECUTE(IdentityModuleTester::test_RequestItem_Start);
    }
  }

  return 1;
}

String IdentityModuleTester::test_RequestContainer_Start()
{
  String s;

  { Msg_Identity_RequestContainerComplete msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Identity_RequestContainerComplete, 0, ApCallbackPosNormal); }  

  int bAvailable = 0;
  if (!s) {
    Msg_Identity_IsContainerAvailable msg;
    msg.sUrl = Test_Identity_RequestContainer_URL;
    msg.sDigest = Test_Identity_RequestContainer_Digest;
    if (msg.Request()) {
      bAvailable = msg.bAvailable;
    }
  }

  if (bAvailable && !s) {
    Msg_Identity_DeleteContainer msg;
    msg.sUrl = Test_Identity_RequestContainer_URL;
    (void) msg.Request();
  }

  if (!s) {
    Msg_Identity_RequestContainer msg;
    msg.hRequest = g_hTest_Identity_RequestContainer = Apollo::newHandle();
    msg.sUrl = Test_Identity_RequestContainer_URL;
    msg.sDigest = Test_Identity_RequestContainer_Digest;
    if (!msg.Request()) {
      s = "Msg_Identity_RequestContainer failed";
    }
  }

  if (!s) {
    Msg_Identity_RequestContainer msg;
    msg.hRequest = g_hTest_Identity_RequestContainer2 = Apollo::newHandle();
    msg.sUrl = Test_Identity_RequestContainer_URL;
    msg.sDigest = Test_Identity_RequestContainer_Digest;
    if (!msg.Request()) {
      s.appendf("Msg_Identity_RequestContainer (%d) failed", __LINE__);
    }
  }

  return s;
}

void IdentityModuleTester::begin()
{
  if (Apollo::getConfig("Test/Identity", 0)) {
    AP_UNITTEST_REGISTER(IdentityModuleTester::test_CacheBasic);
    AP_UNITTEST_REGISTER(IdentityModuleTester::test_CacheStorage);
    AP_UNITTEST_REGISTER(IdentityModuleTester::test_GetProperty);
    AP_UNITTEST_REGISTER(IdentityModuleTester::test_GetItem);
    AP_UNITTEST_REGISTER(IdentityModuleTester::test_GetItemData);
    AP_UNITTEST_REGISTER(IdentityModuleTester::test_Change);
    AP_UNITTEST_REGISTER(IdentityModuleTester::test_Change_Sequence);
    AP_UNITTEST_REGISTER(IdentityModuleTester::test_selectItemId);

    if (Apollo::isLoadedModule("Net")) {
      Msg_Net_IsOnline msg;
      if (msg.Request()) {
        g_bTest_Online = msg.bIsOnline;
      }
    }

    if (g_bTest_Online) {
      AP_UNITTEST_REGISTER(IdentityModuleTester::test_RequestContainer_Start);
      AP_UNITTEST_REGISTER(IdentityModuleTester::test_RequestContainer_End);
      AP_UNITTEST_REGISTER(IdentityModuleTester::test_RequestItem_Start);
      AP_UNITTEST_REGISTER(IdentityModuleTester::test_RequestItem_End);
    }

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Identity_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
}

void IdentityModuleTester::execute()
{
  if (Apollo::getConfig("Test/Identity", 0)) {
  }
}

void IdentityModuleTester::end()
{
}

#endif
