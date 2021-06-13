// headers
#import <Cocoa/Cocoa.h> // ~ Windows.h or xlib.h
#import <Foundation/Foundation.h>

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

  [window setTitle:@"RMC | macOS Window"];
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
}

// ~ WM_DESTROY
- (void)windowWillClose:(NSNotification *)aNotification {
  // code
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
  NSString *centralText;
}

- (id)initWithFrame:(NSRect)frame {
  // code
  self = [super initWithFrame:frame];

  if (self) {
    centralText = @"Hello World!";
  }
  return (self);
}

- (void)drawRect:(NSRect)dirtyRect {
  // code
  // black color for background
  NSColor *backgroundColor = [NSColor blackColor];
  [backgroundColor set];
  NSRectFill(dirtyRect);

  NSDictionary *dictionaryForTextAttributes = [NSDictionary
      dictionaryWithObjectsAndKeys:[NSFont fontWithName:@"Helvetica" size:32],
                                   NSFontAttributeName, [NSColor greenColor],
                                   NSForegroundColorAttributeName, nil];

  NSSize textSize =
      [centralText sizeWithAttributes:dictionaryForTextAttributes];

  NSPoint point;
  point.x = (dirtyRect.size.width / 2) - (textSize.width / 2);
  // +12 to adjust the title bar of window
  point.y = (dirtyRect.size.height / 2) - (textSize.height / 2) + 12;

  [centralText drawAtPoint:point withAttributes:dictionaryForTextAttributes];
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
  centralText = @"left mouse button is clicked!";
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)rightMouseDown:(NSEvent *)theEvent {
  // code
  centralText = @"right mouse button is clicked!";
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)otherMouseDown:(NSEvent *)theEvent {
  // code
  centralText = @"other mouse button is clicked!";
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)dealloc {
  // code
  [super dealloc];
}

@end

////////////////////////////////////////////////////////////////////////////////
