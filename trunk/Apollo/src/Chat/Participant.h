// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Participant_H_INCLUDED)
#define Participant_H_INCLUDED

class ChatModule;
class ChatWindow;

class Participant
{
public:
  Participant(ChatModule* pModule, ChatWindow* pChatWindow, const ApHandle& hParticipant, int bSelf);

  void Create();
  void Destroy();

  void OnDetailsChanged(Apollo::ValueList& vlKeys);
  void OnReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);

protected:
  void SubscribeAndGetDetail(const String& sKey);
  void GetDetail(const String& sKey);
  void GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailUrl(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void UnsubscribeDetail(const String& sKey);

  void DisplaySetNickname(const String& sNickname);
  void DisplaySetImage(const String& sUrl);
  void DisplaySetOnlineStatus(const String& sStatus);
  void DisplayAddChatline(const String& sText);

protected:
  ChatModule* pModule_;
  ChatWindow* pChatWindow_;
  ApHandle hAp_;
  int bSelf_;

  String sNickname_;
  String sImage_;
  String sOnlineStatus_;

  static Apollo::ValueList noMimeTypes_;
  static Apollo::ValueList imageMimeTypes_;
};

#endif // Participant_H_INCLUDED
