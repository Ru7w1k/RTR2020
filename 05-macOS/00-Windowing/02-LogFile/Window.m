// headers
#import <Cocoa/Cocoa.h> // ~ Windows.h or xlib.h
#import <Foundation/Foundation.h>

// log file
FILE *gpFile = NULL;

// NS -> NeXTSTEP

//// forward declarations of interface /////////////////////////////////////////

// :  extends
// <> implements
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface MyView : NSView
@end

////////////////////////////////////////////////////////////////////////////////

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

//// Implementation of interfaces //////////////////////////////////////////////

@implementation AppDelegate {
@private
  NSWindow *window;
  MyView *view;
}

// ~ WM_CREATE
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  // code

  // create log file
  NSBundle *appBundle = [NSBundle mainBundle];
  NSString *appDirPath = [appBundle bundlePath];
  NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
  NSString *logFileNameWithPath =
      [NSString stringWithFormat:@"%@/window.log", parentDirPath];

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
                          NSWindowStyleMaskMiniaturizable |
                          NSWindowStyleMaskResizable
                  backing:NSBackingStoreBuffered
                    defer:NO];

  [window setTitle:@"macOS | Log file"];
  [window center];

  // create view
  view = [[MyView alloc] initWithFrame:winRect];

  // assign view to window and set delegate to self
  [window setContentView:view];
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
  [view release];
  [window release];
  [super dealloc];
}

@end

@implementation MyView {
@private
}

- (id)initWithFrame:(NSRect)frame {
  // code
  self = [super initWithFrame:frame];

  if (self) {
  }

  return (self);
}

- (void)drawRect:(NSRect)dirtyRect {
  // code
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
  case 27: // esc key
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
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)rightMouseDown:(NSEvent *)theEvent {
  // code
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)otherMouseDown:(NSEvent *)theEvent {
  // code
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)dealloc {
  // code
  [super dealloc];
}

@end

////////////////////////////////////////////////////////////////////////////////
