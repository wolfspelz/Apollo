// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Graphics_H_INCLUDED)
#define Graphics_H_INCLUDED

#include "Apollo.h"
#include "Image.h"

#if defined(AP_TEST)
#include "SceneModuleTester.h"
#endif // #if defined(AP_TEST)

#include "GraphicsContext.h"

class Surface;

class Color
{
public:
  Color()
    :r(0.0)
    ,g(0.0)
    ,b(0.0)
    ,a(0.0)
  {}
  double r;
  double g;
  double b;
  double a;
};

class Graphics
{
public:
  Graphics(Surface* pSurface)
    :pSurface_(pSurface)
  {}
  virtual ~Graphics() {}

  virtual void Draw(DrawContext& gc) = 0;

  inline virtual bool IsRectangle() { return false; }
  inline virtual bool IsImage() { return false; }
  inline virtual bool IsShape() { return false; }
  inline virtual bool IsText() { return false; }
  inline virtual bool IsSensor() { return false; }

protected:
  Surface* pSurface_;

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

class Shape: public Graphics
{
public:
  Shape(Surface* pSurface)
    :Graphics(pSurface)
    ,fX_(0.0)
    ,fY_(0.0)
    ,bFillColor_(false)
    ,bFillImageFile_(false)
    ,fFillImageX_(0.0)
    ,fFillImageY_(0.0)
    ,fStrokeWidth_(1.0)
    ,bStrokeColor_(false)
    ,bStrokeImageFile_(false)
    ,fStrokeImageX_(0.0)
    ,fStrokeImageY_(0.0)
  {}
  virtual ~Shape() {}

  inline virtual bool IsShape() { return true; }

  virtual void SetCoordinates(double fX, double fY);
  virtual void SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha);
  virtual void SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha);
  virtual void SetStrokeImageFile(const String& sFile);
  virtual void SetFillImageFile(const String& sFile);
  virtual void SetStrokeImageOffset(double fX, double fY);
  virtual void SetFillImageOffset(double fX, double fY);
  virtual void SetStrokeWidth(double fWidth);

  virtual void Draw(DrawContext& gc) = 0;

protected:
  void FillAndStroke(DrawContext& gc);

protected:
  double fX_;
  double fY_;

  bool bFillColor_;
  Color cFill_;
  bool bFillImageFile_;
  String sFillImageFile_;
  double fFillImageX_;
  double fFillImageY_;

  double fStrokeWidth_;
  bool bStrokeColor_;
  Color cStroke_;
  bool bStrokeImageFile_;
  String sStrokeImageFile_;
  double fStrokeImageX_;
  double fStrokeImageY_;
};

class RectangleX: public Shape
{
public:
  RectangleX(Surface* pSurface)
    :Shape(pSurface)
    ,fW_(0.0)
    ,fH_(0.0)
  {}
  virtual ~RectangleX() {}

  inline virtual bool IsRectangle() { return true; }

  inline virtual void SetSize(double fW, double fH) { fW_ = fW; fH_ = fH; }

  virtual void Draw(DrawContext& gc);

protected:
  double fW_;
  double fH_;
};

class ImageX: public Graphics
{
public:
  ImageX(Surface* pSurface)
    :Graphics(pSurface)
    ,fX_(0.0)
    ,fY_(0.0)
    ,bData_(false)
    ,bFile_(false)
    ,fAlpha_(1.0)
  {}
  virtual ~ImageX() {}

  inline virtual bool IsImage() { return true; }

  inline virtual void SetCoordinates(double fX, double fY) { fX_ = fX; fY_ = fY; }
  virtual void SetImageData(const Apollo::Image& image);
  virtual void DeleteImageData();
  virtual void SetImageFile(const String& sFile);
  virtual void DeleteImageFile();
  inline virtual void SetAlpha(double fAlpha) { fAlpha_ = fAlpha; }

  void GetSize(DrawContext& gc, double& fW, double& fH);

  void Draw(DrawContext& gc);

protected:
  double fX_;
  double fY_;
  bool bData_;
  Apollo::Image image_;
  bool bFile_;
  String sFile_;
  double fAlpha_;
};

class TextExtents
{
public:
  double fBearingX_;
  double fBearingY_;
  double fWidth_;
  double fHeight_;
  double fAdvanceX_;
  double fAdvanceY_;
};

class TextX: public Shape
{
public:
  // Same as Msg_Scene_SetText::FontFlags
  typedef enum _FontFlags { Normal = 0
    ,Italic = 1 << 0
    ,Bold = 1 << 1
    ,LastFlag = 1 << 2
  } FontFlags;

  TextX(Surface* pSurface)
    :Shape(pSurface)
    ,fSize_(0.0)
    ,nFlags_(0)
  {}
  virtual ~TextX() {}

  inline virtual bool IsText() { return true; }

  inline virtual void SetCoordinates(double fX, double fY) { fX_ = fX; fY_ = fY; }
  inline virtual void SetString(const String& sText) { sText_ = sText; }
  inline virtual void SetFont(const String& sFont, double fSize, int nFlags) { sFont_ = sFont; fSize_ = fSize; nFlags_ = nFlags; }

  void Measure(DrawContext& gc, TextExtents& te);

  virtual void Draw(DrawContext& gc);

protected:
  double fSize_;
  String sText_;
  String sFont_;
  int nFlags_;
};

class Sensor: public RectangleX
{
public:
  Sensor(Surface* pSurface, const String& sPath)
    :RectangleX(pSurface)
    ,sPath_(sPath)
    ,bHitAll_(0)
  {}
  virtual ~Sensor();

  inline bool IsSensor() { return true; }

  void CaptureMouse();
  void ReleaseMouse();
  virtual void MouseEvent(EventContext& gc, double fX, double fY);

protected:
  String sPath_;
  int bHitAll_;

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

#endif // Graphics_H_INCLUDED
