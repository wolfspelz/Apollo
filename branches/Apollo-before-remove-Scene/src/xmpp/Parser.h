// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Parser_H_INCLUDED)
#define Parser_H_INCLUDED

#include "XMLProcessor.h"

class Client;

class Parser : public Apollo::XMLProcessor
{
public:
  Parser(Client* pClient);
  virtual ~Parser();

  int OnXMLDocBegin(Apollo::XMLNode* _node);
  int OnXMLNodeTree(Apollo::XMLNode* _node);
  int OnXMLDocEnd(const char* _name);

protected:
  Client* pClient_;
};

#endif // !defined(Parser_H_INCLUDED)
