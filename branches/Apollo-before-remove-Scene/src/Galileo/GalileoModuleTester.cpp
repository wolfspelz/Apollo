// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "GalileoModule.h"
#include "GalileoModuleTester.h"
#include "MsgConfig.h"
#include "MsgDB.h"
#include "Local.h"

#if defined(AP_TEST)

static void Test_Galileo_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Galileo_UnitTest_TokenEnd", "Finished Test/Galileo"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

static void Test_Galileo_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Galileo_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Galileo_UnitTest_Token", "Starting Test/Galileo"));
  int bTokenEndNow = 1;

  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_Parse);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_LoadGIF);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectByGroup);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_PlayStill);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_PlayWave);
  //AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_PlayMoveRight);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectNextSequence_KeepStatus);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectNextSequence_Event);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectNextSequence_StatussedEvent);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectNextSequence_StatusTransition);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectNextSequence_StatusTransitionNextSequence);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectNextSequence_ConditionedEvent);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectNextSequence_StatussedConditionedEvent);
  AP_UNITTEST_EXECUTE(GalileoModuleTester::Test_SelectNextSequence_ConditionedStatusTransition);

  if (bTokenEndNow) { Test_Galileo_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#define Test_Galileo_DB "GalileoTest"
#define GalileoModuleTester_Parse_Url "http://ydentiti.org/test/Galileo/avatar.xml"

#define GalileoModuleTester_Parse_Data \
"<config xmlns='http://schema.bluehands.de/character-config' version='1.0'>\n" \
"  <param name='defaultsequence' value='idle'/>\n" \
"  <sequence group='idle' name='still' type='status' probability='1000' in='standard' out='standard'><animation src='idle.gif'/></sequence>\n" \
"  <sequence group='idle' name='idle1' type='status' probability='100' in='standard' out='standard'><animation src='idle-1.gif'/></sequence>\n" \
"  <sequence group='moveleft' name='moveleft' type='basic' probability='1' in='moveleft' out='moveleft'><animation dx='-55' dy='0' src='walk-l.gif'/></sequence>\n" \
"  <sequence group='moveright' name='moveright' type='basic' probability='1' in='moveright' out='moveright'><animation dx='55' dy='0' src='walk-r.gif'/></sequence>\n" \
"  <sequence group='chat' name='chat1' type='basic' probability='1000' in='standard' out='standard'><animation src='chat.gif'/></sequence>\n" \
"  <sequence group='chat' name='chat2' type='basic' probability='100' in='standard' out='standard'><animation src='chat-2.gif'/></sequence>\n" \
"  <sequence group='wave' name='wave' type='emote' probability='1000' in='standard' out='standard'><animation src='http://ydentiti.org/test/Tassadar/wave.gif'/></sequence>\n" \
"  <sequence group='sleep' name='sleep' type='status' probability='1000' in='standard' out='standard'><animation src='sleep.gif'/></sequence>\n" \
"</config>"

String GalileoModuleTester::Test_Parse()
{
  String s;

  ApHandle hItem = Apollo::newHandle();
  if (!s) {
    Msg_Animation_Create msg;
    msg.hItem = hItem;
    msg.sMimeType = "avatar/gif";
    if (!msg.Request()) {
      s = "Msg_Animation_Create failed";
    }
  }

  if (!s) {
    Msg_Animation_SetData msg;
    msg.hItem = hItem;
    msg.sbData.SetData(GalileoModuleTester_Parse_Data);
    msg.sSourceUrl = GalileoModuleTester_Parse_Url;
    if (!msg.Request()) {
      s = "Msg_Animation_SetData failed";
    }
  }

  if (!s) {
    GalileoModule* pGalileo = GalileoModuleInstance::Get();
    Item* pItem = 0;
    pGalileo->items_.Get(hItem, pItem);
    
    if (pItem->lGroups_.length() != 6) { s = "expected 6 groups"; }
    Group* pGroup_idle = pItem->lGroups_.FindByName("idle"); if (!s) { if (pGroup_idle->length() != 2) { s = "expected 2 sequences in group idle"; } }
    Group* pGroup_moveleft = pItem->lGroups_.FindByName("moveleft"); if (!s) { if (pGroup_moveleft->length() != 1) { s = "expected 1 sequence in group moveleft"; } }
    Group* pGroup_moveright = pItem->lGroups_.FindByName("moveright"); if (!s) { if (pGroup_moveright->length() != 1) { s = "expected 1 sequence in group moveright"; } }
    Group* pGroup_chat = pItem->lGroups_.FindByName("chat"); if (!s) { if (pGroup_chat->length() != 2) { s = "expected 2 sequences in group chat"; } }
    Group* pGroup_wave = pItem->lGroups_.FindByName("wave"); if (!s) { if (pGroup_wave->length() != 1) { s = "expected 1 sequence in group wave"; } }
    Group* pGroup_sleep = pItem->lGroups_.FindByName("sleep"); if (!s) { if (pGroup_sleep->length() != 1) { s = "expected 1 sequence in group sleep"; } }
    Sequence* pSequence_still = pGroup_idle->FindByName("still"); if (!s) { if (pSequence_still->length() != 1) { s = "expected 1 animation in sequence still"; } }
    Sequence* pSequence_idle1 = pGroup_idle->FindByName("idle1"); if (!s) { if (pSequence_idle1->length() != 1) { s = "expected 1 animation in sequence idle1"; } }
    Sequence* pSequence_moveleft = pGroup_moveleft->FindByName("moveleft"); if (!s) { if (pSequence_moveleft->length() != 1) { s = "expected 1 animation in sequence moveleft"; } }
    Sequence* pSequence_moveright = pGroup_moveright->FindByName("moveright"); if (!s) { if (pSequence_moveright->length() != 1) { s = "expected 1 animation in sequence moveright"; } }
    Sequence* pSequence_chat1 = pGroup_chat->FindByName("chat1"); if (!s) { if (pSequence_chat1->length() != 1) { s = "expected 1 animation in sequence chat1"; } }
    Sequence* pSequence_chat2 = pGroup_chat->FindByName("chat2"); if (!s) { if (pSequence_chat2->length() != 1) { s = "expected 1 animation in sequence chat2"; } }
    Sequence* pSequence_wave = pGroup_wave->FindByName("wave"); if (!s) { if (pSequence_wave->length() != 1) { s = "expected 1 animation in sequence wave"; } }
    Sequence* pSequence_sleep = pGroup_sleep->FindByName("sleep"); if (!s) { if (pSequence_sleep->length() != 1) { s = "expected 1 animation in sequence sleep"; } }
    Animation* pAnimation_still = pSequence_still->First(); if (!s) { if (!pAnimation_still->sSrc_) { s = "expected src for animation in sequence still"; } }
    Animation* pAnimation_idle1 = pSequence_idle1->First(); if (!s) { if (!pAnimation_idle1->sSrc_) { s = "expected src for animation in sequence idle1"; } }
    Animation* pAnimation_moveleft = pSequence_moveleft->First(); if (!s) { if (!pAnimation_moveleft->sSrc_) { s = "expected src for animation in sequence moveleft"; } }
    Animation* pAnimation_moveright = pSequence_moveright->First(); if (!s) { if (!pAnimation_moveright->sSrc_) { s = "expected src for animation in sequence moveright"; } }
    Animation* pAnimation_chat1 = pSequence_chat1->First(); if (!s) { if (!pAnimation_chat1->sSrc_) { s = "expected src for animation in sequence chat1"; } }
    Animation* pAnimation_chat2 = pSequence_chat2->First(); if (!s) { if (!pAnimation_chat2->sSrc_) { s = "expected src for animation in sequence chat2"; } }
    Animation* pAnimation_wave = pSequence_wave->First(); if (!s) { if (pAnimation_wave->sSrc_ != "http://ydentiti.org/test/Tassadar/wave.gif") { s = "expected different src for animation in sequence wave"; } }
    Animation* pAnimation_sleep = pSequence_sleep->First(); if (!s) { if (pAnimation_sleep->sSrc_ != "http://ydentiti.org/test/Galileo/sleep.gif") { s = "expected different src for animation in sequence sleep"; } }
  }

  if (!s) {
    Msg_Animation_Destroy msg;
    msg.hItem = hItem;
    msg.hItem = hItem;
    if (!msg.Request()) {
      s = "Msg_Animation_Destroy failed";
    }
  }

  return s;
}

//---------------------------

String GalileoModuleTester::Test_LoadGIF()
{
  String s;

  String sOrigDb;
  { Msg_Galileo_SetStorageName msg; msg.sName = Test_Galileo_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }

  GalileoModule m;
  Animation a(&m);
  Apollo::loadFile(Apollo::getAppResourcePath() + "test/tassadar/" + "walk-l.gif", a.sbData_);
  a.Load();
  if (!s) { if (a.length() != 13) { s = "expected 13 frames"; } }
  if (!s) { if (a.nDurationMSec_ != 1300) { s = "expected 1300 ms duration"; } }
  if (!s) { if (a.nFramesCount_ != 13) { s = "expected frame count 13"; } }
  int nCnt = 0;
  for (Frame* pFrame = 0; (pFrame = a.Next(pFrame)) != 0; ) {
    nCnt++;
    if (!s) { if (pFrame->img_.Pixels() == 0) { s.appendf("frame %d: expected pixels", nCnt); } }
    if (!s) { if (pFrame->img_.Size() != 40000) { s.appendf("frame %d: expected byte count 40.000", nCnt); } }
    if (!s) { if (pFrame->img_.Width() != 100) { s.appendf("frame %d: expected width 100", nCnt); } }
    if (!s) { if (pFrame->img_.Height() != 100) { s.appendf("frame %d: expected height 100", nCnt); } }
  }

  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Galileo_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Galileo_DB; (void) msg.Request(); }

  return s;
}

