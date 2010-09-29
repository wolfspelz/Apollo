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
          ok = pChildren_->Set(sPart, pElement);
        }
      }
    } else {
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

void Element::SetRectangle(double fX, double fY, double fW, double fH)
{
  if (pGraphics_) {
    if (pGraphics_->IsRectangle()) {
      ((RectangleX*) pGraphics_)->SetCoordinates(fX, fY, fW, fH);
    } else {
      delete pGraphics_;
      pGraphics_ = 0;
    }
  }

  if (pGraphics_ == 0) {
    pGraphics_ = new RectangleX(fX, fY, fW, fH);
  }

  if (pGraphics_ == 0 || !pGraphics_->IsRectangle()) {
    throw ApException("Element::SetRectangle failed");
  }
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

void Element::SetStrokeWidth(double fWidth)
{
  if (pGraphics_ && pGraphics_->IsShape()) {
    ((Shape*) pGraphics_)->SetStrokeWidth(fWidth);
  } else {
    throw ApException("Element::SetFillColor: not a Shape");
  }
}

// ----------------------------------------------------------

void Element::Draw(cairo_t* cr)
{
  if (pGraphics_) {
    pGraphics_->Draw(cr);
  }

  if (pChildren_) {
    ElementIterator iter(*pChildren_);
    for (ElementNode* pNode = 0; pNode = iter.Next(); ) {
      Element* pElement = pNode->Value();
      if (pElement != 0) {
        pElement->Draw(cr);
      }
    }
  }
}

