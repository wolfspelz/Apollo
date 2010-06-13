// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Context.h"

Context::Context(ApHandle hContext)
:hAp_(hContext)
,bVisible_(0)
,nX_(0)
,nY_(0)
,nW_(0)
,nH_(0)
,nCntWindows_(0)
,hInstance_(NULL)
,hWnd_(NULL)
,hBitmap_(NULL)
,pBits_(0)
{
}

Context::~Context()
{
  DestroySurface();
}

void Context::SetVisibility(int bVisible)
{
  int bChanged = (bVisible_ != bVisible);
  bVisible_ = bVisible;

  if (bChanged) {
    ShowSurface();
  }
}

void Context::SetPosition(int nX, int nY)
{
  nX_ = nX;
  nY_ = nY;
  MoveSurface();
}

void Context::SetSize(int nW, int nH)
{
  nW_ = nW;
  nH_ = nH;
  SizeSurface();
}

//--------------------------------------------------------------

#include "MsgMainLoop.h"

#define Context_WindowClass _T("ApolloContextClass_gthjbvi43765iftvb")
#define Context_WindowCaption _T("ApolloContext")

LRESULT CALLBACK Context::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
    Context* pContext = (Context*) ::GetWindowLong(hWnd, GWL_USERDATA);
#pragma warning(pop)
    if (pContext != 0) {
      nResult = pContext->WndProc(hWnd, message, wParam, lParam);
    } else {
      nResult = ::DefWindowProc(hWnd, message, wParam, lParam);
    }
  }

  return nResult;
}

LRESULT CALLBACK Context::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, message, wParam, lParam);
}

int Context::CreateSurface()
{
  int ok = 0;

  {
    Msg_Win32_GetInstance msg;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Context::CreateSurface", "Msg_Win32_GetInstance failed"));
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
      wcex.lpszClassName = Context_WindowClass;
      wcex.lpfnWndProc = Context::StaticWndProc;
      BOOL bOK = ::RegisterClassEx(&wcex);
      if (!bOK) {
        apLog_Error((LOG_CHANNEL, "Context::CreateSurface", "RegisterClassEx failed"));
      }
    }

    hWnd_ = ::CreateWindowEx(
      WS_EX_LAYERED | WS_EX_TOOLWINDOW, 
      Context_WindowClass, 
      Context_WindowCaption, 
      0, 
      0, 200, 100, 100, 
      NULL, 
      NULL, 
      hInstance_, 
      this
      );
    if (hWnd_ == NULL) {
      apLog_Error((LOG_CHANNEL, "Context::CreateSurface", "CreateWindowEx failed"));
    }

    if (hWnd_ != NULL) {
      ok = 1;
    }
  }

  return ok;
}

void Context::DestroySurface()
{
  if (hWnd_ != NULL) {
    ::DestroyWindow(hWnd_);
    hWnd_ = NULL;

    nCntWindows_--;
    if (nCntWindows_ == 0) {
      ::UnregisterClass(Context_WindowClass, hInstance_);
    }
  }

  if (hBitmap_ != NULL) {
    ::DeleteObject(hBitmap_);
    hBitmap_ = NULL;
    pBits_ = 0;
  }
}

void Context::ShowSurface()
{
  ::ShowWindow(hWnd_, bVisible_ ? SW_SHOW : SW_HIDE);
}

