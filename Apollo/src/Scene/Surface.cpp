// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#if defined(WIN32)
  #include "MsgMainLoop.h"
#endif // WIN32
#include "Local.h"
#include "Surface.h"
#include "Graphics.h"
#include "GraphicsContext.h"

int Surface::nCntWindows_ = 0;

Surface::~Surface()
{
  DestroyBitmap();
}

#if defined(WIN32)
#define Surface_WindowClass _T("ApolloSurfaceClass_gthjbvi43765iftvb")
#define Surface_WindowCaption _T("ApolloSurface")

LRESULT CALLBACK Surface::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT nResult = 0;

  if (message == WM_CREATE) {
    LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam; 
    #pragma warning(push)
    #pragma warning(disable : 4311)
    ::SetWindowLong(hWnd, GWL_USERDATA, (LONG) lpcs->lpCreateParams);
    #pragma warning(pop)
    nResult = 0;
  } else {
    #pragma warning(push)
    #pragma warning(disable : 4312)
    Surface* pSurface = (Surface*) ::GetWindowLong(hWnd, GWL_USERDATA);
    #pragma warning(pop)
    if (pSurface != 0) {
      nResult = pSurface->WndProc(hWnd, message, wParam, lParam);
    } else {
      nResult = ::DefWindowProc(hWnd, message, wParam, lParam);
    }
  }

  return nResult;
}

LRESULT CALLBACK Surface::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif // WIN32

int Surface::Create()
{
  int ok = 0;

#if defined(WIN32)
  {
    Msg_Win32_GetInstance msg;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Surface::Create", "Msg_Win32_GetInstance failed"));
    } else {
      hInstance_ = msg.hInstance;
    }
  }

  if (hInstance_ != NULL) {
    if (nCntWindows_ == 0) {
      nCntWindows_++;

      WNDCLASSEX wcex;
      memset(&wcex, 0, sizeof(wcex));
      wcex.cbSize = sizeof(wcex);
      wcex.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
      wcex.hCursor = LoadCursor(NULL, IDC_HAND);
      wcex.hInstance = hInstance_;
      wcex.lpszClassName = Surface_WindowClass;
      wcex.lpfnWndProc = Surface::StaticWndProc;
      BOOL bOK = ::RegisterClassEx(&wcex);
      if (!bOK) {
        apLog_Error((LOG_CHANNEL, "Surface::Create", "RegisterClassEx failed"));
      }
    }

    hWnd_ = ::CreateWindowEx(
      WS_EX_LAYERED | WS_EX_TOOLWINDOW, 
      Surface_WindowClass, 
      Surface_WindowCaption, 
      0, 
      0, 200, 100, 100, 
      NULL, 
      NULL, 
      hInstance_, 
      this
      );
    if (hWnd_ == NULL) {
      apLog_Error((LOG_CHANNEL, "Surface::Create", "CreateWindowEx failed"));
    }

    if (hWnd_ != NULL) {
      ok = 1;
    }
  }
#endif // WIN32

  return ok;
}

void Surface::Destroy()
{
#if defined(WIN32)
  if (hWnd_ != NULL) {
    ::DestroyWindow(hWnd_);
    hWnd_ = NULL;

    nCntWindows_--;
    if (nCntWindows_ == 0) {
      ::UnregisterClass(Surface_WindowClass, hInstance_);
    }
  }

  if (hBitmap_ != NULL) {
    ::DeleteObject(hBitmap_);
    hBitmap_ = NULL;
    pBits_ = 0;
  }
#endif // WIN32
}

//------------------------------------

