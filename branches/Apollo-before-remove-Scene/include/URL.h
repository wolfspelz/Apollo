// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(URL_h_INCLUDED)
#define URL_h_INCLUDED

#include "SSystem.h"
#include "ApExports.h"

AP_NAMESPACE_BEGIN

class APOLLO_API URL: public String
{
  typedef String super;

public:
  URL(const char* url);
  URL();
  virtual ~URL();

  void Normalize(void);

  enum {
    MAX_REQUEST_PATH_SECTIONS= 4,
    MAX_URL_PART_PROTO_SIZE= 128,
    MAX_URL_PART_CRED_SIZE= 64,
    MAX_URL_PART_HOST_SIZE= 256,
    MAX_URL_PART_PORT_SIZE= 20,
    MAX_URL_PART_PATH_SIZE= 1024,
    MAX_URL_PART_FILE_SIZE= MAX_URL_PART_PATH_SIZE, // sometimes, there is no path and the path part used as the file part
    MAX_URL_PART_ARG_SIZE= 4096,

    MAX_URL_SIZE= MAX_URL_PART_PROTO_SIZE \
    + MAX_URL_PART_CRED_SIZE \
    + MAX_URL_PART_HOST_SIZE \
    + MAX_URL_PART_PORT_SIZE \
    + MAX_URL_PART_PATH_SIZE \
    + MAX_URL_PART_FILE_SIZE \
    + MAX_URL_PART_ARG_SIZE,

    MAX_URL_PART_USER_SIZE= 256, // for mailto: URLs (not yet implemented)
    MAX_URL_PART_NUM_SIZE= 128, // for ICQ URLs, etc  (not yet implemented)

    MAX_URL_PART_dummy= 0
  };

  virtual String &protocol(void);
  virtual String &credentials(void);
  virtual String &host(void);
  virtual String &port(void);
  virtual int portnum(void);
  virtual String &path(void);
  virtual String &file(void);
  virtual String &args(void);

  virtual String &filebase(void); // proto://host:port/path/
  virtual String &base(void); // proto://host:port/
  virtual String &uri(void); // /path/file?args

  typedef enum Status { UNKNOWN= 0
    ,RAW
    ,OK
    ,NOTOK= OK+1
  };

public:
  int SplitURL(char* url, int forceRelative, int* implicitport, char* protostr, char* credstr, char* hoststr, char* portstr, char* pathstr, char* filestr, char* argstr);

private:
  Status status_;
  int force_relative_;

private:
  String proto_;
  String cred_;
  String host_;
  String port_;
  String path_;
  String file_;
  String arg_;

  int portnum_;

  String base_;
  String filebase_;
  String uri_;

#if defined(WIN32)
#else
  int ScanForProtocol(char* url, char* *partstr, int *partstrlen, char* *nextstr);
  int ScanForCredentials(char* url, char* *partstr, int *partstrlen, char* *nextstr);
  int ScanForHost(char* url, char* *partstr, int *partstrlen, char* *nextstr, int preferhost);
  int ScanForPort(char* url, char* *partstr, int *partstrlen, char* *nextstr);
  int ScanForPath(char* url, char* *partstr, int *partstrlen, char* *nextstr);
  int ScanForArguments(char* url, char* *partstr, int *partstrlen, char* *nextstr);

  int IsDirectoryIndex(const char* filename);
#endif

  //char* SimplifyPath(char* url);

  void Prepare(void);
  void PrepareBase(void);
  void PrepareFileBase(void);
  void PrepareURI(void);
};

//----------------------------------------------------------

class APOLLO_API UriBuilder
{
public:
  void setPath(const String& sPath) { sPath_ = sPath; }
  void setFile(const String& sFile) { sFile_ = sFile; }
  void setQuery(const String& sQuery);
  void setQueryParam(const String& sKey, const String& sValue);

  String getUri();
  String operator()() { return getUri(); }

  String getQuery();

  String sPath_;
  String sFile_;
  String sArgs_;
  List lQuery_;
};

//----------------------------------------------------------

class APOLLO_API UrlBuilder: public UriBuilder
{
public:
  UrlBuilder() {}
  UrlBuilder(const String& sUrl)
    :sBase_(sUrl)
  {}

  void setBase(const String& sBase) { sBase_ = sBase; }
  void setProtocol(const String& sProtocol) { sProtocol_ = sProtocol; }
  void setHost(const String& sHost) { sHost_ = sHost; }
  void setPort(int nPort);

  String getUrl();
  String operator()() { return getUrl(); }

  String sBase_;
  String sProtocol_;
  String sHost_;
  String sPort_;
};

AP_NAMESPACE_END

#endif // !defined(URL_h_INCLUDED)
