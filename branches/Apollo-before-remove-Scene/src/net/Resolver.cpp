// ============================================================================
//
// Apollo
//
// ============================================================================

#define STRSAFE_NO_DEPRECATE 1

#if defined(LINUX) || defined(MAC)
  #include <sys/types.h>
  #include <arpa/inet.h>
  #include <netinet/ip.h>
  #include <sys/socket.h>
  #include <netdb.h>
#endif

#include "Resolver.h"
#include "NetModule.h"
#include "NetOS.h"

//----------------------------------------------------------

void DNSResolveTask::Execute()
{
  //--------------------------------
  // Declare and initialize variables.
  String sName = sName_;
  String sPort = "80";
  struct addrinfo aiHints;
  struct addrinfo *aiList = NULL;
  int retVal;

  //--------------------------------
  // Setup the hints address info structure
  // which is passed to the getaddrinfo() function
  memset(&aiHints, 0, sizeof(aiHints));
  aiHints.ai_family = AF_INET;
  aiHints.ai_socktype = SOCK_STREAM;
  aiHints.ai_protocol = IPPROTO_TCP;

  //--------------------------------
  // Call getaddrinfo(). If the call succeeds,
  // the aiList variable will hold a linked list
  // of addrinfo structures containing response
  // information about the host

  int ok = 0;
  String sAddress;
  String sError;

  if ((retVal = ::getaddrinfo(sName, NULL, &aiHints, &aiList)) != 0) {
    if (!NetModuleInstance::Get()->inShutdown()) {
      sError = ::gai_strerror(retVal);
    }
  } else {
    if (!NetModuleInstance::Get()->inShutdown()) {
      if (aiList == 0) {
        sError = "aiList == 0";
      } else {
        if (aiList->ai_family == AF_INET) {
          struct sockaddr_in* pSai = (struct sockaddr_in*) aiList->ai_addr;
          sAddress = NetModule::ip4_LongtoStr(pSai->sin_addr.s_addr);
          ok = 1;
        } else {
          sError.appendf("Address family not supported: %d", aiList->ai_family);
        }
        ::freeaddrinfo(aiList);
      }
    }
  }

  if (!NetModuleInstance::Get()->inShutdown()) {
    ApAsyncMessage<Msg_Net_DNS_Resolved> msg;
    msg->hResolver = hAp_;
    msg->sName = sName;
    if (ok) {
      msg->sAddress = sAddress;
    } else {
      msg->bSuccess = 0;
      msg->sComment = sError;
    }
    msg.Post();
  }

/*
  if (!NetModuleInstance::Get()->inShutdown()) {
    if (ok) {
      TCPConnectTask* pTask = new TCPConnectTask(hAp_, saAddress);
      if (pTask != 0) {
        NetModuleInstance::Get()->oSocketIO_.AddTask(pTask);
      }
    } else {
      apLog_Warning((LOG_CHANNEL, "DNSResolveTask::Execute", "getaddrinfo failed %s %s: %s", StringType(sAddress), StringType(sPort), StringType(sError)));

      Msg_Net_TCP_Connected* pMsg = new Msg_Net_TCP_Connected();
      if (pMsg !=0) {
        pMsg->hConnection = hAp_;
        pMsg->bSuccess = 0;
        pMsg->sComment = sError;
        pMsg->PostAsync();
      }
    }
  }
*/
}

//----------------------------------------------------------

void Resolver::CloseAll()
{
  bFinished_ = 1;
}
