//
//  main.m
//  Window
//
//  Created by Rutwik Choughule on 03/07/21.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char *argv[]) {
  // code
  NSString *appDelegateClassName;

  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  // Setup code that might create autoreleased objects goes here.
  appDelegateClassName = NSStringFromClass([AppDelegate class]);
  int ret = UIApplicationMain(argc, argv, nil, appDelegateClassName);

  // release resources from autorelease pool
  [pool release];

  return (ret);
}
