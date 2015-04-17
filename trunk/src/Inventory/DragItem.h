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
    ,bInDropZone_(0)
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

  void SetInDropZone(int bInside) { bInDropZone_ = bInside; }
  int GetInDropZone() { return bInDropZone_; }

protected:
  ApHandle hView_;
  ApHandle hItem_;
  int nPinX_;
  int nPinY_;
  int bInDropZone_;
};

#endif // DragItem_H_INCLUDED
