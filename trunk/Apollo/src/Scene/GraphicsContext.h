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
  GraphicsContext() {}
  virtual ~GraphicsContext() {}

  void Implement();
  inline cairo_t* Cairo() { return pCairo_; }

  cairo_t* pCairo_;
};

#endif // GraphicsContext_H_INCLUDED
