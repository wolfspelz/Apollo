// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Stanza_h_INCLUDED)
#define Stanza_h_INCLUDED

#include "XMLProcessor.h"

class Stanza : public Apollo::XMLNode
{
public:
  Stanza(const char* szName);

  int getError(int& nError, String& sError);
};

class IQStanza : public Stanza
{
public:
  IQStanza();
  Apollo::XMLNode& addQuery(const char* szNameSpace);
};

class GetStanza : public IQStanza
{
public:
  GetStanza(const char* szId, const char* szTo);
};

class SetStanza : public IQStanza
{
public:
  SetStanza(const char* szId, const char* szTo);
};

class ResultStanza : public IQStanza
{
public:
  ResultStanza(const char* szId, const char* szTo);
};

class PresenceStanza: public Stanza
{
public:
  PresenceStanza(const char* szType, const char* szTo);
};

class MessageStanza: public Stanza
{
public:
  MessageStanza(const char* szType, const char* szTo);
  Apollo::XMLNode& addText(const char* szText);
};

class GroupchatMessageStanza: public MessageStanza
{
public:
  GroupchatMessageStanza(const char* szTo);
};

#endif // !defined(Stanza_h_INCLUDED)
