// ============================================================================
//
// Apollo
//
// ============================================================================

#import "AppController.h"
#import "MsgMainLoop.h"
#import "MsgSystem.h"

@implementation NSApMessage

-(id) initWithApMessage:(ApMessage*)pMsg
{
  if ((self = [super init])) {
    pMsg_ = pMsg;
  }
  return self;
}

-(ApMessage*) getApMessage
{
  return pMsg_;
}

@end

@implementation ApNotificationQueue

- (id) init
{
  if ((self = [super init])) {
    pNotificationQueue_ = [[NSMutableArray alloc] init];
    pMainThread_ = [[NSThread currentThread] retain];
    pQueueLock_ = [[NSLock alloc] init];
    pNotificationPort_ = [[NSMachPort alloc] init];
    [pNotificationPort_ setDelegate:self];
    [[NSRunLoop currentRunLoop] addPort:pNotificationPort_
                                forMode:(NSString*)kCFRunLoopCommonModes
                                ];
  }
  return self;
}

- (void) dealloc
{
  [[NSRunLoop currentRunLoop] removePort:pNotificationPort_
                                 forMode:(NSString *) kCFRunLoopCommonModes
                                 ];

  [pNotificationQueue_ release];
  [pMainThread_ release];
  [pQueueLock_ release];

  [pNotificationPort_ release];
  [super dealloc];
}

- (void) registerForNotifications:(NSString*)sNotificationId
{
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(processNotification:)
                                               name:sNotificationId
                                             object:nil
                                             ];
}

- (void) handleMachMessage:(void *)msg
{
  AP_UNUSED_ARG(msg);
  
  [pQueueLock_ lock];
  while ([pNotificationQueue_ count] > 0) {
    NSNotification* pNotification = [[pNotificationQueue_ objectAtIndex:0] retain];
    [pNotificationQueue_ removeObjectAtIndex:0];
    [pQueueLock_ unlock];
    [self processNotification:pNotification];
    [pNotification release];
    [pQueueLock_ lock];
  }
  [pQueueLock_ unlock];
}

- (void) processNotification:(NSNotification *)pNotification
{
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
  
  if ([NSThread currentThread] != pMainThread_) {
    [pQueueLock_ lock];
    [pNotificationQueue_ addObject:pNotification];
    [pQueueLock_ unlock];
    
    [pNotificationPort_ sendBeforeDate:[NSDate date]
                            components:nil
                                  from:nil
                              reserved:0
                        ];
    
  } else {
    NSApMessage* pNSApMsg = (NSApMessage*)[pNotification object];
    ApMessage* pMsg = [pNSApMsg getApMessage];
    [pNSApMsg release];
    (void)Apollo::Call(pMsg);
    delete pMsg;
    pMsg = NULL;
  }
  
  [pPool release];
}


@end

@implementation AppController

- (id)init
{
  if((self = [super init])) {
    bFirstTimer_ = 1;
    pSecTimer_ = nil;
    pMainWindow_ = nil;
    
    pShutdownDelayTimer_ = nil;
    nWaitingForModules_ = 0;
    nFinishedModules_ = 0;

  }
  return self;
}

