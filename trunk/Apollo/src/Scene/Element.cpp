// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Element.h"

Element* Element::FindElement(const String& sPath)
{
  Element* pResult = 0;

  String sElement = sPath;
  String sPart;
  if (sElement.nextToken("/", sPart)) {
    Element* pElement = 0;
    list_.Get(sElement, pElement);
    if (sElement.empty()) {
      pResult = pElement;
    } else {
      pResult = pElement->FindElement(sElement);
    }
  }

  return pResult;
}

void Element::AddElement(const String& sPath, Element* pElement)
{
  String sElement = sPath;
  String sPart;
  if (sElement.nextToken("/", sPart)) {
    if (sElement.empty()) {
      list_.Set(sPart, pElement);
    } else {
      Element* pNext = 0;
      if (list_.Get(sPart, pNext)) {
        pNext->AddElement(sElement, pElement);
      } else {
        pNext = new Node();
        if (pNext) {
          list_.Set(sPart, pNext);
          pNext->AddElement(sElement, pElement);
        }
      }
    }
  }
  
}

void Element::DrawAll(cairo_t* cr)
{
  Draw(cr);

  ElementIterator iter(list_);
  for (ElementNode* pNode = 0; pNode = iter.Next(); ) {
    Element* pElement = pNode->Value();
    if (pElement != 0) {
      pElement->DrawAll(cr);
    }
  }
}

// ----------------------------------------------------------

void Shape::SetFillColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
  bFillColor_ = true;
  cFill_.r = fRed;
  cFill_.g = fGreen;
  cFill_.b = fBlue;
  cFill_.a = fAlpha;
}

void Shape::SetStrokeColor(float fWidth, float fRed, float fGreen, float fBlue, float fAlpha)
{
  bStrokeColor_ = true;
  fStrokeWidth_ = fWidth;
  cStroke_.r = fRed;
  cStroke_.g = fGreen;
  cStroke_.b = fBlue;
  cStroke_.a = fAlpha;
}

// ----------------------------------------------------------

void RectangleX::Draw(cairo_t* cr)
{
  cairo_rectangle(cr, fX_, fY_, fW_, fH_);
  if (bFillColor_) {
    cairo_set_source_rgba(cr, cFill_.r, cFill_.g, cFill_.b, cFill_.a > 0.99 ? 0.99 : cFill_.a);
    if (bStrokeColor_) {
      cairo_fill_preserve(cr);
    } else {
      cairo_fill(cr);
    }
  }
  if (bStrokeColor_) {
    cairo_set_line_width(cr, fStrokeWidth_);
    cairo_set_source_rgba(cr, cStroke_.r, cStroke_.g, cStroke_.b, cStroke_.a > 0.99 ? 0.99 : cStroke_.a);
    cairo_stroke(cr);
  }
}

