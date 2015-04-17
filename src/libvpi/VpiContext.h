// ============================================================================
//
// libvpi
//
// ============================================================================

#include "SSystem.h"
#include "XMLProcessor.h"

using namespace Apollo;

#define VPI_CHECKPTR(cls, inst, ptr, ret) cls* inst = (cls*) ptr; if (inst == 0) { return ret; };

typedef int (*VpiRequestFileCallback) (long ref, const char* url);
typedef int (*VpiComposeLocationCallback) (long ref, const char* protocol, const char* name, const char* service, char* url, unsigned int* url_len);
typedef int (*VpiSetDataCallback) (long ref, const char* key, void* data, unsigned long data_len);
typedef int (*VpiGetDataCallback) (long ref, const char* key, void* data, unsigned long* data_len);

typedef enum _VpiSearchPhase { NoPhase
  ,PhaseLocal
  ,PhaseGlobal
} VpiSearchPhase;

typedef enum _VpiSearchResult { NoResult
  ,ResultMissingData
  ,ResultNoMatch
  ,ResultDelegate
  ,ResultMatch
} VpiSearchResult;

//----------------------------------------------------------

class VpiSearchState
{
public:
  String sOriginalUrl_;
  String sVpiUrl_;
  String sDelegateUrl_;
  String sLocationXml_;
};

//----------------------------------------------------------

class VpiFile: public Elem
{
public:
  VpiFile(const String& sUrl);
  virtual ~VpiFile();

  int SetData(const String& sXml, time_t tCreated);
  int Expired(time_t tNow);
  VpiSearchResult CheckMatch(VpiSearchState& state);

protected:
  int ParseXml();

protected:
  String sXml_;
  XMLProcessor xml_;
  int bXmlParsed_;
  time_t tCreated_;
  int nLifetime_;
};

//----------------------------------------------------------

class VpiContext
{
public:
  VpiContext();
  virtual ~VpiContext();

  void SetRequestFileCallback(VpiRequestFileCallback fpCallback, long nRef);
  void SetComposeLocationCallback(VpiComposeLocationCallback fpCallback, long nRef);

  void SetTime(time_t tTime) { tNow_ = tTime; }

  int Clear();
  int ExpireFiles();
  int CountFiles() { return lFiles_.length(); }
  int SetXmlFile(const String& sUrl, const String& sXml);
  int SetSuffix(String& sSuffix);
  int GetSuffix(String& sSuffix);

  // This is what we basically need
  int GetLocationXml(String& sUrl, String& sLocationXml);

  // The general feature extraction
  int GetDetailXml(String& sXml, String& sPath, int nFlags, String& sDetailXml);
  // Extractor for chat the room URL (location URL) extraction
  int GetLocationUrl(String& sXml, String& sLocationUrl);
  // Extractor for suffix options
  int GetSelectOptions(String& sXml, List& lOptions);
  // Extractor for properties (tags, attributes) of a single suffix options
  int GetSelectOptionProperties(String& sXml, String& sSuffix, List& lOptions);

  int GetLastError() { return nError_; }
  String GetLastErrorString() { return sError_; }
  void SetError(const String& sWhere, int nError, const String& sError);
  int GetStatus(List& lStatus);
  int GetTraversedFiles(List& lFiles);

protected:
  VpiSearchResult CheckMatch(VpiSearchState& state);
  void IncrementStatusCounter(const String& sKey);

  int nError_;
  String sError_;

  VpiRequestFileCallback fpRequestFileCallback_;
  long nRequestFileCallbackRef_;
  VpiComposeLocationCallback fpComposeLocationCallback_;
  long nComposeLocationCallbackRef_;

  time_t tNow_;
  ListT<VpiFile, Elem> lFiles_;
  time_t tLastExpireFiles_;

  List lRequestedFiles_;
  List lTraversedFiles_;

  String sSuffix_;

  List lStatus_;
};
