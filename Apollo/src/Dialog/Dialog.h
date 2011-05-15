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
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(300)
    ,nHeight_(200)
  {}
  virtual ~Dialog();

  inline ApHandle apHandle() { return hAp_; }
  inline ApHandle GetView() { return hView_; }

  void Create(int nLeft, int nTop, int nWidth, int nHeight, int bVisible, const String& sContentUrl);
  void Destroy();

protected:
  ApHandle hAp_;
  ApHandle hView_;
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;
};

#endif // Dialog_H_INCLUDED
