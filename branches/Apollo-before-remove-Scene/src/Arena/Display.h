// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Display_H_INCLUDED)
#define Display_H_INCLUDED

#include "ApContainer.h"
#include "Meta.h"
#include "Hud.h"

class Avatar;

typedef ApHandlePointerTree<Avatar*> AvatarList;
typedef ApHandlePointerTreeNode<Avatar*> AvatarListNode;
typedef ApHandlePointerTreeIterator<Avatar*> AvatarListIterator;

typedef StringPointerTree<Layer*> LayerList;
typedef StringPointerTreeNode<Layer*> LayerListNode;
typedef StringPointerTreeIterator<Layer*> LayerListIterator;

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
  void SetPosition(int nX, int nY);
  void SetSize(int nW, int nH);
  
  void AttachLocation(const ApHandle& hLocation);
  void DetachLocation(const ApHandle& hLocation);

  void OnEnterRequested();
  void OnEnterBegin();
  void OnEnterComplete();
  void OnLeaveRequested();
  void OnLeaveBegin();
  void OnLeaveComplete();

  void OnParticipantsChanged();
  void OnParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys);
  void OnReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void OnAvatarAnimationFrame(const ApHandle& hParticipant, const Apollo::Image& image);
  int OnTimerEvent(Msg_Timer_Event* pMsg);
  int OnMouseEvent(Msg_Scene_MouseEvent* pMsg);
  void OnContextDetailsChanged(Apollo::ValueList& vlKeys);

  inline int GetWidth() { return nW_; }
  inline int GetHeight() { return nH_; }
  inline ApHandle GetScene() { return hScene_; }
  inline ArenaModule* GetModule() { return pModule_; }
  inline ApHandle GetContext() { return hContext_; }
  inline ApHandle GetLocation() { return hLocation_; }

  void SetDebugFrameColor(const String& sPath);

protected:
  void ProcessAvatarList(Apollo::ValueList& vlParticipants);
  void InitRemovedParticipants(ParticipantFlags& removedParticipants);
  void InitAddedParticipants(ParticipantFlags& addedParticipants);
  void RemoveFromRemovedParticipants(const ApHandle& h, ParticipantFlags& removedParticipants);
  void AddToAddedParticipants(const ApHandle& h, ParticipantFlags& addedParticipants);
  void EvaluateNewAvatarList(Apollo::ValueList& vlParticipants, ParticipantFlags& addedParticipants, ParticipantFlags& removedParticipants);
  void ProcessRemovedParticipants(ParticipantFlags& removedParticipants);
  void ProcessAddedParticipants(ParticipantFlags& addedParticipants);

  void RemoveAllAvatars();
  void RemoveAllObjects();
  void ResetLocationInfo();

  Meta* GetMeta();
  Hud* GetHud();

protected:
  ArenaModule* pModule_;
  ApHandle hContext_;
  ApHandle hLocation_;

  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  ApHandle hScene_;

  Apollo::TimeValue tvEnterRequested_;
  Apollo::TimeValue tvEnterBegin_;
  Apollo::TimeValue tvLeaveRequested_;
  Apollo::TimeValue tvLeaveBegin_;

  AvatarList avatars_;
  LayerList layers_;
};

#endif // Display_H_INCLUDED