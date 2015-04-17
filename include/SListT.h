// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SListT_h_INCLUDED)
#define SListT_h_INCLUDED

template <class E, class L> class ListT: public L
{
private:
  E* pHead_;
  long nElem_;
  const E& dummy() const
  {
    static E dummyElem;
    return dummyElem;
  }

public:
  ListT()
  {
    ClearList();
  }

  ListT(const String& sName)
  {
    ClearList();
    L::setName(sName);
  }

  ListT(ListT<E, L>& l)
  {
    *this = l;
  }

  ~ListT()
  {
    Empty();
  }

  void ClearList()
  {
    pHead_ = 0;
    nElem_ = 0;
  }

  // --------------------------------
  // Add/Remove

  E* Add(E* e)
  {
    if (e != 0) {
      e->pLink_ = pHead_;
      pHead_ = e;
      nElem_++;
      return e;
    }
    return e;
  }

  E* Add(const String& sName)
  {
    E* e = new E();
    if (e != 0) {
      e->setName(sName);
  	  return Add(e);
    }
    return e;
  }

  E* Add(const String& sName, const String& sStr)
  {
    E* e = new E();
    if (e != 0) {
      e->setName(sName);
      e->setString(sStr);
  	  return Add(e);
    }
    return e;
  }

  E* Add(const String& sName, int nValue)
  {
    E* e = new E();
    if (e != 0) {
      e->setName(sName);
      e->setInt(nValue);
  	  return Add(e);
    }
    return e;
  }

  E* AddLast(const String& sName)
  {
    E* e = new E();
    if (e != 0) {
      e->setName(sName);
  	  AddLast(e);
    }
    return e;
  }

  E* AddLast(const String& sName, const String& sStr)
  {
    E* e = new E();
    if (e != 0) {
      e->setName(sName);
      e->setString(sStr);
  	  return AddLast(e);
    }
    return e;
  }

  E* AddFirst(E* e)
  {
    (void) Add(e);
    return e;
  }

  E* AddLast(E* e)
  {
    E** pReferer = 0;
    bool bFound = false;
  
    if (e != 0) {
      pReferer = &(pHead_);
      while (! bFound) {
        if (*pReferer == 0) {
          bFound = true;
          *pReferer = e;
          e->pLink_ = 0;
          nElem_++;
        } else {
          pReferer = (E**) &((*pReferer)->pLink_) ;
        }
      }
    }
    return e;
  }

  E* AddAfter(E* ePrevious, E* e)
  {
    if (ePrevious == 0) {
      pHead_ = e;
      e->pLink_ = 0;
    } else {
      e->pLink_ = ePrevious->pLink_;
      ePrevious->pLink_ = e;
    }
    nElem_++;
    return e;
  }

  E* Insert(E* ePrevious, E* e)
  {
    e->pLink_= ePrevious->pLink_;
    ePrevious->pLink_= e;
    nElem_++;
    return e;
  }

  void Remove(E* e)
  {
    FindElemCore(&Elem_FindByObjectRefFilter, (void*) e, true);
  }  

  // --------------------------------
  // Use

  String Collapse(const char* szSep)
  {
    String sResult;
    for (E* e = 0; (e = Next(e)) != 0; ) {
      if (sResult != "") { sResult += szSep; }
      sResult += e->getName();
    }
    return sResult;
  }

  void Empty()
  {
    E* eSub = 0;
    while (pHead_ != 0) {
      eSub = pHead_;
      Remove(eSub);
      delete eSub;
    }
  }

  E* Next(E* e)
  {
    E* eResult= 0;
  
    if (e== 0) {
      eResult = pHead_;
    } else {
      eResult = (E*) (e->pLink_);
    }
    return eResult;
  }

  const E* Next(const E* e) const
  {
    const E* eResult= 0;
  
    if (e== 0) {
      eResult = pHead_;
    } else {
      eResult = (E*) (e->pLink_);
    }
    return eResult;
  }

  E* First()
  {
    return Next(0);
  }
  
  const E* First() const
  {
    return Next(0);
  }

  long NumElem() const
  {
    return nElem_;
  }
  size_t length() { return NumElem(); }  

  bool IsEmpty() const
  {
    return (nElem_ == 0);
  }

  E* Last()
  {
    E* eResult = 0;
    E* eTmp = pHead_;
    while (eTmp != 0) {
      eResult= eTmp;
      eTmp= (E*) eTmp->pLink_;
    }
    return eResult;
  }

  const E* Last() const
  {
    E* eResult = 0;
    E* eTmp = pHead_;
    while (eTmp != 0) {
      eResult= eTmp;
      eTmp= (E*) eTmp->pLink_;
    }
    return eResult;
  }

  // --------------------------------
  // Find

  E* FindElemCore(ElemFindFilterProc fpFilter, void* nRefCon, bool dqFlag)
  {
    E* eResult = 0;
    E** pReferer = 0;
    bool bFound = false;
  
    if (pHead_ != 0) {
      pReferer = &(pHead_);
      while (((*pReferer) != 0) && ! bFound) {
        if ((*fpFilter) (nRefCon, *pReferer)) {
          bFound = true;
        } else {
          // ru: We still get warning in gcc release compile here, hmpf...
          // warning: "dereferencing type-punned pointer may break strict aliasing rules" :/
          Elem** pLink = &((*pReferer)->pLink_);
          pReferer =  (E**)pLink;
        }
      }
    }
  
    if (bFound) {
      eResult = *pReferer;
      if (dqFlag) {
        *pReferer = (E*) (*pReferer)->pLink_;
        nElem_--;
      }
    }
  
    return eResult;
  }

  const E* FindElemCore(ElemFindFilterProc fpFilter, void* nRefCon) const
  {
    const E* eResult = 0;
    const E** pReferer = 0;
    bool bFound = false;
  
    if (pHead_ != 0) {
      pReferer = (const E**) &(pHead_);
      while (((*pReferer) != 0) && ! bFound) {
        if ((*fpFilter) (nRefCon, (void*) *pReferer)) {
          bFound = true;
        } else {
          // ru: We still get warning in gcc release compile here, hmpf...
          // warning: "dereferencing type-punned pointer may break strict aliasing rules" :/
          const Elem** pLink = (const Elem**) &((*pReferer)->pLink_);
          pReferer =  (const E**)pLink;
        }
      }
    }
  
    if (bFound) {
      eResult = *pReferer;
    }
  
    return eResult;
  }

  E* FindByString(const String& sString)
  {
	  return FindElemCore(&Elem_FindByStringFilter, (void*) (const char*) sString, false);
  }

  const E* FindByString(const String& sString) const
  {
	  return FindElemCore(&Elem_FindByStringFilter, (void*) (const char*) sString);
  }

  E* FindByName(const String& sName)
  {
    return FindElemCore(&Elem_FindByNameFilter, (void*) (const char*) sName, false);
  }

  const E* FindByName(const String& sName) const
  {
    return FindElemCore(&Elem_FindByNameFilter, (void*) (const char*) sName);
  }

  E* FindByNameCase(const String& sName)
  {
    return FindElemCore(&Elem_FindByNameCaseFilter, (void*) (const char*) sName, false);
  }

  const E* FindByNameCase(const String& sName) const
  {
    return FindElemCore(&Elem_FindByNameCaseFilter, (void*) (const char*) sName);
  }

  E* FindByLong(int nValue)
  {
    return FindElemCore(&Elem_FindByIntFilter, (void*) nValue, false);
  }

  const E* FindByLong(int nValue) const
  {
    return FindElemCore(&Elem_FindByIntFilter, (void*) nValue);
  }

  E* FindByPtr(void* ptr)
  {
    return FindElemCore(&Elem_FindByPtrFilter, ptr, false);
  }

  const E* FindByPtr(void* ptr) const
  {
    return FindElemCore(&Elem_FindByPtrFilter, ptr);
  }

  E* Find(ElemFindFilterProc fpFilter, void* p)
  {
    return FindElemCore(fpFilter, p, false);
  }

  const E* Find(ElemFindFilterProc fpFilter, void* p) const
  {
    return FindElemCore(fpFilter, p);
  }

  E* Find(E* e)
  {
    return FindElemCore(&Elem_FindByObjectRefFilter, (void*) e, false);
  }

  const E* Find(E* e) const
  {
    return FindElemCore(&Elem_FindByObjectRefFilter, (void*) e);
  }

  void SortByName()
  {
    E** a = new E*[length()];
    if (a != 0) {
      int nCnt = 0;
      while (First() != 0) {
        E* e = First();
        Remove(e);
        a[nCnt++] = e;
      }

      qsort((void*) a, (size_t) nCnt, sizeof(E*), List_SortByName_compare);

      for (int i = 0; i < nCnt; i++) {
        AddLast(a[i]);
      }

      delete [] a;
    }
  }

  E& operator[](const String& sName)
  {
    E* e = FindByName(sName);
    if (e == 0) {
      e = new E();
      if (e != 0) {
        e->setName(sName);
        Add(e);
      }
    }
    return *e;
  }

  const E& operator[](const String& sName) const
  {
    const E* e = FindByName(sName);
    if (e == 0) {
      return dummy();
    }
    return *e;
  }

  E& operator[](int nIndex)
  {
    E* e = FindByLong(nIndex);
    if (e == 0) {
      e = new E();
      if (e != 0) {
        e->setInt(nIndex);
        Add(e);
      }
    }
    return *e;
  }

  const E& operator[](int nIndex) const
  {
    const E* e = FindByLong(nIndex);
    if (e == 0) {
      return dummy();
    }
    return *e;
  }

  void operator=(ListT<E, L>& l)
  {
    ClearList();

    L::operator=(l);

    E* e = 0;
    while ((e= l.Next(e)) != 0) {
      E* eDuplicate = new E();
      *eDuplicate = *e;

      AddLast(eDuplicate);
    }
  }

};

