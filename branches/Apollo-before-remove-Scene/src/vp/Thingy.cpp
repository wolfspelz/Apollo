// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Thingy.h"

Thingy::~Thingy()
{
  if (pProvider_) {
    delete pProvider_;
    pProvider_ = 0;
  }
}

void Thingy::addSubscription()
{
  if (pProvider_) { pProvider_->onSubscribe(); }

  nSubscriber_++;
}

void Thingy::removeSubscription()
{
  if (pProvider_) { pProvider_->onUnsubscribe(); }

  nSubscriber_--;
  if (nSubscriber_ < 0) {
    nSubscriber_ = 0;
  }
}

void Thingy::clearSubscriptions()
{
  if (pProvider_) { pProvider_->onUnsubscribeAll(); }

  nSubscriber_ = 0;
}
