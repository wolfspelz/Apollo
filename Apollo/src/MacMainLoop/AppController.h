// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(AppController_H_INCLUDED)
#define AppController_H_INCLUDED

#import "Apollo.h"
#import "ApLog.h"
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#define LOG_CHANNEL "MacMainLoop"
#define MODULE_NAME "MacMainLoop"

#define APTHREADMSG_NOTIFICATIONID @"ApThreadMessage"

//@class ApMessage; // Forward decl
@interface NSApMessage: NSObject
{
  ApMessage* pMsg_;
}
-(id)initWithApMessage:(ApMessage*)pMsg;
-(ApMessage*)getApMessage;
@end

@interface ApNotificationQueue: NSObject
{
    NSMutableArray* pNotificationQueue_;
    NSThread* pMainThread_;
    NSLock *pQueueLock_;
    NSMachPort *pNotificationPort_;
}
 
- (id)   init;
- (void) registerForNotifications:(NSString*)sNotificationId;
- (void) dealloc;
- (void) handleMachMessage:(void *)pMsg;
- (void) processNotification:(NSNotification *)pNotification;
@end

@interface AppController : NSObject
{
  NSWindow* pMainWindow_;
  // ======
  NSTimer* pSecTimer_;
  BOOL bFirstTimer_;
  // Shutdown delay Timer
  NSTimer* pShutdownDelayTimer_;
  int nWaitingForModules_;
  int nFinishedModules_;
}

//- (void)applicationWillFinishLaunching:(NSNotification *)pNotification;
- (id) init;
- (void) dealloc;
- (void) applicationDidFinishLaunching:(NSNotification *)pNotification; // Send eventloop_begin
- (NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication *)pApp; // process eventloop_before_end
- (void) applicationWillTerminate:(NSNotification *)pNotification; // Send Msg_MainLoop_EventLoopEnd

// ========

- (void) onApSecTimer:(NSTimer*)pTheTimer;

// ========

- (void) onShutdownDelayTimer:(NSTimer*)pTheTimer;
- (void) terminateNow;

// ========

- (void) moduleFinished;

@end

#endif // AppController_H_INCLUDED

