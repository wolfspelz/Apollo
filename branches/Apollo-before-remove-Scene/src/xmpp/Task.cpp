// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ApLog.h"
#include "XMLProcessor.h"
#include "MessageDigest.h"
#include "Protocol.h"
#include "Client.h"
#include "Stanza.h"
#include "Task.h"
#include "Room.h"
#include "Buddy.h"

int Task::handleStanza(Stanza& stanza, StanzaHandlerResult& result)
{
  AP_UNUSED_ARG(stanza);
  AP_UNUSED_ARG(result);
  
  int ok = 1;
  return ok;
}

// -------------------------------------------------------------------

int LoginTask::handleStanza(Stanza& stanza, StanzaHandlerResult& result)
{
  int ok = 1;

  switch(nPhase_) {
  case StreamOpen:
    // <stream:stream xmlns:stream='http://etherx.jabber.org/streams' id='43956666' xmlns='jabber:client' from='user.virtual-presence.org'>
    if (stanza.getName() == "stream:stream") {
      result.stanzaHandled(1);
      result.stanzaConsumed(1);
      sStreamId_ = stanza.getAttribute("id").getValue();
      if (sStreamId_.empty()) {
        apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "<stream:stream> No stream ID"));
        nPhase_ = LoginFailed;
        result.connectionFinished(1);
        result.taskFinished(1);
        pClient_->onProtocolStartFailed();
      } else {
        sendAuthMechQuery();
        nPhase_ = AuthMechanisms;
        pClient_->onProtocolStart();
      }
    }
    break;

  case AuthMechanisms:
    if (stanza.getName() == "iq") {
      // <iq id='1' type='result'><query xmlns='jabber:iq:auth'><username>lluna_484de02a54c5</username><digest/><password/><resource/></query></iq>      
      if (nId_ == String::atoi(stanza.getAttribute("id").getValue())) {
        result.stanzaHandled(1);
        result.stanzaConsumed(1);
        String sType = stanza.getAttribute("type").getValue();
        if (sType == "error") {
          String sError; int nError = 0;
          if (stanza.getError(nError, sError)) {
            apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "jabber:iq:auth error result: %d %s", nError, StringType(sError)));
          } else {
            apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "jabber:iq:auth unknown error result"));
          }
          nPhase_ = LoginFailed;
          result.connectionFinished(1);
          pClient_->onProtocolLoginFailed();
        } else {
          Apollo::XMLNode& query = stanza.getChildRef("query");
          Apollo::XMLNode& digest = query.getChildRef("digest");
          Apollo::XMLNode& password = query.getChildRef("password");
          AP_UNUSED_VARIABLE Apollo::XMLNode& resource = query.getChildRef("resource");
          if (digest) {
            sendDigestAuth();
            nPhase_ = DigestAuth;
          } else if (password) {
            sendPasswordAuth();
            nPhase_ = PasswordAuth;
          } else {
            apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "jabber:iq:auth result: neither digest nor password auth available"));
            nPhase_ = LoginFailed;
            result.connectionFinished(1);
            pClient_->onProtocolLoginFailed();
          }
        }
      }
    }
    break;

  case DigestAuth:
    if (stanza.getName() == "iq") {
      // <iq id='2' type='result'/>
      if (nId_ == String::atoi(stanza.getAttribute("id").getValue())) {
        result.stanzaHandled(1);
        result.stanzaConsumed(1);
        result.taskFinished(1);
        String sType = stanza.getAttribute("type").getValue();
        if (sType == "error") {
          String sError; int nError = 0;
          if (stanza.getError(nError, sError)) {
            apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "DigestAuth error result: %d %s", nError, StringType(sError)));
          } else {
            apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "DigestAuth unknown error result"));
          }
          nPhase_ = LoginFailed;
          result.connectionFinished(1);
          pClient_->onProtocolLoginFailed();
        } else {
          apLog_Info((LOG_CHANNEL, "LoginTask::handleStanza", "Logged in"));
          nPhase_ = LoggedIn;
          pClient_->onProtocolLogin();
        }
      }
    }
    break;

  case PasswordAuth:
    if (stanza.getName() == "iq") {
      // <iq id='2' type='result'/>
      if (nId_ == String::atoi(stanza.getAttribute("id").getValue())) {
        result.stanzaHandled(1);
        result.stanzaConsumed(1);
        result.taskFinished(1);
        String sType = stanza.getAttribute("type").getValue();
        if (sType == "error") {
          String sError; int nError = 0;
          if (stanza.getError(nError, sError)) {
            apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "PasswordAuth error result: %d %s", nError, StringType(sError)));
          } else {
            apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "PasswordAuth unknown error result"));
          }
          nPhase_ = LoginFailed;
          result.connectionFinished(1);
          pClient_->onProtocolLoginFailed();
        } else {
          apLog_Info((LOG_CHANNEL, "LoginTask::handleStanza", "Logged in"));
          nPhase_ = LoggedIn;
          pClient_->onProtocolLogin();
        }
      }
    }
    break;

  case LoginFailed:
    apLog_Warning((LOG_CHANNEL, "LoginTask::handleStanza", "Ignoring stanza after LoginFailed"));
    result.connectionFinished(1);
    break;

  default:
    apLog_Error((LOG_CHANNEL, "LoginTask::handleStanza", "Unknown phase"));
    result.connectionFinished(1);
  }

  return ok;
}

