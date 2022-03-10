

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"

using namespace vmath;

enum {
  RMC_ATTRIBUTE_POSITION = 0,
  RMC_ATTRIBUTE_COLOR,
  RMC_ATTRIBUTE_NORMAL,
  RMC_ATTRIBUTE_TEXTURE0,

};

// light settings
typedef struct _Light {
  GLfloat lightAmbient[4];
  GLfloat lightDiffuse[4];
  GLfloat lightSpecular[4];
  GLfloat lightPosition[4];
} Light;

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

  Light lights[2];

  GLfloat materialAmbient[4];
  GLfloat materialDiffuse[4];
  GLfloat materialSpecular[4];
  GLfloat materialShininess;

  GLuint vao_pyramid;           // vertex array object
  GLuint vbo_position_pyramid;  // vertex buffer object
  GLuint vbo_normal_pyramid;    // vertex buffer object

  GLuint mUniform;
  GLuint vUniform;
  GLuint pUniform;

  GLuint laUniform_red;
  GLuint ldUniform_red;
  GLuint lsUniform_red;
  GLuint lightPositionUniform_red;

  GLuint laUniform_blue;
  GLuint ldUniform_blue;
  GLuint lsUniform_blue;
  GLuint lightPositionUniform_blue;

  GLuint kaUniform;
  GLuint kdUniform;
  GLuint ksUniform;
  GLuint shininessUniform;

  GLuint enableLightUniform;

  mat4 perspectiveProjectionMatrix;
  BOOL bLight;
  GLfloat anglePyramid;

  ///
}

