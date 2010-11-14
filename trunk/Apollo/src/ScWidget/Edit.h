// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Edit_H_INCLUDED)
#define Edit_H_INCLUDED

#include "Widget.h"

class Edit: public Widget
{
public:
  Edit(const ApHandle& hScene, const String& sPath);
  virtual ~Edit() {}

  inline int IsEdit() { return 1; }

  void Create();
  void SetText(const String& sText);

  void OnMouseEvent(Msg_Scene_MouseEvent* pMsg);
  void OnKeyEvent(Msg_Scene_KeyEvent* pMsg);

protected:
  String GetTextPath();
  void ShowText();

protected:
  String sText_;
};

#endif // Edit_H_INCLUDED
