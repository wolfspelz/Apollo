// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Meta_H_INCLUDED)
#define Meta_H_INCLUDED

class Display;

class Meta
{
public:
  Meta(Display* pDisplay);
  virtual ~Meta();

  int Meta::Create();
  void Meta::Destroy();

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

protected:

protected:
  Display* pDisplay_;
  int bActive_;
  State nState_;
  ApHandle hScene_;
};

#endif // Meta_H_INCLUDED