String GalileoModuleTester::Test_SelectByGroup1(Group& g, int nRnd, const String& sExpectedSequence)
{
  String s;

  Sequence* pSequence = g.GetRandomSequence(nRnd);
  if (pSequence) {
    if (sExpectedSequence) {
      if (pSequence->getName() != sExpectedSequence) {
        s.appendf("Sequence got=%s expected=%s", StringType(pSequence->getName()), StringType(sExpectedSequence));
      }
    } else {
      s = "Expected no sequence";
    }
  } else {
    if (sExpectedSequence) {
      s = "No sequence";
    }
  }

  return s;
}

String GalileoModuleTester::Test_SelectByGroup()
{
  String s;

  GalileoModule m;
  Group g("g");
  Sequence *s1 = new Sequence("s1", &m, "g", "status", "normal", 100, "", "", 0, 0);
  Sequence *s2 = new Sequence("s2", &m, "g", "status", "normal", 100, "", "", 0, 0);
  Sequence *s3 = new Sequence("s3", &m, "g", "status", "normal", 100, "", "", 0, 0);
  g.AddSequence(s1);
  g.AddSequence(s2);
  g.AddSequence(s3);

  for (int i = 0; i< 10; i++) {
    int n = Apollo::getRandom(3);
    int m = n;
  }

  if (!s) { s = Test_SelectByGroup1(g, 0, "s1"); }
  if (!s) { s = Test_SelectByGroup1(g, 1, "s1"); }
  if (!s) { s = Test_SelectByGroup1(g, 99, "s1"); }
  if (!s) { s = Test_SelectByGroup1(g, 100, "s2"); }
  if (!s) { s = Test_SelectByGroup1(g, 150, "s2"); }
  if (!s) { s = Test_SelectByGroup1(g, 299, "s3"); }
  if (!s) { s = Test_SelectByGroup1(g, 300, ""); }

  return s;
}

