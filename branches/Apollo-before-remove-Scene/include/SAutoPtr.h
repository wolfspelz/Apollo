// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SAutoPtr_h_INCLUDED)
#define SAutoPtr_h_INCLUDED

#include "SSystem.h"

template <class X> class AutoPtr
{
public:
  // = Initialization and termination methods
  AutoPtr(X *p = 0) : p_(p) {}

  AutoPtr(AutoPtr<X> &ap);
  AutoPtr<X> &operator=(AutoPtr<X> &rhs);
  ~AutoPtr(void);

  void reset(X *p = 0);
  X& operator=(X* p);

  X& operator*() const;
  X* get(void) const;
  X* release(void);
  X* operator->() const;
  bool operator==(X* p);
  bool operator!=(X* p);

protected:
  X *p_;
};

// -------------------------------------------------------

template<class X> SInline AutoPtr<X>::AutoPtr(AutoPtr<X> &ap)
:p_(ap.release())
{
}

template<class X> SInline X& AutoPtr<X>::operator=(X* p)
{
  this->reset(p);
  return *this->get();
}

template<class X> SInline X* AutoPtr<X>::get(void) const
{
  return this->p_;
}

template<class X> SInline X* AutoPtr<X>::release(void)
{
  X *old = this->p_;
  this->p_ = 0;
  return old;
}

template<class X> SInline void AutoPtr<X>::reset(X *p)
{
  if (this->get() != p) {
    delete this->get();
  }
  this->p_ = p;
}

template<class X> SInline AutoPtr<X> & AutoPtr<X>::operator= (AutoPtr<X> &rhs)
{
  if (this != &rhs) {
    this->reset (rhs.release());
  }
  return *this;
}

template<class X> SInline AutoPtr<X>::~AutoPtr(void)
{
  delete this->get();
}

template<class X> SInline X& AutoPtr<X>::operator*() const
{
  return *this->get();
}

template<class X> SInline X* AutoPtr<X>::operator->() const
{
  return this->get();
}

template<class X> SInline bool AutoPtr<X>::operator==(X* p)
{
  return this->get() == p;
}

template<class X> SInline bool AutoPtr<X>::operator!=(X* p)
{
  return ! this->operator==(p);
}

#endif // !defined(SAutoPtr_h_INCLUDED)