void Context::MoveSurface()
{
  ::SetWindowPos(hWnd_, NULL, nX_, nY_ - nH_, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void Context::SizeSurface()
{
  if (hBitmap_!= NULL) {
    ::DeleteObject(hBitmap_);
    hBitmap_ = NULL;
  }

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
    //apLog_Error((LOG_CHANNEL, "Context::SizeSurface", "CreateDIBSection failed: %s", StringType(_FormatLastError()));
    apLog_Error((LOG_CHANNEL, "Context::SizeSurface", "CreateDIBSection failed: GetLastError()=%d", dw));
  }

  MoveSurface();
  DrawSurface();

  ::ReleaseDC(NULL, dcScreen);
}

#include <cairo.h>
#include <cairo-win32.h>

void Context::DrawSurface()
{
  if (hBitmap_ == NULL) { return; }

  int nW = nW_;
  int nH = nH_;

  HDC dcScreen = ::GetDC(NULL);
	HDC dcMemory = ::CreateCompatibleDC(dcScreen);

  HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(dcMemory, hBitmap_);

  //{ // almost transp background
  //  for (int y = 0; y < nH; ++y) {
  //    BYTE *pPixel= pBits_ + nW * 4 * y;
  //    for (int x = 0; x < nW ; ++x) {
  //      pPixel[3] = 0x80;
  //      pPixel[0] = 0x80;
  //      pPixel[1] = 0;
  //      pPixel[2] = 0;
  //      pPixel+= 4;
  //    }
  //  }
  //}

  #define M_PI 3.14159265358979323
  cairo_surface_t *surface = cairo_win32_surface_create(dcMemory);
  cairo_t *cr = cairo_create(surface);

  double xc = 128.0;
  double yc = 128.0;
  double radius = 100.0;
  double angle1 = 45.0  *(M_PI/180.0);  /* angles are specified */
  double angle2 = 180.0 *(M_PI/180.0);  /* in radians           */
  cairo_set_line_width(cr, 10.0);
  cairo_arc(cr, xc, yc, radius, angle1, angle2);
  cairo_stroke(cr);
  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  cairo_set_line_width(cr, 20.0);
  cairo_arc(cr, xc, yc, 10.0, 0, 2*M_PI);
  cairo_fill(cr);
  cairo_arc(cr, xc, yc, radius, angle1, angle1);
  cairo_line_to(cr, xc, yc);
  cairo_arc(cr, xc, yc, radius, angle2, angle2);
  cairo_line_to(cr, xc, yc);
  cairo_stroke(cr);

  int w, h;
  cairo_surface_t *image;
  image = cairo_image_surface_create_from_png(Apollo::getAppResourcePath() + "tassadar.png");
  w = cairo_image_surface_get_width(image);
  h = cairo_image_surface_get_height(image);
  cairo_scale(cr, 100.0/w, 100.0/h);
  cairo_set_source_surface(cr, image, 0, 0);
  cairo_paint(cr);
  cairo_select_font_face(cr, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 32.0);
  const char* text = "Tassadar";
  cairo_text_extents_t extents;
  cairo_text_extents(cr, text, &extents);
  cairo_set_line_width(cr, 1.0);
  int b = 3;
  int text_x = 64, text_y = 90;
  cairo_move_to(cr, text_x, text_y);
  cairo_rel_move_to(cr, -b, +b);
  cairo_rel_line_to(cr, 0, -extents.height - 2*b);
  cairo_rel_line_to(cr, extents.width + 2*b, 0);
  cairo_rel_line_to(cr, 0, extents.height + 2*b);
  cairo_rel_line_to(cr, -extents.width - 2*b, 0);
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.3);
  cairo_fill(cr);
  cairo_move_to(cr, text_x + 0.5, text_y + 0.5);
  cairo_rel_move_to(cr, -b, +b);
  cairo_rel_line_to(cr, 0, -extents.height - 2*b);
  cairo_rel_line_to(cr, extents.width + 2*b, 0);
  cairo_rel_line_to(cr, 0, extents.height + 2*b);
  cairo_rel_line_to(cr, -extents.width - 2*b, 0);
  cairo_set_source_rgba(cr, 0.6, 0.8, 0.2, 1.0);
  cairo_stroke(cr);
  cairo_move_to(cr, text_x, text_y);
  //cairo_show_text(cr, text);
  cairo_text_path(cr, text);
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.5);
  cairo_fill(cr);

  cairo_destroy(cr); 
  cairo_surface_destroy(surface);

  // setup the blend function
	BLENDFUNCTION blendPixelFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
  POINT ptSrc = {0, 0}; // start point of the copy from dcMemory to dcScreen

  // calculate the new window position/size based on the bitmap size
	POINT ptWindowPosition; 
  ptWindowPosition.x = nX_;
  ptWindowPosition.y = nY_ - nH_;

	SIZE szWindowSize;
  szWindowSize.cx = nW;
  szWindowSize.cy = nH;

  // perform the alpha blend
	BOOL bRet= ::UpdateLayeredWindow(
      hWnd_, 
      dcScreen, 
      0, //&ptWindowPosition, 
      &szWindowSize, 
      dcMemory,
		  &ptSrc, 
      0, 
      &blendPixelFunction, 
      ULW_ALPHA
      );

	// clean up
  ::SelectObject(dcMemory, hOldBitmap);

  ::DeleteDC(dcMemory);
  ::ReleaseDC(NULL, dcScreen);
}
