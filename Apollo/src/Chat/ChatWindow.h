// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Chat_H_INCLUDED)
#define Chat_H_INCLUDED

#include "Participant.h"

class ChatModule;

typedef ApHandlePointerTree<Participant*> ParticipantList;
typedef ApHandlePointerTreeNode<Participant*> ParticipantListNode;
typedef ApHandlePointerTreeIterator<Participant*> ParticipantListIterator;

class ChatModule;

class ChatWindow
{
public:
  ChatWindow::ChatWindow(ChatModule* pModule, const ApHandle& hChat)
    :pModule_(pModule)
    ,hAp_(hChat)
    ,bVisible_(0)
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(600)
    ,nHeight_(400)
  {}
  virtual ~ChatWindow();

  inline ChatModule* GetModule() { return pModule_; }
  inline ApHandle apHandle() { return hAp_; }
  inline ApHandle GetContext() { return hContext_; }
  inline ApHandle GetLocation() { return hLocation_; }

  void AttachToLocation(const ApHandle& hLocation);

  void Open();
  void Close();

  void OnLoaded();
  void OnUnload();
  void OnCallModule(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response);

  void OnParticipantAdded(const ApHandle& hParticipant, int bSelf);
  void OnParticipantRemoved(const ApHandle& hParticipant);
  void OnParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys);
  void OnReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv);
  void OnLocationDetailsChanged(Apollo::ValueList& vlKeys);

protected:
  void ShowLocationDetailState(const String& sValue);

protected:
  ChatModule* pModule_;
  ApHandle hAp_;
  ApHandle hContext_;
  ApHandle hLocation_;

  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;

  ApHandle hSelf_;
  ParticipantList participants_;
};

#endif // Chat_H_INCLUDED
