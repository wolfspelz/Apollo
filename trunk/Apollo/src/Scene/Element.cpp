// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Element.h"
#include "Surface.h"

Element::~Element()
{
  if (pChildren_) {
    delete pChildren_;
    pChildren_ = 0;
  }
}

Element* Element::FindElement(const String& sPath)
{
  Element* pResult = 0;

  if (sPath.empty()) {
    pResult = this;
  } else {
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
  }

  return pResult;
}

void Element::AddChild(const String& sName, Element* pElement)
{
  int ok = 0;

  if (pChildren_ == 0) {
    pChildren_ = new ElementList();
  }
  if (pChildren_) {
    Element* pOld = 0;
    if (pChildren_->Get(sName, pOld)) {
      pChildren_->Unset(sName);
      delete pOld;
      pOld = 0;
    }
    ok = pChildren_->Set(sName, pElement);
  }

  if (!ok) {
    throw ApException("Element::AddChild failed: %s", StringType(sName));
  }
}

Element* Element::CreateElement(const String& sPath)
{
  Element* pResult = 0;

  String sElement = sPath;
  String sPart;
  if (sElement.nextToken("/", sPart)) {
    if (sElement.empty()) {
      Element* pElement = new Element(pSurface_);
      if (pElement) {
        AddChild(sPart, pElement);
        pResult = pElement;
      }
    } else {
      if (pChildren_ == 0) {
        pChildren_ = new ElementList();
      } 
      if (pChildren_) {
        Element* pNext = 0;
        if (pChildren_->Get(sPart, pNext)) {
          pResult = pNext->CreateElement(sElement);
        } else {
          pNext = new Element(pSurface_);
          if (pNext) {
            pChildren_->Set(sPart, pNext);
            pResult = pNext->CreateElement(sElement);
          }
        }
      }
    }
  }

  return pResult;
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

void Element::GetChildren(Apollo::ValueList& vlChildren)
{
  if (pChildren_) {
    ElementIterator iter(*pChildren_);
    for (ElementNode* pNode = 0; (pNode = iter.Next()); ) {
      vlChildren.add(pNode->Key());
    }
  }
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

void Element::Hide(int bHide)
{
  bHide_ = bHide;
}

void Element::CopyMode(int nMode)
{
  nCopyMode_ = nMode;
  CheckSaveRestore();
}

// ----------------------------------------------------------

void Element::GetTranslate(double& fX, double& fY)
{
  fX = fTranslateX_;
  fY = fTranslateY_;
}

void Element::GetScale(double& fX, double& fY)
{
  fX = fScaleX_;
  fY = fScaleY_;
}

void Element::GetRotate(double& fRotate)
{
  fRotate = fRotate_;
}

// ----------------------------------------------------------

RectangleElement* Element::AsRectangle()
{
  if (IsRectangle()) {
    return (RectangleElement*) this;
  } else {
    throw ApException("Element::AsRectangle: not a Rectangle");
  }
}

TextElement* Element::AsText()
{
  if (IsText()) {
    return (TextElement*) this;
  } else {
    throw ApException("Element::AsText: not a Text");
  }
}

ShapeElement* Element::AsShape()
{
  if (IsShape()) {
    return (ShapeElement*) this;
  } else {
    throw ApException("Element::AsShape: not a Shape");
  }
}

ImageElement* Element::AsImage()
{
  if (IsImage()) {
    return (ImageElement*) this;
  } else {
    throw ApException("Element::AsImage: not a Image");
  }
}

SensorElement* Element::AsSensor()
{
  if (IsSensor()) {
    return (SensorElement*) this;
  } else {
    throw ApException("Element::AsSensor: not a Sensor");
  }
}

// ----------------------------------------------------------

void Element::DrawRecursive(DrawContext& gc)
{
  if (bHide_) { return; }

  if (gc.nDepth_ == 0) {
    if (pSurface_->LogDraw()) {
      apLog_Verbose((LOG_CHANNEL, "Element::DrawRecursive", "Begin"));
    }
  }

  gc.nDepth_++;

  if (bSave_) {
    cairo_save(gc.Cairo());

    if (fTranslateX_ != 0.0 || fTranslateY_ != 0.0) {
      cairo_translate(gc.Cairo(), fTranslateX_, fTranslateY_);
    }
    if (fRotate_ != 0.0) {
      cairo_rotate(gc.Cairo(), -fRotate_);
    }
    if (fScaleX_ != 0.0 || fScaleY_ != 0.0) {
      cairo_scale(gc.Cairo(), fScaleX_, fScaleY_);
    }
    if (nCopyMode_ != CAIRO_OPERATOR_OVER) {
      cairo_set_operator(gc.Cairo(), (cairo_operator_t) nCopyMode_);
    }
  }

  // Base first...
  Draw(gc);

  // ...the depth
  if (pChildren_) {
    ElementIterator iter(*pChildren_);
    String sWSP = "                      ";
    for (ElementNode* pNode = 0; pNode = iter.Next(); ) {
      Element* pElement = pNode->Value();
      if (pElement != 0) {
        if (pSurface_->LogDraw()) {
          apLog_Verbose((LOG_CHANNEL, "Element::DrawRecursive", "%s%s", StringType(sWSP.subString(0, 2*gc.nDepth_)), StringType(pNode->Key())));
        }
        pElement->DrawRecursive(gc);
      }
    }
  }

  if (bSave_) {
    cairo_restore(gc.Cairo());
  }

  gc.nDepth_--;

  if (gc.nDepth_ == 0) {
    if (pSurface_->LogDraw()) {
      apLog_Verbose((LOG_CHANNEL, "Element::DrawRecursive", "End"));
    }
  }

}

void Element::MouseEventRecursive(EventContext& gc, double fX, double fY)
{
  if (bHide_) { return; }

  gc.nDepth_++;

  // Depth first
  if (pChildren_) {
    ElementIterator iter(*pChildren_);
    for (ElementNode* pNode = 0; (pNode = iter.Next()) && !gc.Fired(); ) {
      Element* pElement = pNode->Value();
      if (pElement != 0) {
        pElement->MouseEventRecursive(gc, fX, fY);
      }
    }
  }

  if (IsSensor()) {
    AsSensor()->MouseEvent(gc, fX, fY);
  }

  gc.nDepth_--;
}

