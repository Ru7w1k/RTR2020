

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

  GLuint vao;         // vertex array object
  GLuint vbo_vertex;  // vertex buffer object
  GLuint vbo_color;   // vertex buffer object

  GLuint vaoAxes;         // vertex array object
  GLuint vbo_vertexAxes;  // vertex buffer object
  GLuint vbo_colorAxes;   // vertex buffer object

  GLuint vaoShapes;
  GLuint vbo_vertexShapes;

  GLuint mvpUniform;
  mat4 perspectiveProjectionMatrix;

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
                                                     "in vec4 vColor;"
                                                     "uniform mat4 u_mvp_matrix;"
                                                     "out vec4 out_Color;"
                                                     "void main (void)"
                                                     "{"
                                                     "    gl_Position = u_mvp_matrix * vPosition;"
                                                     "    out_Color = vColor;"
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
                                                       "in vec4 out_Color;"
                                                       "out vec4 FragColor;"
                                                       "void main (void)"
                                                       "{"
                                                       "    FragColor = out_Color;"
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
    GLfloat *graphCoords = NULL;
    int coords = [self GenerateGraphCoordinates:&graphCoords];

    // color array
    const GLfloat axisCoords[] = {-1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    const GLfloat axisColors[] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    GLfloat *smallAxisColors = (GLfloat *)malloc(coords * 3 * sizeof(GLfloat));
    for (int i = 0; i < (coords * 3); i += 3) {
      smallAxisColors[i + 0] = 0.0f;
      smallAxisColors[i + 1] = 0.0f;
      smallAxisColors[i + 2] = 1.0f;
    }

    GLfloat shapesCoords[1300 * 3];
    int shapesCoordsCount = 0;
    float fX, fY;

    shapesCoordsCount = [self generateOuterCircleCoords:shapesCoords:shapesCoordsCount];

    fX = fY = (GLfloat)cos(M_PI / 4.0);
    shapesCoordsCount = [self generateSquareCoords:fX:fY:shapesCoords:shapesCoordsCount];
    shapesCoordsCount = [self generateTriangleAndIncircleCoords:
                                                             fX:fY:shapesCoords:shapesCoordsCount];

    // create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // vbo position axes
    glGenBuffers(1, &vbo_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
    glBufferData(GL_ARRAY_BUFFER, coords * 3 * sizeof(GLfloat), graphCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    // vbo color axes
    glGenBuffers(1, &vbo_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
    glBufferData(GL_ARRAY_BUFFER, coords * 3 * sizeof(GLfloat), smallAxisColors, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);

    // vao for Major axis
    glGenVertexArrays(1, &vaoAxes);
    glBindVertexArray(vaoAxes);

    // vbo position axes
    glGenBuffers(1, &vbo_vertexAxes);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexAxes);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisCoords), axisCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    // vbo color axes
    glGenBuffers(1, &vbo_colorAxes);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colorAxes);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisColors), axisColors, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);

    // vao for shapes
    glGenVertexArrays(1, &vaoShapes);
    glBindVertexArray(vaoShapes);

    // shapes vertices
    glGenBuffers(1, &vbo_vertexShapes);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexShapes);

    glBufferData(GL_ARRAY_BUFFER, sizeof(shapesCoords), shapesCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    glVertexAttrib3f(RMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);

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

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use shader program
  glUseProgram(gShaderProgramObject);

  // declaration of matrices
  mat4 translationMatrix;
  mat4 modelViewMatrix;
  mat4 modelViewProjectionMatrix;

  // intialize above matrices to identity
  translationMatrix = mat4::identity();
  modelViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // perform necessary transformations
  translationMatrix = translate(0.0f, 0.0f, -2.5f);

  // do necessary matrix multiplication
  modelViewMatrix *= translationMatrix;
  modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

  // bind with vao (this will avoid many binding to vbo_vertex)
  glBindVertexArray(vao);

  // bind with textures

  // draw necessary scene
  glLineWidth(1.0f);
  glDrawArrays(GL_LINES, 0, 160);

  // bind with vao (this will avoid many binding to vbo_vertex)
  glBindVertexArray(vaoAxes);

  // draw necessary scene
  glLineWidth(3.0f);
  glDrawArrays(GL_LINES, 0, 4);

  // shapes
  glBindVertexArray(vaoShapes);

  // draw necessary scene
  glLineWidth(2.0f);
  glDrawArrays(GL_LINE_LOOP, 0, 629);
  glDrawArrays(GL_LINE_LOOP, 629, 4);
  glDrawArrays(GL_LINE_LOOP, 633, 3);
  glDrawArrays(GL_LINE_LOOP, 636, 629);

  // unbind vao
  glBindVertexArray(0);
  //////////////////////////////////////////////////////////////////////////////////////////

  // unuse program
  glUseProgram(0);

  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
  [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
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
  perspectiveProjectionMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Failed to create complete Framebuffer object %x/n",
           glCheckFramebufferStatus(GL_FRAMEBUFFER));
  }

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

