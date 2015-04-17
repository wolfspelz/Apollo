// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Dialog_H_INCLUDED)
#define Dialog_H_INCLUDED

class Dialog
{
public:
  Dialog::Dialog(const ApHandle& hDialog)
    :hAp_(hDialog)
    ,bVisible_(0)
    ,bDocumentLoaded_(0)
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(300)
    ,nHeight_(200)
  {}
  virtual ~Dialog();

  inline ApHandle apHandle() { return hAp_; }
  inline ApHandle GetView() { return hView_; }

  void Create(int nLeft, int nTop, int nWidth, int nHeight, int bVisible, const String& sCaption, const String& sIconUrl, const String& sContentUrl);
  void Destroy();
  void SetCaption(const String& sCaption);
  void SetIcon(const String& sIconUrl);
  void GetContentRect(int& nLeft, int& nTop,int& nWidth, int& nHeight);
  String CallScriptFunction(const String& sFunction, List& lArgs);
  void ContentCall(const String& sFunction, Apollo::SrpcMessage& srpc, Apollo::SrpcMessage& response);

  void OnDocumentLoaded();
  void OnContentLoaded(const String& sUrl);
  void OnLoadError(const String& sUrl, const String& sError);
  void OnReceivedFocus();
  void OnLostFocus();
  void OnUnload();

protected:
  ApHandle hAp_;
  ApHandle hView_;
  int bDocumentLoaded_;
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;
  String sUrl_;
  String sCaption_;
  String sIconUrl_;
  String sContentUrl_;
};

#endif // Dialog_H_INCLUDED