//---------------------------

#define GalileoModuleTester_Play_Url "http://ydentiti.org/test/Galileo/avatar.xml"

#define GalileoModuleTester_Play_Data \
"<config xmlns='http://schema.bluehands.de/character-config' version='1.0'>\n" \
"  <param name='defaultsequence' value='idle'/>\n" \
"  <sequence group='idle' name='still' type='status' probability='1000' in='standard' out='standard'><animation src='idle.gif'/></sequence>\n" \
"  <sequence group='moveleft' name='moveleft1' type='basic' probability='1' in='moveleft' out='moveleft'><animation dx='-55' dy='0' src='walk-l.gif'/></sequence>\n" \
"  <sequence group='moveright' name='moveright1' type='basic' probability='1' in='moveright' out='moveright'><animation dx='55' dy='0' src='walk-r.gif'/></sequence>\n" \
"  <sequence group='chat' name='chat1' type='basic' probability='1000' in='standard' out='standard'><animation src='chat.gif'/></sequence>\n" \
"  <sequence group='wave' name='wave1' type='emote' probability='1000' in='standard' out='standard'><animation src='http://ydentiti.org/test/Tassadar/wave.gif'/></sequence>\n" \
"  <sequence group='sleep' name='sleep1' type='status' probability='1000' in='standard' out='standard'><animation src='sleep.gif'/></sequence>\n" \
"</config>"

