// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApTypes.h"
#include "Apollo.h"
#include "ApCompiler.h"

// ---------------------------------------------------------

ApHandle ApHandle::hNoHandle_;

void ApHandle::fromString(const String& s)
{ 
  if (s.startsWith("[") && s.endsWith("]")) {
    String s1 = s.subString(1);
    String s2 = s1.subString(0, s1.chars() - 1);
    if (s2.chars() > 9) {
      // hi and lo
      String s2hi = s2.subString(0, s2.chars() - 9);
      String s2lo = s2.subString(s2.chars() - 9);
      nHi_ = String::atoi(s2hi);
      nLo_ = String::atoi(s2lo);
    } else {
      // lo only
      nHi_ = 0;
      nLo_ = String::atoi(s2);
    }
  }
}

String ApHandle::toString()
{ 
  String s;
  if (nHi_ > 0) {
    s.appendf("[%d%09d]", nHi_, nLo_);
  } else {
    s.appendf("[%d]", nLo_);
  }
  return s;
}

ApHandle ApHandle::operator++()
{
  nLo_++;
  if (nLo_ >= 1000000000) {
    nLo_ = 0;
    nHi_++;
  }

  return *this;
}

bool ApHandle::operator<(const ApHandle &h) const
{
  if (nHi_ < h.nHi_) {
    return true; 
  } else if (nHi_ == h.nHi_) {
    return nLo_ < h.nLo_;
  } else {
    return false;
  }
}

// ---------------------------------------------------------

int Apollo::StringList::Init(const String& sData)
{
  int ok = 1;

  Empty();
  KeyValueBlob2List(sData, *this, " ", "=", "");

  return ok;
}

int Apollo::StringList::IsSet(const String& sKey)
{
  Elem* e = FindByName(sKey);
  return (e != 0);
}

// ---------------------------------------------------------

int Apollo::ValueElem::getInt()
{
  switch (nType_) {
    case TypeString:
      return String::atoi(Elem::getString());
      break;
    case TypeHandle:
      // tsnh
      break;
  }
  return Elem::getInt();
}

String& Apollo::ValueElem::getString()
{
  switch (nType_) {
    case TypeInt:
      Elem::setString(String::from(Elem::getInt()));
      break;
    case TypeHandle:
      Elem::setString(getHandle().toString());
      break;
  }
  return Elem::getString();
}

ApHandle Apollo::ValueElem::getHandle()
{
  switch (nType_) {
    case TypeInt:
      // tsnh
      break;
    case TypeString:
      hAp_.fromString(Elem::getString());
      break;
  }
  return hAp_;
}

// ---------------------------------------------------------

void Apollo::ValueList::add(int n)
{
  Apollo::ValueElem* e = new Apollo::ValueElem();
  if (e != 0) {
    e->setInt(n);
    AddLast((Elem*) e);
  } 
}

void Apollo::ValueList::add(const String& s)
{
  Apollo::ValueElem* e = new Apollo::ValueElem();
  if (e != 0) {
    e->setString(s);
    AddLast((Elem*) e);
  }
}

void Apollo::ValueList::add(ApHandle h)
{
  Apollo::ValueElem* e = new Apollo::ValueElem();
  if (e != 0) {
    e->setHandle(h);
    AddLast((Elem*) e);
  }
}

Apollo::ValueElem* Apollo::ValueList::elemAtIndex(int nIndex)
{
  int nCnt = 0;
  for (Apollo::ValueElem* e = 0; (e = nextElem(e)) != 0; ) {
    if (nCnt == nIndex) {
      return e;
    }
    nCnt++;
  }
  return 0;
}

int Apollo::ValueList::atIndex(int nIndex, int nDefault)
{
  Apollo::ValueElem* e = elemAtIndex(nIndex);
  if (e != 0) {
    return e->getInt();
  }
  return nDefault;
}

String Apollo::ValueList::atIndex(int nIndex, const String& sDefault)
{
  Apollo::ValueElem* e = elemAtIndex(nIndex);
  if (e != 0) {
    return e->getString();
  }
  return sDefault;
}

