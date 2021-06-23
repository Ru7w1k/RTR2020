#import "AppDelegate.h"
#import "MyView.h"

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
                          NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
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