static void GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(Item& i, const String& sName, const String& sBaseUrl)
{
  String sAnimationPath = Apollo::getAppResourcePath() + "test/tassadar/" + sName;
  String sAnimationUrl = sBaseUrl + sName;
  Buffer sbAnimationData;
  Apollo::loadFile(sAnimationPath, sbAnimationData);
  i.SetAnimationData(sAnimationUrl, sbAnimationData, "image/gif");
}

static void GalileoModuleTester_Test_Item_AnimationData_AbsoluteUrl(Item& i, const String& sName, const String& sUrl)
{
  String sAnimationPath = Apollo::getAppResourcePath() + "test/tassadar/" + sName;
  Buffer sbAnimationData;
  Apollo::loadFile(sAnimationPath, sbAnimationData);
  i.SetAnimationData(sUrl, sbAnimationData, "image/gif");
}

String GalileoModuleTester::Test_PlayStep(Item& i, Apollo::TimeValue& t, const String& sExpectedSequence, int nExpectedTime)
{
  String s;

  i.Step(t);
  if (i.pCurrentSequence_->Group() != sExpectedSequence) {
    s.appendf("sequence=%s expected=%s", StringType(i.pCurrentSequence_->getName()), StringType(sExpectedSequence));
  } else if (i.nSpentInCurrentSequenceMSec_ != nExpectedTime) {
    s.appendf("time=%d expected=%d", i.nSpentInCurrentSequenceMSec_, nExpectedTime);
  //} else if (i.nX_ != nExpectedX) {
  //  s.appendf("x=%d expected=%d", i.nX_, nExpectedX);
  }

  return s;
}

String GalileoModuleTester::Test_PlayStill()
{
  String s;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }
  Apollo::setModuleConfig(MODULE_NAME, "DefaultFrameDuration", 1000);
  Apollo::setModuleConfig(MODULE_NAME, "DefaultAnimationDuration", 1000);

  String sOrigDb;
  { Msg_Galileo_SetStorageName msg; msg.sName = Test_Galileo_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_Play_Data);
  String sUrl = GalileoModuleTester_Play_Url;
  i.SetData(sbConfig, sUrl);

  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "idle.gif", String::filenameBasePath(sUrl));
  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "walk-l.gif", String::filenameBasePath(sUrl));
  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "walk-r.gif", String::filenameBasePath(sUrl));
  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "chat.gif", String::filenameBasePath(sUrl));
  GalileoModuleTester_Test_Item_AnimationData_AbsoluteUrl(i, "wave.gif", "http://ydentiti.org/test/Tassadar/wave.gif");
  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "sleep.gif", String::filenameBasePath(sUrl));

  Apollo::TimeValue t = Apollo::TimeValue::getTime();
  Apollo::TimeValue d(0, 110000);
  if (!s) {         s = Test_PlayStep(i, t, "idle", 0); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 110); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 220); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 330); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 440); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 550); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 660); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 770); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 880); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 990); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 100); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 210); }

  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Galileo_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Galileo_DB; (void) msg.Request(); }

  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }

  return s;
}

String GalileoModuleTester::Test_PlayWave()
{
  String s;

  String sOriginalPlane = "default";
  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
  { Msg_Config_Clear msg; msg.Request(); }
  Apollo::setModuleConfig(MODULE_NAME, "DefaultFrameDuration", 1000);
  Apollo::setModuleConfig(MODULE_NAME, "DefaultAnimationDuration", 1000);

  String sOrigDb;
  { Msg_Galileo_SetStorageName msg; msg.sName = Test_Galileo_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_Play_Data);
  String sUrl = GalileoModuleTester_Play_Url;
  i.SetData(sbConfig, sUrl);

  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "idle.gif", String::filenameBasePath(sUrl));
  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "walk-l.gif", String::filenameBasePath(sUrl));
  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "walk-r.gif", String::filenameBasePath(sUrl));
  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "chat.gif", String::filenameBasePath(sUrl));
  GalileoModuleTester_Test_Item_AnimationData_AbsoluteUrl(i, "wave.gif", "http://ydentiti.org/test/Tassadar/wave.gif");
  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "sleep.gif", String::filenameBasePath(sUrl));

  Apollo::TimeValue t = Apollo::TimeValue::getTime();
  Apollo::TimeValue d(0, 110000);
  if (!s) {         s = Test_PlayStep(i, t, "idle", 0); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 110); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 220); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 330); }
  i.PlayEvent("wave");
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 440); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 550); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 660); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 770); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 880); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 990); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 100); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 210); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 320); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 430); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 540); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 650); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 760); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 870); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 980); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 1090); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 1200); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 1310); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 1420); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 1530); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 1640); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "wave", 1750); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 60); }
  if (!s) { t += d; s = Test_PlayStep(i, t, "idle", 170); }

  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }
  { Msg_Galileo_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
  { Msg_DB_DeleteFile msg; msg.sName = Test_Galileo_DB; (void) msg.Request(); }

  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }

  return s;
}

