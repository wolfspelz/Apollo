// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Element.h"

Element::~Element()
{
  if (pGraphics_) {
    delete pGraphics_;
    pGraphics_ = 0;
  }
  if (pChildren_) {
    delete pChildren_;
    pChildren_ = 0;
  }
}

Element* Element::FindElement(const String& sPath)
{
  Element* pResult = 0;

  if (pChildren_) {
    String sElement = sPath;
    String sPart;
    if (sElement.nextToken("/", sPart)) {
      ElementNode* pNode = pChildren_->Find(sPart);
      if (pNode) {
        Element* pElement = pNode->Value();
        if (sElement.empty()) {
          pResult = pElement;
        } else {
          pResult = pElement->FindElement(sElement);
        }
      }
    }
  }

  return pResult;
}

int Element::CreateElement(const String& sPath)
{
  int ok = 0;

  String sElement = sPath;
  String sPart;
  if (sElement.nextToken("/", sPart)) {
    if (sElement.empty()) {
      Element* pElement = new Element();
      if (pElement) {
        if (pChildren_ == 0) {
          pChildren_ = new ElementList();
        }
        if (pChildren_) {
          Element* pOld = 0;
          if (pChildren_->Get(sPart, pOld)) {
            pChildren_->Unset(sPart);
            delete pOld;
            pOld = 0;
          }
          ok = pChildren_->Set(sPart, pElement);
        }
      }
    } else {
      if (pChildren_ == 0) {
        pChildren_ = new ElementList();
      } 
      if (pChildren_) {
        Element* pNext = 0;
        if (pChildren_->Get(sPart, pNext)) {
          ok = pNext->CreateElement(sElement);
        } else {
          pNext = new Element();
          if (pNext) {
            pChildren_->Set(sPart, pNext);
            ok = pNext->CreateElement(sElement);
          }
        }
      }
    }
  }

  return ok;
}

int Element::DeleteElement(const String& sPath)
{
  int ok = 0;

  if (pChildren_) {
    String sElement = sPath;
    String sPart;
    if (sElement.nextToken("/", sPart)) {
      ElementNode* pNode = pChildren_->Find(sPart);
      if (pNode) {
        Element* pElement = pNode->Value();
        if (sElement.empty()) {
          pChildren_->Unset(sPart);
          delete pElement;
          pElement = 0;
          ok = 1;
        } else {
          ok = pElement->DeleteElement(sElement);
        }
      }
    }
  }

  return ok;
}

// ----------------------------------------------------------

void Element::CheckSaveRestore()
{
  bSave_ = (0
    || fTranslateX_ != 0.0
    || fTranslateY_ != 0.0
    || fScaleX_ != 1.0
    || fScaleY_ != 1.0
    || fRotate_ != 0.0
    || nCopyMode_ != CAIRO_OPERATOR_OVER
    );
}

void Element::Translate(double fX, double fY)
{
  fTranslateX_ = fX;
  fTranslateY_ = fY;
  //fTranslateY_ = -fY; // -V
  CheckSaveRestore();
}

void Element::Scale(double fX, double fY)
{
  fScaleX_ = fX;
  fScaleY_ = fY;
  CheckSaveRestore();
}

void Element::Rotate(double fAngle)
{
  fRotate_ = fAngle;
  CheckSaveRestore();
}

void Element::CopyMode(int nMode)
{
  nCopyMode_ = nMode;
  CheckSaveRestore();
}

void Element::Hide(int bHide)
{
  bHide_ = bHide;
}

// ----------------------------------------------------------

void Element::DeleteGraphics()
{
  if (pGraphics_) {
    delete pGraphics_;
    pGraphics_ = 0;
  }
}

void Element::CreateRectangle(double fX, double fY, double fW, double fH)
{
  DeleteGraphics();
  RectangleX* p = new RectangleX();
  if (p == 0) { throw ApException("Element::CreateRectangle failed"); }
  p->SetCoordinates(fX, fY, fW, fH);
  pGraphics_ = p;
}

void Element::CreateImageFromData(double fX, double fY, const Apollo::Image& image)
{
  DeleteGraphics();
  ImageX* p = new ImageX();
  if (p == 0) { throw ApException("Element::CreateImageFromData failed"); }
  p->SetCoordinates(fX, fY);
  p->SetImageData(image);
  pGraphics_ = p;
}

void Element::CreateImageFromFile(double fX, double fY, const String& sFile)
{
  DeleteGraphics();
  ImageX* p = new ImageX();
  if (p == 0) { throw ApException("Element::CreateImageFromFile failed"); }
  p->SetCoordinates(fX, fY);
  p->SetImageFile(sFile);
  pGraphics_ = p;
}