int LoginTask::sendAuthMechQuery()
{
  // #login possibilities:
  // <iq id='1' to='jabber.org' type='get'>
  //   <query xmlns='jabber:iq:auth'>
  //     <username>wolfspelz</username>
  //   </query>
  // </iq>
  int ok = 1;

  nId_ = pClient_->getNextStanzaId();
  GetStanza request(nId_, pClient_->getJabberId().host());
  Apollo::XMLNode& query = request.addQuery(JABBER_NS_AUTH);

  Apollo::XMLNode& username = query.addChildRef("username");
  username.setCData(pClient_->getJabberId().user());

  ok = pClient_->sendStanza(request);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "LoginTask::sendAuthMechQuery", "pClient_->sendStanza() failed <%s ...", StringType(request.getName())));
  }

  return ok;
}

int LoginTask::sendDigestAuth()
{
  // #login: stream-id(43956666) . password(a) = '43956666a' -> sha1('43956666a') = 'f944d75c9cd15c106b69f5795b0171fe86b7c182'
  // <iq id='2' type='set'>
  //   <query xmlns='jabber:iq:auth'>
  //     <username>wolfspelz</username>
  //     <digest>f944d75c9cd15c106b69f5795b0171fe86b7c182</digest>
  //     <resource>atHome</resource>
  //   </query>
  // </iq>
  int ok = 1;

  String sToken = sStreamId_; 
  sToken += pClient_->getPassword();
  Apollo::MessageDigest sha1((unsigned char*) sToken.c_str(), sToken.bytes());
  sToken = sha1.getSHA1Hex();

  nId_ = pClient_->getNextStanzaId();
  SetStanza request(nId_, pClient_->getJabberId().host());
  Apollo::XMLNode& query = request.addQuery(JABBER_NS_AUTH);

  Apollo::XMLNode& username = query.addChildRef("username");
  username.setCData(pClient_->getJabberId().user());

  Apollo::XMLNode& digest = query.addChildRef("digest");
  digest.setCData(sToken);

  Apollo::XMLNode& resource = query.addChildRef("resource");
  resource.setCData(pClient_->getResource());

  ok = pClient_->sendStanza(request);
  if (!ok) {
    apLog_Error((LOG_CHANNEL, "LoginTask::sendDigestAuth", "pClient_->sendStanza() failed <%s ...", StringType(request.getName())));
  }

  return ok;
}

