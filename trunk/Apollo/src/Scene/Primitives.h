// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Primitives_H_INCLUDED)
#define Primitives_H_INCLUDED

#include "Element.h"

class ShapeElement: public Element
{
public:
  ShapeElement(Surface* pSurface)
    :Element(pSurface)
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
  virtual ~ShapeElement() {}

  inline virtual bool IsShape() { return true; }

  virtual void SetPosition(double fX, double fY);
  virtual void SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha);
  virtual void SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha);
  virtual void SetStrokeImageFile(const String& sFile);
  virtual void SetFillImageFile(const String& sFile);
  virtual void SetStrokeImageOffset(double fX, double fY);
  virtual void SetFillImageOffset(double fX, double fY);
  virtual void SetStrokeWidth(double fWidth);

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

  static double PI;
};

class RectangleElement: public ShapeElement
{
public:
  RectangleElement(Surface* pSurface)
    :ShapeElement(pSurface)
    ,fW_(0.0)
    ,fH_(0.0)
    ,nCorners_(NormalCorners)
    ,fRadius_(0.0)
  {}
  virtual ~RectangleElement() {}

  typedef enum _Corners { NormalCorners
    ,RoundCorners
    ,CurvedCorners
  } Corners;

  inline virtual bool IsRectangle() { return true; }
  virtual void Draw(DrawContext& gc);

  inline virtual void SetSize(double fW, double fH) { fW_ = fW; fH_ = fH; }
  inline virtual void SetCorners(Corners nCorners) { nCorners_ = nCorners; }
  inline virtual void SetCornerRadius(double fRadius) { fRadius_ = fRadius; }

protected:
  double fW_;
  double fH_;
  Corners nCorners_;
  double fRadius_;
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

class TextElement: public ShapeElement
{
public:
  // Same as Msg_Scene_SetText::FontFlags
  typedef enum _FontFlags { Normal = 0
    ,Italic = 1 << 0
    ,Bold = 1 << 1
    ,LastFlag = 1 << 2
  } FontFlags;

  TextElement(Surface* pSurface)
    :ShapeElement(pSurface)
    ,fSize_(0.0)
    ,nFlags_(0)
  {}
  virtual ~TextElement() {}

  inline virtual bool IsText() { return true; }
  virtual void Draw(DrawContext& gc);

  inline virtual void SetString(const String& sText) { sText_ = sText; }
  inline virtual void SetFont(const String& sFont, double fSize, int nFlags) { sFont_ = sFont; fSize_ = fSize; nFlags_ = nFlags; }

  void GetExtents(DrawContext& gc, TextExtents& te);

protected:
  double fSize_;
  String sText_;
  String sFont_;
  int nFlags_;
};

class ImageElement: public Element
{
public:
  ImageElement(Surface* pSurface)
    :Element(pSurface)
    ,fX_(0.0)
    ,fY_(0.0)
    ,bData_(false)
    ,bFile_(false)
    ,fAlpha_(1.0)
  {}
  virtual ~ImageElement() {}

  inline virtual bool IsImage() { return true; }
  void Draw(DrawContext& gc);

  inline virtual void SetPosition(double fX, double fY) { fX_ = fX; fY_ = fY; }
  virtual void SetData(const Apollo::Image& image);
  virtual void DeleteData();
  virtual void SetFile(const String& sFile);
  virtual void DeleteFile();
  inline virtual void SetAlpha(double fAlpha) { fAlpha_ = fAlpha; }

  void GetSize(DrawContext& gc, double& fW, double& fH);

protected:
  double fX_;
  double fY_;
  bool bData_;
  Apollo::Image image_;
  bool bFile_;
  String sFile_;
  double fAlpha_;
};

class SensorElement: public RectangleElement
{
public:
  SensorElement(Surface* pSurface, const String& sPath)
    :RectangleElement(pSurface)
    ,sPath_(sPath)
    ,bCaptured_(0)
  {}
  virtual ~SensorElement() {}

  inline bool IsSensor() { return true; }
  void Draw(DrawContext& gc);
  void MouseEvent(EventContext& gc, double fX, double fY);

  void MouseCaptured();
  void MouseReleased();

protected:
  String sPath_;
  int bCaptured_;
};

#endif // Primitives_H_INCLUDED
