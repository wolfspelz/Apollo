// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgThreadPool_H_INCLUDED)
#define MsgThreadPool_H_INCLUDED

#include "ApMessage.h"

class Msg_ThreadPool_Finished : public ApNotificationMessage
{
public:
  Msg_ThreadPool_Finished() : ApNotificationMessage("ThreadPool_Finished") {}
public:
  ApHandle hPool;
};

#endif // MsgThreadPool_H_INCLUDED


