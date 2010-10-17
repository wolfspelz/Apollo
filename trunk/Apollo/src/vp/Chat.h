// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Chat_H_INCLUDED)
#define Chat_H_INCLUDED

class Chat: public Elem
{
public:
  Chat(const ApHandle& hLocation, const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, Apollo::TimeValue& tvStamp);
  ~Chat();

  String& getText() { return getString(); }
  Apollo::TimeValue& getTime() { return tvCreated_; }

  void Chat::send_VpView_LocationPublicChat();

protected:
  Apollo::TimeValue tvCreated_;
  ApHandle hLocation_;
  ApHandle hParticipant_;
  ApHandle hAp_;
};

#endif // Chat_H_INCLUDED
