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
    if (ok ) {
      surfaces_.Set(hSurface, pSurface);
    } else {
      delete pSurface;
      pSurface = 0;
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
  return pSurface;
}

//---------------------------

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Create)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  if (pSurface != 0) { throw ApException("SceneModule::Scene_Create: scene=" ApHandleFormat " already exists", ApHandleType(pMsg->hScene)); }

  pSurface = CreateSurface(pMsg->hScene);
  if (pSurface == 0) { throw ApException("SceneModule::Scene_Create: CreateSurface(" ApHandleFormat ") failed", ApHandleType(pMsg->hScene)); }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Destroy)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  if (pSurface == 0) { throw ApException("SceneModule::Scene_Destroy: FindSurface(" ApHandleFormat ") failed", ApHandleType(pMsg->hScene)); }

  if (pSurface) { 
    DeleteSurface(pMsg->hScene); 
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Position)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  if (pSurface == 0) { throw ApException("SceneModule::Scene_Position: FindSurface(" ApHandleFormat ") failed", ApHandleType(pMsg->hScene)); }

  pSurface->SetPosition(pMsg->nX, pMsg->nY, pMsg->nW, pMsg->nH);

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Visibility)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  if (pSurface == 0) { throw ApException("SceneModule::Scene_Visibility: FindSurface(" ApHandleFormat ") failed", ApHandleType(pMsg->hScene)); }

  pSurface->SetVisibility(pMsg->bVisible);

  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
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

  AP_UNITTEST_HOOK(SceneModule, this);

  return ok;
}

void SceneModule::Exit()
{
  AP_UNITTEST_UNHOOK(SceneModule, this);
  AP_MSG_REGISTRY_FINISH;
}
