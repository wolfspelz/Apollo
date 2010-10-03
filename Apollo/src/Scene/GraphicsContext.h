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
    ,nH_(0)
    ,fTranslateX_(0.0)
    ,fTranslateY_(0.0)
  {}
  virtual ~GraphicsContext() {}

  void Implement();
  inline cairo_t* Cairo() { return pCairo_; }

  cairo_t* pCairo_;
  int nH_;

  double fTranslateX_;
  double fTranslateY_;
};

#endif // GraphicsContext_H_INCLUDED