- (id)initWithFrame:(CGRect)frame {
  // code
  self = [super initWithFrame:frame];

  bLight = false;

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
                  "precision highp float;"
                  "in vec4 vPosition;"
                  "in vec3 vNormal;"
                  "uniform mat4 u_m_matrix;"
                  "uniform mat4 u_v_matrix;"
                  "uniform mat4 u_p_matrix;"
                  "uniform vec4 u_light_position_red;"
                  "uniform vec4 u_light_position_blue;"
                  "uniform int u_enable_light;"
                  "out vec3 tnorm;"
                  "out vec3 viewer_vector;"
                  "out vec3 light_direction_red;"
                  "out vec3 light_direction_blue;"
                  "void main(void)"
                  "{"
                  "   if (u_enable_light == 1) "
                  "   { "
                  "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;"
                  "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;"
                  "       viewer_vector = vec3(-eye_coordinates.xyz);"
                  "       light_direction_red   = vec3(u_light_position_red - eye_coordinates);"
                  "       light_direction_blue  = vec3(u_light_position_blue - eye_coordinates);"
                  "    }"
                  "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;"
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
                  "in vec3 tnorm;"
                  "in vec3 light_direction_red;"
                  "in vec3 light_direction_blue;"
                  "in vec3 viewer_vector;"
                  "uniform vec3 u_la_red;"
                  "uniform vec3 u_ld_red;"
                  "uniform vec3 u_ls_red;"
                  "uniform vec3 u_la_blue;"
                  "uniform vec3 u_ld_blue;"
                  "uniform vec3 u_ls_blue;"
                  "uniform vec3 u_ka;"
                  "uniform vec3 u_kd;"
                  "uniform vec3 u_ks;"
                  "uniform float u_shininess;"
                  "uniform int u_enable_light;"
                  "out vec4 FragColor;"
                  "void main(void)"
                  "{"
                  "   if (u_enable_light == 1) "
                  "   { "
                  "       vec3 ntnorm = normalize(tnorm);"
                  "       vec3 nviewer_vector = normalize(viewer_vector);"
                  "                                                                                "
                  "                                            "
                  "       vec3 nlight_direction_red = normalize(light_direction_red);"
                  "       vec3 reflection_vector_red = reflect(-nlight_direction_red, ntnorm);"
                  "       float tn_dot_ldir_red = max(dot(ntnorm, nlight_direction_red), 0.0);"
                  "       vec3 ambient_red  = u_la_red * u_ka;"
                  "       vec3 diffuse_red  = u_ld_red * u_kd * tn_dot_ldir_red;"
                  "       vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red, "
                  "nviewer_vector), 0.0), u_shininess);"
                  "                                                                                "
                  "                                            "
                  "       vec3 nlight_direction_blue = normalize(light_direction_blue);"
                  "       vec3 reflection_vector_blue = reflect(-nlight_direction_blue, ntnorm);"
                  "       float tn_dot_ldir_blue = max(dot(ntnorm, nlight_direction_blue), 0.0);"
                  "       vec3 ambient_blue  = u_la_blue * u_ka;"
                  "       vec3 diffuse_blue  = u_ld_blue * u_kd * tn_dot_ldir_blue;"
                  "       vec3 specular_blue = u_ls_blue * u_ks * "
                  "pow(max(dot(reflection_vector_blue, nviewer_vector), 0.0), u_shininess);"
                  "                                                                                "
                  "                                            "
                  "       vec3 phong_ads_light = ambient_red + diffuse_red + specular_red + "
                  "ambient_blue + diffuse_blue + specular_blue;"
                  "       FragColor = vec4(phong_ads_light, 1.0);"
                  "    }"
                  "   else"
                  "   {"
                  "        FragColor = vec4(1.0);"
                  "   }"
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
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_NORMAL, "vNormal");

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
    mUniform = glGetUniformLocation(gShaderProgramObject, "u_m_matrix");
    vUniform = glGetUniformLocation(gShaderProgramObject, "u_v_matrix");
    pUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");

    laUniform_red = glGetUniformLocation(gShaderProgramObject, "u_la_red");
    ldUniform_red = glGetUniformLocation(gShaderProgramObject, "u_ld_red");
    lsUniform_red = glGetUniformLocation(gShaderProgramObject, "u_ls_red");
    lightPositionUniform_red = glGetUniformLocation(gShaderProgramObject, "u_light_position_red");

    laUniform_blue = glGetUniformLocation(gShaderProgramObject, "u_la_blue");
    ldUniform_blue = glGetUniformLocation(gShaderProgramObject, "u_ld_blue");
    lsUniform_blue = glGetUniformLocation(gShaderProgramObject, "u_ls_blue");
    lightPositionUniform_blue = glGetUniformLocation(gShaderProgramObject, "u_light_position_blue");

    kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_shininess");

    enableLightUniform = glGetUniformLocation(gShaderProgramObject, "u_enable_light");

    const GLfloat pyramideVertices[] = {
        /* Front */
        0.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,

        /* Right */
        0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,

        /* Left */
        0.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,

        /* Back */
        0.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};

    const GLfloat pyramideNormals[] = {
        /* Front */
        0.0f, 0.447214f, 0.894427f, 0.0f, 0.447214f, 0.894427f, 0.0f, 0.447214f, 0.894427f,

        /* Right */
        0.894427f, 0.447214f, 0.0f, 0.894427f, 0.447214f, 0.0f, 0.894427f, 0.447214f, 0.0f,

        /* Left */
        -0.894427f, 0.447214f, 0.0f, -0.894427f, 0.447214f, 0.0f, -0.894427f, 0.447214f, 0.0f,

        /* Back */
        0.0f, 0.447214f, -0.894427f, 0.0f, 0.447214f, -0.894427f, 0.0f, 0.447214f, -0.894427f};

    // create vao
    glGenVertexArrays(1, &vao_pyramid);
    glBindVertexArray(vao_pyramid);

    // vertex positions
    glGenBuffers(1, &vbo_position_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramideVertices), pyramideVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    // vertex normals
    glGenBuffers(1, &vbo_normal_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_pyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramideNormals), pyramideNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //////////////////////////////////////////////////////////////////////

    // light configurations
    // light red
    lights[0].lightAmbient[0] = 0.0f;
    lights[0].lightAmbient[1] = 0.0f;
    lights[0].lightAmbient[2] = 0.0f;
    lights[0].lightAmbient[3] = 1.0f;

    lights[0].lightDiffuse[0] = 1.0f;
    lights[0].lightDiffuse[1] = 0.0f;
    lights[0].lightDiffuse[2] = 0.0f;
    lights[0].lightDiffuse[3] = 1.0f;

    lights[0].lightSpecular[0] = 1.0f;
    lights[0].lightSpecular[1] = 0.0f;
    lights[0].lightSpecular[2] = 0.0f;
    lights[0].lightSpecular[3] = 1.0f;

    lights[0].lightPosition[0] = -2.0f;
    lights[0].lightPosition[1] = 0.0f;
    lights[0].lightPosition[2] = 0.0f;
    lights[0].lightPosition[3] = 1.0f;

    // light blue
    lights[1].lightAmbient[0] = 0.0f;
    lights[1].lightAmbient[1] = 0.0f;
    lights[1].lightAmbient[2] = 0.0f;
    lights[1].lightAmbient[3] = 1.0f;

    lights[1].lightDiffuse[0] = 0.0f;
    lights[1].lightDiffuse[1] = 0.0f;
    lights[1].lightDiffuse[2] = 1.0f;
    lights[1].lightDiffuse[3] = 1.0f;

    lights[1].lightSpecular[0] = 0.0f;
    lights[1].lightSpecular[1] = 0.0f;
    lights[1].lightSpecular[2] = 1.0f;
    lights[1].lightSpecular[3] = 1.0f;

    lights[1].lightPosition[0] = 2.0f;
    lights[1].lightPosition[1] = 0.0f;
    lights[1].lightPosition[2] = 0.0f;
    lights[1].lightPosition[3] = 1.0f;

    // material configuration
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 0.0f;

    materialDiffuse[0] = 1.0f;
    materialDiffuse[1] = 1.0f;
    materialDiffuse[2] = 1.0f;
    materialDiffuse[3] = 1.0f;

    materialSpecular[0] = 1.0f;
    materialSpecular[1] = 1.0f;
    materialSpecular[2] = 1.0f;
    materialSpecular[3] = 1.0f;

    materialShininess = 50.0;

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
  mat4 modelMatrix;
  mat4 ViewMatrix;

  ///// PYRAMID ///////////////////////////////////////////////////////////////////////////////

  // intialize above matrices to identity
  translationMatrix = mat4::identity();
  rotationMatrix = mat4::identity();
  modelMatrix = mat4::identity();
  ViewMatrix = mat4::identity();

  // perform necessary transformations
  translationMatrix *= translate(0.0f, 0.0f, -4.0f);
  rotationMatrix *= rotate(0.0f, anglePyramid, 0.0f);

  // do necessary matrix multiplication
  modelMatrix *= translationMatrix;
  modelMatrix *= rotationMatrix;

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mUniform, 1, GL_FALSE, modelMatrix);
  glUniformMatrix4fv(vUniform, 1, GL_FALSE, ViewMatrix);
  glUniformMatrix4fv(pUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

  glUniform3fv(laUniform_red, 1, lights[0].lightAmbient);
  glUniform3fv(ldUniform_red, 1, lights[0].lightDiffuse);
  glUniform3fv(lsUniform_red, 1, lights[0].lightSpecular);
  glUniform4fv(lightPositionUniform_red, 1, lights[0].lightPosition);

  glUniform3fv(laUniform_blue, 1, lights[1].lightAmbient);
  glUniform3fv(ldUniform_blue, 1, lights[1].lightDiffuse);
  glUniform3fv(lsUniform_blue, 1, lights[1].lightSpecular);
  glUniform4fv(lightPositionUniform_blue, 1, lights[1].lightPosition);

  glUniform3fv(kaUniform, 1, materialAmbient);
  glUniform3fv(kdUniform, 1, materialDiffuse);
  glUniform3fv(ksUniform, 1, materialSpecular);
  glUniform1f(shininessUniform, materialShininess);

  if (bLight == TRUE)
    glUniform1i(enableLightUniform, 1);
  else
    glUniform1i(enableLightUniform, 0);

  // bind with vao (this will avoid many binding to vbo_vertex)
  glBindVertexArray(vao_pyramid);

  glDrawArrays(GL_TRIANGLES, 0, 12);

  // unbind vao
  glBindVertexArray(0);

  // unuse program
  glUseProgram(0);
  [self update];

  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
  [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)update {
  if (anglePyramid >= 360.0) {
    anglePyramid = 0.0;
  } else {
    anglePyramid += 1.0f;
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
  if (vbo_position_pyramid) {
    glDeleteBuffers(1, &vbo_position_pyramid);
    vbo_position_pyramid = 0;
  }

  if (vbo_normal_pyramid) {
    glDeleteBuffers(1, &vbo_normal_pyramid);
    vbo_normal_pyramid = 0;
  }

  if (vao_pyramid) {
    glDeleteVertexArrays(1, &vao_pyramid);
    vao_pyramid = 0;
  }

  if (gShaderProgramObject) {
    GLsizei shaderCount;
    GLsizei shaderNumber;

    glUseProgram(gShaderProgramObject);
    glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

    GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
    if (pShaders) {
      glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

      for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
        // detach shader
        glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);

        // delete shader
        glDeleteShader(pShaders[shaderNumber]);
        pShaders[shaderNumber] = 0;
      }
      free(pShaders);
    }

    glDeleteProgram(gShaderProgramObject);
    gShaderProgramObject = 0;
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
