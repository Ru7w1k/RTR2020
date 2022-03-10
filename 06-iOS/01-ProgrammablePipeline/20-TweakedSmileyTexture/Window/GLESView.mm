

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

  GLuint vertexShaderObject;
  GLuint fragmentShaderObject;
  GLuint shaderProgramObject;

  GLuint vao;           // vertex array object
  GLuint vbo_position;  // vertex buffer object
  GLuint vbo_texture;
  GLuint texture_smiley;
  GLuint samplerUniform;
  GLuint mvpUniform;
  mat4 perspectiveProjectionMatrix;

  GLuint texture_white;
  GLubyte whiteImage[64][64][4];

  int pressedDigit;

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
                                                     "in vec2 vTexcoord;"
                                                     "uniform mat4 u_mvp_matrix;"
                                                     "out vec2 out_Texcoord;"
                                                     "void main (void)"
                                                     "{"
                                                     "    gl_Position = u_mvp_matrix * vPosition;"
                                                     "    out_Texcoord = vTexcoord;"
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
                  "in vec2 out_Texcoord;"
                  "uniform sampler2D u_sampler;"
                  "out vec4 FragColor;"
                  "void main (void)"
                  "{"
                  "    FragColor = texture(u_sampler, out_Texcoord);"
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
    glBindAttribLocation(shaderProgramObject, RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
    samplerUniform = glGetUniformLocation(shaderProgramObject, "u_sampler");

    // vertex array
    const GLfloat rectangleVertices[] = {1.0f,  1.0f,  0.0f, -1.0f, 1.0f,  0.0f,
                                         -1.0f, -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f};

    const GLfloat rectangleTexcoords[] = {1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

    // create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    glGenBuffers(1, &vbo_texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleTexcoords), rectangleTexcoords, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_TEXCOORD);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //////////////////////////////////////////////////////////////////////

    // clear the depth buffer
    glClearDepthf(1.0f);

    // clear the screen by OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // enable depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = mat4::identity();

    // texture
    glEnable(GL_TEXTURE_2D);
    texture_smiley = [self loadTextureFromBMPFile:@"smiley":@"bmp"];
    texture_white = [self loadWhiteTexture];

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

- (GLuint)loadWhiteTexture {
  GLuint tex;
  int i, j, c;

  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

      whiteImage[i][j][0] = (GLubyte)255;
      whiteImage[i][j][1] = (GLubyte)255;
      whiteImage[i][j][2] = (GLubyte)255;
      whiteImage[i][j][3] = (GLubyte)255;
    }
  }

  glGenTextures(1, &tex);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteImage);
  return tex;
}

- (GLuint)loadTextureFromBMPFile:(NSString *)texFileName:(NSString *)extension {
  NSString *textureFileNameWithPath = [[NSBundle mainBundle] pathForResource:texFileName
                                                                      ofType:extension];

  UIImage *bmpImage = [[UIImage alloc] initWithContentsOfFile:textureFileNameWithPath];
  if (!bmpImage) {
    NSLog(@"Can't find %@", textureFileNameWithPath);
    return (0);
  }

  CGImageRef cgImage = bmpImage.CGImage;

  int w = (int)CGImageGetWidth(cgImage);
  int h = (int)CGImageGetHeight(cgImage);
  CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
  void *pixels = (void *)CFDataGetBytePtr(imageData);

  GLuint bmpTexture;
  glGenTextures(1, &bmpTexture);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // set 1 rather that default 4, for better performance
  glBindTexture(GL_TEXTURE_2D, bmpTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  // create mipmaps for this texture for better image quality
  glGenerateMipmap(GL_TEXTURE_2D);

  CFRelease(imageData);
  return (bmpTexture);
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
  glUseProgram(shaderProgramObject);

  // declaration of matrices
  mat4 translationMatrix;
  mat4 modelViewMatrix;
  mat4 modelViewProjectionMatrix;

  // intialize above matrices to identity
  translationMatrix = mat4::identity();
  modelViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // perform necessary transformations
  translationMatrix = translate(0.0f, 0.0f, -3.0f);

  modelViewMatrix *= translationMatrix;

  // do necessary matrix multiplication
  modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

  // bind with textures
  if (pressedDigit != 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_smiley);
    glUniform1i(samplerUniform, 0);
  } else {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_white);
    glUniform1i(samplerUniform, 0);
  }

  // bind with vao (this will avoid many binding to vbo)
  glBindVertexArray(vao);

  GLfloat rectTexcoords[8] = {1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0};

  if (pressedDigit == 1) {
    rectTexcoords[0] = 0.5f;
    rectTexcoords[1] = 0.5f;
    rectTexcoords[2] = 0.0f;
    rectTexcoords[3] = 0.5f;
    rectTexcoords[4] = 0.0f;
    rectTexcoords[5] = 0.0f;
    rectTexcoords[6] = 0.5f;
    rectTexcoords[7] = 0.0f;
  } else if (pressedDigit == 2) {
    rectTexcoords[0] = 1.0f;
    rectTexcoords[1] = 1.0f;
    rectTexcoords[2] = 0.0f;
    rectTexcoords[3] = 1.0f;
    rectTexcoords[4] = 0.0f;
    rectTexcoords[5] = 0.0f;
    rectTexcoords[6] = 1.0f;
    rectTexcoords[7] = 0.0f;
  } else if (pressedDigit == 3) {
    rectTexcoords[0] = 2.0f;
    rectTexcoords[1] = 2.0f;
    rectTexcoords[2] = 0.0f;
    rectTexcoords[3] = 2.0f;
    rectTexcoords[4] = 0.0f;
    rectTexcoords[5] = 0.0f;
    rectTexcoords[6] = 2.0f;
    rectTexcoords[7] = 0.0f;
  } else if (pressedDigit == 4) {
    rectTexcoords[0] = 0.5f;
    rectTexcoords[1] = 0.5f;
    rectTexcoords[2] = 0.5f;
    rectTexcoords[3] = 0.5f;
    rectTexcoords[4] = 0.5f;
    rectTexcoords[5] = 0.5f;
    rectTexcoords[6] = 0.5f;
    rectTexcoords[7] = 0.5f;
  }

  if (pressedDigit != 0) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectTexcoords), rectTexcoords, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  // draw necessary scene
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  // unbind vao
  glBindVertexArray(0);

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
  pressedDigit++;

  if (pressedDigit > 4) {
    pressedDigit = 0;
  }
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
  if (vbo_position) {
    glDeleteBuffers(1, &vbo_position);
    vbo_position = 0;
  }

  if (vbo_texture) {
    glDeleteBuffers(1, &vbo_texture);
    vbo_texture = 0;
  }

  if (vao) {
    glDeleteVertexArrays(1, &vao);
    vao = 0;
  }

  if (shaderProgramObject) {
    GLsizei shaderCount;
    GLsizei shaderNumber;

    glUseProgram(shaderProgramObject);
    glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

    GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
    if (pShaders) {
      glGetAttachedShaders(shaderProgramObject, shaderCount, &shaderCount, pShaders);

      for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
        // detach shader
        glDetachShader(shaderProgramObject, pShaders[shaderNumber]);

        // delete shader
        glDeleteShader(pShaders[shaderNumber]);
        pShaders[shaderNumber] = 0;
      }
      free(pShaders);
    }

    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;
    glUseProgram(0);
  }

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
