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

void Apollo::Image::CopyData_PreMultiplyAlpha_mem_RGBA_to_cairo_ARGB_which_actually_is_BGRA_in_mem_on_little_endian(const Apollo::Image& iSource)
{
  Free();

  Allocate(iSource.nWidth_, iSource.nHeight_);

  unsigned int* pDstPixels = (unsigned int*) Pixels();
  unsigned int* pSrcPixels = (unsigned int*) iSource.Pixels();

  if (pDstPixels != 0 && pSrcPixels != 0) {

    int nW = iSource.Width();
    int nH = iSource.Height();

    for (int y = 0; y < nH; ++y) {
      for (int x = 0; x < nW ; ++x) {

        // >> 8 instead of / 255.0, which would be correct but slower
        unsigned int color = *pSrcPixels++; // AABBGGRR
        unsigned int a = (color & 0xff000000) >> 24;
        unsigned int cairo_color = 
               (color & 0xff000000) // a
          | (( (color & 0x000000ff)        * a) & 0x0000ff00) << 8 // r
          | ((((color & 0x0000ff00) >> 8)  * a) & 0x0000ff00) // g
          | ((((color & 0x00ff0000) >> 16) * a) >> 8) // b
          ;
        *pDstPixels++ = cairo_color; // AARRGGBB

      }
    }

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
