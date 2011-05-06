// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApCompiler.h"
#include "XMLProcessor.h"

AP_NAMESPACE_BEGIN

static void Expat_startElement(void* userData, const XML_Char* name, const XML_Char** atts)
{
  XMLProcessor* self = (XMLProcessor*) userData;
  self->startElement(name, atts);
}

static void Expat_endElement(void* userData, const XML_Char* name)
{
  XMLProcessor* self = (XMLProcessor*) userData;
  self->endElement(name);
}

static void Expat_characterData(void* userData, const XML_Char* s, int len)
{
  XMLProcessor* self = (XMLProcessor*) userData;
  self->characterData(s, len);
} 

static void Expat_defaultHandler(void* userData, const XML_Char* s, int len)
{
  AP_UNUSED_ARG(userData);
  AP_UNUSED_ARG(s);
  AP_UNUSED_ARG(len);
  //XMLProcessor* self = (XMLProcessor*) userData;
}

// ============================================================================

XMLProcessor::XMLProcessor()
:expat_(0)
,depth_(0)
,root_node_(0)
{
  //encoding_ = "utf-8";
}

XMLProcessor::~XMLProcessor()
{
  (void) Free();
}

int XMLProcessor::Init()
{
  int ok = 1;

  expat_ = XML_ParserCreate(encoding_.empty() ? 0 : encoding_.c_str());
  if (expat_ == 0) {
    error_ = "XMLProcessor::XMLProcessor, XML_ParserCreate failed";
    ok = 0;
  } else {
    XML_SetUserData(expat_, this);
    XML_SetElementHandler(expat_, Expat_startElement, Expat_endElement);
    XML_SetCharacterDataHandler(expat_, Expat_characterData);
    XML_SetDefaultHandler(expat_, Expat_defaultHandler);

    depth_ = 0;
  }

  return ok;
}

void XMLProcessor::Free(void)
{
  if (root_node_ != 0) {
    delete root_node_;
    root_node_ = 0;
  }

  if (expat_ != 0) {
    XML_ParserFree(expat_);
    expat_ = 0;
  }
}

int XMLProcessor::Exit(void)
{
  int ok = 1;

  (void) Free();

  return ok;
}

// ============================================================================

int XMLProcessor::XmlText(const String& _xml, int _final)
{
  int ok = 1;

  ok = Init();
  if (ok) {
    ok = Parse(_xml, _xml.bytes(), _final);
  }

  return ok;
}

int XMLProcessor::XmlText(const char* _xml_utf8, size_t _len, int _final)
{
  int ok = 1;

  ok = Init();
  if (ok) {
    ok = Parse(_xml_utf8, _len, _final);
  }

  return ok;
}

int XMLProcessor::Parse(const char* _data, size_t _len, int _final)
{
  int ok = 0;

  if (expat_ != 0) {
    app_error_ = "";
    ok = XML_Parse(expat_, _data, (int) _len, _final);
    if (! app_error_.empty()) {
      ok = 0;
    }
  }

  return ok;
}

int XMLProcessor::GetErrorCode(void)
{
  return XML_GetErrorCode(expat_);
}

String XMLProcessor::GetErrorString(void)
{
  String s;

  if (XML_GetErrorCode(expat_) != XML_ERROR_NONE) {
    String xml_error; 
    xml_error = XML_ErrorString(XML_GetErrorCode(expat_));
    
    s.appendf(
      "%s in line %d col %d"
      ,(const char*) xml_error
      ,XML_GetCurrentLineNumber(expat_)
      ,XML_GetCurrentColumnNumber(expat_)
      );
  }

  if (! app_error_.empty()) {
    s += " ";
    s += app_error_;
  }

  if (! error_.empty()) {
    s += " ";
    s += error_;
  }

  return s;
}

// ============================================================================