- (int)GenerateGraphCoordinates:(GLfloat **)ppos {
  int iNoOfCoords = 0;

  *ppos = (GLfloat *)malloc(3 * sizeof(GLfloat) * 160);

  GLfloat *pos = *ppos;

  for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f)) {
    pos[(iNoOfCoords * 3) + 0] = -1.0f;
    pos[(iNoOfCoords * 3) + 1] = fOffset;
    pos[(iNoOfCoords * 3) + 2] = 0.0f;
    iNoOfCoords++;

    pos[(iNoOfCoords * 3) + 0] = 1.0f;
    pos[(iNoOfCoords * 3) + 1] = fOffset;
    pos[(iNoOfCoords * 3) + 2] = 0.0f;
    iNoOfCoords++;

    pos[(iNoOfCoords * 3) + 0] = -1.0f;
    pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0f + (1.0f / 20.0f);
    pos[(iNoOfCoords * 3) + 2] = 0.0f;
    iNoOfCoords++;

    pos[(iNoOfCoords * 3) + 0] = 1.0f;
    pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0f + (1.0f / 20.0f);
    pos[(iNoOfCoords * 3) + 2] = 0.0f;
    iNoOfCoords++;
  }

  for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f)) {
    pos[(iNoOfCoords * 3) + 0] = fOffset;
    pos[(iNoOfCoords * 3) + 1] = -1.0f;
    pos[(iNoOfCoords * 3) + 2] = 0.0f;
    iNoOfCoords++;

    pos[(iNoOfCoords * 3) + 0] = fOffset;
    pos[(iNoOfCoords * 3) + 1] = 1.0f;
    pos[(iNoOfCoords * 3) + 2] = 0.0f;
    iNoOfCoords++;

    pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0f + (1.0f / 20.0f);
    pos[(iNoOfCoords * 3) + 1] = -1.0f;
    pos[(iNoOfCoords * 3) + 2] = 0.0f;
    iNoOfCoords++;

    pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0f + (1.0f / 20.0f);
    pos[(iNoOfCoords * 3) + 1] = 1.0f;
    pos[(iNoOfCoords * 3) + 2] = 0.0f;
    iNoOfCoords++;
  }

  return iNoOfCoords;
}

- (int)generateTriangleAndIncircleCoords:(float)fY:(float)fX:(GLfloat *)coords:(int)idx {
  // variables
  GLfloat s, a, b, c;
  GLfloat fRadius = 1.0f;
  GLfloat fAngle = 0.0f;

  /* Triangle */
  coords[idx++] = 0.0f;
  coords[idx++] = fY;
  coords[idx++] = 0.0f;

  coords[idx++] = -fX;
  coords[idx++] = -fY;
  coords[idx++] = 0.0f;

  coords[idx++] = fX;
  coords[idx++] = -fY;
  coords[idx++] = 0.0f;

  /* Radius Of Incircle */
  a = (GLfloat)sqrt(pow((-fX - 0.0f), 2.0f) + pow(-fY - fY, 2.0f));
  b = (GLfloat)sqrt(pow((fX - (-fX)), 2.0f) + pow(-fY - (-fY), 2.0f));
  c = (GLfloat)sqrt(pow((fX - 0.0f), 2.0f) + pow(-fY - fY, 2.0f));
  s = (a + b + c) / 2.0f;
  fRadius = (GLfloat)sqrt(s * (s - a) * (s - b) * (s - c)) / s;

  /* Incircle */
  for (fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f) {
    coords[idx++] = fRadius * (GLfloat)cos(fAngle);
    coords[idx++] = (fRadius * (GLfloat)sin(fAngle)) - fX + fRadius;
    coords[idx++] = 0.0f;
  }

  return idx;
}

- (int)generateSquareCoords:(GLfloat)fX:(GLfloat)fY:(GLfloat *)coords:(int)idx {
  coords[idx++] = fX;
  coords[idx++] = fY;
  coords[idx++] = 0.0f;

  coords[idx++] = -fX;
  coords[idx++] = fY;
  coords[idx++] = 0.0f;

  coords[idx++] = -fX;
  coords[idx++] = -fY;
  coords[idx++] = 0.0f;

  coords[idx++] = fX;
  coords[idx++] = -fY;
  coords[idx++] = 0.0f;

  return idx;
}

- (int)generateOuterCircleCoords:(GLfloat *)coords:(int)idx {
  float fRadius = 1.0f;

  for (float fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f) {
    coords[idx++] = fRadius * (GLfloat)cos(fAngle);
    coords[idx++] = fRadius * (GLfloat)sin(fAngle);
    coords[idx++] = 0.0f;
  }

  return idx;
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
