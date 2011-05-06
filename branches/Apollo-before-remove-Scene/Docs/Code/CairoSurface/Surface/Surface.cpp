// Surface.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Surface.h"

//---------------------------------------------------------------------------

#define Surface_WindowClass _T("SurfaceClass_gthjbvi43765iftvb")
#define Surface_WindowCaption _T("Surface")

class Tile
{
public:
  Tile()
    :hWnd_(NULL)
    ,nX_(0)
    ,nY_(0)
    ,nW_(0)
    ,nH_(0)
    ,hBitmap_(NULL)
    ,pBits_(0)
  {}
  virtual ~Tile();

  int Create(HINSTANCE hInstance);
  void Destroy();

  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
  HWND hWnd_;

  int nX_;
  int nY_;
  int nW_;
  int nH_;

  HBITMAP hBitmap_;
  unsigned char* pBits_;
};

class Surface
{
public:
  Surface()
    :hWnd_(NULL)
    ,bValidSize_(0)
    ,bValidPosition_(0)
    ,nX_(0)
    ,nY_(0)
    ,nW_(0)
    ,nH_(0)
    ,hBitmap_(NULL)
    ,pBits_(0)
    ,nCnt_(0)
  {}
  virtual ~Surface();

  static int Init(HINSTANCE hInstance, HWND hWnd);
  static void Exit();
  
  static Surface* Instance();
  static void DeleteInstance();
  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  int Create();
  void Destroy();
  void Position(int nX, int nY);
  void Size(int nW, int nH);
  void Timer();

  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
  int AdjustSize();
  void Draw();

protected:
  static Surface* pInstance_;
  static HINSTANCE hInstance_;
  static HWND hTargetWnd_;

  HWND hWnd_;

  int bValidSize_;
  int bValidPosition_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  HBITMAP hBitmap_;
  unsigned char* pBits_;
  int nCnt_;
};

Surface* Surface::pInstance_ = 0;
HINSTANCE Surface::hInstance_ = NULL;
HWND Surface::hTargetWnd_ = NULL;

Surface::~Surface()
{
  if (hWnd_ != NULL) {
    Destroy();
  }
}

void Surface::Destroy()
{
  if (hWnd_ != NULL) {
    ::DestroyWindow(hWnd_);
    hWnd_ = NULL;
  }

  if (hBitmap_ != NULL) {
    ::DeleteObject(hBitmap_);
    hBitmap_ = NULL;
    pBits_ = 0;
  }
}

Surface* Surface::Instance()
{
  if (pInstance_ == 0) {
    pInstance_ = new Surface();
  }
  return pInstance_;
}

void Surface::DeleteInstance()
{
  if (pInstance_) {
    delete pInstance_;
    pInstance_ = 0;
  }
}

int Surface::Init(HINSTANCE hInstance, HWND hWnd)
{
  hInstance_ = hInstance;
  hTargetWnd_ = hWnd;
  return 1;
}

void Surface::Exit()
{
  DeleteInstance();
}

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
  //switch (message) {
  //case WM_NCHITTEST:
  //  {
  //    return HTNOWHERE;
  //  }
  //case WM_TIMER:
  //  {
  //    if (wParam == nStackingTimer) {
  //      DoCheckStacking();
  //    }
  //    return 0;
  //  }
  //}
  return DefWindowProc(hWnd, message, wParam, lParam);
}

int Surface::Create()
{
  int ok = 0;

  WNDCLASSEX wcex;
  memset(&wcex, 0, sizeof(wcex));
  wcex.cbSize = sizeof(wcex);
  wcex.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wcex.hCursor = LoadCursor(NULL, IDC_HAND);
  wcex.hInstance = hInstance_;
  wcex.lpszClassName = Surface_WindowClass;
  wcex.lpfnWndProc = Surface::StaticWndProc;
  if (::RegisterClassEx(&wcex)) {
    hWnd_ = ::CreateWindowEx(
      WS_EX_LAYERED | 
      WS_EX_TOOLWINDOW, 
      Surface_WindowClass, 
      Surface_WindowCaption, 
      WS_VISIBLE, 
      0, 200, 100, 100, 
      NULL, 
      NULL, 
      hInstance_, 
      this
      );

    if (hWnd_ == 0) { ok = 0; }
  }

  return ok;
}