//String GalileoModuleTester::Test_PlayMoveRight()
//{
//  String s;
//
//  String sOriginalPlane = "default";
//  { Msg_Config_GetPlane msg; if (msg.Request()) { sOriginalPlane = msg.sPlane; } }
//  { Msg_Config_SetPlane msg; msg.sPlane = "test"; msg.Request(); }
//  { Msg_Config_Clear msg; msg.Request(); }
//  Apollo::setModuleConfig(MODULE_NAME, "DefaultFrameDuration", 1000);
//  Apollo::setModuleConfig(MODULE_NAME, "DefaultAnimationDuration", 1000);
//
//  String sOrigDb;
//  { Msg_Galileo_SetStorageName msg; msg.sName = Test_Galileo_DB; if (msg.Request()) { sOrigDb = msg.sPreviousName; } }
//  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }
//
//  GalileoModule m;
//  Item i(Apollo::newHandle(), &m);
//
//  Buffer sbConfig;
//  sbConfig.SetData(GalileoModuleTester_Play_Data);
//  String sUrl = GalileoModuleTester_Play_Url;
//  i.SetData(sbConfig, sUrl);
//  i.SetPosition(100);
//
//  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "idle.gif", String::filenameBasePath(sUrl));
//  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "walk-l.gif", String::filenameBasePath(sUrl));
//  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "walk-r.gif", String::filenameBasePath(sUrl));
//  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "chat.gif", String::filenameBasePath(sUrl));
//  GalileoModuleTester_Test_Item_AnimationData_AbsoluteUrl(i, "wave.gif", "http://ydentiti.org/test/Tassadar/wave.gif");
//  GalileoModuleTester_Test_Item_AnimationData_RelativeUrl(i, "sleep.gif", String::filenameBasePath(sUrl));
//
//  Apollo::TimeValue t = Apollo::TimeValue::getTime();
//  Apollo::TimeValue d(0, 110000);
//  if (!s) {         s = Test_PlayStep(i, t, "still", 0, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 110, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 220, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 330, 100); }
//  i.MoveTo(250);
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 440, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 550, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 660, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 770, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 880, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 990, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 100, 105); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 210, 110); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 320, 115); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 430, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 540, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 650, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 760, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 870, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 980, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 1090, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 1200, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 10, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 120, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 230, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 340, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 450, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 560, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 670, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 780, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 890, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 1000, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 1110, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "moveright", 1220, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 30, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 140, 100); }
//  if (!s) { t += d; s = Test_PlayStep(i, t, "still", 250, 100); }
//
//  { Msg_Galileo_ClearAllStorage msg; (void) msg.Request(); }
//  { Msg_Galileo_SetStorageName msg; msg.sName = sOrigDb; (void) msg.Request(); }
//  { Msg_DB_DeleteFile msg; msg.sName = Test_Galileo_DB; (void) msg.Request(); }
//
//  { Msg_Config_SetPlane msg; msg.sPlane = sOriginalPlane; msg.Request(); }
//
//  return s;
//}

//---------------------------

#define GalileoModuleTester_SelectNextSequence_Url "http://ydentiti.org/test/Galileo/avatar.xml"

