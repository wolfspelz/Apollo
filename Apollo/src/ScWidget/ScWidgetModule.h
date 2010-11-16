// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ScWidgetModule_H_INCLUDED)
#define ScWidgetModule_H_INCLUDED

#include "ApModule.h"
#include "ApContainer.h"
#include "MsgUnitTest.h"
#include "MsgScene.h"
#include "MsgScWidget.h"
#include "ScWidgetModuleTester.h"

class Widget;

typedef StringPointerTree<Widget*> WidgetList;
typedef StringPointerTreeNode<Widget*> WidgetListNode;
typedef StringPointerTreeIterator<Widget*> WidgetListIterator;

class Scene
{
public:
  Scene() {}
  virtual ~Scene() {}

  void AddWidget(const String& sPath, Widget* pWidget);
  void DeleteWidget(const String& sPath);
  Widget* FindWidget(const String& sPath);
  int HasWidget(const String& sPath);

protected:
  WidgetList widgets_;
};

typedef ApHandlePointerTree<Scene*> SceneList;
typedef ApHandlePointerTreeNode<Scene*> SceneListNode;
typedef ApHandlePointerTreeIterator<Scene*> SceneListIterator;

class ScWidgetModule
{
public:
  ScWidgetModule() {}
  virtual ~ScWidgetModule() {}

  int Init();
  void Exit();

  void On_ScWidget_CreateButton(Msg_ScWidget_CreateButton* pMsg);
  void On_ScWidget_SetCoordinates(Msg_ScWidget_SetCoordinates* pMsg);
  void On_ScWidget_SetButtonState(Msg_ScWidget_SetButtonState* pMsg);
  void On_ScWidget_SetButtonActive(Msg_ScWidget_SetButtonActive* pMsg);
  void On_ScWidget_SetButtonText(Msg_ScWidget_SetButtonText* pMsg);
  void On_ScWidget_DeleteButtonText(Msg_ScWidget_DeleteButtonText* pMsg);
  void On_ScWidget_SetButtonTextFont(Msg_ScWidget_SetButtonTextFont* pMsg);
  void On_ScWidget_SetButtonTextSize(Msg_ScWidget_SetButtonTextSize* pMsg);
  void On_ScWidget_SetButtonTextFlags(Msg_ScWidget_SetButtonTextFlags* pMsg);
  void On_ScWidget_SetButtonTextColor(Msg_ScWidget_SetButtonTextColor* pMsg);
  void On_ScWidget_SetButtonImageFile(Msg_ScWidget_SetButtonImageFile* pMsg);
  void On_ScWidget_SetButtonTextOffset(Msg_ScWidget_SetButtonTextOffset* pMsg);
  void On_ScWidget_CreateEdit(Msg_ScWidget_CreateEdit* pMsg);
  void On_ScWidget_SetEditText(Msg_ScWidget_SetEditText* pMsg);
  void On_ScWidget_GetEditText(Msg_ScWidget_GetEditText* pMsg);
  void On_ScWidget_SetEditAlign(Msg_ScWidget_SetEditAlign* pMsg);
  void On_ScWidget_SetEditPadding(Msg_ScWidget_SetEditPadding* pMsg);
  void On_ScWidget_SetEditTextFont(Msg_ScWidget_SetEditTextFont* pMsg);
  void On_ScWidget_SetEditTextSize(Msg_ScWidget_SetEditTextSize* pMsg);
  void On_ScWidget_SetEditTextFlags(Msg_ScWidget_SetEditTextFlags* pMsg);
  void On_ScWidget_SetEditTextColor(Msg_ScWidget_SetEditTextColor* pMsg);
  void On_Scene_DeleteElement(Msg_Scene_DeleteElement* pMsg);
  void On_Scene_MouseEvent(Msg_Scene_MouseEvent* pMsg);
  void On_Scene_KeyEvent(Msg_Scene_KeyEvent* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

protected:
  Scene* CreateScene(const ApHandle& hScene);
  void DeleteScene(const ApHandle& hScene);
  Scene* FindScene(const ApHandle& hScene);
  Scene* FindOrCreateScene(const ApHandle& hScene);
  int HasScene(const ApHandle& hScene);
  Widget* FindWidget(const ApHandle& hScene, const String& sPath);

protected:
  SceneList scenes_;

public:

  AP_MSG_REGISTRY_DECLARE;

#if defined(AP_TEST)
  friend class ScWidgetModuleTester;
#endif
};

typedef ApModuleSingleton<ScWidgetModule> ScWidgetModuleInstance;

#endif // ScWidgetModule_H_INCLUDED
