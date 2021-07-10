//
//  GLESView.m
//  Window
//
//  Created by Rutwik Choughule on 03/07/21.
//

#import <OpenGLES/ES3/gl.h>  // OpenGLES 3.0
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

@implementation GLESView {
 @private
  EAGLContext *eaglContext;

  GLuint defaultFramebuffer;
  GLuint colorRenderbuffer;
  GLuint depthRenderbuffer;

  id displayLink;
  NSInteger animationFrameInterval;
  BOOL isAnimating;
}

- (id)initWithFrame:(CGRect)frame {
  // code
  self = [super initWithFrame:frame];

  if (self) {
    
    // create drawable (layer/surface)
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];
    [eaglLayer setOpaque:YES];
    [eaglLayer
        setDrawableProperties:[NSDictionary
                                  dictionaryWithObjectsAndKeys:
                                      [NSNumber numberWithBool:NO],
                                      kEAGLDrawablePropertyRetainedBacking,  // do not retain frame
                                      kEAGLColorFormatRGBA8,  // eagl color format 32 bit
                                      kEAGLDrawablePropertyColorFormat, nil]];

    // create context
    eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (eaglContext == nil) {
      printf("OpenGLES Context creation failed..\n");
      return (nil);
    }

    // set context
    [EAGLContext setCurrentContext:eaglContext];

    // create framebuffer with color and depth attachments
    glGenFramebuffers(1, &defaultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);

    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                              colorRenderbuffer);

    GLint backingWidth;
    GLint backingHeight;

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);

    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                              depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      printf("Framebuffer is not complete..\n");
      [self uninitialize];
      return (nil);
    }

    // animation settings
    animationFrameInterval = 60;  // from iOS 8.2 it is defaulted to 60
    isAnimating = NO;

    // opengles code
    
    // fetch OpenGL related details
    printf("OpenGLES Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("OpenGLES Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGLES Version:  %s\n", glGetString(GL_VERSION));
    printf("GLSL ES  Version:  %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // user input handling (gestures)
    UITapGestureRecognizer *singleTapGestureRecognizer =
        [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onSingleTap:)];
    [singleTapGestureRecognizer setNumberOfTapsRequired:1];
    [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
    [singleTapGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:singleTapGestureRecognizer];

    UITapGestureRecognizer *doubleTapGestureRecongnizer =
        [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onDoubleTap:)];
    [doubleTapGestureRecongnizer setNumberOfTapsRequired:2];
    [doubleTapGestureRecongnizer setNumberOfTouchesRequired:1];
    [doubleTapGestureRecongnizer setDelegate:self];
    [self addGestureRecognizer:doubleTapGestureRecongnizer];

    // this will separate two single taps from double tap
    [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecongnizer];

    UISwipeGestureRecognizer *swipeGestureReconginzer =
        [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(onSwipe:)];
    [swipeGestureReconginzer setDelegate:self];
    [self addGestureRecognizer:swipeGestureReconginzer];

    UILongPressGestureRecognizer *longPressGestureRecognizer =
        [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(onLongPress:)];
    [longPressGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:longPressGestureRecognizer];
  }

  return (self);
}

//- (void)drawRect:(CGRect)rect {
//  // Drawing code
//}

+ (Class)layerClass {
  // code
  return ([CAEAGLLayer class]);
}

// ~ resize
- (void)layoutSubviews {
  // code
  GLint width;
  GLint height;

  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
  [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)[self layer]];

  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

  glGenRenderbuffers(1, &depthRenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                            depthRenderbuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Framebuffer is not complete..\n");
  }

  if (height < 0) {
    height = 1;
  }

  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  [self drawView:nil];
}

- (void)drawView:(id)sender {
  // code
  [EAGLContext setCurrentContext:eaglContext];
  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
  [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)startAnimation {
  // code
  if (isAnimating == NO) {
    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self
                                                                    selector:@selector(drawView:)];
    [displayLink setPreferredFramesPerSecond:animationFrameInterval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

    isAnimating = YES;
  }
}

- (void)stopAnimation {
  // code
  if (isAnimating == YES) {
    [displayLink invalidate];
    displayLink = nil;

    isAnimating = NO;
  }
}

- (void)onSingleTap:(UITapGestureRecognizer *)gr {
  // code
}

- (void)onDoubleTap:(UITapGestureRecognizer *)gr {
  // code
}

- (void)onSwipe:(UISwipeGestureRecognizer *)gr {
  // code
  [self uninitialize];
  [self release];
  exit(0);
}

- (void)onLongPress:(UIGestureRecognizer *)gr {
  // code
}

- (void)uninitialize {
  // code
  if (depthRenderbuffer) {
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    depthRenderbuffer = 0;
  }

  if (colorRenderbuffer) {
    glDeleteRenderbuffers(1, &colorRenderbuffer);
    colorRenderbuffer = 0;
  }

  if (defaultFramebuffer) {
    glDeleteFramebuffers(1, &defaultFramebuffer);
    defaultFramebuffer = 0;
  }

  if (eaglContext) {
    if ([EAGLContext currentContext] == eaglContext) {
      [EAGLContext setCurrentContext:nil];
      [eaglContext release];
      eaglContext = nil;
    }
  }
}

- (void)dealloc {
  // code
  [self uninitialize];
  [super dealloc];
}

@end
