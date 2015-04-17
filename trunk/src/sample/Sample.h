// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Sample_H_INCLUDED)
#define Sample_H_INCLUDED

class Sample
{
public:
  Sample::Sample(const ApHandle& hSample)
    :hAp_(hSample)
    ,bVisible_(0)
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(600)
    ,nHeight_(400)
  {}
  virtual ~Sample();

  inline ApHandle apHandle() { return hAp_; }

  void Create();
  void Destroy();

protected:
  ApHandle hAp_;
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;
};

#endif // Sample_H_INCLUDED
