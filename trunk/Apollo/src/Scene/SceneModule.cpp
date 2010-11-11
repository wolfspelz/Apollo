// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgConfig.h"
#include "Local.h"
#include "SceneModule.h"

Scene* SceneModule::CreateScene(const ApHandle& hScene)
{
  Scene* pScene = new Scene(hScene);
  if (pScene) {
    int ok = pScene->Create();
    if (ok) {
      scenes_.Set(hScene, pScene);
    } else {
      delete pScene;
      pScene = 0;
      throw ApException("SceneModule::CreateScene " ApHandleFormat " Create() failed", ApHandleType(hScene));
    }
  }

  return pScene;
}

void SceneModule::DeleteScene(const ApHandle& hScene)
{
  Scene* pScene = FindScene(hScene);
  if (pScene) {
    pScene->Destroy();
    scenes_.Unset(hScene);
    delete pScene;
    pScene = 0;
  }
}

Scene* SceneModule::FindScene(const ApHandle& hScene)
{
  Scene* pScene = 0;  

  scenes_.Get(hScene, pScene);
  if (pScene == 0) { throw ApException("SceneModule::FindScene no scene=" ApHandleFormat "", ApHandleType(hScene)); }

  return pScene;
}

int SceneModule::HasScene(const ApHandle& hScene)
{
  return scenes_.IsSet(hScene);
}

