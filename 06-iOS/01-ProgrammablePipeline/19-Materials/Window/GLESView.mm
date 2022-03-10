

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "materials.h"
#import "vmath.h"

using namespace vmath;

enum {
  RMC_ATTRIBUTE_POSITION = 0,
  RMC_ATTRIBUTE_COLOR,
  RMC_ATTRIBUTE_NORMAL,
  RMC_ATTRIBUTE_TEXCOORD,

};

// types
typedef struct _Light {
  GLfloat lightAmbient[4];
  GLfloat lightDiffuse[4];
  GLfloat lightSpecular[4];
  GLfloat lightPosition[4];
  GLfloat angle;
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

  GLuint vao_sphere;           // vertex array object
  GLuint vbo_position_sphere;  // vertex buffer object
  GLuint vbo_normal_sphere;    // vertex buffer object

  Light lights[3];

  GLuint gShaderProgramObject;
  GLuint vertexShaderObject;
  GLuint fragmentShaderObject;

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

  GLuint enableLightUniform;

  mat4 perspectiveProjectionMatrix;
  BOOL bLight;

  bool bXLight;
  bool bYLight;
  bool bZLight;

  float gWidth, gHeight;

  int coords;

  ///
}

- (id)initWithFrame:(CGRect)frame {
  bLight = NO;

  bXLight = true;
  bYLight = true;
  bZLight = true;

  InitMaterials();

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

    //////////// P E R   F R A G M E N T ////////////////////////////////////////

    // create vertex shader object
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    // vertex shader source code
    const GLchar *vertexShaderSourceCode =
        (GLchar *)"#version 300 es"
                  "\n"
                  "precision highp float;"
                  "precision lowp int;"
                  "in vec4 vPosition;"
                  "in vec3 vNormal;"
                  "uniform mat4 u_m_matrix;"
                  "uniform mat4 u_v_matrix;"
                  "uniform mat4 u_p_matrix;"
                  "uniform vec4 u_light_position;"
                  "uniform int u_enable_light;"
                  "out vec3 tnorm;"
                  "out vec3 viewer_vector;"
                  "out vec3 light_direction;"
                  "void main(void)"
                  "{"
                  "   if (u_enable_light == 1) "
                  "   { "
                  "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;"
                  "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;"
                  "       viewer_vector = vec3(-eye_coordinates.xyz);"
                  "       light_direction   = vec3(u_light_position - eye_coordinates);"
                  "    }"
                  "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;"
                  "}";

    // attach source code to vertex shader
    glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

    // compile vertex shader source code
    glCompileShader(vertexShaderObject);

    // compilation errors
    int iShaderCompileStatus = 0;
    int iInfoLogLength = 0;
    char *szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE) {
      glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
      if (iInfoLogLength > 0) {
        szInfoLog = (GLchar *)malloc(iInfoLogLength);
        if (szInfoLog != NULL) {
          GLsizei written;
          glGetShaderInfoLog(vertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

          printf("Vertex Shader Compiler Info Log: %s", szInfoLog);
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
                  "out vec4 FragColor;"
                  "void main(void)"
                  "{"
                  "   if (u_enable_light == 1) "
                  "   { "
                  "       vec3 ntnorm = normalize(tnorm);"
                  "       vec3 nviewer_vector = normalize(viewer_vector);"
                  "                                                                                "
                  "                                            "
                  "       vec3 nlight_direction = normalize(light_direction);"
                  "       vec3 reflection_vector = reflect(-nlight_direction, ntnorm);"
                  "       float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);"
                  "       vec3 ambient  = u_la * u_ka;"
                  "       vec3 diffuse  = u_ld * u_kd * tn_dot_ldir;"
                  "       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, "
                  "nviewer_vector), 0.0), u_shininess);"
                  "                                                                                "
                  "                                            "
                  "       vec3 phong_ads_light = ambient + diffuse + specular;"
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

          printf("Fragment Shader Compiler Info Log: %s", szInfoLog);
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
    int iProgramLinkStatus = 0;
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

          printf("Shader Program Linking Info Log: %s", szInfoLog);
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

    laUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
    ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    lsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

    kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_shininess");

    enableLightUniform = glGetUniformLocation(gShaderProgramObject, "u_enable_light");

    /////////////////////////////////////////////////////////////////////////

    // vertex array
    GLfloat *sphereVertices = NULL;
    GLfloat *sphereNormals = NULL;
    GLfloat *sphereTexcoords = NULL;
    coords = [self generateSphereCoords:0.5f
                                 slices:100
                                    pos:&sphereVertices
                                   norm:&sphereNormals
                                    tex:&sphereTexcoords];

    // create vao
    glGenVertexArrays(1, &vao_sphere);
    glBindVertexArray(vao_sphere);

    // vertex positions
    glGenBuffers(1, &vbo_position_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_sphere);
    glBufferData(GL_ARRAY_BUFFER, 3 * coords * sizeof(GLfloat), sphereVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    // vertex normals
    glGenBuffers(1, &vbo_normal_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_sphere);
    glBufferData(GL_ARRAY_BUFFER, 3 * coords * sizeof(GLfloat), sphereNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //////////////////////////////////////////////////////////////////////

    // light configurations
    // WHITE light
    lights[0].lightAmbient[0] = 0.0f;
    lights[0].lightAmbient[1] = 0.0f;
    lights[0].lightAmbient[2] = 0.0f;
    lights[0].lightAmbient[3] = 1.0f;

    lights[0].lightDiffuse[0] = 1.0f;
    lights[0].lightDiffuse[1] = 1.0f;
    lights[0].lightDiffuse[2] = 1.0f;
    lights[0].lightDiffuse[3] = 1.0f;

    lights[0].lightSpecular[0] = 1.0f;
    lights[0].lightSpecular[1] = 1.0f;
    lights[0].lightSpecular[2] = 1.0f;
    lights[0].lightSpecular[3] = 1.0f;

    lights[0].lightPosition[0] = 0.0f;
    lights[0].lightPosition[1] = 0.0f;
    lights[0].lightPosition[2] = 0.0f;
    lights[0].lightPosition[3] = 1.0f;

    lights[0].angle = 0.0f;

    /////////////////////////////////////////////////////////////////////////

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
  mat4 modelMatrix;
  mat4 ViewMatrix;
  mat4 modelViewProjectionMatrix;

  ///// CUBE ///////////////////////////////////////////////////////////////////////////////

  // intialize above matrices to identity
  translationMatrix = mat4::identity();
  modelMatrix = mat4::identity();
  ViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(vUniform, 1, false, ViewMatrix);

  if (bLight == YES) {
    glUniform3fv(laUniform, 1, lights[0].lightAmbient);
    glUniform3fv(ldUniform, 1, lights[0].lightDiffuse);
    glUniform3fv(lsUniform, 1, lights[0].lightSpecular);

    if (bXLight)
      glUniform4f(lightPositionUniform, 0.0f, 100.0f * cos(lights[0].angle),
                  100.0f * sin(lights[0].angle), 1.0f);

    if (bYLight)
      glUniform4f(lightPositionUniform, 100.0f * cos(lights[1].angle), 0.0f,
                  100.0f * sin(lights[1].angle), 1.0f);

    if (bZLight)
      glUniform4f(lightPositionUniform, 100.0f * cos(lights[2].angle),
                  100.0f * sin(lights[2].angle), 0.0f, 1.0f);

    glUniform1i(enableLightUniform, 1);
  } else
    glUniform1i(enableLightUniform, 0);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 4; j++) {
      glViewport(i * (float)gWidth / 6.0, j * (float)gHeight / 4.0, (float)gWidth / 6.0,
                 (float)gHeight / 4.0);

      perspectiveProjectionMatrix =
          perspective(60.0, ((GLsizei)gWidth / 6.0) / ((GLsizei)gHeight / 4.0), 0.1f, 40.0f);

      modelMatrix = translationMatrix * translate(0.0f, 0.0f, -1.5f);
      glUniformMatrix4fv(mUniform, 1, false, modelMatrix);
      glUniformMatrix4fv(pUniform, 1, false, perspectiveProjectionMatrix);

      glUniform3fv(kaUniform, 1, Materials[i][j].MaterialAmbient);
      glUniform3fv(kdUniform, 1, Materials[i][j].MaterialDiffuse);
      glUniform3fv(ksUniform, 1, Materials[i][j].MaterialSpecular);
      glUniform1f(shininessUniform, Materials[i][j].MaterialShininess[0]);

      // bind with vao (this will avoid many binding to vbo_vertex)
      glBindVertexArray(vao_sphere);
      glDrawArrays(GL_TRIANGLE_FAN, 0, coords);
      // unbind vao
      glBindVertexArray(0);
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////////

  // unuse program
  glUseProgram(0);

  [self update];

  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
  [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)update {
  if (lights[0].angle < 360.0f) {
    lights[0].angle += 0.02f;
  } else {
    lights[0].angle = 0.0f;
  }

  if (lights[1].angle < 360.0f) {
    lights[1].angle += 0.02f;
  } else {
    lights[1].angle = 0.0f;
  }

  if (lights[2].angle < 360.0f) {
    lights[2].angle += 0.02f;
  } else {
    lights[2].angle = 0.0f;
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
  gWidth = width;
  gHeight = height;

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

- (int)generateSphereCoords:(GLfloat)r
                     slices:(int)n
                        pos:(GLfloat **)ppos
                       norm:(GLfloat **)pnorm
                        tex:(GLfloat **)ptex {
  int iNoOfCoords = 0;
  int i, j;
  GLfloat phi1, phi2, theta, s, t;
  GLfloat ex, ey, ez, px, py, pz;

  *ppos = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
  *pnorm = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
  *ptex = (GLfloat *)malloc(2 * sizeof(GLfloat) * n * (n + 1) * 2);

  GLfloat *pos = *ppos;
  GLfloat *norm = *pnorm;
  GLfloat *tex = *ptex;

  if (ppos && pnorm && ptex) {
    // iNoOfCoords = n * (n + 1);

    for (j = 0; j < n; j++) {
      phi1 = j * M_PI * 2 / n;
      phi2 = (j + 1) * M_PI * 2 / n;

      // fprintf(gpFile, "phi1 [%g]...\n", phi1);
      // fprintf(gpFile, "phi2 [%g]...\n", phi2);

      for (i = 0; i <= n; i++) {
        theta = i * M_PI / n;

        ex = sin(theta) * cos(phi2);
        ey = sin(theta) * sin(phi2);
        ez = cos(theta);
        px = r * ex;
        py = r * ey;
        pz = r * ez;

        // glNormal3f(ex, ey, ez);
        norm[(iNoOfCoords * 3) + 0] = ex;
        norm[(iNoOfCoords * 3) + 1] = ey;
        norm[(iNoOfCoords * 3) + 2] = ez;

        s = phi2 / (M_PI * 2);   // column
        t = 1 - (theta / M_PI);  // row
        // glTexCoord2f(s, t);
        tex[(iNoOfCoords * 2) + 0] = s;
        tex[(iNoOfCoords * 2) + 1] = t;

        // glVertex3f(px, py, pz);
        pos[(iNoOfCoords * 3) + 0] = px;
        pos[(iNoOfCoords * 3) + 1] = py;
        pos[(iNoOfCoords * 3) + 2] = pz;

        /*printf("pos[%d]...\n", (iNoOfCoords * 3) + 0);
         printf("pos[%d]...\n", (iNoOfCoords * 3) + 1);
         printf( "pos[%d]...\n", (iNoOfCoords * 3) + 2);*/

        ex = sin(theta) * cos(phi1);
        ey = sin(theta) * sin(phi1);
        ez = cos(theta);
        px = r * ex;
        py = r * ey;
        pz = r * ez;

        // glNormal3f(ex, ey, ez);
        norm[(iNoOfCoords * 3) + 3] = ex;
        norm[(iNoOfCoords * 3) + 4] = ey;
        norm[(iNoOfCoords * 3) + 5] = ez;

        s = phi1 / (M_PI * 2);   // column
        t = 1 - (theta / M_PI);  // row
        // glTexCoord2f(s, t);
        tex[(iNoOfCoords * 2) + 2] = s;
        tex[(iNoOfCoords * 2) + 3] = t;

        // glVertex3f(px, py, pz);
        pos[(iNoOfCoords * 3) + 3] = px;
        pos[(iNoOfCoords * 3) + 4] = py;
        pos[(iNoOfCoords * 3) + 5] = pz;

        /*printf( "pos[%d]...\n", (iNoOfCoords * 3) + 3);
         printf( "pos[%d]...\n", (iNoOfCoords * 3) + 4);
         printf( "pos[%d]...\n", (iNoOfCoords * 3) + 5);*/

        iNoOfCoords += 2;
      }
    }
  }

  return iNoOfCoords;
}

- (void)dealloc {
  // code

  if (vbo_position_sphere) {
    glDeleteBuffers(1, &vbo_position_sphere);
    vbo_position_sphere = 0;
  }

  if (vbo_normal_sphere) {
    glDeleteBuffers(1, &vbo_normal_sphere);
    vbo_normal_sphere = 0;
  }

  if (vao_sphere) {
    glDeleteVertexArrays(1, &vao_sphere);
    vao_sphere = 0;
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
