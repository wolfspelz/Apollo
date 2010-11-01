// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Display_H_INCLUDED)
#define Display_H_INCLUDED

class Display
{
public:
  Display(const ApHandle& hContext);
  virtual ~Display();

  int Create();
  void Destroy();

  void SetVisibility(int bVisible);
  void SetPosition(int nX, int nY);
  void SetSize(int nW, int nH);

  void EnterRequested();
  void EnterBegin();
  void EnterComplete();
  void LeaveRequested();
  void LeaveBegin();
  void LeaveComplete();

  inline int GetWidth() { return nW_; }
  inline int GetHeight() { return nH_; }

  inline ApHandle Scene() { return hScene_; }

protected:
  ApHandle hContext_;
  ApHandle hLocation_;
  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  ApHandle hScene_;
};

#endif // Display_H_INCLUDED
