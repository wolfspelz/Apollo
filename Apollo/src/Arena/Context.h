// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Context_H_INCLUDED)
#define Context_H_INCLUDED

class Context
{
public:
  Context(ApHandle hContext);
  virtual ~Context();

  int Create();
  void Destroy();

  void SetVisibility(int bVisible);
  void SetPosition(int nX, int nY);
  void SetSize(int nW, int nH);

protected:
  ApHandle hAp_;
  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  ApHandle hScene_;
};

#endif // Context_H_INCLUDED
