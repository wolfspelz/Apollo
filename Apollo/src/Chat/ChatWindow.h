// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Chat_H_INCLUDED)
#define Chat_H_INCLUDED

class ChatWindow
{
public:
  ChatWindow::ChatWindow(const ApHandle& hChat)
    :hAp_(hChat)
    ,bVisible_(0)
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(600)
    ,nHeight_(400)
  {}
  virtual ~ChatWindow();

  inline ApHandle apHandle() { return hAp_; }
  inline ApHandle GetContext() { return hContext_; }

  void AttachToLocation(const ApHandle& hLocation);

  void Open();
  void Close();
  void OnUnload();
  void OnCallModule(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

protected:
  ApHandle hAp_;
  ApHandle hContext_;
  ApHandle hLocation_;
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;
};

#endif // Chat_H_INCLUDED
