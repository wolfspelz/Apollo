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

class Button;

class Widget
{
public:
  Widget(const ApHandle& hScene, const String& sPath)
    :hScene_(hScene)
    ,sPath_(sPath)
  {}
  virtual ~Widget() {}

  inline int IsButton() { return 0; }

  virtual void Delete() {}
  virtual void SetCoordinates(double fX, double fY, double fW, double fH);
  Button* AsButton();

  virtual void OnMouseEvent(Msg_Scene_MouseEvent* pMsg) {}

protected:
  ApHandle hScene_;
  String sPath_;

  double fX_;
  double fY_;
  double fW_;
  double fH_;
};

class ButtonStateConfig
{
public:
  ButtonStateConfig()
    :fX_(0.0)
    ,fY_(0.0)
  {}

  String sFile_;
  double fX_;
  double fY_;
};

typedef Tree<int, ButtonStateConfig, LessThan<int>> ButtonStateConfigList;
typedef TreeNode<int, ButtonStateConfig> ButtonStateConfigListNode;
typedef TreeIterator<int, ButtonStateConfig, LessThan<int>> ButtonStateConfigListIterator;

class Button: public Widget
{
public:
  Button(const ApHandle& hScene, const String& sPath);
  virtual ~Button() {}

  typedef enum _ButtonState { NoButtonState
    ,NormalButtonState
    ,DownButtonState
    ,HighButtonState
    ,DisabledButtonState
    ,LastButtonState
  } ButtonState;

  inline int IsButton() { return 1; }

  static ButtonState String2State(const String& sName);

  void Create();
  void Active(int bActive);
  void SetState(ButtonState nState);
  void SetText(const String& sText);
  void DeleteText();
  void SetFontFamily(const String& sFont);
  void SetFontSize(double fSize);
  void SetFontFlags(int nFlags);
  void SetFontColor(double fRed, double fGreen, double fBlue, double fAlpha);
  void SetImageFile(ButtonState nState, const String& sFile, double fX, double fY);
  String GetTextPath();

  void OnMouseEvent(Msg_Scene_MouseEvent* pMsg);

protected:
  void ShowState();
  void PositionText();
  void DoClick();
  void GetTextPos(double& fTextX, double& fTextY);

protected:
  ButtonStateConfigList states_;
  ButtonState nState_;
  int bActive_;
  int bMouseDown_;

  int bHasText_;
  double fTextOffsetX_;
  double fTextOffsetY_;
};

typedef StringPointerTree<Widget*> WidgetList;
typedef StringPointerTreeNode<Widget*> WidgetListNode;
typedef StringPointerTreeIterator<Widget*> WidgetListIterator;

class Scene
{
public:
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
  ScWidgetModule()
    {}

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
  void On_Scene_DeleteElement(Msg_Scene_DeleteElement* pMsg);
  void On_Scene_MouseEvent(Msg_Scene_MouseEvent* pMsg);

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
