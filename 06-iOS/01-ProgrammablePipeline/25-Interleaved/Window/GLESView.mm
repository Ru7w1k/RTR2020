

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

  GLuint vao;
  GLuint vbo;

  GLuint texture_marble;

  GLuint samplerUniform;

  GLuint mUniform;
  GLuint vUniform;
  GLuint pUniform;

  GLuint laUniform;
  GLuint ldUniform;
  GLuint lsUniform;
  GLuint lightPositionUniform;

  GLuint kaUniform;
  GLuint kdUniform;
  GLuint ksUniform;
  GLuint shininessUniform;

  BOOL bLight;
  GLuint enableLightUniform;

  mat4 perspectiveProjectionMatrix;

  GLfloat angleCube;

  // light settings
  GLfloat lightAmbient[4];
  GLfloat lightDiffuse[4];
  GLfloat lightSpecular[4];
  GLfloat lightPosition[4];

  GLfloat materialAmbient[4];
  GLfloat materialDiffuse[4];
  GLfloat materialSpecular[4];
  GLfloat materialShininess;

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
    const GLchar *vertexShaderSourceCode =
        (GLchar *)"#version 300 es"
                  "\n"
                  "precision lowp int;"
                  "in vec4 vPosition;"
                  "in vec4 vColor;"
                  "in vec3 vNormal;"
                  "in vec2 vTexcoord;"
                  "uniform mat4 u_m_matrix;"
                  "uniform mat4 u_v_matrix;"
                  "uniform mat4 u_p_matrix;"
                  "uniform vec4 u_light_position;"
                  "uniform int u_enable_light;"
                  "out vec3 tnorm;"
                  "out vec3 light_direction;"
                  "out vec3 viewer_vector;"
                  "out vec2 out_Texcoord;"
                  "out vec4 out_Color;"
                  "void main (void)"
                  "{"
                  "   if (u_enable_light == 1) "
                  "   { "
                  "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;"
                  "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;"
                  "       light_direction = vec3(u_light_position - eye_coordinates);"
                  "       float tn_dot_ldir = max(dot(tnorm, light_direction), 0.0);"
                  "       viewer_vector = vec3(-eye_coordinates.xyz);"
                  "    }"
                  "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;"
                  "    out_Texcoord = vTexcoord;"
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
    const GLchar *fragmentShaderSourceCode =
        (GLchar *)"#version 300 es"
                  "\n"
                  "precision highp float;"
                  "precision lowp int;"
                  "in vec2 out_Texcoord;"
                  "in vec4 out_Color;"
                  "in vec3 tnorm;"
                  "in vec3 light_direction;"
                  "in vec3 viewer_vector;"
                  "uniform vec3 u_la;"
                  "uniform vec3 u_ld;"
                  "uniform vec3 u_ls;"
                  "uniform vec3 u_ka;"
                  "uniform vec3 u_kd;"
                  "uniform vec3 u_ks;"
                  "uniform float u_shininess;"
                  "uniform int u_enable_light;"
                  "uniform sampler2D u_sampler;"
                  "out vec4 FragColor;"
                  "void main (void)"
                  "{"
                  "    vec3 phong_ads_light = vec3(1.0);"
                  "   if (u_enable_light == 1) "
                  "   { "
                  "       vec3 ntnorm = normalize(tnorm);"
                  "       vec3 nlight_direction = normalize(light_direction);"
                  "       vec3 nviewer_vector = normalize(viewer_vector);"
                  "       vec3 reflection_vector = reflect(-nlight_direction, ntnorm);"
                  "       float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);"
                  "       vec3 ambient  = u_la * u_ka;"
                  "       vec3 diffuse  = u_ld * u_kd * tn_dot_ldir;"
                  "       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, "
                  "nviewer_vector), 0.0), u_shininess);"
                  "       phong_ads_light = ambient + diffuse + specular;"
                  "    }"
                  "    vec4 tex = texture(u_sampler, out_Texcoord);"
                  "    FragColor = vec4((vec3(tex) * vec3(out_Color) * phong_ads_light), 1.0);"
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

          printf("\n%s\n", szInfoLog);
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
    glBindAttribLocation(shaderProgramObject, RMC_ATTRIBUTE_COLOR, "vColor");
    glBindAttribLocation(shaderProgramObject, RMC_ATTRIBUTE_NORMAL, "vNormal");
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
          printf("\n%s\n", szInfoLog);
          free(szInfoLog);
          [self release];
          exit(0);
        }
      }
    }

    // post-linking retrieving uniform locations
    mUniform = glGetUniformLocation(shaderProgramObject, "u_m_matrix");
    vUniform = glGetUniformLocation(shaderProgramObject, "u_v_matrix");
    pUniform = glGetUniformLocation(shaderProgramObject, "u_p_matrix");

    samplerUniform = glGetUniformLocation(shaderProgramObject, "u_sampler");

    laUniform = glGetUniformLocation(shaderProgramObject, "u_la");
    kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
    ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
    kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
    lsUniform = glGetUniformLocation(shaderProgramObject, "u_ls");
    ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
    shininessUniform = glGetUniformLocation(shaderProgramObject, "u_shininess");

    enableLightUniform = glGetUniformLocation(shaderProgramObject, "u_enable_light");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");

    // vertex array
    const GLfloat cubeData[] = {
        /* Top */
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

        /* Bottom */
        1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f,

        /* Front */
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        /* Back */
        1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        /* Right */
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        /* Left */
        -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f};

    // create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // vertex position
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void *)(0 * sizeof(float)));
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    // vertex colors
    glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);

    // vertex normals
    glVertexAttribPointer(RMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(RMC_ATTRIBUTE_NORMAL);

    // vertex texcoords
    glVertexAttribPointer(RMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void *)(9 * sizeof(float)));
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
    angleCube = 0.0f;

    // light settings
    lightAmbient[0] = 0.5f;
    lightAmbient[1] = 0.5f;
    lightAmbient[2] = 0.5f;
    lightAmbient[3] = 1.0f;

    lightDiffuse[0] = 1.0f;
    lightDiffuse[1] = 1.0f;
    lightDiffuse[2] = 1.0f;
    lightDiffuse[3] = 1.0f;

    lightSpecular[0] = 1.0f;
    lightSpecular[1] = 1.0f;
    lightSpecular[2] = 1.0f;
    lightSpecular[3] = 1.0f;

    lightPosition[0] = 100.0f;
    lightPosition[1] = 100.0f;
    lightPosition[2] = 100.0f;
    lightPosition[3] = 1.0f;

    materialAmbient[0] = 0.5f;
    materialAmbient[1] = 0.5f;
    materialAmbient[2] = 0.5f;
    materialAmbient[3] = 1.0f;

    materialDiffuse[0] = 1.0f;
    materialDiffuse[1] = 1.0f;
    materialDiffuse[2] = 1.0f;
    materialDiffuse[3] = 1.0f;

    materialSpecular[0] = 1.0f;
    materialSpecular[1] = 1.0f;
    materialSpecular[2] = 1.0f;
    materialSpecular[3] = 1.0f;

    materialShininess = 128.0f;

    // texture
    glEnable(GL_TEXTURE_2D);
    texture_marble = [self loadTextureFromBMPFile:@"marble":@"bmp"];

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

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // use shader program
  glUseProgram(shaderProgramObject);

  // declaration of matrices
  mat4 translationMatrix;
  mat4 modelMatrix;
  mat4 ViewMatrix;
  mat4 modelViewProjectionMatrix;

  // intialize above matrices to identity
  translationMatrix = mat4::identity();
  modelMatrix = mat4::identity();
  ViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // perform necessary transformations
  translationMatrix *= translate(0.0f, 0.0f, -6.0f);

  // do necessary matrix multiplication
  modelMatrix *= translationMatrix;
  modelMatrix *= rotate(angleCube, angleCube, angleCube);

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mUniform, 1, false, modelMatrix);
  glUniformMatrix4fv(vUniform, 1, false, ViewMatrix);
  glUniformMatrix4fv(pUniform, 1, false, perspectiveProjectionMatrix);

  glUniform3fv(laUniform, 1, lightAmbient);
  glUniform3fv(ldUniform, 1, lightDiffuse);
  glUniform3fv(lsUniform, 1, lightSpecular);
  glUniform4fv(lightPositionUniform, 1, lightPosition);

  glUniform3fv(kaUniform, 1, materialAmbient);
  glUniform3fv(kdUniform, 1, materialDiffuse);
  glUniform3fv(ksUniform, 1, materialSpecular);
  glUniform1f(shininessUniform, materialShininess);

  if (bLight == YES)
    glUniform1i(enableLightUniform, 1);
  else
    glUniform1i(enableLightUniform, 0);

  // bind with textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_marble);
  glUniform1i(samplerUniform, 0);

  // bind with vao (this will avoid many binding to vbo)
  glBindVertexArray(vao);

  // draw necessary scene
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

  // unbind vao
  glBindVertexArray(0);

  // unuse program
  glUseProgram(0);

  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
  [eaglContext presentRenderbuffer:GL_RENDERBUFFER];

  [self update];
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
  bLight = !bLight;
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

  if (vbo) {
    glDeleteBuffers(1, &vbo);
    vbo = 0;
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
