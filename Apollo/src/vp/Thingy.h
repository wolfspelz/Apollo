// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Thingy_H_INCLUDED)
#define Thingy_H_INCLUDED

class Thingy;

class ThingyProvider
{
public:
  ThingyProvider(): pThingy_(0) {}
  virtual ~ThingyProvider() {}
  void setOwner(Thingy* pThingy);

  virtual void getString(String& sValue, String& sMimeType) = 0;
  virtual void getData(Buffer& sbData, String& sMimeType, String& sSource) = 0;
  virtual void onSubscribe() {}
  virtual void onUnsubscribe() {}
  virtual void onUnsubscribeAll() {}

protected:
  Thingy* pThingy_;
};

class Thingy: public Elem
{
public:
  Thingy(const String& sKey, ThingyProvider* pProvider)
    :Elem(sKey)
    ,pProvider_(pProvider)
    ,nSubscriber_(0)
    ,bChanged_(0)
  {}
  virtual ~Thingy();

  ThingyProvider* getProvider() { return pProvider_; }

  virtual void setChanged() { bChanged_ = 1; }
  virtual int isChanged() { return bChanged_; }
  virtual void clearChanged() { bChanged_ = 0; }

  virtual int isSubscribed() { return nSubscriber_ > 0; }
  virtual void addSubscription();
  virtual void removeSubscription();
  virtual void clearSubscriptions();

protected:
  ThingyProvider* pProvider_;
  int nSubscriber_;
  int bChanged_;
};

template <class E> class ThingyList: public ListT<E, Elem>
{
public:
  ThingyList() :nInChanges_(0) {}
  virtual ~ThingyList() {}

  E* getOrCreate(const String& sKey)
  {
    E* pThingy = FindByName(sKey);
    if (pThingy == 0) {
      pThingy = newThingy(sKey);
      if (pThingy != 0) {
        Add(pThingy);
      }
    }

    return pThingy;
  }

  E* newThingy(const String& sKey)
  {
    Thingy* pThingy = 0;

    ThingyProvider* pProvider = newProvider(sKey);
    if (pProvider) {
      pThingy = new Thingy(sKey, pProvider);
    }
    
    if (!pThingy) {
      if (pProvider) { delete pProvider; }
    } else {
      pProvider->setOwner(pThingy);
    }

    return (E*) pThingy;
  }

  void beginChanges() { nInChanges_++; }
  void endChanges()
  {
    nInChanges_--;
    if (nInChanges_ <= 0) {
      nInChanges_ = 0;

      Apollo::ValueList vlChanges;
      for (E* pThingy = 0; (pThingy = Next(pThingy)) != 0; ) {
        if (pThingy->isChanged()) {
          if (pThingy->isSubscribed()) {
            vlChanges.add(pThingy->getName());
          }
          pThingy->clearChanged();
        }
      }

      if (vlChanges.length() > 0) {
        sendChanges(vlChanges);
      }
    }
  }

  void setChanged(const String& sKey)
  {
    E* pThingy = FindByName(sKey);
    if (pThingy != 0) {
      pThingy->setChanged();
    }
  }
  
  virtual void sendChanges(Apollo::ValueList& vlChanges) = 0;

protected:
  virtual ThingyProvider* newProvider(const String& sKey) = 0;
  int nInChanges_;
};

#endif // Thingy_H_INCLUDED
