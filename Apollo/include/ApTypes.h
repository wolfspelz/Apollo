// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApTypes_h_INCLUDED)
#define ApTypes_h_INCLUDED

#include "SSystem.h"
#include "ApExports.h"
#include "ApCompiler.h"

// Apollo types begin here

class ApMessage;

typedef void (*ApCallback) (ApMessage* pMsg);
typedef void* ApCallbackRef;
typedef long  ApCallbackPos;

// typedef unsigned long long ApHandle;
//typedef unsigned long ApHandle;

class APOLLO_API ApHandle
{
public:
  inline ApHandle(): nLo_(0) ,nHi_(0) {}
  inline ApHandle(const ApHandle& h): nLo_(h.nLo_) ,nHi_(h.nHi_) {}
  inline ApHandle(unsigned long nHi, unsigned long nLo): nLo_(nLo) ,nHi_(nHi) {}
  //ApHandle(const String& s);  // replaced by fromString(), because of accidental conversions

  inline bool operator==(const ApHandle& h) { return h.nLo_ == nLo_ && h.nHi_ == nHi_; }
  inline bool operator!=(const ApHandle& h) { return h.nLo_ != nLo_ || h.nHi_ != nHi_; }

  //operator String(); // replaced by toString(), because of accidental conversions
  void fromString(const String& s);
  String toString() const;
  ApHandle operator++();
  bool operator<(const ApHandle &h) const;

  unsigned long nLo_;
  unsigned long nHi_;

  static ApHandle hNoHandle_;
};

#define ApNoHandle ApHandle::hNoHandle_
#define ApIsHandle(__h__) (ApNoHandle != __h__)

//#define ApHandleFormat "[0x%016llx]" // format for printf
//typedef unsigned long long ApHandleType; // extractor for printf
#define ApHandleFormat "%s" // format for printf
//typedef unsigned long ApHandleType; // extractor for printf
//#define ApHandleType((unsigned long)) // extractor for printf
#define ApHandleType(__h__) (const char*) __h__.toString() // extractor for printf

// ----------- OS dependent ------------------

// OS dependent types begin here

#if !defined(WIN32)
  typedef void *HANDLE;
  typedef void *HMODULE;
#if !defined(UINT)
  typedef unsigned int UINT;
#endif // UINT
#if !defined(DWORD)
  typedef unsigned long DWORD;
#endif
#if !defined(SOCKET)
  typedef int SOCKET;
#endif
#if !defined(MAC) && !defined(BOOL)
  typedef int BOOL;
#endif
#endif // WIN32

AP_NAMESPACE_BEGIN

// ----------- StringList ------------------

class APOLLO_API StringList: public List
{
public:
  StringList();
  StringList(const String& sData) { (void) Init(sData); }
  int Init(const String& sData);
  int IsSet(const String& sKey);
};

// ----------- KeyValueList ------------------

class APOLLO_API ValueElem: protected Elem
{
public:
  ValueElem()
    :Elem()
    ,nType_(NoType)
    ,hAp_(ApNoHandle)
  {}

  typedef enum _Type { NoType = 0
    ,TypeInt
    ,TypeString
    ,TypeHandle
  } Type;

  inline Type getType() { return nType_; }

  inline int getInt();
  inline String& getString();
  inline ApHandle getHandle();

  inline void setInt(int n) { Elem::setInt(n); setType(TypeInt); }
  inline void setString(const String& s) { Elem::setString(s); setType(TypeString); }
  inline void setHandle(const ApHandle& h) { hAp_ = h; setType(TypeHandle); }

protected: 
  friend class KeyValueList;
  inline void setType(Type nType) { nType_ = nType; }

protected: 
  Type nType_;
  ApHandle hAp_;
};

class APOLLO_API KeyValueElem: public ValueElem
{
  typedef Elem ValueElem;

public:
  KeyValueElem()
  {}
  KeyValueElem(const String& sKey)
  {
    setName(sKey);
  }

  inline String& getKey() { return getName(); }
};

class APOLLO_API ValueList: protected List
{
public:
  ValueList() {}
  virtual ~ValueList() {}

  void add(int n);
  void add(const String& s);
  void add(const ApHandle& h);

  ValueElem* findString(const String& s) { return (ValueElem*) List::FindByString(s); }

