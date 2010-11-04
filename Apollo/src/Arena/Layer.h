// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Layer_H_INCLUDED)
#define Layer_H_INCLUDED

class Display;

class Layer
{
public:
  Layer(Display* pDisplay);
  virtual ~Layer();

  virtual void OnSetSize(int nW, int nH);
  virtual int OnTimer(const ApHandle& hTimer);
  virtual int OnMouseEvent(Msg_Scene_MouseEvent* pMsg);

protected:
  Display* pDisplay_;
  ApHandle hScene_;
};

#endif // Layer_H_INCLUDED
