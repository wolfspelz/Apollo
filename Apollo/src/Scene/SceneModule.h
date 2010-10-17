// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SceneModule_H_INCLUDED)
#define SceneModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgScene.h"
#include "Surface.h"
#include "SceneModuleTester.h"

typedef ApHandlePointerTree<Surface*> SurfaceList;
typedef ApHandlePointerTreeNode<Surface*> SurfaceListNode;
typedef ApHandlePointerTreeIterator<Surface*> SurfaceListIterator;

class SceneModule
{
public:
  SceneModule()
    {}

  int Init();
  void Exit();

  void On_Scene_Create(Msg_Scene_Create* pMsg);
  void On_Scene_Destroy(Msg_Scene_Destroy* pMsg);
  void On_Scene_Position(Msg_Scene_Position* pMsg);
  void On_Scene_Visibility(Msg_Scene_Visibility* pMsg);
  void On_Scene_SetAutoDraw(Msg_Scene_SetAutoDraw* pMsg);
  void On_Scene_AutoDraw(Msg_Scene_AutoDraw* pMsg);
  void On_Scene_Draw(Msg_Scene_Draw* pMsg);
  void On_Scene_CreateElement(Msg_Scene_CreateElement* pMsg);
  void On_Scene_DeleteElement(Msg_Scene_DeleteElement* pMsg);
  void On_Scene_CreateRectangle(Msg_Scene_CreateRectangle* pMsg);
  void On_Scene_TranslateElement(Msg_Scene_TranslateElement* pMsg);
  void On_Scene_GetTranslateElement(Msg_Scene_GetTranslateElement* pMsg);
  void On_Scene_ScaleElement(Msg_Scene_ScaleElement* pMsg);
  void On_Scene_HideElement(Msg_Scene_HideElement* pMsg);
  void On_Scene_RotateElement(Msg_Scene_RotateElement* pMsg);
  void On_Scene_ElementCopyMode(Msg_Scene_ElementCopyMode* pMsg);
  void On_Scene_CreateImageFromData(Msg_Scene_CreateImageFromData* pMsg);
  void On_Scene_CreateImageFromFile(Msg_Scene_CreateImageFromFile* pMsg);
  void On_Scene_CreateText(Msg_Scene_CreateText* pMsg);
  void On_Scene_SetCoordinates(Msg_Scene_SetCoordinates* pMsg);
  void On_Scene_SetRectangle(Msg_Scene_SetRectangle* pMsg);
  void On_Scene_SetFillColor(Msg_Scene_SetFillColor* pMsg);
  void On_Scene_SetStrokeColor(Msg_Scene_SetStrokeColor* pMsg);
  void On_Scene_SetStrokeImageFile(Msg_Scene_SetStrokeImageFile* pMsg);
  void On_Scene_SetFillImageFile(Msg_Scene_SetFillImageFile* pMsg);
  void On_Scene_SetStrokeImageOffset(Msg_Scene_SetStrokeImageOffset* pMsg);
  void On_Scene_SetFillImageOffset(Msg_Scene_SetFillImageOffset* pMsg);
  void On_Scene_SetStrokeWidth(Msg_Scene_SetStrokeWidth* pMsg);
  void On_Scene_SetImageData(Msg_Scene_SetImageData* pMsg);
  void On_Scene_DeleteImageData(Msg_Scene_DeleteImageData* pMsg);
  void On_Scene_SetImageFile(Msg_Scene_SetImageFile* pMsg);
  void On_Scene_DeleteImageFile(Msg_Scene_DeleteImageFile* pMsg);
  void On_Scene_SetImageAlpha(Msg_Scene_SetImageAlpha* pMsg);
  void On_Scene_MeasureText(Msg_Scene_MeasureText* pMsg);
  void On_Scene_GetImageSizeFromData(Msg_Scene_GetImageSizeFromData* pMsg);
  void On_Scene_GetImageSizeFromFile(Msg_Scene_GetImageSizeFromFile* pMsg);
  void On_Scene_CreateMouseSensor(Msg_Scene_CreateMouseSensor* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

protected:
  Surface* CreateSurface(const ApHandle& hSurface);
  Surface* FindSurface(const ApHandle& hSurface);
  void DeleteSurface(const ApHandle& hSurface);

protected:
  SurfaceList surfaces_;

public:

  AP_MSG_REGISTRY_DECLARE;

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

typedef ApModuleSingleton<SceneModule> SceneModuleInstance;

#endif // SceneModule_H_INCLUDED