#define GalileoModuleTester_SelectNextSequence_Data \
"<config xmlns='http://schema.bluehands.de/character-config' version='1.0'>\n" \
"  <param name='defaultsequence' value='idle'/>\n" \
"  <sequence group='idle' name='idle1' type='status' probability='1000' in='standard' out='standard'><animation src='idle.gif'/></sequence>\n" \
"  <sequence group='moveleft' name='moveleft1' type='basic' probability='1' in='moveleft' out='moveleft'><animation dx='-55' dy='0' src='walk-l.gif'/></sequence>\n" \
"  <sequence group='moveright' name='moveright1' type='basic' probability='1' in='moveright' out='moveright'><animation dx='55' dy='0' src='walk-r.gif'/></sequence>\n" \
"  <sequence group='chat' name='chat1' type='basic' probability='1000' in='standard' out='standard'><animation src='chat.gif'/></sequence>\n" \
"  <sequence group='wave' name='wave1' type='emote' probability='1000' in='standard' out='standard'><animation src='http://ydentiti.org/test/Tassadar/wave.gif'/></sequence>\n" \
"  <sequence group='sleep' name='sleep1' type='status' probability='1000' in='standard' out='standard'><animation src='sleep.gif'/></sequence>\n" \
"  <sequence group='wave@sleep' name='wave2' type='emote' probability='1000' in='standard' out='standard'><animation src='http://ydentiti.org/test/Tassadar/wave.gif'/></sequence>\n" \
"  <sequence group='idle2sleep' name='idle2sleep1' type='emote' probability='1000' in='standard' out='standard'><animation src='http://ydentiti.org/test/Tassadar/wave.gif'/></sequence>\n" \
"  <sequence group='wave#damaged' name='wave3' type='emote' probability='1000' in='standard' out='standard'><animation src='http://ydentiti.org/test/Tassadar/wave.gif'/></sequence>\n" \
"  <sequence group='wave@sleep#damaged' name='wave3' type='emote' probability='1000' in='standard' out='standard'><animation src='http://ydentiti.org/test/Tassadar/wave.gif'/></sequence>\n" \
"  <sequence group='idle2sleep#damaged' name='idle2sleep2' type='emote' probability='1000' in='standard' out='standard'><animation src='http://ydentiti.org/test/Tassadar/wave.gif'/></sequence>\n" \
"</config>"

String GalileoModuleTester::Test_SelectNextSequence_KeepStatus()
{
  String s;

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_SelectNextSequence_Data);
  i.SetData(sbConfig, GalileoModuleTester_SelectNextSequence_Url);
  i.sStatus_ = "idle";
  i.pCurrentSequence_ = i.GetSequenceByGroup("idle");

  Sequence* p = i.SelectNextSequence();

  String sExpected = "idle";
  if (p->Group() != sExpected) {
    s = "Sequence group=" + p->Group() + " expected=" + sExpected;
  }

  return s;
}

String GalileoModuleTester::Test_SelectNextSequence_Event()
{
  String s;

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_SelectNextSequence_Data);
  i.SetData(sbConfig, GalileoModuleTester_SelectNextSequence_Url);
  i.sStatus_ = "idle";
  i.pCurrentSequence_ = i.GetSequenceByGroup("idle");

  i.PlayEvent("wave");
  Sequence* p = i.SelectNextSequence();

  String sExpected = "wave";
  if (p->Group() != sExpected) {
    s = "Sequence group=" + p->Group() + " expected=" + sExpected;
  }

  return s;
}

String GalileoModuleTester::Test_SelectNextSequence_StatussedEvent()
{
  String s;

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_SelectNextSequence_Data);
  i.SetData(sbConfig, GalileoModuleTester_SelectNextSequence_Url);
  i.sStatus_ = "sleep";
  i.pCurrentSequence_ = i.GetSequenceByGroup("sleep");

  i.PlayEvent("wave");
  Sequence* p = i.SelectNextSequence();

  String sExpected = "wave@sleep";
  if (p->Group() != sExpected) {
    s = "Sequence group=" + p->Group() + " expected=" + sExpected;
  }

  return s;
}

String GalileoModuleTester::Test_SelectNextSequence_StatusTransition()
{
  String s;

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_SelectNextSequence_Data);
  i.SetData(sbConfig, GalileoModuleTester_SelectNextSequence_Url);
  i.sStatus_ = "idle";
  i.pCurrentSequence_ = i.GetSequenceByGroup("idle");

  i.SetStatus("sleep");
  Sequence* p = i.SelectNextSequence();

  String sExpected = "idle2sleep";
  if (p->Group() != sExpected) {
    s = "Sequence group=" + p->Group() + " expected=" + sExpected;
  }

  return s;
}