void Surface::Position(int nX, int nY)
{
  int bChanged = !bValidPosition_;

  if (nX != nX_ || nY != nY_) {
    bChanged = 1;
  }

  nX_ = nX;
  nY_ = nY;
  bValidPosition_ = 1;

  if (bChanged) {
    Draw();
  }
}

void Surface::Size(int nW, int nH)
{
  int bChanged = !bValidSize_;

  if (nW != nW_ || nH != nH_) {
    bChanged = 1;
  }

  nW_ = nW;
  nH_ = nH;
  bValidSize_ = 1;

  if (bChanged) {
    AdjustSize();
    Draw();
  }
}

int Surface::AdjustSize()
{
  int ok = 1;

  if (hBitmap_!= NULL) {
    ::DeleteObject(hBitmap_);
    hBitmap_ = NULL;
  }

  HDC dcScreen = ::GetDC(NULL);

  BITMAPINFO bi;
  bi.bmiHeader.biSize= sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth= nW_;
  bi.bmiHeader.biHeight= nH_;
  bi.bmiHeader.biPlanes= 1;
  bi.bmiHeader.biBitCount= 32;
  bi.bmiHeader.biCompression= BI_RGB;
  bi.bmiHeader.biSizeImage= (bi.bmiHeader.biWidth * bi.bmiHeader.biHeight) * 4;
  bi.bmiHeader.biXPelsPerMeter= 0;
  bi.bmiHeader.biYPelsPerMeter= 0;
  bi.bmiHeader.biClrUsed= 0;
  bi.bmiHeader.biClrImportant= 0;

  hBitmap_ = CreateDIBSection(
    dcScreen,
    &bi,
    DIB_RGB_COLORS,
    (void**) &pBits_,
    NULL,
    0    
  );

  DWORD dw = ::GetLastError(); // returns ERROR_NOT_ENOUGH_MEMORY ?

  ::ReleaseDC(NULL, dcScreen);

  return ok;
}

