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
  virtual ~Image();

  void CopyReference(const Image& iSource);
  void CopyData(const Image& iSource);
  void CopyData_PreMultiplyAlpha_mem_RGBA_to_cairo_ARGB_which_actually_is_BGRA_in_mem_on_little_endian(const Apollo::Image& iSource);

  Pixel* Allocate(int nWidth, int nHeight);
  void Free();
  Pixel* Pixels() const { return pPixels_; }
  size_t Size() const { return nSize_; }
  int Width() const { return nWidth_; }
  int Height() const { return nHeight_; }

protected:
  // Do not use copy contructor or assignment
  // Use CopyReference or CopyData explicitly
  Image(const Image& iSource);
  Image& operator=(const Image& iSource);

  Pixel* pPixels_;
  size_t nSize_;
  int bFree_;
  int nWidth_;
  int nHeight_;
};

AP_NAMESPACE_END

#endif // !defined(Image_h_INCLUDED)