int Surface::CreateBitmap()
{
  int ok = 1;

  HDC dcScreen = ::GetDC(NULL);

  BITMAPINFO bi;
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = nW_;
  bi.bmiHeader.biHeight = nH_;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = (bi.bmiHeader.biWidth * bi.bmiHeader.biHeight) * 4;
  bi.bmiHeader.biXPelsPerMeter = 0;
  bi.bmiHeader.biYPelsPerMeter = 0;
  bi.bmiHeader.biClrUsed = 0;
  bi.bmiHeader.biClrImportant = 0;

  hBitmap_ = CreateDIBSection(
    dcScreen,
    &bi,
    DIB_RGB_COLORS,
    (void**) &pBits_,
    NULL,
    0    
  );
  if (hBitmap_ == NULL) {
    DWORD dw = ::GetLastError(); // returns ERROR_NOT_ENOUGH_MEMORY ?
    apLog_Error((LOG_CHANNEL, "Surface::Size", "CreateDIBSection failed: GetLastError()=%d", dw));
    ok = 0;
  }

  dcMemory_ = ::CreateCompatibleDC(dcScreen);
  if (dcMemory_ == NULL) {
    DWORD dw = ::GetLastError();
    apLog_Error((LOG_CHANNEL, "Surface::Size", "::CreateCompatibleDC(dcScreen) failed: GetLastError()=%d", dw));
    ok = 0;
  }

  ::ReleaseDC(NULL, dcScreen);

  hOldBitmap_ = (HBITMAP) ::SelectObject(dcMemory_, hBitmap_);
  pSurface_ = cairo_win32_surface_create(dcMemory_);
  pCairo_ = cairo_create(pSurface_);

  //cairo_translate(pCairo_, 0.0, nH_);
  //cairo_scale(pCairo_, 1.0, -1.0);

  return ok;
}

void Surface::DestroyBitmap()
{
  if (hBitmap_!= NULL) {
    ::DeleteObject(hBitmap_);
    hBitmap_ = NULL;
  }

  if (pCairo_ != 0) {
    cairo_destroy(pCairo_);
    pCairo_ = 0;
  }

  if (pSurface_ != 0) {
    cairo_surface_destroy(pSurface_);
    pSurface_ = 0;
  }

  if (hOldBitmap_ != NULL && dcMemory_ != NULL) {
    ::SelectObject(dcMemory_, hOldBitmap_);
  }

  if (dcMemory_ != NULL) {
    ::DeleteDC(dcMemory_);
    dcMemory_ = NULL;
  }
}

