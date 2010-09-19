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

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Rectangle)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  if (pSurface == 0) { throw ApException("SceneModule::Scene_Rectangle: FindSurface(" ApHandleFormat ") failed", ApHandleType(pMsg->hScene)); }

  pSurface->SetRectangle(
    pMsg->sPath,
    pMsg->bFill, pMsg->fFillRed, pMsg->fFillGreen, pMsg->fFillBlue, pMsg->fFillAlpha,
    pMsg->bStroke, pMsg->fStrokeWidth, pMsg->fStrokeRed, pMsg->fStrokeGreen, pMsg->fStrokeBlue, pMsg->fStrokeAlpha,
    pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH
  );

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Draw)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  if (pSurface == 0) { throw ApException("SceneModule::Scene_Draw: FindSurface(" ApHandleFormat ") failed", ApHandleType(pMsg->hScene)); }

  pSurface->Draw();

  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
    AP_UNITTEST_REGISTER(TestRectangle);
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
    AP_UNITTEST_EXECUTE(TestRectangle);
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

String SceneModule::TestRectangle()
{
  ApHandle hScene = Apollo::newHandle();

  {
    Msg_Scene_Create msg;
    msg.hScene = hScene;
    msg.Request();
  }

  {
    Msg_Scene_Position msg;
    msg.hScene = hScene;
    msg.nX = 100;
    msg.nY = 100;
    msg.nW = 300;
    msg.nH = 300;
    msg.Request();
  }

  {
    Msg_Scene_Visibility msg;
    msg.hScene = hScene;
    msg.bVisible = 1;
    msg.Request();
  }

  {
    Msg_Scene_Rectangle msg;
    msg.hScene = hScene;
    msg.sPath = "/rect1";
    msg.fX = 0;
    msg.fY = 0;
    msg.fW = 100;
    msg.fH = 100;
    msg.bFill = 1;
    msg.fFillRed = 1;
    msg.fFillGreen = 0;
    msg.fFillBlue = 0;
    msg.fFillAlpha = 0.5;
    //msg.bStroke = 0;
    //msg.fStrokeWidth = 3;
    //msg.fStrokeRed = 0;
    //msg.fStrokeGreen = 0;
    //msg.fStrokeBlue = 1;
    //msg.fStrokeAlpha = 0.5;
    msg.Request();
  }

  {
    Msg_Scene_Draw msg;
    msg.hScene = hScene;
    msg.Request();
  }

  if (0) {
    Msg_Scene_Destroy msg;
    msg.Request();
  }

  return "";
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Draw, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(SceneModule, this);

  return ok;
}

void SceneModule::Exit()
{
  AP_UNITTEST_UNHOOK(SceneModule, this);
  AP_MSG_REGISTRY_FINISH;
}