//----------------------------------------------------------

template <class E> class SafeListIteratorT;

//template <class E, class L> class ListT: public L
template <class E> class SafeListT: public ListT<E, Elem>
{
public:
  SafeListT<E>(): ListT<E, Elem>(), pIter_(0) {}
  SafeListT<E>(const String& sName): ListT<E, Elem>(sName), pIter_(0) {}
  ~SafeListT<E>()
  {
    if (pIter_ != 0) {
      pIter_->DestructingList();
    }
  }

  void Remove(E* e)
  {
    if (pIter_ != 0) {
      pIter_->RemovingElem(e);
    }
    ListT<E, Elem>::Remove(e);
  }

  SafeListIteratorT<E>* SetIterator(SafeListIteratorT<E>* pIter) { SafeListIteratorT<E>* pPreviousIter = pIter_; pIter_ = pIter; return pPreviousIter; }

protected:
  SafeListIteratorT<E>* pIter_;

private:
  //Elem* Next(Elem* e);
};

template <class E> class SafeListIteratorT
{
public:
  SafeListIteratorT<E>(SafeListT<E>& l)
    :pList_(&l)
    ,pCurrent_(0)
    ,pNext_(0)
    ,nPos_(Begin)
  {
    if (pList_ != 0) { pList_->SetIterator(this); }
  }
  ~SafeListIteratorT<E>()
  {
    if (pList_ != 0) { (void) pList_->SetIterator(0); }
  }

  typedef enum _Position { NoPosition
    ,Begin
    ,Inside
    ,End
  } Position;

  E* Next()
  {
    switch (nPos_) {
    case Begin:
      pCurrent_ = pList_->First();
      nPos_ = Inside;
      break;
    case Inside:
      if (pNext_ != 0) {
        pCurrent_ = pNext_;
        pNext_ = 0;
      } else {
        pCurrent_ = pList_->Next(pCurrent_);
      }
      if (pCurrent_ == 0) {
        nPos_ = End;
      } else {
        nPos_ = Inside;
      }
      break;
    case End:
      pCurrent_ = 0;
      nPos_ = End;
      break;
    default:
      pCurrent_ = pList_->First();
      nPos_ = Inside;
    }
    return pCurrent_;
  }

  E* Current()
  {
    switch (nPos_) {
    case Begin:
      return 0;
      break;
    case Inside:
      return pCurrent_;
      break;
    case End:
      return 0;
      break;
    default:
      return 0;
      break;
    }
    return 0;
  }

  void RemovingElem(E* e)
  {
    if (e == pCurrent_) {
      pCurrent_ = 0;
      pNext_ = (E*) e->pLink_;
      if (pNext_ == 0) {
        nPos_ = End;
      }
    }
  }

  void DestructingList()
  {
    pList_ = 0;
    pCurrent_ = 0;
    nPos_ = End;
  }

  SafeListT<E>* pList_;
  E* pCurrent_;
  E* pNext_;
  Position nPos_;
};

typedef SafeListT<Elem> SafeList;
typedef SafeListIteratorT<Elem> SafeListIterator;

#endif // !defined(SListT_h_INCLUDED)