int LoginTask::sendPasswordAuth()
{
  // <iq id='2' type='set'>
  //   <query xmlns='jabber:iq:auth'>
  //     <username>wolfspelz</username>
  //     <password>gaga123</password>
  //     <resource>atHome</resource>
  //   </query>
  // </iq>
  int ok = 1;

  nId_ = pClient_->getNextStanzaId();
  SetStanza request(nId_, pClient_->getJabberId().host());
  Apollo::XMLNode& query = request.addQuery(JABBER_NS_AUTH);

  Apollo::XMLNode& username = query.addChildRef("username");
  username.setCData(pClient_->getJabberId().user());
  Apollo::XMLNode& oPassword = query.addChildRef("password");
  oPassword.setCData(pClient_->getPassword());
  Apollo::XMLNode& resource = query.addChildRef("resource");
  resource.setCData(pClient_->getResource());

  ok = pClient_->sendStanza(request);

  return ok;
}

// -------------------------------------------------------------------

int PresenceTask::handleStanza(Stanza& stanza, StanzaHandlerResult& result)
{
  // Buddy presence
  // <presence/>
  int ok = 1;
  int bUsed = 0;

  if (stanza.getName() == "presence") {
    JabberId from = stanza.getAttribute("from").getValue();

    apLog_Verbose((LOG_CHANNEL, "PresenceTask::handleStanza", "<presence from=%s ...", StringType(from)));

    Room* pRoom = pClient_->findRoom(from.base());
    Buddy* pBuddy = pClient_->findBuddy(from.base());

    String sType = stanza.getAttribute("type").getValue();
    if (sType.empty()) {
      sType = JABBER_PRESENCE_AVAILABLE;
    }

    if (0) {
    } else if (sType == JABBER_PRESENCE_AVAILABLE) {

      if (0) {
      } else if (pRoom != 0){
        ok = pRoom->presenceAvailable(stanza);
        bUsed = 1;
      } else if (pBuddy != 0) {
        ok = pBuddy->presenceAvailable(stanza);
        bUsed = 1;
      } else if (from.base() == pClient_->getJabberId()) {
        ok = pClient_->selfPresenceAvailable(stanza);
        bUsed = 1;
      }

    } else if (sType == JABBER_PRESENCE_UNAVAILABLE) {

      if (0) {
      } else if (pRoom != 0){
        ok = pRoom->presenceUnavailable(stanza);
        bUsed = 1;
      } else if (pBuddy != 0) {
        ok = pBuddy->presenceUnavailable(stanza);
        bUsed = 1;
      } else if (from.base() == pClient_->getJabberId()) {
        ok = pClient_->selfPresenceUnavailable(stanza);
        bUsed = 1;
      }

    } else if (sType == JABBER_PRESENCE_ERROR) {

      if (0) {
      } else if (pRoom != 0){
        ok = pRoom->presenceError(stanza);
        bUsed = 1;
      } else if (pBuddy != 0) {
        ok = pBuddy->presenceError(stanza);
        bUsed = 1;
      } else if (from.base() == pClient_->getJabberId()) {
        ok = pClient_->selfPresenceError(stanza);
        bUsed = 1;
      }

    } else {
      // not handled
    }
  }

  if (bUsed) {
    result.stanzaHandled(1);
    result.stanzaConsumed(1);
  }

  return ok;
}

// -------------------------------------------------------------------
//  <iq 
//    from='341973964fa3faf9c1d6e8c9255bb3fd84fcb005@muc1.virtual-presence.org' 
//    to='wolfspelz@jabber.org/ApolloTest' 
//    id='muc_ping_174059' 
//    type='get'
//    >
//    <query xmlns='jabber:iq:version'/>
//    <x xmlns='halvar:ping' pingcount='3' stanzatime='2008-07-20T09:23:01.008488Z' idlesince='2008-07-20T09:14:10.509189Z'/>
//  </iq>
int VersionTask::handleStanza(Stanza& stanza, StanzaHandlerResult& result)
{
  int ok = 1;
  int AP_UNUSED_VARIABLE bUsed = 0;

  if (stanza.getName() == "iq") {
    Apollo::XMLNode& query = stanza.getChildRef("query");
    if (query) {
      if (query.getAttribute("xmlns").getValue() == JABBER_NS_VERSION) {
        result.stanzaHandled(1);
        result.stanzaConsumed(1);
        ok = sendResponse(stanza);
        if (!ok) {
          apLog_Error((LOG_CHANNEL, "VersionTask::handleStanza", "sendResponse() failed"));
        }
      }
    }
  }

  return ok;
}

