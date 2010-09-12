// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgScene_h_INCLUDED)
#define MsgScene_h_INCLUDED

#include "ApMessage.h"

class Msg_Scene_Create: public ApRequestMessage
{
public:
  Msg_Scene_Create() : ApRequestMessage("Scene_Create") {}
  ApIN ApHandle hScene;
};

class Msg_Scene_Destroy: public ApRequestMessage
{
public:
  Msg_Scene_Destroy() : ApRequestMessage("Scene_Destroy") {}
  ApIN ApHandle hScene;
};

class Msg_Scene_Position: public ApRequestMessage
{
public:
  enum { NoFlags = 0
    ,Move = 1<<0
    ,Size = 1<<1
    ,Visibility = 1<<2
  };

public:
  Msg_Scene_Position() : ApRequestMessage("Scene_Position"), nX(0), nY(0), nW(0), nH(0), bVisible(0), nFlags(0) {}
  ApIN ApHandle hScene;
  ApIN int nX;
  ApIN int nY;
  ApIN int nW;
  ApIN int nH;
  ApIN int bVisible;
  ApIN int nFlags;
};

#endif // !defined(MsgScene_h_INCLUDED)
