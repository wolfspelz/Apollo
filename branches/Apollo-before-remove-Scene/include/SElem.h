// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SElem_h_INCLUDED)
#define SElem_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SExport Elem  
{
public:
  Elem();
  Elem(const String& sName);
  Elem(const String& sName, int nValue);
  Elem(const String& sName, const String& sStr);
  Elem(const String& sName, int nValue, const String& sStr);
  Elem(int nValue);
  void operator=(Elem &elem);
  virtual ~Elem();

  // write/read the data part as plain data
  bool writeData(size_t nDataLen, char* pDataPtr);
  bool readData(size_t *nDataLen, char* *pDataPtr);
  bool readData(size_t *nDataLen, const char* *pDataPtr) const;

  // write/read the int value
  int getInt() const; // read int
  int setInt(int nValue); // write int
  void operator=(int nValue) { setInt(nValue); }

  void setString(const String& sStr);
  String& getString();
  void operator=(const String& szStr) { setString(szStr); }
  const String& getString() const;

  void setName(const String& szName);
  String& getName();
  const String& getName() const;

  // write/read the pointer
  void* getPtr(); // read pointer
  const void* getPtr() const; // read pointer
  void setPtr(void* pPtr); // write pointer

protected:
  // clear fields, not free
  void zero();

public:
  static const char* szEmpty_;

  Elem* pLink_;
protected:
  String sName_;
  String sString_;
  int nInt_; // stores an int value
  void* pPtr_; // stores a void pointer
  char* pData_; // stores data
  size_t nLen_; // length of data bytes (<=size)
  size_t nSize_; // size of allocated data area
};

extern Elem Elem_eEmpty;

#if defined(__cplusplus)
extern "C" {
#endif

SExport bool Elem_FindByObjectRefFilter(void* refCon, void* e);
SExport bool Elem_FindByStringFilter(void* refCon, void* e);
SExport bool Elem_FindByNameFilter(void* refCon, void* e);
SExport bool Elem_FindByNameCaseFilter(void* refCon, void* e);
SExport bool Elem_FindByIntFilter(void* refCon, void* e);
SExport bool Elem_FindByPtrFilter(void* refCon, void* e);

SExport int Elem_CompareNameFilter(void* refCon, void* e);

#if defined(__cplusplus)
}
#endif

// ============================================================================
// Binary buffer wrapper for Elem

class SExport Buffer: public Elem
{
public:
  Buffer() :Elem() {}
  Buffer(const unsigned char* pDataPtr, size_t nDataLen) :Elem() { SetData(pDataPtr, nDataLen); }
  Buffer(Buffer const &b) { SetData(b.Data(), b.Length()); }

  bool SetData(const unsigned char* pDataPtr, size_t nDataLen);
  bool SetData(const char* pDataPtr, size_t nDataLen) { return SetData((const unsigned char*) pDataPtr, nDataLen); }
  bool SetData(const String& data) { return SetData((const unsigned char*) data.c_str(), data.bytes()); }
  Buffer& operator=(Buffer const &b) { SetData(b.Data(), b.Length()); return *this; }

  unsigned char* Data(void);
  const unsigned char* Data(void) const;
  int GetString(String& _data) const;
  size_t Length(void) const;

  bool Append(unsigned char* pDataPtr, size_t nDataLen);
  bool Discard(size_t usedLen);
  void Empty(void) { SetData((const unsigned char*) 0, 0); }

public:
  int encodeBase64(String& _string_out);
  int decodeBase64(const String& _string_in);
  int dump_encode(String& _string_out);
  void encodeBinhex(String& _string_out);
  void decodeBinhex(const String& _string_in);
private:
  int HTUU_encode (unsigned char* bufin, unsigned int nbytes, char* bufcoded);
  int HTUU_decode(const char* bufcoded, unsigned char* bufplain, int outbufsize);

private:
  // disallow to avoid confusion with Data()
  void* getPtr() { return (void*) 0; }
  void setPtr(void*) {};
};

// ============================================================================

class SExport xFile: protected Buffer
{
  typedef Buffer super;
public:
  xFile(const char* szPathname) { sPathname_ = szPathname; }
  String& Path(void) { return sPathname_; }
  void Path(const String& szPathname) { sPathname_ = szPathname; }
  int Exists(void);
  int Delete(void);
  int Age(void);
  int Load(void);
  int Save(int nFlags = 0);
  int CreatePath(void);
  int Rename(String& sNewName);
  int GetData(Buffer& sbData);
  int GetData(String& sData);
  int SetData(Buffer& sbData);
  int SetData(const String& sData) { return Buffer::SetData(sData); }
  int SetData(unsigned char* pDataPtr, size_t nDataLen) { return Buffer::SetData(pDataPtr, nDataLen); }
  int AppendToFile(unsigned char* pData, size_t nLen, int nFlags);
  enum {
    SaveFlag_CreatePath = 1
  };
private:
  String sPathname_;
  unsigned char* Data(void) { return Buffer::Data(); }
  long Length(void) { return Buffer::Length(); }
};

// ============================================================================

typedef bool (*ElemFindFilterProc) (void* refCon, void* e);

class SExport List: public Elem
{
public:
  List();
  List(const String& sName);
  List(List& l);
  virtual ~List();
  void ClearList();

  // --------------------------------
  // Add/Remove
  
  Elem* Add(Elem* e);
  Elem* Add(const String& sName);
  Elem* Add(const String& sName, const String& sStr);
  Elem* Add(const String& sName, int nValue);
  Elem* AddLast(Elem* e);
  Elem* AddLast(const String& sName);
  Elem* AddLast(const String& sName, const String& sStr);
  Elem* AddLast(const String& sName, int nValue);
  Elem* AddFirst(Elem* e);
  Elem* AddFirst(const String& sName);
  Elem* AddAfter(Elem* ePrevious, Elem* e);
  Elem* Insert(Elem* ePrevious, Elem* e);
  void Remove(Elem* e);

  // --------------------------------
  // Use

  String Collapse(const char* szSep);
  void Empty();
  Elem* Next(Elem* e);
  Elem* First();
  long NumElem();
  inline size_t length() { return NumElem(); }  
  bool IsEmpty();
  Elem* Last();
  
  // --------------------------------
  // Find
  
  Elem* FindElemCore(ElemFindFilterProc fpFilter, void* nRefCon, bool dqFlag);
  Elem* FindByString(const String& sString);
  Elem* FindByName(const String& sName);
  Elem* FindByNameCase(const String& sName);
  Elem* FindByInt(int nValue);
  Elem* FindByPtr(void* ptr);
  Elem* Find(ElemFindFilterProc fpFilter, void* p);
  Elem* Find(Elem* e);
  void SortByName();
  Elem& operator[](const String& sName);
  Elem& operator[](int nIndex);
  void operator=(List& l);

  // --------------------------------
  // Private
  
private:
  Elem* pHead_;
  long nElem_;
};

//----------------------------------------------------------

extern SExport void KeyValueLfBlob2List(const char* _text, List &_list);
extern SExport void KeyValueBlob2List(const char* _text, List &_list, const char* _linesep, const char* _fieldsep, const char* _escape);
extern SExport int List_SortByName_compare(const void *arg1, const void *arg2);

#endif // !defined(SElem_h_INCLUDED)