- (void)dealloc
{
  if (pSecTimer_ != nil) {
    apLog_Warning((LOG_CHANNEL, "AppController::dealloc", "pSecTimer_ != NULL"));
    [pSecTimer_ release];
  }
  if (pMainWindow_ != nil) {
    [pMainWindow_ release];
  }
  [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)pNotification
{
  //apLog_Debug((LOG_CHANNEL, "AppController::applicationDidFinishLaunching", "< --- >"));
  AP_UNUSED_ARG(pNotification);
  
  bFirstTimer_ = YES;
  pSecTimer_ = [NSTimer scheduledTimerWithTimeInterval:1
                                               target:self
                                             selector:@selector(onApSecTimer:)
                                             userInfo:nil
                                              repeats:YES
                                          ];
  
  
  if (pMainWindow_ != nil) {
    [pMainWindow_ setReleasedWhenClosed:NO];
    @try {
      [pMainWindow_ makeKeyAndOrderFront:self];
      //[pMainWindow_ release];
      if (NO == [pMainWindow_ canBecomeMainWindow]) {
        apLog_Warning((LOG_CHANNEL, "AppController::applicationDidFinishLaunching", "Cannot make pMainWindow_ the main application window"));
      } else {
        //apLog_Debug((LOG_CHANNEL, "AppController::applicationDidFinishLaunching", "Making pMainWindow_ the main application window"));
        [pMainWindow_ makeMainWindow];
        [NSApp activateIgnoringOtherApps:YES];
        //[pMainWindow_ release];
      }
    } @catch(NSException* pEx) {
      apLog_Error((LOG_CHANNEL, "AppController::applicationDidFinishLaunching", "Exception in NSWindow::makeKeyAndOrderFront:self"));
    }
  }
  
  
} // Send eventloop_begin

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)pApp
{
  apLog_Debug((LOG_CHANNEL, "AppController::applicationShouldTerminate", "< --- >"));
  AP_UNUSED_ARG(pApp);
  
  double fSecsDelay = 0.1;
  {
    Msg_MainLoop_EventLoopBeforeEnd msg; 
    msg.nDelaySec = Apollo::GetModuleConfig(MODULE_NAME, "ShutdownDelay", 2);
    msg.Filter();
    if ((nWaitingForModules_ = msg.nWaitCount) > 0) {
      fSecsDelay = (double)msg.nDelaySec;
    }
  }
  
  pShutdownDelayTimer_ = [NSTimer timerWithTimeInterval:fSecsDelay
                                                 target:self
                                               selector:@selector(onShutdownDelayTimer:)
                                               userInfo:nil
                                                repeats:NO
                          ];
  
  [[NSRunLoop currentRunLoop] addTimer: pShutdownDelayTimer_ 
                           forMode: (NSString*)kCFRunLoopCommonModes
   ];
  
  //return NSTerminateLater;
  return NSTerminateCancel;

} // process eventloop_before_end

- (void)applicationWillTerminate:(NSNotification *)pNotification
{
  AP_UNUSED_ARG(pNotification);
  apLog_Debug((LOG_CHANNEL, "AppController::applicationWillTerminate", "< --- >"));
  
  //{ Msg_MainLoop_EventLoopEnd msg; msg.Send(); }
} // Send Msg_MainLoop_EventLoopEnd

- (void)onApSecTimer:(NSTimer*)pTheTimer
{
  
  if (pTheTimer != pSecTimer_) { return; }
  
  if (bFirstTimer_ == YES) {
    bFirstTimer_ = NO;
    { Msg_MainLoop_EventLoopBegin msg; msg.Send(); }
  } else {
    { Msg_System_SecTimer msg; msg.Send(); }
  }
}

- (void) onShutdownDelayTimer:(NSTimer*)pTheTimer
{
  apLog_Debug((LOG_CHANNEL, "AppController::onShutdownDelayTimer", "Called"));
  
  if (pTheTimer != pShutdownDelayTimer_) { AP_DEBUG_BREAK(); }  
  [self terminateNow];
}

- (void) terminateNow
{
  apLog_Debug((LOG_CHANNEL, "AppController::terminateNow", "Called"));

  if (pSecTimer_ && [pSecTimer_ isValid]) { 
    [pSecTimer_ invalidate]; 
    pSecTimer_ = nil; 
  }  

  if (pShutdownDelayTimer_ && [pShutdownDelayTimer_ isValid]) {
    [pShutdownDelayTimer_ invalidate];
    pShutdownDelayTimer_ = nil;
  }

  Msg_MainLoop_EventLoopEnd msg; msg.Send();

  [NSApp stop:nil];
  
}

- (void) moduleFinished
{
  if (++nFinishedModules_ >= nWaitingForModules_) {
    [self terminateNow];
  }
  apLog_Debug((LOG_CHANNEL, "AppController::moduleFinished", "waitcount: %d # finished: %d", nWaitingForModules_, nFinishedModules_));
}

@end
