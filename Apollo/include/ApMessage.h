// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ApMessage_h_INCLUDED)
#define ApMessage_h_INCLUDED

#include "ApExports.h"
#include "ApTypes.h"
#include "SSystem.h"
#include "SrpcMessage.h"

// -------------------------------------------------------------------

#define ApIN
#define ApOUT
#define ApINOUT

class APOLLO_API ApMessage: public Elem
{
public:
  ApMessage(const char* szType)
    :Elem(szType)
    ,apStatus(Unknown)
    ,pRef_(0)
    ,bForward_(true)
    ,bOnce_(false)
  {}
  virtual ~ApMessage() {}

  typedef enum _ApStatus { Unknown = -1
    ,Error = 0
    ,Ok = 1
  } ApStatus;

  inline String& Type() { return getName(); }

  inline void Forward(bool bFlag) { bForward_ = bFlag; }
  inline bool Forward() { return bForward_; }
  inline void Stop() { Forward(false); }
  inline ApCallbackRef Ref() { return pRef_; }
  inline void Ref(ApCallbackRef pRef) { pRef_ = pRef; }
  inline void Once(bool bFlag) { bOnce_ = bFlag; }
  inline bool Once() { return bOnce_; }

  int Hook(const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition);
  int Unhook(const char* szModuleName, ApCallback fnHandler, ApCallbackRef nRef);

  // Never use this on local variables. Only on new-ed messages
  int PostAsync();

public: // Dispatcher internal use
  ApStatus apStatus;
  String sComment;

protected: // Dispatcher internal use
  ApCallbackRef pRef_;
  bool bForward_; // Call next handler
  bool bOnce_; // Call only 1 handler
};

// ------------------------------

template <class MSG> class ApAsyncMessage
{
public:
  ApAsyncMessage()
    :pMsg_(new MSG())
  {}

  MSG* operator->()
  {
    if (pMsg_) {
      return pMsg_;
    } else {
      return &msg_;
    }
  }

  void Post()
  {
    if (pMsg_ != 0) {
      pMsg_->PostAsync();
      pMsg_ = 0;
    }
  }

protected:
  MSG* pMsg_;

  // Return this member if new MSG() failed.
  // Accept member settings, but do not post it.
  // This only to avoid 0 pointer in very rare cases. 
  MSG msg_;
};

// ------------------------------

class APOLLO_API ApRequestMessage: public ApMessage
{
public:
  ApRequestMessage(const char* szType) : ApMessage(szType) {}
  virtual ~ApRequestMessage() {}
  int Request();
};

class APOLLO_API ApNotificationMessage: public ApMessage
{
public:
  ApNotificationMessage(const char* szType) : ApMessage(szType) {}
  virtual ~ApNotificationMessage() {}
  void Send();
};

class APOLLO_API ApFilterMessage: public ApMessage
{
public:
  ApFilterMessage(const char* szType) : ApMessage(szType) {}
  virtual ~ApFilterMessage() {}
  void Filter();
};

class APOLLO_API ApSRPCMessage: public ApMessage
{
public:
  ApSRPCMessage(const char* szType) : ApMessage(szType) {}
  virtual ~ApSRPCMessage() {}
  int Call();
public:
  ApIN Apollo::SrpcMessage srpc;
  ApOUT Apollo::SrpcMessage response;
};

// -------------------------------------------------------------------

#define ApCallbackPosSegment 1000
#define ApCallbackPosNormal 0
#define ApCallbackPosEarly (-1000 * ApCallbackPosSegment)
#define ApCallbackPosLate (1000 * ApCallbackPosSegment)
#define ApCallbackPosVeryEarly (-2000 * ApCallbackPosSegment)
#define ApCallbackPosIncrediblyEarly (-3000 * ApCallbackPosSegment)
#define ApCallbackPosBreathtakinglyEarly (-4000 * ApCallbackPosSegment)
#define ApCallbackPosOverwhelminglyEarly (-5000 * ApCallbackPosSegment)
#define ApCallbackPosRidiculouslyEarly (-6000 * ApCallbackPosSegment)

#define AP_MSG_HANDLER_METHOD(_class_, _msgtype_) \
void _class_ ## _On_ ## _msgtype_(Msg_ ## _msgtype_* pMsg) \
{ \
  _class_* self = (_class_*) pMsg->Ref(); \
  if (self != 0) { \
    self->On_ ## _msgtype_(pMsg); \
  } \
}\
void _class_ ## :: ## On_ ## _msgtype_(Msg_## _msgtype_* pMsg)

#define AP_MSGCLASS_HANDLER_METHOD(_class_, _msgtype_, _msgclass_) \
void _class_ ## _On_ ## _msgtype_(_msgclass_* pMsg) \
{ \
  _class_* self = (_class_*) pMsg->Ref(); \
  if (self != 0) { \
    self->On_ ## _msgtype_(pMsg); \
  } \
}\
void _class_ ## :: ## On_ ## _msgtype_(_msgclass_* pMsg)

//----------------------------------------------------------

// Deprecated, use AP_MSG_REGISTRY_ADD
#define AP_REFINSTANCE_MSG_CALLBACK(_class_, _msgtype_) (ApCallback) _class_ ## _On_## _msgtype_

// Deprecated, use AP_MSG_HANDLER_METHOD
#define AP_REFINSTANCE_MSG_HANDLER(_class_, _msgtype_) \
void _class_ ## _On_ ## _msgtype_(Msg_ ## _msgtype_* pMsg) \
{ \
  _class_* self = (_class_*) pMsg->Ref(); \
  if (self != 0) { \
    self->On_ ## _msgtype_(pMsg); \
  } \
}

//----------------------------------------------------------

class APOLLO_API ApMsgHandlerRegistrant: public Elem
{
public:
  ApMsgHandlerRegistrant(const char* szModuleName, const char* szMsgType, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition);
  ~ApMsgHandlerRegistrant();

  String sModuleName_;
  ApCallback fnHandler_;
  ApCallbackRef nRef_;

protected:
  friend class ListT<ApMsgHandlerRegistrant, Elem>;
  ApMsgHandlerRegistrant() {}
};

class APOLLO_API ApMsgHandlerRegistrantList: protected ListT<ApMsgHandlerRegistrant, Elem>
{
public:
  void add(const char* szModuleName, const char* szMsgType, ApCallback fnHandler, ApCallbackRef nRef, ApCallbackPos nPosition);
  void finish() { Empty(); }
};

#define AP_MSG_REGISTRY_DECLARE ApMsgHandlerRegistrantList apMsgRegistry_;

#define AP_MSG_REGISTRY_ADD(_mod_, _class_, _msgtype_, _ref_, _pos_) \
  apMsgRegistry_.add(_mod_, # _msgtype_, (ApCallback) _class_ ## _On_## _msgtype_, _ref_, Apollo::modulePos(_pos_, _mod_));

#define AP_MSG_REGISTRY_FINISH apMsgRegistry_.finish();

#endif // !defined(ApMessage_h_INCLUDED)
