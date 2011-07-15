// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Display_H_INCLUDED)
#define Display_H_INCLUDED

#include "ApContainer.h"

class Avatar;

typedef ApHandlePointerTree<Avatar*> AvatarList;
typedef ApHandlePointerTreeNode<Avatar*> AvatarListNode;
typedef ApHandlePointerTreeIterator<Avatar*> AvatarListIterator;

typedef ApHandleTree<int> ParticipantFlags;

class ArenaModule;

class Display
{
public:
  Display(ArenaModule* pModule, const ApHandle& hContext);
  virtual ~Display();

  int Create();
  void Destroy();

  void SetVisibility(int bVisible);
  void SetPosition(int nLeft, int nBottom);
  void SetSize(int nWidth, int nHeight);

  void AttachLocation(const ApHandle& hLocation);
  void DetachLocation(const ApHandle& hLocation);

  void OnEnterRequested();
  void OnEnterBegin();
  void OnEnterComplete();
  void OnLeaveRequested();
  void OnLeaveBegin();
  void OnLeaveComplete();

  void OnViewLoaded();
  void OnViewUnload();
  void OnParticipantsChanged();
  void OnParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys);
  void OnReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void OnReceivePublicAction(const ApHandle& hParticipant, const String& sAction);
  void OnContextDetailsChanged(Apollo::ValueList& vlKeys);
  void OnLocationDetailsChanged(Apollo::ValueList& vlKeys);
  void OnAvatarAnimationBegin(const ApHandle& hParticipant, const String& sUrl);
  void OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);
  void OnShowChat(int bShow);
  void OnShowDebug(int bShow);
  void OnNavigatorCallDisplay(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

  inline int GetWidth() { return nWidth_; }
  inline int GetHeight() { return nHeight_; }
  inline ApHandle GetView() { return hView_; }
  inline ArenaModule* GetModule() { return pModule_; }
  inline ApHandle GetContext() { return hContext_; }
  inline ApHandle GetLocation() { return hLocation_; }

  // For tesing. Do not use.
  inline void _SetView(const ApHandle& h) { hView_ = h; }

protected:
  void StartDisplay();
  void ProcessAvatarList(Apollo::ValueList& vlParticipants, ApHandle& hSelf);
  void InitRemovedParticipants(ParticipantFlags& removedParticipants);
  void InitAddedParticipants(ParticipantFlags& addedParticipants);
  void RemoveFromRemovedParticipants(const ApHandle& h, ParticipantFlags& removedParticipants);
  void AddToAddedParticipants(const ApHandle& h, ParticipantFlags& addedParticipants);
  void EvaluateNewAvatarList(Apollo::ValueList& vlParticipants, ParticipantFlags& addedParticipants, ParticipantFlags& removedParticipants);
  void ProcessRemovedParticipants(ParticipantFlags& removedParticipants);
  void ProcessAddedParticipants(ParticipantFlags& addedParticipants);
  void ShowContextDetailDocumentUrl(const String& sValue);
  void ShowContextDetailLocationUrl(const String& sValue);
  void ShowLocationDetailState(const String& sValue);

  void RemoveAllAvatars();
  void RemoveAllObjects();
  void ResetLocationInfo();

  void SendVisibility();
  void SendPosition();

protected:
  ArenaModule* pModule_;
  ApHandle hContext_;
  ApHandle hLocation_;

  int bVisible_;
  int nLeft_;
  int nBottom_;
  int nWidth_;
  int nHeight_;
  int bSendVisibility_;
  int bHasPosition_;
  int bHasSize_;
  int bDebug_;

  ApHandle hView_;
  int bViewLoaded_;
  ApHandle hSelf_; // Self Participant

  Apollo::TimeValue tvEnterRequested_;
  Apollo::TimeValue tvEnterBegin_;
  Apollo::TimeValue tvLeaveRequested_;
  Apollo::TimeValue tvLeaveBegin_;

  AvatarList avatars_;
};

//---------------------------------------------------

class ViewSrpcMessage: public Msg_WebView_ViewCall
{
  typedef Msg_WebView_ViewCall base;
public:
  ViewSrpcMessage(Display* pDisplay, const String& sMethod);
  int Request();
};

#endif // Display_H_INCLUDED