void Surface::SetPosition(int nX, int nY, int nW, int nH)
{
  int bMove = 0;
  int bSize = 0;

  int nOldWindowTop = nY_ - nH_;
  int nNewWindowTop = nY - nH;

  if (nOldWindowTop != nNewWindowTop || nX_ != nX) {
    bMove = 1;
  }
  if (nW_ != nW || nH_ != nH) {
    bSize = 1;
  }

  nX_ = nX;
  nY_ = nY;
  nW_ = nW;
  nH_ = nH;

#if defined(WIN32)
  if (bSize) {

    DestroyBitmap();

    if (bMove) {
      ::SetWindowPos(hWnd_, NULL, nX_, nY_ - nH_, nW_, nH_, SWP_NOZORDER | SWP_NOACTIVATE);
    } else {
      ::SetWindowPos(hWnd_, NULL, 0, 0, nW_, nH_, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    if (!CreateBitmap()) {
      apLog_Error((LOG_CHANNEL, "Surface::SetPosition", "CreateBitmap failed"));
    }

    if (bVisible_) {
      Draw();
    }

  } else {
    if (bMove) {
      ::SetWindowPos(hWnd_, NULL, nX_, nY_ - nH_, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
  }
#endif // WIN32
}

void Surface::SetVisibility(int bVisible)
{
  int bChanged = (bVisible_ != bVisible);
  bVisible_ = bVisible;

  if (bChanged) {

    if (bVisible_) {
      Draw();
    }

#if defined(WIN32)
    ::ShowWindow(hWnd_, bVisible_ ? SW_SHOW : SW_HIDE);
#endif // WIN32
  }
}

//------------------------------------

Element* Surface::FindElement(const String& sPath)
{
  String sFixedPath = sPath; sFixedPath.trim("/");
  Element* pElement = root_.FindElement(sFixedPath);
  if (pElement == 0) { throw ApException("Surface::FindElement scene=" ApHandleFormat " no element=%s", ApHandleType(hAp_), StringType(sPath)); }

  return pElement;
}

void Surface::CreateElement(const String& sPath)
{
  String sFixedPath = sPath; sFixedPath.trim("/");
  if (!root_.CreateElement(sFixedPath)) { throw ApException("Surface::CreateElement scene=" ApHandleFormat " root_.CreateElement(%s) failed", ApHandleType(hAp_), StringType(sPath)); }
}

void Surface::DeleteElement(const String& sPath)
{
  String sFixedPath = sPath; sFixedPath.trim("/");
  if (!root_.DeleteElement(sFixedPath)) { throw ApException("Surface::DeleteElement scene=" ApHandleFormat " root_.DeleteElement(%s) failed", ApHandleType(hAp_), StringType(sPath)); }
}

void Surface::MeasureText(const String& sText, const String& sFont, double fSize, int nFlags, TextExtents& te)
{
  GraphicsContext gc;
  gc.pCairo_ = pCairo_;
  gc.nH_ = nH_;

  TextX t(0.0, 0.0, sText, sFont, fSize, nFlags);
  t.Measure(gc, te);
}

//------------------------------------

#if defined(_DEBUG)

#include "ximagif.h"
#include "Image.h"

#endif

void Surface::EraseBackground()
{
  //unsigned char* pPixel = pBits_;
  //for (int y = 0; y < nH_; ++y) {
  //  for (int x = 0; x < nW_ ; ++x) {
  //    *pPixel++ = 0;
  //    *pPixel++ = 0;
  //    *pPixel++ = 0;
  //    *pPixel++ = 0;
  //  }
  //}

  cairo_save(pCairo_);
  cairo_set_operator(pCairo_, CAIRO_OPERATOR_CLEAR);
  cairo_paint(pCairo_);
  cairo_restore(pCairo_);
}

void Surface::Draw()
{
  if (hBitmap_ == NULL) { return; }
//  if (!bVisible_) { return; }

  EraseBackground();

  GraphicsContext gc;
  gc.pCairo_ = pCairo_;
  gc.nH_ = nH_;

  root_.Draw(gc);

  if (0) {
    cairo_rectangle(pCairo_, 50, 50, 100, 100);
    cairo_set_source_rgba(pCairo_, 1.0, 0.0, 0.0, 0.5);
    cairo_fill(pCairo_);
  }

#if defined(_DEBUG) && 0
  #define M_PI 3.14159265358979323

  double xc = 128.0;
  double yc = 128.0;
  double radius = 100.0;
  double angle1 = 45.0  *(M_PI/180.0);  // angles are specified
  double angle2 = 180.0 *(M_PI/180.0);  // in radians/
  cairo_set_line_width(pCairo_, 10.0);
  cairo_arc(pCairo_, xc, yc, radius, angle1, angle2);
  cairo_stroke(pCairo_);
  cairo_set_source_rgba(pCairo_, 1, 0.2, 0.2, 0.6);
  cairo_set_line_width(pCairo_, 20.0);
  cairo_arc(pCairo_, xc, yc, 10.0, 0, 2*M_PI);
  cairo_fill(pCairo_);
  cairo_arc(pCairo_, xc, yc, radius, angle1, angle1);
  cairo_line_to(pCairo_, xc, yc);
  cairo_arc(pCairo_, xc, yc, radius, angle2, angle2);
  cairo_line_to(pCairo_, xc, yc);
  cairo_stroke(pCairo_);

#define FROM_DATA
#define TASSADAR_GIF
//#define TEST_PNG

#ifdef FROM_FILE
#ifdef TASSADAR_PNG
  cairo_surface_t *image = cairo_image_surface_create_from_png(Apollo::getAppResourcePath() + "tassadar.png");
#endif
#ifdef TEST_PNG
  cairo_surface_t *image = cairo_image_surface_create_from_png(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + "test.png");
#endif
#endif // FROM_FILE

#ifdef FROM_DATA
  Buffer sbData;
#ifdef TASSADAR_GIF
  Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + "idle.gif", sbData);
#endif
#ifdef TEST_PNG
  Apollo::loadFile(Apollo::getAppResourcePath() + "tassadar" + String::filenamePathSeparator() + "test.png", sbData);
#endif
  CxImage cxImg(sbData.Data(), sbData.Length(), CXIMAGE_FORMAT_UNKNOWN);
#ifdef TASSADAR_GIF
  cxImg.SetRetreiveAllFrames(true);
  int nFrames = cxImg.GetNumFrames();
  cxImg.SetFrame(nFrames - 1);
  cxImg.Decode(sbData.Data(), sbData.Length(), CXIMAGE_FORMAT_GIF);
  CxImage* pCxImgFrame = cxImg.GetFrame(0);
#endif
#ifdef TEST_PNG
  CxImage* pCxImgFrame = &cxImg;
#endif
  Apollo::Image apImg;
  apImg.Allocate(pCxImgFrame->GetWidth(), pCxImgFrame->GetHeight());
  CxMemFile mfDest((BYTE*) apImg.Pixels(), apImg.Size());
  pCxImgFrame->AlphaFromTransparency();
  pCxImgFrame->Encode2RGBA(&mfDest, true);

  Apollo::Image apImgPremultiplied;
  apImgPremultiplied.Allocate(apImg.Width(), apImg.Height());
  apImgPremultiplied.CopyData_PreMultiplyAlpha_mem_RGBA_to_cairo_ARGB_which_actually_is_BGRA_in_mem_on_little_endian(apImg);

  //{
  //  Apollo::TimeValue tvBegin = Apollo::TimeValue::getTime();
  //  for (int i = 0; i < 10000; ++i) {
  //    _PreMultiplyAlpha_mem_RGBA_to_cairo_ARGB_which_actually_is_BGRA_in_mem_on_little_endian(mfDest.GetBuffer(false), pCxImgFrame->GetWidth(), pCxImgFrame->GetHeight());
  //  }
  //  Apollo::TimeValue tvEnd = Apollo::TimeValue::getTime();
  //  Apollo::TimeValue tvDelay = tvEnd - tvBegin;
  //  apLog_Debug((LOG_CHANNEL, "############# 1", "usec=%d.%06d", tvDelay.Sec(), tvDelay.MicroSec()));
  //}

  cairo_surface_t *pCairoImage = cairo_image_surface_create_for_data((unsigned char*) apImgPremultiplied.Pixels(), CAIRO_FORMAT_ARGB32, apImgPremultiplied.Width(), apImgPremultiplied.Height(), apImgPremultiplied.Width() * 4);
#endif // FROM_DATA

  int w = cairo_image_surface_get_width(pCairoImage);
  int h = cairo_image_surface_get_height(pCairoImage);

  cairo_set_source_surface(pCairo_, pCairoImage, 0, 0);
  cairo_paint(pCairo_);

  cairo_select_font_face(pCairo_, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(pCairo_, 32.0);
  const char* text = "Tassadar";
  cairo_text_extents_t extents;
  cairo_text_extents(pCairo_, text, &extents);

  cairo_set_line_width(pCairo_, 1.0);
  int b = 3;
  int text_x = 110, text_y = 100;
  cairo_move_to(pCairo_, text_x, text_y);
  cairo_rel_move_to(pCairo_, -b, +b);
  cairo_rel_line_to(pCairo_, 0, -extents.height - 2*b);
  cairo_rel_line_to(pCairo_, extents.width + 2*b, 0);
  cairo_rel_line_to(pCairo_, 0, extents.height + 2*b);
  cairo_rel_line_to(pCairo_, -extents.width - 2*b, 0);
  cairo_set_source_rgba(pCairo_, 1.0, 1.0, 1.0, 0.3);
  cairo_fill(pCairo_);

  cairo_move_to(pCairo_, text_x + 0.5, text_y + 0.5);
  cairo_rel_move_to(pCairo_, -b, +b);
  cairo_rel_line_to(pCairo_, 0, -extents.height - 2*b);
  cairo_rel_line_to(pCairo_, extents.width + 2*b, 0);
  cairo_rel_line_to(pCairo_, 0, extents.height + 2*b);
  cairo_rel_line_to(pCairo_, -extents.width - 2*b, 0);
  cairo_set_source_rgba(pCairo_, 0.6, 0.8, 0.2, 1.0);
  cairo_stroke(pCairo_);

  cairo_move_to(pCairo_, text_x, text_y);
  //cairo_show_text(pCairo_, text);
  cairo_text_path(pCairo_, text);
  cairo_set_source_rgba(pCairo_, 0.0, 0.0, 0.0, 0.5);
  cairo_fill(pCairo_);

#endif

  // setup the blend function
	BLENDFUNCTION blendPixelFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
  POINT ptSrc = {0, 0}; // start point of the copy from dcMemory to dcScreen

  // calculate the new window position/size based on the bitmap size
	POINT ptWindowPosition; 
  ptWindowPosition.x = nX_;
  ptWindowPosition.y = nY_ - nH_;

	SIZE szWindowSize;
  szWindowSize.cx = nW_;
  szWindowSize.cy = nH_;

  HDC dcScreen = ::GetDC(NULL);
  
  // perform the alpha blend
	BOOL bRet= ::UpdateLayeredWindow(
      hWnd_, 
      dcScreen, 
      0, //&ptWindowPosition, 
      &szWindowSize, 
      dcMemory_,
		  &ptSrc, 
      0, 
      &blendPixelFunction, 
      ULW_ALPHA
      );

  ::ReleaseDC(NULL, dcScreen);
}

