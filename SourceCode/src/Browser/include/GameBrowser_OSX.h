/*
---------------------------------------------------------------------------------------
   ___                   _															   
  /___\__ _ _ __ ___    /_\  _ __  _ __
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \											   
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \
\___/ \__, |_|  \___| \_/ \_/ .__/| .___/
      |___/                 |_|   |_|                                 
      Ogre 1.9.x Application for VC10
      https://plus.google.com/+bahaonguyen/
---------------------------------------------------------------------------------------
| Filename   : GameBrowser_OSX.h                                                           
---------------------------------------------------------------------------------------
| Written in : C++                                                     
---------------------------------------------------------------------------------------
| Compiler   : Microsoft Visual C++ 2010 Service Pack 1.                               
---------------------------------------------------------------------------------------
| Engine     : Ogre3D; ....................................                                                                     
---------------------------------------------------------------------------------------
| Written by : Ba Hao Nguyen – Sports editors, IT.                                
---------------------------------------------------------------------------------------
| Note       : ....................................                                
---------------------------------------------------------------------------------------
| Create File: 21:29:59 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:29:59 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef __GameBrowser_OSX_H__
#define __GameBrowser_OSX_H__

#include "OgrePlatform.h"

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
#error This header is for use with Mac OS X only
#endif

#ifdef __OBJC__

#define USE_DISPLAYLINK 0

#import "OgreOSXCocoaWindow.h"
#import <QuartzCore/CVDisplayLink.h>

#include "GameBrowser.h"

using namespace Ogre;

// All this does is suppress some messages in the run log.  NSApplication does not
// implement buttonPressed and apps without a NIB have no target for the action.
@implementation NSApplication (_suppressUnimplementedActionWarning)
- (void) buttonPressed:(id)sender
{
    /* Do nothing */
}
@end


#if defined(MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6
@interface AppDelegate : NSObject <NSApplicationDelegate>
#else
@interface AppDelegate : NSObject
#endif
{
    NSTimer *mTimer;

    NSDate *mDate;
    NSTimeInterval mLastFrameTime;
    CVDisplayLinkRef mDisplayLink; //display link for managing rendering thread
}

- (void)go;
- (void)renderOneFrame:(id)sender;
- (void)shutdown;

@property (retain, atomic) NSTimer *mTimer;
@property (nonatomic) NSTimeInterval mLastFrameTime;

@end

static OgreBites::GameBrowser sb = 0;

#if __LP64__
static id mAppDelegate;

// DisplayLink callback
#if USE_DISPLAYLINK
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    if(!sb.mIsShuttingDown &&
       Ogre::Root::getSingletonPtr() &&
       Ogre::Root::getSingleton().isInitialised())
    {
        NSOpenGLContext *ctx = static_cast<OSXCocoaWindow *>(sb.mWindow)->nsopenGLContext();
        CGLContextObj cglContext = (CGLContextObj)[ctx CGLContextObj];

        // Lock the context before we render into it.
        CGLLockContext(cglContext);

        // Calculate the time since we last rendered.
        Real deltaTime = 1.0 / (outputTime->rateScalar * (Real)outputTime->videoTimeScale / (Real)outputTime->videoRefreshPeriod);

        // Make the context current and dispatch the render.
        [ctx makeCurrentContext];
        dispatch_async(dispatch_get_main_queue(), ^(void)
                       {
                           Ogre::Root::getSingleton().renderOneFrame(deltaTime);
                       });

        CGLUnlockContext(cglContext); 
    }
    else if(sb.mIsShuttingDown)
    {
        [(AppDelegate *)mAppDelegate shutdown];
    }
    return kCVReturnSuccess;
}
#endif
#endif

@implementation AppDelegate

@synthesize mTimer;
@dynamic mLastFrameTime;

- (NSTimeInterval)mLastFrameTime
{
    return mLastFrameTime;
}

- (void)setLastFrameTime:(NSTimeInterval)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        mLastFrameTime = frameInterval;
    }
}

- (void)go {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    mLastFrameTime = 1;
    mTimer = nil;

    try {
        sb.go();
        Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();

        // Clear event times
		Ogre::Root::getSingleton().clearEventTimes();
    } catch( Ogre::Exception& e ) {
        std::cerr << "An exception has occurred: " <<
        e.getFullDescription().c_str() << std::endl;
    }
#if __LP64__ && USE_DISPLAYLINK
    CVReturn ret = kCVReturnSuccess;
    // Create a display link capable of being used with all active displays
    ret = CVDisplayLinkCreateWithActiveCGDisplays(&mDisplayLink);

    // Set the renderer output callback function
    ret = CVDisplayLinkSetOutputCallback(mDisplayLink, &MyDisplayLinkCallback, self);

    // Set the display link for the current renderer
    NSOpenGLContext *ctx = static_cast<OSXCocoaWindow *>(sb.mWindow)->nsopenGLContext();
    NSOpenGLPixelFormat *fmt = static_cast<OSXCocoaWindow *>(sb.mWindow)->nsopenGLPixelFormat();
    CGLContextObj cglContext = (CGLContextObj)[ctx CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[fmt CGLPixelFormatObj];
    ret = CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(mDisplayLink, cglContext, cglPixelFormat);

    // Activate the display link
    ret = CVDisplayLinkStart(mDisplayLink);
#else
	mTimer = [[NSTimer timerWithTimeInterval: 0.001 target:self selector:@selector(renderOneFrame:) userInfo:self repeats:true] retain];
	[[NSRunLoop currentRunLoop] addTimer:mTimer forMode: NSDefaultRunLoopMode];
	[[NSRunLoop currentRunLoop] addTimer:mTimer forMode: NSEventTrackingRunLoopMode]; // Ensure timer fires during resize
#endif
    [pool release];
}
    
- (void)applicationDidFinishLaunching:(NSNotification *)application {
    mLastFrameTime = 1;
    mTimer = nil;

    [self go];
}

- (void)shutdown {
    if(mDisplayLink)
    {
        CVDisplayLinkStop(mDisplayLink);
        CVDisplayLinkRelease(mDisplayLink);
        mDisplayLink = nil;
    }

    [NSApp terminate:nil];
}

- (void)renderOneFrame:(id)sender
{
    if(!sb.mIsShuttingDown && Ogre::Root::getSingletonPtr() &&
       Ogre::Root::getSingleton().isInitialised() && !Ogre::Root::getSingleton().endRenderingQueued())
    {
        Ogre::Root::getSingleton().renderOneFrame();
    }
    else if(sb.mIsShuttingDown)
    {
        if(mTimer)
        {
            [mTimer invalidate];
            mTimer = nil;
        }

        [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
    }
    else if(!sb.isLastRun() && sb.isFirstRun())
    {
        sb.closeApp();

        sb.setFirstRun(false);

        sb.initApp();
        sb.loadStartUpGame();
        sb.setFirstRun(true);
    }
}

@end

#endif

#endif
