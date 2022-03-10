

#import <OpenGLES/ES3/gl.h>  // OpenGLES 3.0
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"
using namespace vmath;

enum {
  RMC_ATTRIBUTE_POSITION = 0,
  RMC_ATTRIBUTE_COLOR,
  RMC_ATTRIBUTE_NORMAL,
  RMC_ATTRIBUTE_TEXCOORD,

};

@implementation GLESView {
 @private
  EAGLContext *eaglContext;

  GLuint defaultFramebuffer;
  GLuint colorRenderbuffer;
  GLuint depthRenderbuffer;

  id displayLink;
  NSInteger animationFrameInterval;
  BOOL isAnimating;

  ///

  GLuint vertexShaderObject;
  GLuint fragmentShaderObject;
  GLuint shaderProgramObject;

  GLuint vao;
  GLuint vbo;
  GLuint mvpUniform;

  vmath::mat4 orthographicProjectionMatrix;

  ///
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

    //////////////////////////////////////////////////////////////////

    // create vertex shader object
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    // vertex shader source code
    const GLchar *vertexShaderSourceCode = (GLchar *)"#version 300 es"
                                                     "\n"
                                                     "in vec4 vPosition;"
                                                     "uniform mat4 u_mvp_matrix;"
                                                     "void main (void)"
                                                     "{"
                                                     "   gl_Position = u_mvp_matrix * vPosition;"
                                                     "}";

    // attach source code to vertex shader
    glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

    // compile vertex shader source code
    glCompileShader(vertexShaderObject);

    // compilation errors
    GLint iShaderCompileStatus = 0;
    GLint iInfoLogLength = 0;
    GLchar *szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE) {
      glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
      if (iInfoLogLength > 0) {
        szInfoLog = (GLchar *)malloc(iInfoLogLength);
        if (szInfoLog != NULL) {
          GLsizei written;
          glGetShaderInfoLog(vertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

          free(szInfoLog);
          [self release];
          exit(0);
        }
      }
    }

    // create fragment shader object
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // fragment shader source code
    const GLchar *fragmentShaderSourceCode = (GLchar *)"#version 300 es"
                                                       "\n"
                                                       "precision highp float;"
                                                       "out vec4 FragColor;"
                                                       "void main (void)"
                                                       "{"
                                                       "   FragColor = vec4(1.0, 1.0, 1.0, 1.0);"
                                                       "}";

    // attach source code to fragment shader
    glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

    // compile fragment shader source code
    glCompileShader(fragmentShaderObject);

    // compile errors
    iShaderCompileStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE) {
      glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
      if (iInfoLogLength > 0) {
        szInfoLog = (GLchar *)malloc(iInfoLogLength);
        if (szInfoLog != NULL) {
          GLsizei written;
          glGetShaderInfoLog(fragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

          free(szInfoLog);
          [self release];
          exit(0);
        }
      }
    }

    // create shader program object
    shaderProgramObject = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(shaderProgramObject, vertexShaderObject);

    // attach fragment shader to shader program
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    // pre-linking binding to vertex attribute
    glBindAttribLocation(shaderProgramObject, RMC_ATTRIBUTE_POSITION, "vPosition");

    // link the shader program
    glLinkProgram(shaderProgramObject);

    // linking errors
    GLint iProgramLinkStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;

    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
    if (iProgramLinkStatus == GL_FALSE) {
      glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
      if (iInfoLogLength > 0) {
        szInfoLog = (GLchar *)malloc(iInfoLogLength);
        if (szInfoLog != NULL) {
          GLsizei written;
          glGetProgramInfoLog(shaderProgramObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

          free(szInfoLog);
          [self release];
          exit(0);
        }
      }
    }

    // post-linking retrieving uniform locations
    mvpUniform = glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");

    // vertex array
    const GLfloat triangleVertices[] = {
        0.0f,   50.0f,  0.0f,  //
        -50.0f, -50.0f, 0.0f,  //
        50.0f,  -50.0f, 0.0f   //
    };

    // create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //////////////////////////////////////////////////////////////////////

    // clear the depth buffer
    glClearDepthf(1.0f);

    // clear the screen by OpenGL
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

    // enable depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    orthographicProjectionMatrix = mat4::identity();

    //////////////////////////////////////////////////////////////////

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

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
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

  if (width < height) {
    orthographicProjectionMatrix = ortho(-100.0f, 100.0f, -100.0f * (height / width),
                                         100.0f * (height / width), -100.0f, 100.0f);
  } else {
    orthographicProjectionMatrix = ortho(-100.0f * (width / height), 100.0f * (width / height),
                                         -100.0f, 100.0f, -100.0f, 100.0f);
  }

  [self drawView:nil];
}

- (void)drawView:(id)sender {
  // code
  [EAGLContext setCurrentContext:eaglContext];
  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // use shader program
  glUseProgram(shaderProgramObject);

  // declaration of matrices
  mat4 modelViewMatrix;
  mat4 modelViewProjectionMatrix;

  // intialize above matrices to identity
  modelViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // do necessary matrix multiplication
  modelViewProjectionMatrix = orthographicProjectionMatrix * modelViewMatrix;

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

  // bind with vao (this will avoid many binding to vbo)
  glBindVertexArray(vao);

  // bind with textures

  // draw necessary scene
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // unbind vao
  glBindVertexArray(0);

  // unuse program
  glUseProgram(0);

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
  if (vao) {
    glDeleteVertexArrays(1, &vao);
    vao = 0;
  }

  if (vbo) {
    glDeleteBuffers(1, &vbo);
    vbo = 0;
  }

  // destroy shader programs
  if (shaderProgramObject) {
    GLsizei shaderCount;
    GLsizei i;

    glUseProgram(shaderProgramObject);
    glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

    GLuint *pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));
    if (pShaders) {
      glGetAttachedShaders(shaderProgramObject, shaderCount, &shaderCount, pShaders);

      for (i = 0; i < shaderCount; i++) {
        // detach shader
        glDetachShader(shaderProgramObject, pShaders[i]);

        // delete shader
        glDeleteShader(pShaders[i]);
        pShaders[i] = 0;
      }

      free(pShaders);
    }

    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;
    glUseProgram(0);
  }

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
