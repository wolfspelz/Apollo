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

  // Same as Msg_ScWidget_SetEditAlign
  typedef enum _Align { NoAlign
    ,AlignLeft
    ,AlignCenter
    ,AlignRight
    ,AlignTop
    ,AlignMiddle
    ,AlignBottom
    ,LastAlign
  } Align;

  void Create();
  void SetText(const String& sText);
  String GetText();
  void SetAlign(int nAlignH, int nAlignV);
  void SetPadding(double fPadding);
  void SetFontFamily(const String& sFont);
  void SetFontSize(double fSize);
  void SetFontFlags(int nFlags);
  void SetFontColor(double fRed, double fGreen, double fBlue, double fAlpha);
  inline void SetFocus(int bHasFocus) { bHasFocus_ = bHasFocus; }

  void OnMouseEvent(Msg_Scene_MouseEvent* pMsg);
  void OnKeyEvent(Msg_Scene_KeyEvent* pMsg);
  void OnKeyboardFocusEvent(Msg_Scene_KeyboardFocusEvent* pMsg);

protected:
  String GetTextPath();
  String GetInsertPath();
  void ShowText();
  void PositionText();
  void GetTextRect(double& fTextX, double& fTextY, double& fTextW, double& fTextH);
  void DoEnter();

protected:
  String sText_;
  String sFont_;
  double fSize_;
  int nFlags_;
  int nAlignH_;
  int nAlignV_;
  double fPadding_;
  int bHasFocus_;
  int nInsertPos_;
};

#endif // Edit_H_INCLUDED
