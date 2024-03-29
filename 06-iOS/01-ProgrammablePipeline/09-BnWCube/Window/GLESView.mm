

#import <OpenGLES/ES3/gl.h>
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
  EAGLContext *eaglContext;

  GLuint defaultFramebuffer;
  GLuint colorRenderBuffer;
  GLuint depthRenderBuffer;

  id displayLink;
  NSInteger animationFrameInterval;
  BOOL isAnimating;

  ///

  GLuint gShaderProgramObject;
  GLuint vertexShaderObject;
  GLuint fragmentShaderObject;

  GLuint vao_cube;
  GLuint vbo_position_cube;  // vertex buffer object
  GLuint mvpUniform;
  mat4 perspectiveProjectionMatrix;

  float angleCube;

  ///
}

- (id)initWithFrame:(CGRect)frame {
  // code
  self = [super initWithFrame:frame];

  if (self) {
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)super.layer;

    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = [NSDictionary
        dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO],
                                     kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8,
                                     kEAGLDrawablePropertyColorFormat, nil];

    eaglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (eaglContext == nil) {
      [self release];
      return nil;
    }

    [EAGLContext setCurrentContext:eaglContext];

    glGenBuffers(1, &defaultFramebuffer);
    glGenRenderbuffers(1, &colorRenderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);

    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                              colorRenderBuffer);

    GLint backingWidth;
    GLint backingHeight;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);

    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                              depthRenderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      printf("Failed to create complete Framebuffer object %x/n",
             glCheckFramebufferStatus(GL_FRAMEBUFFER));
      glDeleteRenderbuffers(1, &depthRenderBuffer);
      glDeleteRenderbuffers(1, &colorRenderBuffer);
      glDeleteFramebuffers(1, &defaultFramebuffer);

      return nil;
    }

    printf("Renderer: %s | GL Version: %s | GLSL Version: %s\n", glGetString(GL_RENDERER),
           glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    // hard coded initial values
    isAnimating = NO;
    animationFrameInterval = 60;

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
    const GLchar *fragmentShaderSourceCode =
        (GLchar *)"#version 300 es"
                  "\n"
                  "precision highp float;"
                  "out vec4 FragColor;"
                  "void main (void)"
                  "{"
                  "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
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
    gShaderProgramObject = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject, vertexShaderObject);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject, fragmentShaderObject);

    // pre-linking binding to vertex attribute
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_COLOR, "vColor");

    // link the shader program
    glLinkProgram(gShaderProgramObject);

    // linking errors
    GLint iProgramLinkStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;

    glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
    if (iProgramLinkStatus == GL_FALSE) {
      glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
      if (iInfoLogLength > 0) {
        szInfoLog = (GLchar *)malloc(iInfoLogLength);
        if (szInfoLog != NULL) {
          GLsizei written;
          glGetProgramInfoLog(gShaderProgramObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

          free(szInfoLog);
          [self release];
          exit(0);
        }
      }
    }

    // post-linking retrieving uniform locations
    mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

    // vertex array
    const GLfloat cubeVertices[] = {
        /* Top */
        1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

        /* Bottom */
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,

        /* Front */
        1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,

        /* Back */
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f,

        /* Right */
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,

        /* Left */
        -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f};

    const GLfloat cubeColors[] = {
        /* Top */
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        /* Bottom */
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        /* Front */
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        /* Back */
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

        /* Right */
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

        /* Left */
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f};

    // create vao
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);

    // vertex positions
    glGenBuffers(1, &vbo_position_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //////////////////////////////////////////////////////////////////////

    // clear the depth buffer
    glClearDepthf(1.0f);

    // clear the screen by OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // enable depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = mat4::identity();

    //////////////////////////////////////////////////////////////////

    // GESTURE RECOGNITION
    // tap gesture code
    UITapGestureRecognizer *singleTapGestureRecognizer =
        [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onSingleTap:)];
    [singleTapGestureRecognizer setNumberOfTapsRequired:1];
    [singleTapGestureRecognizer setNumberOfTouchesRequired:1];  // touch of 1 finger
    [singleTapGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:singleTapGestureRecognizer];

    UITapGestureRecognizer *doubleTapGestureRecognizer =
        [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onDoubleTap:)];
    [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
    [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];  // touch of 1 finger
    [doubleTapGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:doubleTapGestureRecognizer];

    // this will allow to differentiate between single tap and double tap
    [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];

    // swipe gesture
    UISwipeGestureRecognizer *swipeGestureRecognizer =
        [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(onSwipe:)];
    [self addGestureRecognizer:swipeGestureRecognizer];

    // long-press gesture recognizer
    UILongPressGestureRecognizer *longPressGestureRecognizer =
        [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(onLongPress:)];
    [self addGestureRecognizer:longPressGestureRecognizer];
  }

  return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