void XMLProcessor::startElement(const XML_Char* name, const XML_Char** atts)
{
  XMLNode* node = 0;

  if (root_node_ == 0) {
    root_node_ = new XMLNode(name);
    if (root_node_ != 0) {
      node = root_node_;
    } else {
      error_ = "XMLProcessor::startElement, new root_node_ failed";
    }

  } else {

    XMLNode* current_node = (XMLNode*) path_[depth_].getPtr();
    if (current_node != 0) {
      node = current_node->addChild(name);
      if (node == 0) {
        error_.appendf("XMLProcessor::startElement, current_node.addChild(%s) failed", StringType(name));
      }
    }
  }

  if (node != 0) {
    int done = 0;
    int i = 0;
    while (!done) {
      const char* attr_key = atts[i++];
      const char* attr_value = 0;
      if (attr_key == 0) {
        done = 1;
      } else {
        attr_value = atts[i++];
        if (attr_value == 0) {
          done = 1;
        }
      }

      if (attr_key != 0 && attr_value != 0) {
        XMLAttr* attribute = node->addAttribute(attr_key, attr_value);
        if (attribute == 0) {
          error_.appendf("XMLProcessor::startElement, current_node.addAttribute(%s=%s) failed", StringType(attr_key), StringType(attr_value));
        }
      }
    }

    if (depth_ <= 0) {
      int ok = OnXMLDocBegin(node);
      if (!ok) {
        error_.appendf("XMLProcessor::startElement, OnXMLDocBegin failed");
      }
    }

    depth_+= 1;
    path_[depth_].setPtr(node);
  }

}

void XMLProcessor::endElement(const XML_Char* name)
{
  XMLNode* current_node = (XMLNode*) path_[depth_].getPtr();

  // deliver node to derived class
  int ok = OnXMLNodeTree(current_node);
  if (!ok) {
    app_error_.appendf("XMLProcessor::endElement, OnXMLNodeTree failed");
  }

  if (current_node->getDelete()) {
    XMLNode* parent_node = (XMLNode*) path_[depth_ - 1].getPtr();
    parent_node->deleteChild(current_node);
    current_node = 0;
  }

  depth_ -= 1;

  if (depth_ <= 0) {
    ok = OnXMLDocEnd(name);
    if (!ok) {
      error_.appendf("XMLProcessor::endElement, OnXMLDocEnd failed");
    }
  }
}

void XMLProcessor::characterData(const XML_Char* s, int len)
{
  XMLNode* current_node = (XMLNode*) path_[depth_].getPtr();
  if (current_node != 0) {

    current_node->appendCData(s, len);
  }
}

// ============================================================================

int XMLProcessor::OnXMLDocBegin(XMLNode* _node)
{
  AP_UNUSED_ARG(_node);
  return 1;
}

int XMLProcessor::OnXMLNodeTree(XMLNode* _node)
{
  AP_UNUSED_ARG(_node);
  return 1;
}

int XMLProcessor::OnXMLDocEnd(const char* _name)
{
  AP_UNUSED_ARG(_name);
  return 1;
}

// ============================================================================

XMLAttr::XMLAttr(const char* key, const char* value)
:Elem(key, value)
{
}

bool XMLAttr::isValid(void)
{
  return this != &empty_attr_;
}

// ============================================================================

XMLNode::XMLNode(const char* name)
:XMLChildList(name)
,depth_(0)
,delete_(0)
{
}

XMLNode::~XMLNode(void)
{
}

XMLNode* XMLNode::addChild(const char* name)
{
  XMLNode* child = new XMLNode(name);
  if (child != 0) {
    child->setDepth(depth_ + 1);
    AddLast(child);
  }
  return child;
}

XMLNode& XMLNode::addChildRef(const char* name)
{
  XMLNode* child = addChild(name);
  if (child != 0) {
    return *child;
  } else {
    return empty_child_;
  }
}

XMLAttr* XMLNode::addAttribute(const char* key, const char* value)
{
  XMLAttr* attr = new XMLAttr(key, value);
  attributes_.AddLast(attr);
  return attr;
}

int XMLNode::appendCData(const char* data)
{
  appendCData(data, String::strlen(data));
  //cdata_ += data;
  return 1;
}

int XMLNode::appendCData(const char* data, int len)
{
  XMLNode* child = new XMLNode();
  if (child != 0) {
    child->getText().set(data, len);
    child->setDepth(depth_ + 1);
    AddLast(child);
  }
  //cdata_.Append(data, data + len);
  return 1;
}

String XMLNode::getCData()
{ 
  //return cdata_;
  String cdata;
  for (XMLNode* child = 0; (child = nextChild(child)) != 0; ) {
    if (child->getName().empty()) {
      cdata += child->getString();
    }
  }
  return cdata;
}

int XMLNode::setCData(const char* data)
{
  int bFoundOne = 0;
  do {
    XMLNode* textchild = (XMLNode*) FindByName("");
    if (textchild != 0) {
      bFoundOne = 1;
      Remove(textchild);
      delete textchild;
      textchild = 0;
    }
    
  } while (bFoundOne);
  return appendCData(data);
}

void XMLNode::removeChild(XMLNode* child)
{
  Remove(child);
}

void XMLNode::removeAttribute(XMLAttr* attr)
{
  attributes_.Remove(attr);
}