//---------------------------

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Create)
{
  if (scenes_.Find(pMsg->hScene) != 0) { throw ApException("SceneModule::Scene_Create: scene=" ApHandleFormat " already exists", ApHandleType(pMsg->hScene)); }
  Scene* pScene = CreateScene(pMsg->hScene);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Destroy)
{
  Scene* pScene = FindScene(pMsg->hScene);
  DeleteScene(pMsg->hScene); 
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Position)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->SetPosition(pMsg->nX, pMsg->nY, pMsg->nW, pMsg->nH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Visibility)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->SetVisibility(pMsg->bVisible);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetAutoDraw)
{
  Scene* pScene = FindScene(pMsg->hScene);
  if (pMsg->nMilliSec == Msg_Scene_SetAutoDraw::Disabled) {
    pScene->DeleteAutoDraw();
  } else {
    pScene->SetAutoDraw(pMsg->nMilliSec, pMsg->bAsync);
  }
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_AutoDraw)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_Draw)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->Draw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->CreateElement(pMsg->sPath);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_DeleteElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->DeleteElement(pMsg->sPath);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_ElementExists)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pMsg->bExists = pScene->HasElement(pMsg->sPath);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetChildren)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->GetChildren(pMsg->vlChildren);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_TranslateElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->Translate(pMsg->fX, pMsg->fY);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_ScaleElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->Scale(pMsg->fX, pMsg->fY);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_RotateElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->Rotate(pMsg->fAngle);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_ShowElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->Show(pMsg->bShow);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetCopyMode)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->CopyMode(pMsg->nMode);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetTranslateElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->GetTranslate(pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetScaleElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->GetScale(pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetRotateElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetElement(pMsg->sPath)->GetRotate(pMsg->fAngle);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateMouseSensor)
{
  Scene* pScene = FindScene(pMsg->hScene);
  SensorElement* pSensor = pScene->CreateSensor(pMsg->sPath);
  pSensor->SetPosition(pMsg->fX, pMsg->fY);
  pSensor->SetSize(pMsg->fW, pMsg->fH);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateRectangle)
{
  Scene* pScene = FindScene(pMsg->hScene);
  RectangleElement* pRectangle = pScene->CreateRectangle(pMsg->sPath);
  pRectangle->SetPosition(pMsg->fX, pMsg->fY);
  pRectangle->SetSize(pMsg->fW, pMsg->fH);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateImage)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ImageElement* pImage = pScene->CreateImage(pMsg->sPath);
  pImage->SetPosition(pMsg->fX, pMsg->fY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateImageFromData)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ImageElement* pImage = pScene->CreateImage(pMsg->sPath);
  pImage->SetData(pMsg->image);
  pImage->SetPosition(pMsg->fX, pMsg->fY);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateImageFromFile)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ImageElement* pImage = pScene->CreateImage(pMsg->sPath);
  pImage->SetFile(pMsg->sFile);
  pImage->SetPosition(pMsg->fX, pMsg->fY);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateText)
{
  Scene* pScene = FindScene(pMsg->hScene);
  TextElement* pText = pScene->CreateText(pMsg->sPath);
  pText->SetPosition(pMsg->fX, pMsg->fY);
  pText->SetString(pMsg->sText);
  pText->SetFont(pMsg->sFont);
  pText->SetSize(pMsg->fSize);
  pText->SetFlags(pMsg->nFlags);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CreateTextElement)
{
  Scene* pScene = FindScene(pMsg->hScene);
  TextElement* pText = pScene->CreateText(pMsg->sPath);
  //pText->Hide(1);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetText)
{
  Scene* pScene = FindScene(pMsg->hScene);
  TextElement* pText = pScene->GetElement(pMsg->sPath)->AsText();
  pText->SetString(pMsg->sText);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFontFamily)
{
  Scene* pScene = FindScene(pMsg->hScene);
  TextElement* pText = pScene->GetElement(pMsg->sPath)->AsText();
  pText->SetFont(pMsg->sFont);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFontSize)
{
  Scene* pScene = FindScene(pMsg->hScene);
  TextElement* pText = pScene->GetElement(pMsg->sPath)->AsText();
  pText->SetSize(pMsg->fSize);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFontFlags)
{
  Scene* pScene = FindScene(pMsg->hScene);
  TextElement* pText = pScene->GetElement(pMsg->sPath)->AsText();
  pText->SetFlags(pMsg->nFlags);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetPosition)
{
  Scene* pScene = FindScene(pMsg->hScene);
  Element* pElement = pScene->GetElement(pMsg->sPath);
  if (pElement->IsShape()) {
    pElement->AsShape()->SetPosition(pMsg->fX, pMsg->fY);
  } else if (pElement->IsImage()) {
    pElement->AsImage()->SetPosition(pMsg->fX, pMsg->fY);
  }
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetRectangle)
{
  Scene* pScene = FindScene(pMsg->hScene);
  RectangleElement* pRectangle = pScene->GetElement(pMsg->sPath)->AsRectangle();
  pRectangle->SetPosition(pMsg->fX, pMsg->fY);
  pRectangle->SetSize(pMsg->fW, pMsg->fH);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetRoundedRectangle)
{
  Scene* pScene = FindScene(pMsg->hScene);
  RectangleElement* pRectangle = pScene->GetElement(pMsg->sPath)->AsRectangle();
  pRectangle->SetCorners(RectangleElement::RoundCorners);
  pRectangle->SetCornerRadius(pMsg->fRadius);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetCurvedRectangle)
{
  Scene* pScene = FindScene(pMsg->hScene);
  RectangleElement* pRectangle = pScene->GetElement(pMsg->sPath)->AsRectangle();
  pRectangle->SetCorners(RectangleElement::CurvedCorners);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFillColor)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ShapeElement* pShape = pScene->GetElement(pMsg->sPath)->AsShape();
  pShape->SetFillColor(pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeColor)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ShapeElement* pShape = pScene->GetElement(pMsg->sPath)->AsShape();
  pShape->SetStrokeColor(pMsg->fRed, pMsg->fGreen, pMsg->fBlue, pMsg->fAlpha);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeWidth)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ShapeElement* pShape = pScene->GetElement(pMsg->sPath)->AsShape();
  pShape->SetStrokeWidth(pMsg->fWidth);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeImageFile)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ShapeElement* pShape = pScene->GetElement(pMsg->sPath)->AsShape();
  pShape->SetStrokeImageFile(pMsg->sFile);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFillImageFile)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ShapeElement* pShape = pScene->GetElement(pMsg->sPath)->AsShape();
  pShape->SetFillImageFile(pMsg->sFile);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetStrokeImageOffset)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ShapeElement* pShape = pScene->GetElement(pMsg->sPath)->AsShape();
  pShape->SetStrokeImageOffset(pMsg->fX, pMsg->fY);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetFillImageOffset)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ShapeElement* pShape = pScene->GetElement(pMsg->sPath)->AsShape();
  pShape->SetFillImageOffset(pMsg->fX, pMsg->fY);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetImageData)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ImageElement* pImage = pScene->GetElement(pMsg->sPath)->AsImage();
  pImage->SetData(pMsg->image);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_DeleteImageData)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ImageElement* pImage = pScene->GetElement(pMsg->sPath)->AsImage();
  pImage->DeleteData();
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetImageFile)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ImageElement* pImage = pScene->GetElement(pMsg->sPath)->AsImage();
  pImage->SetFile(pMsg->sFile);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_DeleteImageFile)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ImageElement* pImage = pScene->GetElement(pMsg->sPath)->AsImage();
  pImage->DeleteFile();
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_SetImageAlpha)
{
  Scene* pScene = FindScene(pMsg->hScene);
  ImageElement* pImage = pScene->GetElement(pMsg->sPath)->AsImage();
  pImage->SetAlpha(pMsg->fAlpha);
  pScene->AutoDraw();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetTextExtents)
{
  Scene* pScene = FindScene(pMsg->hScene);

  TextExtents te;
  pScene->GetTextExtents(pMsg->sText, pMsg->sFont, pMsg->fSize, pMsg->nFlags, te);
  pMsg->fBearingX = te.fBearingX_;
  pMsg->fBearingY = te.fBearingY_;
  pMsg->fWidth = te.fWidth_;
  pMsg->fHeight = te.fHeight_;
  pMsg->fAdvanceX = te.fAdvanceX_;
  pMsg->fAdvanceY = te.fAdvanceY_;
 
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_MeasureText)
{
  Scene* pScene = FindScene(pMsg->hScene);
  TextElement* pText = pScene->GetElement(pMsg->sPath)->AsText();

  String sFont = pText->GetFont();
  double fSize = pText->GetSize();

  if (sFont.empty() || fSize == 0.0) { throw ApException("SceneModule::Scene_GetTextExtents missing font or size scene=" ApHandleFormat " path=%s", ApHandleType(pMsg->hScene), StringType(pMsg->sPath)); }

  TextExtents te;
  pScene->GetTextExtents(pText->GetString(), sFont, fSize, pText->GetFlags(), te);
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
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetImageSizeFromData(pMsg->image, pMsg->fW, pMsg->fH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_GetImageSizeFromFile)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->GetImageSizeFromFile(pMsg->sFile, pMsg->fW, pMsg->fH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_CaptureMouse)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->CaptureMouse(pMsg->sPath);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Scene_ReleaseMouse)
{
  Scene* pScene = FindScene(pMsg->hScene);
  pScene->ReleaseMouse();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(SceneModule, Timer_Event)
{
  // Find scene by timer handle, because scene handle misused as timer handle
  if (HasScene(pMsg->hTimer)) {
    Scene* pScene = FindScene(pMsg->hTimer);
    pScene->OnAutoDrawTimer();
  }
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_ElementExists, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetChildren, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateRectangle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_TranslateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_ScaleElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_RotateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_ShowElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetCopyMode, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetTranslateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetScaleElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetRotateElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateMouseSensor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateImage, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateImageFromData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateImageFromFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CreateTextElement, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFontFamily, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFontSize, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFontFlags, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetRectangle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetRoundedRectangle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetCurvedRectangle, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFillColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeColor, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeWidth, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFillImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetStrokeImageOffset, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetFillImageOffset, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetImageData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_DeleteImageData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_DeleteImageFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_SetImageAlpha, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetTextExtents, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_MeasureText, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetImageSizeFromData, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_GetImageSizeFromFile, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_CaptureMouse, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Scene_ReleaseMouse, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, SceneModule, Timer_Event, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(SceneModule, this);

  return ok;
}

void SceneModule::Exit()
{
  AP_UNITTEST_UNHOOK(SceneModule, this);
  AP_MSG_REGISTRY_FINISH;
}