String GalileoModuleTester::Test_SelectNextSequence_StatusTransitionNextSequence()
{
  String s;

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_SelectNextSequence_Data);
  i.SetData(sbConfig, GalileoModuleTester_SelectNextSequence_Url);
  i.sStatus_ = "idle";
  i.pCurrentSequence_ = i.GetSequenceByGroup("idle");

  i.SetStatus("sleep");

  if (!s) {
    Sequence* p = i.SelectNextSequence();
    String sExpected = "idle2sleep";
    if (p->Group() != sExpected) {
      s = "Sequence 1 group=" + p->Group() + " expected=" + sExpected;
    } else {
      i.pCurrentSequence_ = p;
    }
  }

  if (!s) {
    Sequence* p = i.SelectNextSequence();
    String sExpected = "sleep";
    if (p->Group() != sExpected) {
      s = "Sequence 2 group=" + p->Group() + " expected=" + sExpected;
    } else {
      i.pCurrentSequence_ = p;
    }
  }

  if (!s) {
    Sequence* p = i.SelectNextSequence();
    String sExpected = "sleep";
    if (p->Group() != sExpected) {
      s = "Sequence 3 group=" + p->Group() + " expected=" + sExpected;
    } else {
      i.pCurrentSequence_ = p;
    }
  }

  return s;
}

String GalileoModuleTester::Test_SelectNextSequence_ConditionedEvent()
{
  String s;

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_SelectNextSequence_Data);
  i.SetData(sbConfig, GalileoModuleTester_SelectNextSequence_Url);
  i.pCurrentSequence_ = i.GetSequenceByGroup("sleep");

  i.SetCondition("damaged");
  i.PlayEvent("wave");
  Sequence* p = i.SelectNextSequence();

  String sExpected = "wave#damaged";
  if (p->Group() != sExpected) {
    s = "Sequence group=" + p->Group() + " expected=" + sExpected;
  }

  return s;
}

String GalileoModuleTester::Test_SelectNextSequence_StatussedConditionedEvent()
{
  String s;

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_SelectNextSequence_Data);
  i.SetData(sbConfig, GalileoModuleTester_SelectNextSequence_Url);
  i.pCurrentSequence_ = i.GetSequenceByGroup("sleep");

  i.SetStatus("sleep");
  i.SetCondition("damaged");
  i.PlayEvent("wave");
  Sequence* p = i.SelectNextSequence();

  String sExpected = "wave@sleep#damaged";
  if (p->Group() != sExpected) {
    s = "Sequence group=" + p->Group() + " expected=" + sExpected;
  }

  return s;
}

String GalileoModuleTester::Test_SelectNextSequence_ConditionedStatusTransition()
{
  String s;

  GalileoModule m;
  Item i(Apollo::newHandle(), &m);

  Buffer sbConfig;
  sbConfig.SetData(GalileoModuleTester_SelectNextSequence_Data);
  i.SetData(sbConfig, GalileoModuleTester_SelectNextSequence_Url);
  i.sStatus_ = "idle";
  i.pCurrentSequence_ = i.GetSequenceByGroup("idle");

  i.SetStatus("sleep");
  i.SetCondition("damaged");
  Sequence* p = i.SelectNextSequence();

  String sExpected = "idle2sleep#damaged";
  if (p->Group() != sExpected) {
    s = "Sequence group=" + p->Group() + " expected=" + sExpected;
  }

  return s;
}

//----------------------------------------------------------

void GalileoModuleTester::Begin()
{
  if (Apollo::getConfig("Test/Galileo", 0)) {
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_Parse);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_LoadGIF);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectByGroup);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_PlayStill);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_PlayWave);
    //AP_UNITTEST_REGISTER(GalileoModuleTester::Test_PlayMoveRight);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectNextSequence_KeepStatus);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectNextSequence_Event);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectNextSequence_StatussedEvent);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectNextSequence_StatusTransition);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectNextSequence_StatusTransitionNextSequence);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectNextSequence_ConditionedEvent);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectNextSequence_StatussedConditionedEvent);
    AP_UNITTEST_REGISTER(GalileoModuleTester::Test_SelectNextSequence_ConditionedStatusTransition);

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Galileo_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
}

void GalileoModuleTester::Execute()
{
  if (Apollo::getConfig("Test/Galileo", 0)) {
  }
}

void GalileoModuleTester::End()
{
}

#endif
