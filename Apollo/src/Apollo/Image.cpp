// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Image.h"

Apollo::Image::Image()
:pPixels_(0)
,nSize_(0)
,bFree_(0)
,nWidth_(0)
,nHeight_(0)
{
}

Apollo::Image::~Image()
{
  Free();
}

Apollo::Image::Image(const Apollo::Image& iSource)
{
  CopyReference(iSource);
}

Apollo::Image& Apollo::Image::operator=(const Apollo::Image& iSource)
{
  CopyReference(iSource);
  return *this;
}

void Apollo::Image::CopyReference(const Apollo::Image& iSource)
{
  Free();

  pPixels_ = iSource.pPixels_;
  nSize_ = iSource.nSize_;
  bFree_ = 0;
  nWidth_ = iSource.nWidth_;
  nHeight_ = iSource.nHeight_;
}

void Apollo::Image::CopyData(const Apollo::Image& iSource)
{
  Free();

  Allocate(iSource.nWidth_, iSource.nHeight_);
  if (pPixels_) {
    memcpy(pPixels_, iSource.pPixels_, iSource.nSize_);
  }
}

void Apollo::Image::Free()
{
  if (pPixels_) {
    if (bFree_) {
      Apollo::free(pPixels_);;
      bFree_ = 0;
    }
    pPixels_ = 0;
    nSize_ = 0;
    nWidth_ = 0;
    nHeight_ = 0;
  }
}

Apollo::Pixel* Apollo::Image::Allocate(int nWidth, int nHeight)
{
  Free();

  size_t nSize = nWidth * nHeight * sizeof(Apollo::Pixel);
  pPixels_ = (Apollo::Pixel*) Apollo::malloc(nSize);
  if (pPixels_ != 0) {
    nSize_ = nSize;
    bFree_ = 1;
  }

  nWidth_ = nWidth;
  nHeight_ = nHeight;

  return pPixels_;
}