ApHandle Apollo::ValueList::atIndex(int nIndex, ApHandle hDefault)
{
  Apollo::ValueElem* e = elemAtIndex(nIndex);
  if (e != 0) {
    return e->getHandle();
  }
  return hDefault;
}

void Apollo::ValueList::fromList(List& l)
{
  for (Elem* e = 0; (e = l.Next(e)) != 0; ) {
    add(e->getString());
  }
}

void Apollo::ValueList::toList(List& l)
{
  for (Apollo::ValueElem* e = 0; (e = nextElem(e)) != 0; ) {
    Elem* f = new Elem();
    if (f != 0) {
      if (0) {
      } else if (e->getType() == Apollo::KeyValueElem::TypeInt) {
        *f = e->getInt();
      } else if (e->getType() == Apollo::KeyValueElem::TypeString) {
        *f = e->getString();
      } else if (e->getType() == Apollo::KeyValueElem::TypeHandle) {
        *f = e->getString();
      }
      l.AddLast(f);
    }
  }
}

String Apollo::ValueList::toString()
{
  String sResult;

  for (ValueElem* e = 0; (e = nextElem(e)) != 0; ) {
    if (!sResult.empty()) { sResult += " "; }
    sResult += e->getString();
  }

  return sResult;
}

void Apollo::ValueList::operator=(ValueList& l)
{
  List::Empty();

  for (ValueElem* e = 0; (e = l.nextElem(e)) != 0; ) {
    switch (e->getType()) {
      case ValueElem::TypeInt: add(e->getInt()); break;
      case ValueElem::TypeString: add(e->getString()); break;
      case ValueElem::TypeHandle: add(e->getHandle()); break;
    }
  }
}

// ---------------------------------------------------------

void Apollo::KeyValueList::add(const String& sKey, int n)
{
  Apollo::KeyValueElem* e = new Apollo::KeyValueElem(sKey);
  if (e != 0) {
    e->setInt(n); 
    AddLast((Elem*) e);
  }
}

void Apollo::KeyValueList::add(const String& sKey, const String& s)
{
  Apollo::KeyValueElem* e = new Apollo::KeyValueElem(sKey);
  if (e != 0) {
    e->setString(s); 
    AddLast((Elem*) e);
  }
}

void Apollo::KeyValueList::add(const String& sKey, ApHandle h)
{
  Apollo::KeyValueElem* e = new Apollo::KeyValueElem(sKey);
  if (e != 0) {
    e->setHandle(h);
    AddLast((Elem*) e);
  }
}

Apollo::KeyValueElem& Apollo::KeyValueList::operator[](const String& s)
{
  Apollo::KeyValueElem* e = find(s);
  if (e == 0) {
    e = new Apollo::KeyValueElem(s);
    if (e != 0) {
      addElem(e);
    }
  }
  return *e;
}

void Apollo::KeyValueList::fromList(List& l)
{
  for (Elem* e = 0; (e = l.Next(e)) != 0; ) {
    add(e->getName(), e->getString());
  }
}

void Apollo::KeyValueList::toList(List& l)
{
  for (Apollo::KeyValueElem* e = 0; (e = nextElem(e)) != 0; ) {
    Elem* f = new Elem(e->getKey());
    if (f != 0) {
      if (0) {
      } else if (e->getType() == Apollo::KeyValueElem::TypeInt) {
        *f = e->getInt();
      } else if (e->getType() == Apollo::KeyValueElem::TypeString) {
        *f = e->getString();
      }
      l.AddLast(f);
    }
  }
}

String Apollo::KeyValueList::toString()
{
  String sResult;

  for (Apollo::KeyValueElem* e = 0; (e = nextElem(e)) != 0; ) {
    String sValue = e->getString();
    sValue.escape(String::EscapeCRLF);
    sResult.appendf("%s=%s\n", StringType(e->getName()), StringType(sValue));
  }

  return sResult;
}

void Apollo::KeyValueList::operator=(KeyValueList& l)
{
  List::Empty();

  for (KeyValueElem* e = 0; (e = l.nextElem(e)) != 0; ) {
    switch (e->getType()) {
      case ValueElem::TypeInt: add(e->getKey(), e->getInt()); break;
      case ValueElem::TypeString: add(e->getKey(), e->getString()); break;
      case ValueElem::TypeHandle: add(e->getKey(), e->getHandle()); break;
    }
  }
}

