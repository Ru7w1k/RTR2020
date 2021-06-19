// headers
#import <Cocoa/Cocoa.h>  // ~ Windows.h or xlib.h
#import <Foundation/Foundation.h>

// CoreVideo display link: to match with refresh rate
#import <QuartzCore/CVDisplayLink.h>

// opengl headers
#import <OpenGL/gl3.h>  // gl.h
// #import <OpenGL/gl3ext.h>

// callback function for display link
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *,
                               CVOptionFlags, CVOptionFlags *, void *);

// log file
FILE *gpFile = NULL;

// NS -> NeXTSTEP
// CG -> Core Graphics
// CV -> Core Video
// CGL -> Core Graphics Library

//// forward declarations of interface /////////////////////////////////////////////////////////////

// :  extends
// <> implements
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface MyOpenGLView : NSOpenGLView
@end

////////////////////////////////////////////////////////////////////////////////////////////////////

// entry-point function
int main(int argc, char *argv[]) {
  // code
  NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc] init];

  // global application object, no need to declare
  NSApp = [NSApplication sharedApplication];

  // AppDelegate will handle Application and Window delegate/functions
  [NSApp setDelegate:[[AppDelegate alloc] init]];

  // message loop!
  [NSApp run];

  // control will return here, when program is terminating
  // release resources from autorelease pool
  [pPool release];

  return (0);
}

//// Implementation of interfaces //////////////////////////////////////////////////////////////////

@implementation AppDelegate {
 @private
  NSWindow *window;
  MyOpenGLView *myOpenGLView;
}

// ~ WM_CREATE
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  // code

  // create log file
  NSBundle *appBundle = [NSBundle mainBundle];
  NSString *appDirPath = [appBundle bundlePath];
  NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
  NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/window.log", parentDirPath];

  const char *pszLogFileNameWithPath =
      [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

  gpFile = fopen(pszLogFileNameWithPath, "w");
  if (gpFile == NULL) {
    [self release];
    [NSApp terminate:self];
  }

  fprintf(gpFile, "==== Application Started ====\n");

  // window
  // this is from core library, which is written in C
  NSRect winRect = NSMakeRect(0.0, 0.0, 800.0, 600.0);

  // create window
  window = [[NSWindow alloc]
      initWithContentRect:winRect
                styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                          NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                  backing:NSBackingStoreBuffered
                    defer:NO];

  [window setTitle:@"OpenGL | Blue Screen"];
  [window center];

  // create view
  myOpenGLView = [[MyOpenGLView alloc] initWithFrame:winRect];

  // assign view to window and set delegate to self
  [window setContentView:myOpenGLView];
  [window setDelegate:self];
  [window makeKeyAndOrderFront:self];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // code
  if (gpFile) {
    fprintf(gpFile, "==== Application Terminated ====\n");
    fclose(gpFile);
    gpFile = NULL;
  }
}

// ~ WM_DESTROY
- (void)windowWillClose:(NSNotification *)aNotification {
  // code
  // this will pass to applicationWillTerminate
  [NSApp terminate:self];
}

- (void)dealloc {
  // code
  [myOpenGLView release];
  [window release];
  [super dealloc];
}

@end

@implementation MyOpenGLView {
 @private
  CVDisplayLinkRef displayLink;
}

- (id)initWithFrame:(NSRect)frame {
  // code
  self = [super initWithFrame:frame];

  if (self) {
    // ~ Pixel Format Descriptor
    // ~ glx framebuffer attributes
    // PFA: Pixel Format Attribute
    NSOpenGLPixelFormatAttribute attributes[] = {
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion4_1Core,  // OpenGL 4.1 core profile
        NSOpenGLPFAScreenMask,
        CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),  // single display
        NSOpenGLPFANoRecovery,   // do not shift to SW renderer, if HW renderer is missing
        NSOpenGLPFAAccelerated,  // HW rendering
        NSOpenGLPFAColorSize,
        24,  // RGB
        NSOpenGLPFADepthSize,
        24,
        NSOpenGLPFAAlphaSize,
        8,
        NSOpenGLPFADoubleBuffer,
        0  // end of list
    };

    // get pixel format
    NSOpenGLPixelFormat *pixelFormat =
        [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
    if (pixelFormat == NULL) {
      fprintf(gpFile, "Pixel Format failed..\nTerminating..\n");
      [self release];
      [NSApp terminate:self];
    }

    NSOpenGLContext *glContext = [[[NSOpenGLContext alloc] initWithFormat:pixelFormat
                                                             shareContext:nil] autorelease];

    // set pixel format and context of current view
    [self setPixelFormat:pixelFormat];
    [self setOpenGLContext:glContext];
  }

  return (self);
}

- (CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime {
  // code
  // this function runs in separate thread (multithreading)
  // hence separate autoreleasepool is required
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  // call our drawing method
  [self drawView];

  [pool release];
  return (kCVReturnSuccess);
}

// from NSOpenGLView
- (void)prepareOpenGL {
  // code
  [super prepareOpenGL];

  // get OpenGL context and make it current context
  [[self openGLContext] makeCurrentContext];

  // swap interval
  GLint swapInt = 1;
  [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

  // set clear color
  glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

  // create and start display link
  CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
  CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
  CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
  CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
  CVDisplayLinkStart(displayLink);
}

// from NSOpenGLView
- (void)reshape {
  // code
  [super reshape];

  CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

  NSRect rect = [self bounds];
  if (rect.size.height < 0) {
    rect.size.height = 1;
  }
  glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);

  CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect {
  // code
  [self drawView];
}

- (void)drawView {
  // code
  [[self openGLContext] makeCurrentContext];
  CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
  CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (BOOL)acceptsFirstResponder {
  // code
  [[self window] makeFirstResponder:self];
  return (YES);
}

- (void)keyDown:(NSEvent *)theEvent {
  // code
  int key = (int)[[theEvent characters] characterAtIndex:0];

  switch (key) {
    case 27:  // esc key
      [self release];
      [NSApp terminate:self];
      break;

    case 'F':
    case 'f':
      [[self window] toggleFullScreen:self];
      break;
  }
}

- (void)mouseDown:(NSEvent *)theEvent {
  // code
}

- (void)rightMouseDown:(NSEvent *)theEvent {
  // code
}

- (void)otherMouseDown:(NSEvent *)theEvent {
  // code
}

- (void)dealloc {
  // code
  CVDisplayLinkStop(displayLink);
  CVDisplayLinkRelease(displayLink);

  [super dealloc];
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////

// callback "C" style function in global space
// displayLinkContext: MyOpenGLView pointer set by OS while calling this
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *now,
                               const CVTimeStamp *outputTime, CVOptionFlags flagsIn,
                               CVOptionFlags *flagsOut, void *displayLinkContext) {
  // code
  CVReturn result = [(MyOpenGLView *)displayLinkContext getFrameForTime:outputTime];
  return (result);
}
