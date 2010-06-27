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
#include "NimatorModuleTester.h"

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

  pItem->SetData(pMsg->sbData, pMsg->sSourceUrl);

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

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_SetCondition)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  pItem->SetCondition(pMsg->sCondition);

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

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_SetPosition)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  pItem->SetPosition(pMsg->nX);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_MoveTo)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  pItem->MoveTo(pMsg->nX);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Animation_GetPosition)
{
  ItemListNode* pNode = items_.Find(pMsg->hItem);
  if (pNode == 0) { return; }
  Item* pItem = pNode->Value();

  //pItem->GetPosition();

  pMsg->apStatus = ApMessage::Ok;
}

Item* NimatorModule::GetItemByTimer(ApHandle hTimer)
{
  Item* pResult = 0;

  ItemListIterator iter(items_);
  for (ItemListNode* pNode = 0; pNode = iter.Next(); ) {
    Item* pItem = pNode->Value();
    if (pItem != 0) {
      if (pItem->HasTimer(hTimer)) {
        pResult = pItem;
        break;
      }
    }
  }
  
  return pResult;
}

AP_MSG_HANDLER_METHOD(NimatorModule, Timer_Event)
{
  Item* pItem = GetItemByTimer(pMsg->hTimer);
  if (pItem == 0) { return; }

  pItem->OnTimer();
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  NimatorModuleTester::Begin();
}

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  NimatorModuleTester::Execute();
}

AP_MSG_HANDLER_METHOD(NimatorModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  NimatorModuleTester::End();
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
  //Apollo::loadFile("tassadar/walk-l.gif", sbSource);
  //CxMemFile mfSource(sbSource.Data(), sbSource.Length());
  //img.Decode(&mfSource);

  //CxImage img;
  //String sFilename = "tassadar/walk-l.gif";
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_SetCondition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_Event, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_SetPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_MoveTo, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Animation_GetPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, NimatorModule, Timer_Event, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(NimatorModule, this);

  return ok;
}

void NimatorModule::Exit()
{
  AP_UNITTEST_UNHOOK(NimatorModule, this);
  AP_MSG_REGISTRY_FINISH;
}
