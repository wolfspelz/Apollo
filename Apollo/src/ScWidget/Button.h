// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(´Button_H_INCLUDED)
#define ´Button_H_INCLUDED

#include "Widget.h"

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

#endif // ´Button_H_INCLUDED
