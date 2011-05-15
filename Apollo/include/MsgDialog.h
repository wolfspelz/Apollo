// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgSample_h_INCLUDED)
#define MsgSample_h_INCLUDED

#include "ApMessage.h"

class Msg_Dialog_Create: public ApRequestMessage
{
public:
  Msg_Dialog_Create() : ApRequestMessage("Dialog_Create"), nLeft(100), nTop(100), nWidth(400), nHeight(300), bVisible(1) {}
  ApIN ApHandle hDialog;
  ApIN int nLeft;
  ApIN int nTop;
  ApIN int nWidth;
  ApIN int nHeight;
  ApIN int bVisible;
  ApIN String sContentUrl;
};

class Msg_Dialog_Destroy: public ApRequestMessage
{
public:
  Msg_Dialog_Destroy() : ApRequestMessage("Dialog_Destroy") {}
  ApIN ApHandle hDialog;
};

class Msg_Dialog_GetView: public ApRequestMessage
{
public:
  Msg_Dialog_GetView() : ApRequestMessage("Dialog_GetView") {}
  ApIN ApHandle hDialog;
  ApOUT ApHandle hView;
};

#endif // !defined(MsgSample_h_INCLUDED)
