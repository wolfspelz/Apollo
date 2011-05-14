// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Widget_H_INCLUDED)
#define Widget_H_INCLUDED

#include "MsgScene.h"

class Button;
class Edit;

class Widget
{
public:
  Widget(const ApHandle& hScene, const String& sPath)
    :hScene_(hScene)
    ,sPath_(sPath)
  {}
  virtual ~Widget() {}

  inline int IsButton() { return 0; }
  inline int IsEdit() { return 0; }

  Button* AsButton();
  Edit* AsEdit();

  virtual void Delete() {}
  virtual void SetCoordinates(double fX, double fY, double fW, double fH);

  virtual void OnMouseEvent(Msg_Scene_MouseEvent* pMsg) {}
  virtual void OnKeyEvent(Msg_Scene_KeyEvent* pMsg) {}
  virtual void OnKeyboardFocusEvent(Msg_Scene_KeyboardFocusEvent* pMsg) {}

protected:
  ApHandle hScene_;
  String sPath_;

  double fX_;
  double fY_;
  double fW_;
  double fH_;
};

#endif // Widget_H_INCLUDED