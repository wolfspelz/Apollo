// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Image_h_INCLUDED)
#define Image_h_INCLUDED

#include "Apollo.h"
#include "ApExports.h"

AP_NAMESPACE_BEGIN

typedef struct _Pixel
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
} Pixel;

class APOLLO_API Image
{
public:
  Image();

  Pixel* Allocate(int nWidth, int nHeight);
  Pixel* Pixels() { return pPixels_; }
  size_t Size() { return nSize_; }

protected:
  int nWidth_;
  int nHeight_;
  Pixel* pPixels_;
  size_t nSize_;
};

AP_NAMESPACE_END

#endif // !defined(Image_h_INCLUDED)