// -------------------------------------------------------------------

#if defined(WIN32)
DWORDLONG Apollo::TimeValue::FILETIME_to_timval_skew_ = PLATFORM_S64(0x19db1ded53e8000);

void Apollo::TimeValue::Set(const FILETIME &ft)
{
  ULARGE_INTEGER _100ns;
  _100ns.LowPart = ft.dwLowDateTime;
  _100ns.HighPart = ft.dwHighDateTime;
  _100ns.QuadPart -= FILETIME_to_timval_skew_;

  // Convert 100ns units to seconds;
  nSec_ = (long) (_100ns.QuadPart / (10000 * 1000));
  // Convert remainder to microseconds;
  nMicroSec_ = (long) ((_100ns.QuadPart % (10000 * 1000)) / 10);
}
#endif

Apollo::TimeValue Apollo::TimeValue::getTime()
{
#if defined(WIN32)
  FILETIME ft;
  ::GetSystemTimeAsFileTime(&ft);
  Apollo::TimeValue tv(ft);
  return tv;
#elif defined(LINUX) || defined(MAC)
  struct timeval lin_tv;
  if (::gettimeofday(&lin_tv, NULL) == 0) {
    return TimeValue(lin_tv.tv_sec, lin_tv.tv_usec);
  }
  return TimeValue();
#else
  return TimeValue();
#endif
}

void Apollo::TimeValue::operator=(time_t nSec)
{
  nSec_ = nSec;
  nMicroSec_ = 0;
}

bool Apollo::TimeValue::operator<(Apollo::TimeValue& tv)
{
  if (nSec_ == tv.nSec_) {
    return nMicroSec_ < tv.nMicroSec_;
  } else {
    return nSec_ < tv.nSec_;
  }
}

bool Apollo::TimeValue::operator>(Apollo::TimeValue& tv)
{
  if (nSec_ == tv.nSec_) {
    return nMicroSec_ > tv.nMicroSec_;
  } else {
    return nSec_ > tv.nSec_;
  }
}

bool Apollo::TimeValue::operator==(Apollo::TimeValue& tv)
{
  return nSec_ == tv.nSec_ && nMicroSec_ == tv.nMicroSec_;
}

bool Apollo::TimeValue::operator>=(Apollo::TimeValue& tv)
{
  return operator>(tv) || operator==(tv);
}

bool Apollo::TimeValue::operator<=(Apollo::TimeValue& tv)
{
  return operator<(tv) || operator==(tv);
}

Apollo::TimeValue Apollo::TimeValue::operator+(Apollo::TimeValue& tv)
{
  Apollo::TimeValue tvSum = *this;
  tvSum.nMicroSec_ += tv.nMicroSec_;
  int nCarry = 0;
  if (tvSum.nMicroSec_ >= 1000000) {
    tvSum.nMicroSec_ -= 1000000;
    nCarry = 1;
  }
  tvSum.nSec_ += tv.nSec_ + nCarry;
  return tvSum;
}

Apollo::TimeValue Apollo::TimeValue::operator-(Apollo::TimeValue& tv)
{
  Apollo::TimeValue tvDiff = *this;
  tvDiff.nMicroSec_ -= tv.nMicroSec_;
  int nCarry = 0;
  if (tvDiff.nMicroSec_ < 0) {
    tvDiff.nMicroSec_ += 1000000;
    nCarry = 1;
  }
  tvDiff.nSec_ -= tv.nSec_ + nCarry;
  return tvDiff;
}

String Apollo::TimeValue::toString()
{
  time_t tSec = Sec();
  struct tm tms = *(::localtime(&tSec));
  
  String s;
  s.appendf("%04d.%02d.%02d-%02d:%02d:%02d.%06d", tms.tm_year + 1900, tms.tm_mon + 1, tms.tm_mday, tms.tm_hour, tms.tm_min, tms.tm_sec, MicroSec());
  return s;
}
