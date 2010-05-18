// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(XMLProcessor_h_INCLUDED)
#define XMLProcessor_h_INCLUDED

//#if defined(_UNICODE)
//  #define XML_UNICODE_WCHAR_T
//#endif

#define XML_STATIC
#include "expat/expat.h"

#include "SSystem.h"
#include "ApExports.h"

AP_NAMESPACE_BEGIN

class APOLLO_API XMLAttr : public Elem
{
public:
  XMLAttr(void) {};
  XMLAttr(const char* key, const char* value);
  String& getKey() { return getName(); };
  String& getValue() { return getString(); };

  operator const char*() { return getValue(); };
  operator int() { return getValue().empty() ? 0 : String::atoi(getValue().c_str()); };

public:
  // these are for if statements
  bool isValid(void);
  operator bool (void) { return isValid(); };
  bool operator!(void) { return ! isValid(); };
  bool operator&&(XMLAttr& n) { return isValid() && n.isValid(); };
  bool operator||(XMLAttr& n) { return isValid() || n.isValid(); };
private:
  // disallow external operator long to be used accidently by boolean expressions
  operator long(void);

public:
  static XMLAttr empty_attr_;
};

typedef List XMLChildList;
typedef List XMLPathList;

class APOLLO_API XMLAttrList : public List
{
  typedef XMLChildList super;

public:
  XMLAttr* nextAttribute(XMLAttr* attr) { return (XMLAttr*) super::Next(attr); }
};

class APOLLO_API XMLNode : private XMLChildList
{
  typedef XMLChildList super;

public:
  XMLNode(void) {};
  XMLNode(const char* name);
  ~XMLNode(void);

  // Get
  void outerXml(String& _s, int _escape_cdata = 1);
  String outerXml(int _escape_cdata = 1);
  void innerXml(String& _s, int _escape_cdata = 1);
  String innerXml(int _escape_cdata = 1);

  void setName(char* name) { super::setName(name); };
  String& getName() { return super::getName(); };

  void setText(char* name) { super::setString(name); };
  String& getText() { return super::getString(); };

  XMLNode* nextChild(XMLNode* _node);
  XMLNode* nextChild(XMLNode* _node, const char* name);
  XMLNode* firstChild(void) { return (XMLNode*) super::Next(0); };
  XMLChildList& getChildren() { return *this; };
  XMLNode& getChildRef(const char* name);
  XMLNode* getChild(const char* name);
  XMLNode& getChildRefByParams(const char* name, const char* key, const char* value);
  XMLNode* getChildByPath(const char* path);
  XMLNode* addChild(const char* name);
  XMLNode& addChildRef(const char* name);
  void removeChild(XMLNode* child);
  void deleteChild(XMLNode* child);

  XMLAttrList& getAttributes() { return attributes_; };
  XMLAttr& getAttribute(const char* key);
  XMLAttr *addAttribute(const char* key, const char* value);
  void removeAttribute(XMLAttr *attr);
  void deleteAttribute(XMLAttr *attr);

  String getCData();
  int setCData(const char* data);
  int appendCData(const char* data);
  int appendCData(const char* data, int len);

  void setDelete(int flag) { delete_ = flag; };
  int getDelete(void) { return delete_; };

  void setDepth(int depth) { depth_ = depth; };
  int getDepth(void) { return depth_; };

public:
  // these are for if statements
  bool isValid(void);
  operator bool (void) { return isValid(); };
  bool operator!(void) { return ! isValid(); };
  bool operator&&(XMLNode& n) { return isValid() && n.isValid(); };
  bool operator||(XMLNode& n) { return isValid() || n.isValid(); };
private:
  // disallow external operator long to be used accidently by boolean expressions
  operator long(void);

protected:
  XMLAttrList attributes_;
  int depth_;
  int delete_;

  static XMLNode empty_child_;
};

// ============================================================================

class APOLLO_API XMLProcessor
{
public:
  XMLProcessor();
  virtual ~XMLProcessor();

  virtual int Init(void);
  virtual int Exit(void);

  int XmlText(const String& _xml, int _final = 1);
  int XmlText(const char* _xml_utf8, size_t _len, int _final);
  XMLNode* Root(void) { return root_node_; };
  XMLNode* DetachRoot() { XMLNode* root = root_node_; root_node_ = 0; return root; };

  int Parse(const char *_data, size_t _len, int _final);
  String GetErrorString(void);
  void SetErrorString(const char* _error) { app_error_ = _error; };
  int GetErrorCode(void);

protected:
  virtual int OnXMLDocBegin(XMLNode* _node);
  virtual int OnXMLNodeTree(XMLNode* _node);
  virtual int OnXMLDocEnd(const char* _name);

private:
  virtual void Free(void);

public: 
//private: 
  // should be private, but public for call from global function
  // objectified callbacks
  void startElement(const XML_Char* name, const XML_Char** atts);
  void endElement(const XML_Char* name);
  void characterData(const XML_Char* s, int len);

protected:
  String encoding_;

private:
  XML_Parser expat_;

  String error_;
  String app_error_;

public:
  long depth_;
  XMLNode* root_node_;
  XMLPathList path_;
};

AP_NAMESPACE_END

#endif // !defined(XMLProcessor_h_INCLUDED)