int VersionTask::sendResponse(Stanza& stanza)
{
  //  <iq type='result' id='jcl_9' to='Susi@camouflage.cyland.com/exodus' from='Blub@camouflage.cyland.com/Winjab'>
  //    <query xmlns='jabber:iq:version'>
  //      <name>Winjab</name>
  //      <version>1.1.0.1</version>
  //      <os>NT 5.0</os>
  //    </query>
  //  </iq>
  int ok = 1;

  String sId = stanza.getAttribute("id").getValue();
  String sFrom = stanza.getAttribute("from").getValue();
  if (sFrom.empty()) {
    ok = 0;
    apLog_Warning((LOG_CHANNEL, "VersionTask::sendResponse", "missing from"));
  } else {
    ResultStanza result(sId, sFrom);
    Apollo::XMLNode& AP_UNUSED_VARIABLE query = result.addQuery(JABBER_NS_VERSION);

    String sName = Apollo::getModuleConfig(MODULE_NAME, "ClientInfo/Name", "Apollo");
    String sVersion = Apollo::getModuleConfig(MODULE_NAME, "ClientInfo/Version", "0.1");
    String sOs = Apollo::getModuleConfig(MODULE_NAME, "ClientInfo/OS", "Unknown");

    Apollo::XMLNode& name = result.addChildRef("name");
    if (name) {
      name.setCData(sName);
    }
    Apollo::XMLNode& version = result.addChildRef("version");
    if (version) {
      version.setCData(sVersion);
    }
    Apollo::XMLNode& os = result.addChildRef("os");
    if (os) {
      os.setCData(sOs);
    }

    ok = pClient_->sendStanza(result);
    if (!ok) {
      apLog_Error((LOG_CHANNEL, "VersionTask::sendResponse", "pClient_->sendStanza() failed to=%s", StringType(sFrom)));
    }
  }

  return ok;
}

// -------------------------------------------------------------------

int GroupchatTask::handleStanza(Stanza& stanza, StanzaHandlerResult& result)
{
  // Buddy presence
  // <presence/>
  int ok = 1;
  int bUsed = 0;

  String sType = stanza.getAttribute("type").getValue();
  if (stanza.getName() == "message" && sType == "groupchat") {
    JabberId from = stanza.getAttribute("from").getValue();

    apLog_Verbose((LOG_CHANNEL, "GroupchatTask::handleStanza", "<message from=%s ...", StringType(from)));

    Room* pRoom = pClient_->findRoom(from.base());

    if (pRoom) {
      ok = pRoom->receiveGroupchat(stanza);
      bUsed = 1;
    }
  }

  if (bUsed) {
    result.stanzaHandled(1);
    result.stanzaConsumed(1);
  }

  return ok;
}

// -------------------------------------------------------------------

int StreamErrorTask::handleStanza(Stanza& stanza, StanzaHandlerResult& result)
{
  // Stream error from server. Usually before stream closes
  // <stream:error>
  //   <conflict xmlns='urn:ietf:params:xml:ns:xmpp-streams'/>
  //   <text xml:lang='' xmlns='urn:ietf:params:xml:ns:xmpp-streams'>Replaced by new connection</text>
  // </stream:error>
  int ok = 1;

  if (stanza.getName() == "stream:error") {
    String sError;
    Apollo::XMLNode& text = stanza.getChildRef("text");
    if (text) {
      sError = text.getCData();
    }

    apLog_Info((LOG_CHANNEL, "StreamErrorTask::handleStanza", "%s", StringType(sError)));

    result.stanzaHandled(1);
    result.stanzaConsumed(1);
    result.connectionFinished(1);
  }

  return ok;
}
