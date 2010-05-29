// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgConfig.h"
#include "Local.h"
#include "NimatorModule.h"

NimatorModule::NimatorModule()
{
}

NimatorModule::~NimatorModule()
{
}

//----------------------------------------------------------

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_Create)
{
  if (pMsg->sMimeType == "avatar/gif") {
    // Handle it
  } else {
    return;
  }

  if (items_.Find(pMsg->hItem) != 0) { throw ApException("hItem=" ApHandleFormat ", already exists", ApHandleType(pMsg->hItem)); }

  Item* pItem = new Item(pMsg->hItem);
  if (pItem == 0) { throw ApException("new Item() failed, hItem=" ApHandleFormat "", ApHandleType(pMsg->hItem)); }

  items_.Set(pMsg->hItem, pItem);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_Destroy)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  items_.Unset(pMsg->hItem);
  delete pItem;
  pItem = 0;

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_Start)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  pItem->Start();

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_Stop)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  pItem->Stop();

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_SetRate)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  if (pMsg->nMaxRate > 0 && pMsg->nMaxRate < 25) {
    int nDelay = 1000 / pMsg->nMaxRate;
    pItem->SetDelay(nDelay);
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_SetData)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  pItem->SetData(pMsg->sbData);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_SetStatus)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  pItem->SetStatus(pMsg->sStatus);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_Event)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  pItem->PlayEvent(pMsg->sEvent);

  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

#define NimatorModule_Test_Parse \
"<config xmlns='http://schema.bluehands.de/character-config' version='1.0'>\n" \
"  <param name='defaultsequence' value='idle'/>\n" \
"  <sequence group='idle' name='still' type='status' probability='1000' in='standard' out='standard'><animation src='idle.gif'/></sequence>\n" \
"  <sequence group='idle' name='idle1' type='status' probability='100' in='standard' out='standard'><animation src='idle-1.gif'/></sequence>\n" \
"  <sequence group='moveleft' name='moveleft' type='basic' probability='1' in='moveleft' out='moveleft'><animation dx='-55' dy='0' src='walk-l.gif'/></sequence>\n" \
"  <sequence group='moveright' name='moveright' type='basic' probability='1' in='moveright' out='moveright'><animation dx='55' dy='0' src='walk-r.gif'/></sequence>\n" \
"  <sequence group='chat' name='chat1' type='basic' probability='1000' in='standard' out='standard'><animation src='chat.gif'/></sequence>\n" \
"  <sequence group='chat' name='chat2' type='basic' probability='100' in='standard' out='standard'><animation src='chat-2.gif'/></sequence>\n" \
"  <sequence group='wave' name='wave' type='emote' probability='1000' in='standard' out='standard'><animation src='wave.gif'/></sequence>\n" \
"  <sequence group='sleep' name='sleep' type='status' probability='1000' in='standard' out='standard'><animation src='idle.gif'/></sequence>\n" \
"</config>"

String NimatorModule::Test_Parse()
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
    msg.sbData.SetData(NimatorModule_Test_Parse);
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
    Animation* pAnimation_wave = pSequence_wave->First(); if (!s) { if (!pAnimation_wave->sSrc_) { s = "expected src for animation in sequence wave"; } }
    Animation* pAnimation_sleep = pSequence_sleep->First(); if (!s) { if (!pAnimation_sleep->sSrc_) { s = "expected src for animation in sequence sleep"; } }
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

String NimatorModule::Test_LoadGIF()
{
  String s;

  Animation a;
  Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar-walk-l.gif", a.sbData_);
  a.Load();
  if (!s) { if (a.length() != 13) { s = "expected 13 frames"; } }
  if (!s) { if (a.nTotalDurationMSec_ != 1300) { s = "expected 1300 ms duration"; } }
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

//---------------------------

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Nimator", 0)) {
    AP_UNITTEST_REGISTER(NimatorModule::Test_Parse);
    AP_UNITTEST_REGISTER(NimatorModule::Test_LoadGIF);
  }
}

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Nimator", 0)) {
    AP_UNITTEST_EXECUTE(NimatorModule::Test_Parse);
    AP_UNITTEST_EXECUTE(NimatorModule::Test_LoadGIF);
  }
}

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

//#include "ximagif.h"
//#include "Image.h"

int NimatorModule::Init()
{
  int ok = 1;

  //CxImageGIF img;
  //Buffer sbSource;
  //Apollo::loadFile("tassadar-walk-l.gif", sbSource);
  //CxMemFile mfSource(sbSource.Data(), sbSource.Length());
  //img.Decode(&mfSource);

  //CxImage img;
  //String sFilename = "tassadar-walk-l.gif";
  //img.Load(sFilename);

  //Msg_Animation_Frame msg;
  //msg.iFrame.Allocate(img.GetWidth(), img.GetHeight());
  //CxMemFile mfDest((BYTE*) msg.iFrame.Pixels(), msg.iFrame.Size());
  //img.Encode2RGBA(&mfDest);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Start, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Stop, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_SetRate, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_SetData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_SetStatus, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Event, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(NimatorModule, this);

  return ok;
}

void NimatorModule::Exit()
{
  AP_UNITTEST_UNHOOK(NimatorModule, this);
  AP_MSG_REGISTRY_FINISH;
}