void XMLNode::deleteChild(XMLNode* child)
{
  Remove(child);
  delete child;
}

void XMLNode::deleteAttribute(XMLAttr* attr)
{
  attributes_.Remove(attr);
  delete attr;
}

XMLAttr XMLAttr::empty_attr_("", "");
XMLNode XMLNode::empty_child_("");

XMLAttr& XMLNode::getAttribute(const char* key)
{
  XMLAttr* attr = (XMLAttr*) getAttributes().FindByName(key);
  if (attr != 0) {
    return *attr;
  } else {
    return XMLAttr::empty_attr_;
  }
}

XMLNode* XMLNode::nextChild(XMLNode* _node)
{
  return (XMLNode*) super::Next(_node);
}

XMLNode* XMLNode::nextChild(XMLNode* _node, const char* name)
{
  XMLNode* result = 0;
  XMLNode* child = _node;
  while ((child = (XMLNode*) super::Next(child)) != 0) {
    if (child->getName() == name) {
      result = child;
      break;
    }
  }
  return result;
}

XMLNode& XMLNode::getChildRefByParams(const char* name, const char* key, const char* value)
{
  XMLNode* result = 0;
  XMLNode* child = 0;
  while ((child = (XMLNode*) Next(child)) && ! result) {
    if (name == 0 || *name == '\0' || child->getName() == name) {
      XMLAttr& attr = child->getAttribute(key);

      if (value == 0 || *value == '\0') {
        result = child;
      } else {
        if (! attr.getValue().empty()) {
          if (attr.getValue() == value) {
            result = child;
          }
        }
      } // check attr value
    } // check name

  }

  if (result != 0) {
    return *result;
  } else {
    return empty_child_;
  }
}

XMLNode& XMLNode::getChildRef(const char* name)
{
  XMLNode* child = (XMLNode*) getChildren().FindByNameCase(name);
  if (child != 0) {
    return *child;
  } else {
    return empty_child_;
  }
}

XMLNode* XMLNode::getChild(const char* name)
{
  return (XMLNode*) getChildren().FindByNameCase(name);
}

XMLNode* XMLNode::getChildByPath(const char* path)
{
  String sPath = path;
  String sPart;
  XMLNode* node = this;
  while (sPath.nextToken("/", sPart) && node != 0) {
    node = node->getChild(sPart);
  }

  return node;
}

bool XMLNode::isValid(void)
{
  return this != &empty_child_;
}

static void AppendNodeAsText(String &_s, XMLNode* _node, int _depth, int _escape_cdata, int _include_outer_tag)
{
  int depth = _depth;
  depth++;

  int is_text = (_node->getName().empty());

  if (is_text) {

    if (_escape_cdata) {
      String text = _node->getText();
      text.escape(String::EscapeXML);
      _s += text;
    } else {
      _s += _node->getText();
    }
  
  } else {
  
    XMLAttrList& al = _node->getAttributes();
    XMLChildList& cl = _node->getChildren();

    if (_include_outer_tag) {
      _s.appendf("<%s", StringType(_node->getName()));

      XMLAttr* a = 0;
      while ((a = (XMLAttr*) al.Next(a)) != 0) {
        String name = a->getName();
        String value = a->getValue();
        if (_escape_cdata) {
          name.escape(String::EscapeXML);
          value.escape(String::EscapeXML);
        }
        _s.appendf(" %s='%s'", StringType(name), StringType(value));
      }
    
      if (cl.NumElem() > 0) {
        _s += ">";
      } else {
        _s += " />";
      }
    }

    XMLNode* c = 0;
    while ((c = _node->nextChild(c)) != 0) {
      AppendNodeAsText(_s, c, depth, _escape_cdata, 1);
    }
  
    if (_include_outer_tag) {
      if (cl.NumElem() > 0) {
        _s.appendf("</%s>", StringType(_node->getName()));
      }
    }

  }
}

void XMLNode::outerXml(String& _s, int _escape_cdata)
{
  int include_outer_tag = 1;
  AppendNodeAsText(_s, this, 0, _escape_cdata, include_outer_tag);
}

String XMLNode::outerXml(int _escape_cdata)
{
  String s;
  outerXml(s, _escape_cdata);
  return s;
}

void XMLNode::innerXml(String& _s, int _escape_cdata)
{
  int include_outer_tag = 0;
  AppendNodeAsText(_s, this, 0, _escape_cdata, include_outer_tag);
}

String XMLNode::innerXml(int _escape_cdata)
{
  String s;
  innerXml(s, _escape_cdata);
  return s;
}

AP_NAMESPACE_END