  inline void addElem(ValueElem* e) { List::AddLast((Elem*) e); }
  inline void removeElem(ValueElem* e) { List::Remove((Elem*) e); }
  inline ValueElem* nextElem(ValueElem* e) { return (ValueElem*) List::Next((Elem*) e); }

  Apollo::ValueElem* elemAtIndex(int nIndex);
  int atIndex(int nIndex, int nDefault);
  String atIndex(int nIndex, const String& sDefault);
  ApHandle atIndex(int nIndex, const ApHandle& hDefault);

  inline int length() { return List::length(); }

  void operator=(ValueList& l);
  void removeAll() { List::Empty(); }
  void fromList(List& l);
  void toList(List& l);
  String toString();
};

class APOLLO_API KeyValueList: public ValueList
{
public:
  KeyValueList() {}
  KeyValueList(ValueList& kv);
  virtual ~KeyValueList() {}

  void add(const String& sKey, int n);
  void add(const String& sKey, const String& s);
  void add(const String& sKey, const ApHandle& h);

  typedef enum _Flags { NoFlags = 0, IgnoreCase = 1 } Flags;

  inline KeyValueElem* find(const String& sKey, Flags nFlags = NoFlags) { return nFlags == IgnoreCase ? (KeyValueElem*) List::FindByName(sKey) : (KeyValueElem*) List::FindByNameCase(sKey); }
  KeyValueElem& operator[](const String& s);
  inline KeyValueElem* findString(const String& s) { return (KeyValueElem*) List::FindByString(s); }

  void addElem(KeyValueElem* e) { List::AddLast((Elem*) e); }
  void removeElem(KeyValueElem* e) { List::Remove((Elem*) e); }
  inline KeyValueElem* nextElem(ValueElem* e) { return (KeyValueElem*) List::Next((Elem*) e); }

  void operator=(KeyValueList& l);
  void removeAll() { List::Empty(); }
  void fromList(List& l);
  void toList(List& l);
  String toString();
};

// -------------------------------------------------------------------

class APOLLO_API TimeValue
{
public:
  TimeValue()
    :nSec_(0)
    ,nMicroSec_(0)
  {}

  TimeValue(time_t nSec, unsigned int nMicroSec)
    :nSec_(nSec)
    ,nMicroSec_(nMicroSec)
  {}

#if defined(WIN32)
  TimeValue(const FILETIME &ft)
    :nSec_(0)
    ,nMicroSec_(0)
  { Set(ft); }
#endif
  virtual ~TimeValue() {}

#if defined(WIN32)
  static DWORDLONG FILETIME_to_timval_skew_;
  void Set(const FILETIME &ft);
#elif defined(LINUX) || defined(MAC)
#endif
  static TimeValue getTime();

  inline bool isNull() { return nSec_ == 0 && nMicroSec_ == 0; }
  void operator=(time_t nSec);
  TimeValue operator+(TimeValue& tv);
  inline TimeValue& operator+=(TimeValue& tv) { *this = *this + tv; return *this; }
  TimeValue operator-(TimeValue& tv);
  bool operator<(TimeValue& tv);
  bool operator>(TimeValue& tv);
  bool operator==(const TimeValue& tv);
  inline bool operator!=(const TimeValue& tv) { return !operator==(tv); }
  bool operator>=(TimeValue& tv);
  bool operator<=(TimeValue& tv);
  inline unsigned int Sec() { return nSec_; }
  inline unsigned MicroSec() { return nMicroSec_; }
  inline unsigned MilliSec() { return nSec_ * 1000 + nMicroSec_ / 1000; }

  String toString();

protected:
  time_t nSec_;
  int nMicroSec_; // not unsigned, because operator- uses nMicroSec_ to compute carry
};

class APOLLO_API StopWatch
{
public:
  StopWatch() { Reset(); }

  inline void Reset() { tBegin_ = Apollo::TimeValue::getTime(); }
  inline Apollo::TimeValue GetDuration() { return Apollo::TimeValue::getTime() - tBegin_; }
  operator const char* () { Apollo::TimeValue tDuration = GetDuration(); s_ = ""; s_.appendf("%d.%06d", tDuration.Sec(), tDuration.MicroSec()); return s_; }

  Apollo::TimeValue tBegin_;
  String s_;
};

AP_NAMESPACE_END

#endif // !defined(ApTypes_h_INCLUDED)
