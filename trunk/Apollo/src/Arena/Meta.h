// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Meta_H_INCLUDED)
#define Meta_H_INCLUDED

#include "Layer.h"

class Meta: public Layer
{
public:
  Meta(Display* pDisplay);
  virtual ~Meta();

  typedef enum _State { NoState
    ,StateEnterRequested
    ,StateEnterBegin
    ,StateEnterComplete
    ,StateLeaveRequested
    ,StateLeaveBegin
    ,StateLeaveComplete
  } State;

  inline State GetState() { return nState_; }

  void OnEnterRequested();
  void OnEnterBegin();
  void OnEnterComplete();
  void OnLeaveRequested();
  void OnLeaveBegin();
  void OnLeaveComplete();

  void OnDocumentUrl(const String& sUrl);
  void OnLocationUrl(const String& sUrl);

protected:
  void SetTrayColor(double r, double g, double b);
  void ShowText();

protected:
  State nState_;
  String sDocumentUrl_;
  String sLocationUrl_;
};

#endif // Meta_H_INCLUDED
