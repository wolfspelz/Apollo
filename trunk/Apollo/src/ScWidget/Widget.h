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

  virtual void Delete() {}
  virtual void SetCoordinates(double fX, double fY, double fW, double fH);
  Button* AsButton();
  Edit* AsEdit();

  virtual void OnMouseEvent(Msg_Scene_MouseEvent* pMsg) {}

protected:
  ApHandle hScene_;
  String sPath_;

  double fX_;
  double fY_;
  double fW_;
  double fH_;
};

#endif // Widget_H_INCLUDED