void Element::CreateText(double fX, double fY, const String& sText, const String& sFont, double fSize, int nFlags)
{
  DeleteGraphics();
  TextX* p = new TextX();
  if (p == 0) { throw ApException("Element::CreateText failed"); }
  p->SetCoordinates(fX, fY);
  p->SetString(sText);
  p->SetFont(sFont, fSize, nFlags);
  pGraphics_ = p;
}

void Element::SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  if (pGraphics_ && pGraphics_->IsShape()) {
    ((Shape*) pGraphics_)->SetFillColor(fRed, fGreen, fBlue, fAlpha);
  } else {
    throw ApException("Element::SetFillColor: not a Shape");
  }
}

void Element::SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  if (pGraphics_ && pGraphics_->IsShape()) {
    ((Shape*) pGraphics_)->SetStrokeColor(fRed, fGreen, fBlue, fAlpha);
  } else {
    throw ApException("Element::SetFillColor: not a Shape");
  }
}

void Element::SetStrokeImageFile(const String& sFile)
{
  if (pGraphics_ && pGraphics_->IsShape()) {
    ((Shape*) pGraphics_)->SetStrokeImageFile(sFile);
  } else {
    throw ApException("Element::SetStrokeImageFile: not a Shape");
  }
}

void Element::SetFillImageFile(const String& sFile)
{
  if (pGraphics_ && pGraphics_->IsShape()) {
    ((Shape*) pGraphics_)->SetFillImageFile(sFile);
  } else {
    throw ApException("Element::SetFillImageFile: not a Shape");
  }
}

void Element::SetStrokeImageOffset(double fX, double fY)
{
  if (pGraphics_ && pGraphics_->IsShape()) {
    ((Shape*) pGraphics_)->SetStrokeImageOffset(fX, fY);
  } else {
    throw ApException("Element::SetStrokeImageOffset: not a Shape");
  }
}

void Element::SetFillImageOffset(double fX, double fY)
{
  if (pGraphics_ && pGraphics_->IsShape()) {
    ((Shape*) pGraphics_)->SetFillImageOffset(fX, fY);
  } else {
    throw ApException("Element::SetFillImageOffset: not a Shape");
  }
}

void Element::SetStrokeWidth(double fWidth)
{
  if (pGraphics_ && pGraphics_->IsShape()) {
    ((Shape*) pGraphics_)->SetStrokeWidth(fWidth);
  } else {
    throw ApException("Element::SetFillColor: not a Shape");
  }
}

void Element::SetImageData(const Apollo::Image& image)
{
  if (pGraphics_ && pGraphics_->IsImage()) {
    ((ImageX*) pGraphics_)->SetImageData(image);
  } else {
    throw ApException("Element::SetImageData: not an Image");
  }
}

void Element::DeleteImageData()
{
  if (pGraphics_ && pGraphics_->IsImage()) {
    ((ImageX*) pGraphics_)->DeleteImageData();
  } else {
    throw ApException("Element::DeleteImageData: not an Image");
  }
}

void Element::SetImageFile(const String& sFile)
{
  if (pGraphics_ && pGraphics_->IsImage()) {
    ((ImageX*) pGraphics_)->SetImageFile(sFile);
  } else {
    throw ApException("Element::SetImageFile: not an Image");
  }
}

void Element::DeleteImageFile()
{
  if (pGraphics_ && pGraphics_->IsImage()) {
    ((ImageX*) pGraphics_)->DeleteImageFile();
  } else {
    throw ApException("Element::DeleteImageFile: not an Image");
  }
}

// ----------------------------------------------------------

void Element::Draw(GraphicsContext& gc)
{
  if (bHide_) { return; }

  //GraphicsContext gcNext = gc;
  //gcNext.fTranslateX_ += fTranslateX_;
  //gcNext.fTranslateY_ += fTranslateY_;

  if (bSave_) {
    cairo_save(gc.Cairo());

    if (fTranslateX_ != 0.0 || fTranslateY_ != 0.0) {
      cairo_translate(gc.Cairo(), fTranslateX_, fTranslateY_);
    }
    if (fRotate_ != 0.0) {
      cairo_rotate(gc.Cairo(), fRotate_);
    }
    if (fScaleX_ != 0.0 || fScaleY_ != 0.0) {
      cairo_scale(gc.Cairo(), fScaleX_, fScaleY_);
    }
    if (nCopyMode_ != CAIRO_OPERATOR_OVER) {
      cairo_set_operator(gc.Cairo(), (cairo_operator_t) nCopyMode_);
    }
  }

  if (pGraphics_) {
    pGraphics_->Draw(gc);
  }

  if (pChildren_) {
    ElementIterator iter(*pChildren_);
    for (ElementNode* pNode = 0; pNode = iter.Next(); ) {
      Element* pElement = pNode->Value();
      if (pElement != 0) {
        pElement->Draw(gc);
      }
    }
  }

  if (bSave_) {
    cairo_restore(gc.Cairo());
  }
}
