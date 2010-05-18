// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Image.h"

Apollo::Image::Image()
:nWidth_(0)
,nHeight_(0)
,pPixels_(0)
,nSize_(0)
{
}

Apollo::Pixel* Apollo::Image::Allocate(int nWidth, int nHeight)
{
  if (pPixels_ != 0) {
    Apollo::free(pPixels_);
  }

  size_t nSize = nWidth * nHeight * sizeof(Apollo::Pixel);
  pPixels_ = (Apollo::Pixel*) Apollo::malloc(nSize);
  if (pPixels_ != 0) {
    nSize_ = nSize;
  }

  return pPixels_;
}
