// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "NimatorModule.h"
#include "NimatorModuleTester.h"
#include "Local.h"

#if defined(AP_TEST)

static void Test_Nimator_UnitTest_TokenEnd()
{
  apLog_Info((LOG_CHANNEL, "Test_Nimator_UnitTest_TokenEnd", "Finished Test/Nimator"));
  { ApAsyncMessage<Msg_UnitTest_Token> msg; msg.Post(); }
}

static void Test_Nimator_UnitTest_Token(Msg_UnitTest_Token* pMsg)
{
  AP_UNUSED_ARG(pMsg);
  { Msg_UnitTest_Token msg; msg.UnHook(MODULE_NAME, (ApCallback) Test_Nimator_UnitTest_Token, 0); }
  apLog_Info((LOG_CHANNEL, "Test_Nimator_UnitTest_Token", "Starting Test/Nimator"));
  int bTokenEndNow = 1;

  AP_UNITTEST_EXECUTE(NimatorModuleTester::Test_Parse);
  AP_UNITTEST_EXECUTE(NimatorModuleTester::Test_LoadGIF);
  AP_UNITTEST_EXECUTE(NimatorModuleTester::Test_SelectByGroup);
  AP_UNITTEST_EXECUTE(NimatorModuleTester::Test_Play);

  if (bTokenEndNow) { Test_Nimator_UnitTest_TokenEnd(); }
}

//----------------------------------------------------------

#define NimatorModule_Test_Parse_Url "http://ydentiti.org/test/Nimator/avatar.xml"
#define NimatorModule_Test_Parse_Data \
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

String NimatorModuleTester::Test_Parse()
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
    msg.sbData.SetData(NimatorModule_Test_Parse_Data);
    msg.sSourceUrl = NimatorModule_Test_Parse_Url;
    if (!msg.Request()) {
      s = "Msg_Animation_SetData failed";
    }
  }

  if (!s) {
    NimatorModule* pNimator = NimatorModuleInstance::Get();
    Item* pItem = 0;
    pNimator->items_.Get(hItem, pItem);
    
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
    Animation* pAnimation_sleep = pSequence_sleep->First(); if (!s) { if (pAnimation_sleep->sSrc_ != "http://ydentiti.org/test/Nimator/sleep.gif") { s = "expected different src for animation in sequence sleep"; } }
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

String NimatorModuleTester::Test_LoadGIF()
{
  String s;

  Animation a;
  Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar/walk-l.gif", a.sbData_);
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

  return s;
}

String NimatorModuleTester::Test_SelectByGroup1(Group& g, int nRnd, const String& sExpectedSequence)
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

String NimatorModuleTester::Test_SelectByGroup()
{
  String s;

  Group g("g");
  Sequence *s1 = new Sequence("s1", "status", "normal", 100, "", "", 0, 0);
  Sequence *s2 = new Sequence("s2", "status", "normal", 100, "", "", 0, 0);
  Sequence *s3 = new Sequence("s3", "status", "normal", 100, "", "", 0, 0);
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

String NimatorModuleTester::Test_Play()
{
  String s;

  Item i(Apollo::newHandle());

  Buffer sbConfig;
  sbConfig.SetData(NimatorModule_Test_Parse_Data);
  String sUrl = NimatorModule_Test_Parse_Url;
  i.SetData(sbConfig, sUrl);

  String sBasePath = Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator();
  String sBaseUrl = String::filenameBasePath(sUrl);

  String sName = "idle.gif";

  String sAnimationPath = sBasePath + sName;
  String sAnimationUrl = sBaseUrl + sName;
  Buffer sbAnimationData;
  Apollo::loadFile(sAnimationPath, sbAnimationData);
  i.AnimationData(sbAnimationData, sAnimationUrl);

  Apollo::TimeValue t = Apollo::TimeValue::getTime();
  i.Step(t);

  return s;
}

//----------------------------------------------------------

void NimatorModuleTester::Begin()
{
  if (Apollo::getConfig("Test/Nimator", 0)) {
    AP_UNITTEST_REGISTER(NimatorModuleTester::Test_Parse);
    AP_UNITTEST_REGISTER(NimatorModuleTester::Test_LoadGIF);
    AP_UNITTEST_REGISTER(NimatorModuleTester::Test_SelectByGroup);
    AP_UNITTEST_REGISTER(NimatorModuleTester::Test_Play);

    { Msg_UnitTest_Token msg; msg.Hook(MODULE_NAME, (ApCallback) Test_Nimator_UnitTest_Token, 0, ApCallbackPosNormal); }  
  }
}

void NimatorModuleTester::Execute()
{
  if (Apollo::getConfig("Test/Nimator", 0)) {
  }
}

void NimatorModuleTester::End()
{
}

#endif
