// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Parser.h"
#include "Client.h"

Parser::Parser(Client* pClient)
:pClient_(pClient)
{
}

Parser::~Parser()
{
}

int Parser::OnXMLDocBegin(Apollo::XMLNode* _node)
{
  pClient_->onParserBegin(_node);

  return 1;
}

int Parser::OnXMLNodeTree(Apollo::XMLNode* _node)
{
  if (_node->getDepth() == 1) {
    pClient_->onParserStanza(_node);
    _node->setDelete(1);
  }

  return 1;
}

int Parser::OnXMLDocEnd(const char* _name)
{
  pClient_->onParserEnd(_name);
  return 1;
}
