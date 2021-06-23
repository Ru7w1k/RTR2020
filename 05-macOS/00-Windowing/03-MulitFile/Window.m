// headers
#import <Cocoa/Cocoa.h>  // ~ Windows.h or xlib.h
#import <Foundation/Foundation.h>

#import "AppDelegate.h"
#import "MyView.h"

// NS -> NeXTSTEP

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
