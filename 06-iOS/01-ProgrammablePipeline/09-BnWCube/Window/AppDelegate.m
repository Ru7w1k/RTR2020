//
//  AppDelegate.m
//  Window
//
//  Created by Rutwik Choughule on 03/07/21.
//

#import "AppDelegate.h"
#import "GLESView.h"
#import "ViewController.h"

@implementation AppDelegate {
 @private
  UIWindow *window;
  ViewController *viewController;
  GLESView *view;
}

- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
  // Override point for customization after application launch.
  CGRect winRect = [[UIScreen mainScreen] bounds];

  window = [[UIWindow alloc] initWithFrame:winRect];
  viewController = [[ViewController alloc] init];

  [window setRootViewController:viewController];

  view = [[GLESView alloc] initWithFrame:winRect];

  [viewController setView:view];
  [view release];

  [window makeKeyAndVisible];
  [view startAnimation];

  return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
  // code
  [view stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
  // code
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
  // code
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
  // code
  [view startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application {
  // code
  [view stopAnimation];
}

- (void)dealloc {
  // code
  [view release];
  [viewController release];
  [window release];

  [super dealloc];
}

@end
