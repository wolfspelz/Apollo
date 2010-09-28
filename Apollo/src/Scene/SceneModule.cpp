// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgConfig.h"
#include "Local.h"
#include "SceneModule.h"

Surface* SceneModule::CreateSurface(ApHandle hSurface)
{
  Surface* pSurface = new Surface(hSurface);
  if (pSurface) {
    int ok = pSurface->Create();
    if (ok) {
      surfaces_.Set(hSurface, pSurface);
    } else {
      delete pSurface;
      pSurface = 0;
      throw ApException("SceneModule::CreateSurface " ApHandleFormat " Create() failed", ApHandleType(hSurface));
    }
  }

  return pSurface;
}

void SceneModule::DeleteSurface(ApHandle hSurface)
{
  Surface* pSurface = FindSurface(hSurface);
  if (pSurface) {
    pSurface->Destroy();
    surfaces_.Unset(hSurface);
    delete pSurface;
    pSurface = 0;
  }
}

Surface* SceneModule::FindSurface(ApHandle hSurface)
{
  Surface* pSurface = 0;  

  surfaces_.Get(hSurface, pSurface);
  if (pSurface == 0) { throw ApException("SceneModule::FindSurface no surface=" ApHandleFormat "", ApHandleType(hSurface)); }

  return pSurface;
}

//---------------------------

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Create)
{
  if (surfaces_.Find(pMsg->hScene) != 0) { throw ApException("SceneModule::Scene_Create: scene=" ApHandleFormat " already exists", ApHandleType(pMsg->hScene)); }
  Surface* pSurface = CreateSurface(pMsg->hScene);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Destroy)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  DeleteSurface(pMsg->hScene); 
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Position)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->SetPosition(pMsg->nX, pMsg->nY, pMsg->nW, pMsg->nH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Visibility)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->SetVisibility(pMsg->bVisible);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Rectangle)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->Rectangle(pMsg->sPath, pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFillColor)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->SetFillColor(pMsg->sPath, pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeColor)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->SetStrokeColor(pMsg->sPath, pMsg->fWidth, pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_DeleteElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->DeleteElement(pMsg->sPath);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Draw)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->Draw();
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

#include "SceneModuleTester.h"

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
//    AP_UNITTEST_REGISTER(SceneModuleTester::Rectangle);
    AP_UNITTEST_REGISTER(SceneModuleTester::ElementTree);
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
//    AP_UNITTEST_EXECUTE(SceneModuleTester::Rectangle);
    AP_UNITTEST_EXECUTE(SceneModuleTester::ElementTree);
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int SceneModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Position, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Visibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Rectangle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFillColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_DeleteElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Draw, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(SceneModule, this);

  return ok;
}

void SceneModule::Exit()
{
  AP_UNITTEST_UNHOOK(SceneModule, this);
  AP_MSG_REGISTRY_FINISH;
}