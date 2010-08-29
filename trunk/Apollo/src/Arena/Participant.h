// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Participant_H_INCLUDED)
#define Participant_H_INCLUDED

class Participant
{
public:
  Participant(ApHandle hParticipant);

  void Show();
  void Hide();
  void DetailsChanged(Apollo::ValueList& vlKeys);

protected:
  void SubscribeAndGetDetail(const String& sKey);
  void GetDetail(const String& sKey);
  void GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes);
  void UnSubscribeDetail(const String& sKey);
  void HandleAvatarData(const String& sMimeType, const String& sSource, Buffer& sbData);

protected:
  ApHandle hAp_;
  String sNickname_;

  Apollo::ValueList noMimeTypes_;
  Apollo::ValueList avatarMimeTypes_;
  
  ApHandle hAnimatedItem_;
};

#endif // Participant_H_INCLUDED
