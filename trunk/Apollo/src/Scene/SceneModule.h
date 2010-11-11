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
#include "MsgTimer.h"
#include "Scene.h"
#include "SceneModuleTester.h"

typedef ApHandlePointerTree<Scene*> SceneList;
typedef ApHandlePointerTreeNode<Scene*> SceneListNode;
typedef ApHandlePointerTreeIterator<Scene*> SceneListIterator;

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
  void On_Scene_ElementExists(Msg_Scene_ElementExists* pMsg);
  void On_Scene_GetChildren(Msg_Scene_GetChildren* pMsg);
  void On_Scene_TranslateElement(Msg_Scene_TranslateElement* pMsg);
  void On_Scene_ScaleElement(Msg_Scene_ScaleElement* pMsg);
  void On_Scene_RotateElement(Msg_Scene_RotateElement* pMsg);
  void On_Scene_ShowElement(Msg_Scene_ShowElement* pMsg);
  void On_Scene_SetCopyMode(Msg_Scene_SetCopyMode* pMsg);
  void On_Scene_GetTranslateElement(Msg_Scene_GetTranslateElement* pMsg);
  void On_Scene_GetScaleElement(Msg_Scene_GetScaleElement* pMsg);
  void On_Scene_GetRotateElement(Msg_Scene_GetRotateElement* pMsg);
  void On_Scene_CreateMouseSensor(Msg_Scene_CreateMouseSensor* pMsg);
  void On_Scene_CreateRectangle(Msg_Scene_CreateRectangle* pMsg);
  void On_Scene_CreateImage(Msg_Scene_CreateImage* pMsg);
  void On_Scene_CreateImageFromData(Msg_Scene_CreateImageFromData* pMsg);
  void On_Scene_CreateImageFromFile(Msg_Scene_CreateImageFromFile* pMsg);
  void On_Scene_CreateText(Msg_Scene_CreateText* pMsg);
  void On_Scene_CreateTextElement(Msg_Scene_CreateTextElement* pMsg);
  void On_Scene_SetText(Msg_Scene_SetText* pMsg);
  void On_Scene_SetFontFamily(Msg_Scene_SetFontFamily* pMsg);
  void On_Scene_SetFontSize(Msg_Scene_SetFontSize* pMsg);
  void On_Scene_SetFontFlags(Msg_Scene_SetFontFlags* pMsg);
  void On_Scene_SetPosition(Msg_Scene_SetPosition* pMsg);
  void On_Scene_SetRectangle(Msg_Scene_SetRectangle* pMsg);
  void On_Scene_SetRoundedRectangle(Msg_Scene_SetRoundedRectangle* pMsg);
  void On_Scene_SetCurvedRectangle(Msg_Scene_SetCurvedRectangle* pMsg);
  void On_Scene_SetFillColor(Msg_Scene_SetFillColor* pMsg);
  void On_Scene_SetStrokeColor(Msg_Scene_SetStrokeColor* pMsg);
  void On_Scene_SetStrokeWidth(Msg_Scene_SetStrokeWidth* pMsg);
  void On_Scene_SetStrokeImageFile(Msg_Scene_SetStrokeImageFile* pMsg);
  void On_Scene_SetFillImageFile(Msg_Scene_SetFillImageFile* pMsg);
  void On_Scene_SetStrokeImageOffset(Msg_Scene_SetStrokeImageOffset* pMsg);
  void On_Scene_SetFillImageOffset(Msg_Scene_SetFillImageOffset* pMsg);
  void On_Scene_SetImageData(Msg_Scene_SetImageData* pMsg);
  void On_Scene_DeleteImageData(Msg_Scene_DeleteImageData* pMsg);
  void On_Scene_SetImageFile(Msg_Scene_SetImageFile* pMsg);
  void On_Scene_DeleteImageFile(Msg_Scene_DeleteImageFile* pMsg);
  void On_Scene_SetImageAlpha(Msg_Scene_SetImageAlpha* pMsg);
  void On_Scene_GetTextExtents(Msg_Scene_GetTextExtents* pMsg);
  void On_Scene_MeasureText(Msg_Scene_MeasureText* pMsg);
  void On_Scene_GetImageSizeFromData(Msg_Scene_GetImageSizeFromData* pMsg);
  void On_Scene_GetImageSizeFromFile(Msg_Scene_GetImageSizeFromFile* pMsg);
  void On_Scene_CaptureMouse(Msg_Scene_CaptureMouse* pMsg);
  void On_Scene_ReleaseMouse(Msg_Scene_ReleaseMouse* pMsg);
  void On_Timer_Event(Msg_Timer_Event* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

protected:
  Scene* CreateScene(const ApHandle& hScene);
  void DeleteScene(const ApHandle& hScene);
  Scene* FindScene(const ApHandle& hScene);
  int HasScene(const ApHandle& hScene);

protected:
  SceneList scenes_;

public:

  AP_MSG_REGISTRY_DECLARE;

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

typedef ApModuleSingleton<SceneModule> SceneModuleInstance;

#endif // SceneModule_H_INCLUDED
