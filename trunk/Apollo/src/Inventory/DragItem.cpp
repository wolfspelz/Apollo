// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "DragItem.h"

void DragItem::Init()
{
  Create();
}

void DragItem::Exit()
{
  Destroy();
}

void DragItem::Create()
{
  if (ApIsHandle(hView_)) {
    Destroy();
  }

  ApHandle h = Apollo::newHandle();

  if (!Msg_WebView_Create::_(h, 0, 0, 100, 100)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Create failed"); }
  if (!Msg_WebView_Visibility::Hide(h)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Visibility failed"); }
  if (!Msg_WebView_SetScriptAccessPolicy::Allow(h)) { throw ApException(LOG_CONTEXT, "Msg_WebView_SetScriptAccessPolicy failed"); }
  if (!Msg_WebView_Load::_(h, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "DragItem.html")) { throw ApException(LOG_CONTEXT, "Msg_WebView_Load failed"); }

  hView_ = h;
}

void DragItem::Destroy()
{
  if (ApIsHandle(hView_)) {
    Msg_WebView_Destroy::_(hView_);
    hView_ = ApNoHandle;
  }
  hItem_ = ApNoHandle;
}

void DragItem::SetImage(const String& sUrl)
{
  if (ApIsHandle(hView_)) {
    List lArgs;
    lArgs.AddLast(sUrl);
    String sResult = Msg_WebView_CallScriptFunction::_(hView_, "", "SetImage", lArgs);
  }
}

void DragItem::SetPosition(int nLeft, int nTop, int nWidth, int nHeight, int nPinX, int nPinY)
{
  nPinX_ = nPinX;
  nPinY_ = nPinY;

  if (ApIsHandle(hView_)) {
    if (!Msg_WebView_Position::_(hView_, nLeft, nTop, nWidth, nHeight)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Position failed"); }
  }
}

void DragItem::Show()
{
  if (ApIsHandle(hView_)) {
    if (!Msg_WebView_Visibility::Show(hView_)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Visibility failed"); }

    List lArgs;
    lArgs.AddLast(String::from(nPinX_));
    lArgs.AddLast(String::from(nPinY_));
    String sResult = Msg_WebView_CallScriptFunction::_(hView_, "", "BeginDrag", lArgs);
  }
}

void DragItem::Hide()
{
  if (ApIsHandle(hView_)) {
    if (!Msg_WebView_Visibility::Hide(hView_)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Visibility failed"); }
  }
}
