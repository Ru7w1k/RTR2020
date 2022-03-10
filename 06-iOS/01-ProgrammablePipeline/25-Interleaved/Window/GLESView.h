//
//  MyView.h
//  Window
//
//  Created by Rutwik Choughule on 03/07/21.
//

#import <UIKit/UIKit.h>

@interface GLESView : UIView <UIGestureRecognizerDelegate>

- (void)startAnimation;
- (void)stopAnimation;

@end
