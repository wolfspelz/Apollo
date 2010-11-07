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
#include "MsgScWidget.h"
#include "ScWidgetModuleTester.h"

class Widget
{
public:
  Widget(const String& sPath)
    :sPath_(sPath)
  {}

protected:
  String sPath_;
};

typedef StringPointerTree<Widget*> WidgetList;
typedef StringPointerTreeNode<Widget*> WidgetListNode;
typedef StringPointerTreeIterator<Widget*> WidgetListIterator;

class Scene
{
public:
  Scene(const ApHandle& hScene)
    :hScene_(hScene)
  {}

protected:
  ApHandle hScene_;
  WidgetList widgets_;
};

typedef ApHandlePointerTree<Scene*> SceneList;
typedef ApHandlePointerTreeNode<Scene*> SceneListNode;
typedef ApHandlePointerTreeIterator<Scene*> SceneListIterator;

class ScWidgetModule
{
public:
  ScWidgetModule()
    {}

  int Init();
  void Exit();

  void On_ScWidget_CreateButton(Msg_ScWidget_CreateButton* pMsg);

#if defined(AP_TEST)
  void On_UnitTest_Begin(Msg_UnitTest_Begin* pMsg);
  void On_UnitTest_Execute(Msg_UnitTest_Execute* pMsg);
  void On_UnitTest_End(Msg_UnitTest_End* pMsg);
#endif

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
