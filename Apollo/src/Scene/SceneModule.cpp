// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgConfig.h"
#include "Local.h"
#include "SceneModule.h"

Surface* SceneModule::CreateSurface(const ApHandle& hSurface)
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

void SceneModule::DeleteSurface(const ApHandle& hSurface)
{
  Surface* pSurface = FindSurface(hSurface);
  if (pSurface) {
    pSurface->Destroy();
    surfaces_.Unset(hSurface);
    delete pSurface;
    pSurface = 0;
  }
}

Surface* SceneModule::FindSurface(const ApHandle& hSurface)
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

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetAutoDraw)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  if (pMsg->nMilliSec == Msg_Scene_SetAutoDraw::Disabled) {
    pSurface->DeleteAutoDraw();
  } else {
    pSurface->SetAutoDraw(pMsg->nMilliSec, pMsg->bAsync);
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_AutoDraw)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Draw)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->Draw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->CreateElement(pMsg->sPath);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_DeleteElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->DeleteElement(pMsg->sPath);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_TranslateElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->Translate(pMsg->fX, pMsg->fY);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetTranslateElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->GetTranslate(pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_ScaleElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->Scale(pMsg->fX, pMsg->fY);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_RotateElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->Rotate(pMsg->fAngle);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_ElementCopyMode)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->CopyMode(pMsg->nMode);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_HideElement)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->Hide(pMsg->bHide);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateRectangle)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->CreateElement(pMsg->sPath);
  pSurface->FindElement(pMsg->sPath)->CreateRectangle(pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateImageFromData)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->CreateElement(pMsg->sPath);
  pSurface->FindElement(pMsg->sPath)->CreateImageFromData(pMsg->fX, pMsg->fY, pMsg->image);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateImageFromFile)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->CreateElement(pMsg->sPath);
  pSurface->FindElement(pMsg->sPath)->CreateImageFromFile(pMsg->fX, pMsg->fY, pMsg->sFile);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateText)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->CreateElement(pMsg->sPath);
  pSurface->FindElement(pMsg->sPath)->CreateText(pMsg->fX, pMsg->fY, pMsg->sText, pMsg->sFont, pMsg->fSize, pMsg->nFlags);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetCoordinates)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetCoordinates(pMsg->fX, pMsg->fY);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetRectangle)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetRectangle(pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFillColor)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetFillColor(pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeColor)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetStrokeColor(pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeImageFile)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetStrokeImageFile(pMsg->sFile);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFillImageFile)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetFillImageFile(pMsg->sFile);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeImageOffset)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetStrokeImageOffset(pMsg->fX, pMsg->fY);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFillImageOffset)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetFillImageOffset(pMsg->fX, pMsg->fY);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeWidth)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetStrokeWidth(pMsg->fWidth);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetImageData)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetImageData(pMsg->image);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_DeleteImageData)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->DeleteImageData();
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetImageFile)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetImageFile(pMsg->sFile);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_DeleteImageFile)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->DeleteImageFile();
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetImageAlpha)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->FindElement(pMsg->sPath)->SetImageAlpha(pMsg->fAlpha);
  pSurface->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_MeasureText)
{
  Surface* pSurface = FindSurface(pMsg->hScene);

  TextExtents te;
  pSurface->MeasureText(pMsg->sText, pMsg->sFont, pMsg->fSize, pMsg->nFlags, te);
  pMsg->fBearingX = te.fBearingX_;
  pMsg->fBearingY = te.fBearingY_;
  pMsg->fWidth = te.fWidth_;
  pMsg->fHeight = te.fHeight_;
  pMsg->fAdvanceX = te.fAdvanceX_;
  pMsg->fAdvanceY = te.fAdvanceY_;
 
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetImageSizeFromData)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->GetImageSizeFromData(pMsg->image, pMsg->fW, pMsg->fH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetImageSizeFromFile)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->GetImageSizeFromFile(pMsg->sFile, pMsg->fW, pMsg->fH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateMouseSensor)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->CreateElement(pMsg->sPath);
  pSurface->FindElement(pMsg->sPath)->CreateMouseSensor(pMsg->sPath, pMsg->fX, pMsg->fY, pMsg->fW, pMsg->fH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CaptureMouse)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->CaptureMouse(pMsg->sPath);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_ReleaseMouse)
{
  Surface* pSurface = FindSurface(pMsg->hScene);
  pSurface->ReleaseMouse();
  pMsg->apStatus = ApMessage::Ok;
}

//----------------------------------------------------------

#if defined(AP_TEST)

#include "SceneModuleTester.h"

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
    SceneModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
    SceneModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(SceneModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Scene", 0)) {
    SceneModuleTester::End();
  }
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetAutoDraw, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_AutoDraw, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_Draw, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_DeleteElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_TranslateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetTranslateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_ScaleElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_RotateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_ElementCopyMode, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_HideElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateRectangle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateMouseSensor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateImageFromData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateImageFromFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetCoordinates, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetRectangle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFillColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFillImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeImageOffset, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFillImageOffset, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeWidth, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetImageData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_DeleteImageData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_DeleteImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetImageAlpha, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_MeasureText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetImageSizeFromData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetImageSizeFromFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CaptureMouse, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_ReleaseMouse, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(SceneModule, this);

  return ok;
}

void SceneModule::Exit()
{
  AP_UNITTEST_UNHOOK(SceneModule, this);
  AP_MSG_REGISTRY_FINISH;
}
