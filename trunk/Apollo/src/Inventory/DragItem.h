// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(DragItem_H_INCLUDED)
#define DragItem_H_INCLUDED

class DragItem
{
public:
  DragItem()
    :nPinX_(0)
    ,nPinY_(0)
  {}
  virtual ~DragItem() {}

  void Init();
  void Exit();

  void Create();
  void Destroy();
  ApHandle& GetView() { return hView_; };

  void SetItem(const ApHandle& hItem) { hItem_ = hItem; };
  ApHandle& GetItem() { return hItem_; };
  void SetImage(const String& sUrl);
  void SetPosition(int nLeft, int nTop, int nWidth, int nHeight, int nPinX, int nPinY);
  void Show();
  void Hide();

protected:
  ApHandle hView_;
  ApHandle hItem_;
  int nPinX_;
  int nPinY_;
};

#endif // DragItem_H_INCLUDED
