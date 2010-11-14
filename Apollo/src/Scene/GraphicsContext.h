// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(GraphicsContext_H_INCLUDED)
#define GraphicsContext_H_INCLUDED

#include "Apollo.h"

#include <cairo.h>
#include <cairo-win32.h>

class GraphicsContext
{
public:
  GraphicsContext()
    :pCairo_(0)
    ,nDepth_(0)
  {}

  inline cairo_t* Cairo() { return pCairo_; }

  cairo_t* pCairo_;
  int nDepth_;
};

class DrawContext: public GraphicsContext
{
public:
  DrawContext()
    :nH_(0)
    ,nW_(0)
    ,bLogDraw_(0)
  {}

  inline int Height() { return nH_; }
  inline int Width() { return nW_; }

  int nH_;
  int nW_;
  int bLogDraw_;
  String sLogDraw_;
};

class EventContext: public GraphicsContext
{
public:
  // Same as Msg_Scene_MouseEvent::EventType
  typedef enum _EventType { NoEventType
    ,MouseMove
    ,MouseDown
    ,MouseUp
    ,MouseClick
    ,MouseDoubleClick
    ,MouseOut
    ,LastEventType
  } EventType;

  // Same as Msg_Scene_MouseEvent::MouseButton
  typedef enum _MouseButton { NoMouseButton
    ,LeftButton
    ,MiddleButton
    ,RightButton
    ,LastMouseButton
  } MouseButton;

  EventContext()
    :bFired_(0)
    ,nEvent_(NoEventType)
    ,nButton_(NoMouseButton)
    ,bTimer_(0)
  {}

  inline int Fired() { return bFired_; }

  int bFired_;
  int nEvent_;
  int nButton_;
  int bTimer_;
};

#endif // GraphicsContext_H_INCLUDED
