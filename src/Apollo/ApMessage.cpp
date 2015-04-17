// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "ApLib.h"
//#include "ApMessage.h"

#define LOG_CHANNEL APOLLO_NAME
#define LOG_CONTEXT apLog_Context

void ApNotificationMessage::Send()
{
  int AP_UNUSED_VARIABLE bCalled = Apollo::callMsg(this, 0);
}

int ApRequestMessage::Request()
{
  Once(true); // Handle requests only once

  int bCalled = Apollo::callMsg(this, 0);

  if (!bCalled) {
  #if defined(_DEBUG)
    AP_DEBUG_BREAK();
  #endif
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "No handler for message type=%s", _sz(Type())));
  }

  if (apStatus == Unknown) {
  #if defined(_DEBUG) 
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "No status result for message type=%s", _sz(Type())));
    //AP_DEBUG_BREAK();
  #endif
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "No status result for message type=%s", _sz(Type())));
  }

  return apStatus == Ok;
}

void ApFilterMessage::Filter()
{
  int AP_UNUSED_VARIABLE bCalled = Apollo::callMsg(this, 0);
}

int ApSRPCMessage::Call()
{
  Once(true); // Until someone handles it
  int bCalled = Apollo::callMsg(this, 0);
  return apStatus == ApMessage::Ok;
}

//----------------------

int ApMessage::Hook(const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition)
{
  return Apollo::hookMsg(Type(), szModuleName, fnHandler, nRef, nPosition);
}

int ApMessage::Unhook(const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef)
{
  return Apollo::unhookMsg(Type(), szModuleName, fnHandler, nRef);
}

int ApMessage::PostAsync()
{
  int bCalled = Apollo::callMsg(this, Apollo::CF_ASYNC);
  if (!bCalled) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Async Message (type=%s) not handled, deleting self", _sz(Type())));
    delete this;
  }
  return bCalled;
}

//----------------------------------------------------------

ApMsgHandlerRegistrant::ApMsgHandlerRegistrant(const char* szModuleName, const char* szMsgType, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition)
:Elem(szMsgType)
,sModuleName_(szModuleName)
,fnHandler_(fnHandler)
,nRef_(nRef)
{
  Apollo::hookMsg(getName(), szModuleName, fnHandler, nRef, nPosition);
}

ApMsgHandlerRegistrant::~ApMsgHandlerRegistrant()
{
  Apollo::unhookMsg(getName(), sModuleName_, fnHandler_, nRef_);
}

void ApMsgHandlerRegistrantList::add(const char* szModuleName, const char* szMsgType, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition)
{
  ApMsgHandlerRegistrant* p = new ApMsgHandlerRegistrant(szModuleName, szMsgType, fnHandler, nRef, nPosition);
  if (p != 0) {
    AddFirst(p);
  }
}