void Surface::Timer()
{
  HWND hPrev = ::GetNextWindow(hTargetWnd_, GW_HWNDPREV);
  ::SetWindowPos(hWnd_, hPrev, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

  if (nH_ > 0) {
    nCnt_ = nCnt_++ % nH_;
  }

  Draw();
}

#include <cairo.h>
#include <cairo-win32.h>

void Surface::Draw()
{
  if (hBitmap_ == NULL) { return; }

  int nW = nW_;
  int nH = nH_; if (nH < 40) { return; }

  HDC dcScreen = ::GetDC(NULL);
	HDC dcMemory = ::CreateCompatibleDC(dcScreen);

  HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(dcMemory, hBitmap_);

  { // almost transp background
    for (int y=0; y<nH; ++y) {
      BYTE *pPixel= pBits_ + nW * 4 * y;
      for (int x=0; x<nW ; ++x) {
        pPixel[3] = 1;
        pPixel[0] = 1;
        pPixel[1] = 1;
        pPixel[2] = 1;
        pPixel+= 4;
      }
    }
  }

  //{ // moving grey edge
  //  for (int y=0; y<nH; ++y) {
  //    BYTE *pPixel= pBits_ + nW * 4 * y;
  //    for (int x=0; x<nW ; ++x) {
  //      int a = x * 255 / nW;
  //      int p = ((nCnt_ + y) * 255 / nH) % 255;
  //      pPixel[3] = a;
  //      pPixel[0]= p*pPixel[3]/255;
  //      pPixel[1]= p*pPixel[3]/255;
  //      pPixel[2]= p*pPixel[3]/255;
  //      pPixel+= 4;
  //    }
  //  }
  //}

  //{ // red
  //  for (int y=0; y<30; ++y) {
  //    BYTE *pPixel= pBits_ + (nW * y + 0) * 4;
  //    for (int x=0; x<nW/3 ; ++x) {
  //      int a = 255;
  //      int p = 255;
  //      pPixel[3] = pPixel[3] * 2;
  //      pPixel[0]= 0*pPixel[3]/255;
  //      pPixel[1]= 0*pPixel[3]/255;
  //      pPixel[2]= p*pPixel[3]/255;
  //      pPixel+= 4;
  //    }
  //  }
  //}

  //{ // white hole bottom left
  //  for (int y=10; y<20; ++y) {
  //    BYTE *pPixel= pBits_ + (nW * y + 10) * 4;
  //    for (int x=10; x<nW/3-10 ; ++x) {
  //      int a = 0;
  //      int p = 255;
  //      pPixel[3] = a;
  //      pPixel[0]= p*pPixel[3]/255;
  //      pPixel[1]= p*pPixel[3]/255;
  //      pPixel[2]= p*pPixel[3]/255;
  //      pPixel+= 4;
  //    }
  //  }
  //}

  //{ // black border
  //  for (int y=0; y<nH; ++y) {
  //    BYTE *pPixel= pBits_ + (nW * y + 0) * 4;
  //    for (int x=0; x<nW ; ++x) {
  //      if (y == 0 || y == nH-1 || x == 0 || x == nW-1) {
  //        int a = 128;
  //        int p = 255;
  //        pPixel[3] = a;
  //        pPixel[0]= 0;
  //        pPixel[1]= 0;
  //        pPixel[2]= 0;
  //      }
  //      pPixel+= 4;
  //    }
  //  }
  //}


  #define M_PI 3.14159265358979323
  cairo_surface_t *surface = cairo_win32_surface_create(dcMemory);
  cairo_t *cr = cairo_create(surface);
  
  //cairo_select_font_face(cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  //cairo_set_font_size(cr, 32.0);
  //cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
  //cairo_move_to(cr, 10.0, 50.0);
  //cairo_show_text(cr, "Hello, World");

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

  //cairo_pattern_t *pat;
  //pat = cairo_pattern_create_linear(0.0, 0.0,  0.0, 256.0);
  //cairo_pattern_add_color_stop_rgba(pat, 1, 0, 0, 0, 0.5);
  //cairo_pattern_add_color_stop_rgba(pat, 0, 1, 1, 1, 1);
  //cairo_rectangle(cr, 0, 0, 256, 256);
  //cairo_set_source(cr, pat);
  //cairo_fill(cr);
  //cairo_pattern_destroy(pat);
  //pat = cairo_pattern_create_radial(115.2, 102.4, 25.6,102.4,  102.4, 128.0);
  //cairo_pattern_add_color_stop_rgba(pat, 0, 1, 1, 1, 1);
  //cairo_pattern_add_color_stop_rgba(pat, 1, 0, 0, 0, 1);
  //cairo_set_source(cr, pat);
  //cairo_arc(cr, 128.0, 128.0, 76.8, 0, 2 * M_PI);
  //cairo_fill(cr);
  //cairo_pattern_destroy(pat);

  //{ 
  //  cairo_text_extents_t extents;
  //  const char *utf8 = "cair" "\xC3\xB6";
  //  double x,y;
  //  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  //  cairo_set_font_size(cr, 100.0);
  //  cairo_text_extents(cr, utf8, &extents);
  //  x=25.0;
  //  y=150.0;
  //  cairo_move_to(cr, x,y);
  //  cairo_show_text(cr, utf8);
  //  cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  //  cairo_set_line_width(cr, 6.0);
  //  cairo_arc(cr, x, y, 10.0, 0, 2*M_PI);
  //  cairo_fill(cr);
  //  cairo_move_to(cr, x,y);
  //  cairo_rel_line_to(cr, 0, -extents.height);
  //  cairo_rel_line_to(cr, extents.width, 0);
  //  cairo_rel_line_to(cr, extents.x_bearing, -extents.y_bearing);
  //  cairo_stroke(cr);
  //}

  //cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  //cairo_set_font_size(cr, 90.0);
  //cairo_move_to(cr, 10.0, 135.0);
  //cairo_show_text(cr, "Hello");
  //cairo_move_to(cr, 70.0, 165.0);
  //cairo_text_path(cr, "void");
  //cairo_set_source_rgba(cr, 0.5, 0.5, 1, 0.5);
  //cairo_fill_preserve(cr);
  //cairo_set_source_rgba(cr, 0, 0, 0, 0.5);
  //cairo_set_line_width(cr, 2.56);
  //cairo_stroke(cr);
  //cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
  //cairo_arc(cr, 10.0, 135.0, 5.12, 0, 2*M_PI);
  //cairo_close_path(cr);
  //cairo_arc(cr, 70.0, 165.0, 5.12, 0, 2*M_PI);
  //cairo_fill(cr);

  //int w, h;
  //cairo_surface_t *image;
  //cairo_arc(cr, 128.0, 128.0, 76.8, 0, 2*M_PI);
  //cairo_clip(cr);
  //cairo_new_path(cr); /* path not consumed by clip()*/
  //image = cairo_image_surface_create_from_png("../data/test1.png");
  //w = cairo_image_surface_get_width(image);
  //h = cairo_image_surface_get_height(image);
  //cairo_scale(cr, 256.0/w, 256.0/h);
  //cairo_set_source_surface(cr, image, 0, 0);
  //cairo_paint(cr);

  int w, h;
  cairo_surface_t *image;
  image = cairo_image_surface_create_from_png("../data/tassadar.png");
  w = cairo_image_surface_get_width(image);
  h = cairo_image_surface_get_height(image);
  cairo_scale(cr, 100.0/w, 100.0/h);
  cairo_set_source_surface(cr, image, 0, 0);
  cairo_paint(cr);
  cairo_select_font_face(cr, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 11.0);
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
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.99);
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
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
  cairo_fill(cr);

  cairo_destroy(cr); 
  cairo_surface_destroy(surface);

  // setup the blend function
	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
  POINT ptSrc = {0, 0}; // start point of the copy from dcMemory to dcScreen

  // calculate the new window position/size based on the bitmap size
	POINT ptWindowPosition; 
  ptWindowPosition.x = nX_;
  ptWindowPosition.y = nY_;

	SIZE szWindowSize;
  szWindowSize.cx = nW;
  szWindowSize.cy = nH;

  // perform the alpha blend
	BOOL bRet= ::UpdateLayeredWindow(
      hWnd_, 
      dcScreen, 
      &ptWindowPosition, 
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

//---------------------------------------------------------------------------

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

UINT_PTR g_nTimer = 0;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SURFACE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SURFACE));

  // Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

  Surface::Instance()->Exit();

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SURFACE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SURFACE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      400, 200, 50, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   Surface::Instance()->Init(hInstance, hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE:
    Surface::Instance()->Create();
    g_nTimer = ::SetTimer(hWnd, 12345, 100, NULL);
		break;
	case WM_TIMER:
    if (wParam == g_nTimer) {
      Surface::Instance()->Timer();
    }
		break;
	case WM_MOVE:
    {
      int nX = (int)(short) LOWORD(lParam);
      int nY = (int)(short) HIWORD(lParam);
      Surface::Instance()->Position(nX - 300, nY);
    }
		break;
	case WM_SIZE:
    {
      int nW = (int)(short) LOWORD(lParam);
      int nH = (int)(short) HIWORD(lParam);
      Surface::Instance()->Size(nW + 200, nH);
    }
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
    {
      WCHAR* szText = L"Drag me";
      TextOut(hdc, 0, 0, szText, (int) wcslen(szText));
    }
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
    ::KillTimer(hWnd, 12345);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