+ (Class)layerClass {
  return ([CAEAGLLayer class]);
}

- (void)drawView:(id)sender {
  // code
  [EAGLContext setCurrentContext:eaglContext];

  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use shader program
  glUseProgram(gShaderProgramObject);

  // declaration of matrices
  mat4 translationMatrix;
  mat4 rotationMatrix;
  mat4 modelViewMatrix;
  mat4 modelViewProjectionMatrix;

  ///// CUBE ///////////////////////////////////////////////////////////////////////////////

  // intialize above matrices to identity
  translationMatrix = mat4::identity();
  rotationMatrix = mat4::identity();
  modelViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // perform necessary transformations
  translationMatrix *= translate(0.0f, 0.0f, -4.0f);
  rotationMatrix *= rotate(angleCube, angleCube, angleCube);
  // rotationMatrix *= rotate(angleCube, 1.0f, 1.0f, 1.0f);

  // do necessary matrix multiplication
  modelViewMatrix *= translationMatrix;
  modelViewMatrix *= rotationMatrix;
  modelViewMatrix *= scale(0.8f, 0.8f, 0.8f);
  modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

  // bind with vao (this will avoid many binding to vbo_vertex)
  glBindVertexArray(vao_cube);

  // bind with textures

  // draw necessary scene
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

  // unbind vao
  glBindVertexArray(0);

  //////////////////////////////////////////////////////////////////////////////////////////

  // unuse program
  glUseProgram(0);
  [self update];

  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
  [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)update {
  if (angleCube >= 360.0) {
    angleCube = 0.0;
  } else {
    angleCube += 1.0f;
  }
}

- (void)layoutSubviews {  // ~reshape
  // code
  GLint width;
  GLint height;

  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
  [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

  glGenRenderbuffers(1, &depthRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                            depthRenderBuffer);

  glViewport(0, 0, width, height);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Failed to create complete Framebuffer object %x/n",
           glCheckFramebufferStatus(GL_FRAMEBUFFER));
  }

  perspectiveProjectionMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

  [self drawView:nil];
}

- (void)startAnimation {
  // code
  if (!isAnimating) {
    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self
                                                                    selector:@selector(drawView:)];
    [displayLink setPreferredFramesPerSecond:animationFrameInterval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

    isAnimating = YES;
  }
}

- (void)stopAnimation {
  // code
  if (isAnimating) {
    [displayLink invalidate];
    displayLink = nil;

    isAnimating = NO;
  }
}

// to become the first responder
- (BOOL)acceptsFirstResponder {
  // code
  return (YES);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
  // code
}

- (void)onSingleTap:(UIGestureRecognizer *)gr {
  // code
}

- (void)onDoubleTap:(UIGestureRecognizer *)gr {
  // code
}

- (void)onSwipe:(UIGestureRecognizer *)gr {
  // code
  [self release];
  exit(0);
}

- (void)onLongPress:(UIGestureRecognizer *)gr {
  // code
}

- (void)dealloc {
  // code
  if (depthRenderBuffer) {
    glDeleteRenderbuffers(1, &depthRenderBuffer);
    depthRenderBuffer = 0;
  }

  if (colorRenderBuffer) {
    glDeleteRenderbuffers(1, &colorRenderBuffer);
    colorRenderBuffer = 0;
  }

  if (defaultFramebuffer) {
    glDeleteFramebuffers(1, &defaultFramebuffer);
    defaultFramebuffer = 0;
  }

  if ([EAGLContext currentContext] == eaglContext) {
    [EAGLContext setCurrentContext:nil];
  }

  [eaglContext release];
  eaglContext = nil;

  [super dealloc];
}

@end
