// headers
#import <Cocoa/Cocoa.h>  // ~ Windows.h or xlib.h
#import <Foundation/Foundation.h>

// CoreVideo display link: to match with refresh rate
#import <QuartzCore/CVDisplayLink.h>

// opengl headers
#import <OpenGL/gl3.h>  // gl.h
#import <OpenGL/gl3ext.h>

#import "vmath.h"

// callback function for display link
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *,
                               CVOptionFlags, CVOptionFlags *, void *);

using namespace vmath;

enum {
  RMC_ATTRIBUTE_POSITION = 0,
  RMC_ATTRIBUTE_COLOR,
  RMC_ATTRIBUTE_NORMAL,
  RMC_ATTRIBUTE_TEXCOORD,
};

// log file
FILE *gpFile = NULL;

// NS -> NeXTSTEP
// CG -> Core Graphics
// CV -> Core Video
// CGL -> Core Graphics Library

//// forward declarations of interface /////////////////////////////////////////////////////////////

// :  extends
// <> implements
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface MyOpenGLView : NSOpenGLView
@end

////////////////////////////////////////////////////////////////////////////////////////////////////

// entry-point function
int main(int argc, char *argv[]) {
  // code
  NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc] init];

  // global application object, no need to declare
  NSApp = [NSApplication sharedApplication];

  // AppDelegate will handle Application and Window delegate/functions
  [NSApp setDelegate:[[AppDelegate alloc] init]];

  // message loop!
  [NSApp run];

  // control will return here, when program is terminating
  // release resources from autorelease pool
  [pPool release];

  return (0);
}

//// Implementation of interfaces //////////////////////////////////////////////////////////////////

@implementation AppDelegate {
 @private
  NSWindow *window;
  MyOpenGLView *myOpenGLView;
}

// ~ WM_CREATE
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  // code

  // create log file
  NSBundle *appBundle = [NSBundle mainBundle];
  NSString *appDirPath = [appBundle bundlePath];
  NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
  NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/window.log", parentDirPath];

  const char *pszLogFileNameWithPath =
      [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

  gpFile = fopen(pszLogFileNameWithPath, "w");
  if (gpFile == NULL) {
    [self release];
    [NSApp terminate:self];
  }

  fprintf(gpFile, "==== Application Started ====\n");

  // window
  // this is from core library, which is written in C
  NSRect winRect = NSMakeRect(0.0, 0.0, 800.0, 600.0);

  // create window
  window = [[NSWindow alloc]
      initWithContentRect:winRect
                styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                          NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                  backing:NSBackingStoreBuffered
                    defer:NO];

  [window setTitle:@"OpenGL | Pyramid and Cube Texture"];
  [window center];

  // create view
  myOpenGLView = [[MyOpenGLView alloc] initWithFrame:winRect];

  // assign view to window and set delegate to self
  [window setContentView:myOpenGLView];
  [window setDelegate:self];
  [window makeKeyAndOrderFront:self];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // code
  if (gpFile) {
    fprintf(gpFile, "==== Application Terminated ====\n");
    fclose(gpFile);
    gpFile = NULL;
  }
}

// ~ WM_DESTROY
- (void)windowWillClose:(NSNotification *)aNotification {
  // code
  // this will pass to applicationWillTerminate
  [NSApp terminate:self];
}

- (void)dealloc {
  // code
  [myOpenGLView release];
  [window release];
  [super dealloc];
}

@end

@implementation MyOpenGLView {
 @private
  CVDisplayLinkRef displayLink;

  GLuint gShaderProgramObject;

  GLuint vaoPyramid;
  GLuint vboPositionPyramid;
  GLuint vboColorPyramid;

  GLuint vaoCube;
  GLuint vboPositionCube;
  GLuint vboColorCube;

  GLuint mvpMatrixUniform;
  GLuint textureSamplerUniform;

  mat4 perspectiveProjectionMatrix;

  GLfloat anglePyramid;
  GLfloat angleCube;

  GLuint stoneTexture;
  GLuint kundaliTexture;
}

- (id)initWithFrame:(NSRect)frame {
  // code
  self = [super initWithFrame:frame];

  if (self) {
    // ~ Pixel Format Descriptor
    // ~ glx framebuffer attributes
    // PFA: Pixel Format Attribute
    NSOpenGLPixelFormatAttribute attributes[] = {
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion4_1Core,  // OpenGL 4.1 core profile
        NSOpenGLPFAScreenMask,
        CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),  // single display
        NSOpenGLPFANoRecovery,   // do not shift to SW renderer, if HW renderer is missing
        NSOpenGLPFAAccelerated,  // HW rendering
        NSOpenGLPFAColorSize,
        24,  // RGB
        NSOpenGLPFADepthSize,
        24,
        NSOpenGLPFAAlphaSize,
        8,
        NSOpenGLPFADoubleBuffer,
        0  // end of list
    };

    // get pixel format
    NSOpenGLPixelFormat *pixelFormat =
        [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
    if (pixelFormat == NULL) {
      fprintf(gpFile, "Pixel Format failed..\nTerminating..\n");
      [self release];
      [NSApp terminate:self];
    }

    NSOpenGLContext *glContext = [[[NSOpenGLContext alloc] initWithFormat:pixelFormat
                                                             shareContext:nil] autorelease];

    // set pixel format and context of current view
    [self setPixelFormat:pixelFormat];
    [self setOpenGLContext:glContext];

    anglePyramid = 0.0f;
    angleCube = 0.0f;
  }

  return (self);
}

- (CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime {
  // code
  // this function runs in separate thread (multithreading)
  // hence separate autoreleasepool is required
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  // call our drawing method
  [self drawView];

  [pool release];
  return (kCVReturnSuccess);
}

// from NSOpenGLView
- (void)prepareOpenGL {
  // code
  [super prepareOpenGL];

  // get OpenGL context and make it current context
  [[self openGLContext] makeCurrentContext];

  // swap interval
  GLint swapInt = 1;
  [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

  // fetch OpenGL related details
  fprintf(gpFile, "OpenGL Vendor:   %s\n", glGetString(GL_VENDOR));
  fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
  fprintf(gpFile, "OpenGL Version:  %s\n", glGetString(GL_VERSION));
  fprintf(gpFile, "GLSL Version:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  // fetch OpenGL enabled extensions
  GLint numExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

  fprintf(gpFile, "==== OpenGL Extensions ====\n");
  for (int i = 0; i < numExtensions; i++) {
    fprintf(gpFile, "  %s\n", glGetStringi(GL_EXTENSIONS, i));
  }
  fprintf(gpFile, "===========================\n\n");

  //// vertex shader
  // create shader
  GLuint gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

  // clang-format off
  // provide source code to shader
  const GLchar *vertexShaderSourceCode =
    "#version 410 core \n" \
    
    "in vec4 vPosition; \n" \
    "in vec2 vTexcoord; \n" \

    "uniform mat4 u_mvpMatrix; \n" \

    "out vec2 out_Texcoord; \n" \

    "void main (void) \n" \
    "{ \n" \
    "	gl_Position = u_mvpMatrix * vPosition; \n" \
    "	out_Texcoord = vTexcoord; \n" \
    "} \n";
  // clang-format on

  glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

  // compile shader
  glCompileShader(gVertexShaderObject);

  // compilation errors
  GLint iShaderCompileStatus = 0;
  GLint iInfoLogLength = 0;
  GLchar *szInfoLog = NULL;

  glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
  if (iShaderCompileStatus == GL_FALSE) {
    glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
    if (iInfoLogLength > 0) {
      szInfoLog = (GLchar *)malloc(iInfoLogLength);
      if (szInfoLog != NULL) {
        GLsizei written;
        glGetShaderInfoLog(gVertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

        fprintf(gpFile, "Vertex Shader Compiler Info Log: \n%s\n", szInfoLog);
        free(szInfoLog);
        [self release];
        [NSApp terminate:self];
      }
    }
  }

  //// fragment shader
  // create shader
  GLuint gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

  // clang-format off
  // provide source code to shader
  const GLchar *fragmentShaderSourceCode = 
    "#version 410 core \n" \

    "in vec2 out_Texcoord; \n" \

    "uniform sampler2D u_textureSampler; \n"

    "out vec4 FragColor; \n" \

    "void main (void) \n" \
    "{ \n" \
    "	FragColor = texture(u_textureSampler, out_Texcoord); \n" \
    "} \n";
  // clang-format on

  glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

  // compile shader
  glCompileShader(gFragmentShaderObject);

  // compile errors
  iShaderCompileStatus = 0;
  iInfoLogLength = 0;
  szInfoLog = NULL;

  glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
  if (iShaderCompileStatus == GL_FALSE) {
    glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
    if (iInfoLogLength > 0) {
      szInfoLog = (GLchar *)malloc(iInfoLogLength);
      if (szInfoLog != NULL) {
        GLsizei written;
        glGetShaderInfoLog(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

        fprintf(gpFile, "Fragment Shader Compiler Info Log: \n%s\n", szInfoLog);
        free(szInfoLog);
        [self release];
        [NSApp terminate:self];
      }
    }
  }

  //// shader program
  // create
  gShaderProgramObject = glCreateProgram();

  // attach shaders
  glAttachShader(gShaderProgramObject, gVertexShaderObject);
  glAttachShader(gShaderProgramObject, gFragmentShaderObject);

  // pre-linking binding to vertex attribute
  glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_POSITION, "vPosition");
  glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

  // link shader
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

        fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
        free(szInfoLog);
        [self release];
        [NSApp terminate:self];
      }
    }
  }

  // post-linking retrieving uniform locations
  mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
  textureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_textureSampler");

  // clang-format off
  // position array of pyramid
  const GLfloat pyramidVertices[] = {
    /* Front */
     0.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 1.0f,
     1.0f, -1.0f, 1.0f,

    /* Right */
    0.0f,  1.0f,  0.0f,
    1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,

    /* Left */
     0.0f,  1.0f,  0.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,

    /* Back */
     0.0f,  1.0f,  0.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
  };

  // color array of pyramid
  const GLfloat pyramidTexcoords[] = {
    /* Front */
    0.5f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    /* Right */
    0.5f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,

    /* Left */
    0.5f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    /* Back */
    0.5f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
  };

  // position array of cube
  const GLfloat cubeVertices[] = {
    /* Top */
     1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f,  1.0f,
     1.0f, 1.0f,  1.0f,

    /* Bottom */
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,

    /* Front */
     1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
     1.0f, -1.0f, 1.0f,

    /* Back */
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    /* Right */
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,

    /* Left */
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
  };

  // color array of cube
  const GLfloat cubeTexcoords[] = {
    /* Top */
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    /* Bottom */
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    /* Front */
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    /* Back */
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    /* Right */
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    /* Left */
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
  };
  // clang-format on

  // create vao for pyramid
  glGenVertexArrays(1, &vaoPyramid);
  glBindVertexArray(vaoPyramid);

  // create vbo for position
  glGenBuffers(1, &vboPositionPyramid);
  glBindBuffer(GL_ARRAY_BUFFER, vboPositionPyramid);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // create vbo for color
  glGenBuffers(1, &vboColorPyramid);
  glBindBuffer(GL_ARRAY_BUFFER, vboColorPyramid);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexcoords), pyramidTexcoords, GL_STATIC_DRAW);
  glVertexAttribPointer(RMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(RMC_ATTRIBUTE_TEXCOORD);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  // create vao for cube
  glGenVertexArrays(1, &vaoCube);
  glBindVertexArray(vaoCube);

  // create vbo for position
  glGenBuffers(1, &vboPositionCube);
  glBindBuffer(GL_ARRAY_BUFFER, vboPositionCube);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // create vbo for color
  glGenBuffers(1, &vboColorCube);
  glBindBuffer(GL_ARRAY_BUFFER, vboColorCube);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
  glVertexAttribPointer(RMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(RMC_ATTRIBUTE_TEXCOORD);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  // set clear color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // set clear depth
  glClearDepth(1.0f);

  // depth test
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // load textures
  glEnable(GL_TEXTURE_2D);
  stoneTexture = [self loadTextureFromBMPFile:"Stone.bmp"];
  kundaliTexture = [self loadTextureFromBMPFile:"Vijay_Kundali.bmp"];

  perspectiveProjectionMatrix = mat4::identity();

  // create and start display link
  CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
  CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
  CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
  CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
  CVDisplayLinkStart(displayLink);
}

// from NSOpenGLView
- (void)reshape {
  // code
  [super reshape];

  CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

  NSRect rect = [self bounds];
  if (rect.size.height < 0) {
    rect.size.height = 1;
  }
  glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);

  perspectiveProjectionMatrix =
      vmath::perspective(45.0f, (float)rect.size.width / (float)rect.size.height, 0.1f, 100.0f);

  CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect {
  // code
  [self drawView];
}

- (void)drawView {
  // code
  [[self openGLContext] makeCurrentContext];
  CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

  // code
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // start using OpenGL program object
  glUseProgram(gShaderProgramObject);

  // declaration of matrices
  mat4 translateMatrix;
  mat4 rotateMatrix;
  mat4 scaleMatrix;
  mat4 modelViewMatrix;
  mat4 modelViewProjectionMatrix;

  //// pyramid ////////////////////////

  // intialize above matrices to identity
  translateMatrix = mat4::identity();
  rotateMatrix = mat4::identity();
  modelViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // transformations
  translateMatrix = translate(-1.5f, 0.0f, -6.0f);
  rotateMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
  modelViewMatrix = translateMatrix * rotateMatrix;

  // do necessary matrix multiplication
  modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

  // bind the texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, stoneTexture);
  glUniform1i(textureSamplerUniform, 0);

  // bind with vaoPyramid (this will avoid many binding to vbo)
  glBindVertexArray(vaoPyramid);

  // draw necessary scene
  glDrawArrays(GL_TRIANGLES, 0, 12);

  // unbind vaoPyramid
  glBindVertexArray(0);

  //// cube ////////////////////////

  // intialize above matrices to identity
  translateMatrix = mat4::identity();
  rotateMatrix = mat4::identity();
  scaleMatrix = mat4::identity();
  modelViewMatrix = mat4::identity();
  modelViewProjectionMatrix = mat4::identity();

  // transformations
  translateMatrix = translate(1.5f, 0.0f, -6.0f);
  rotateMatrix = rotate(angleCube, 1.0f, 0.0f, 0.0f);
  rotateMatrix *= rotate(angleCube, 0.0f, 1.0f, 0.0f);
  rotateMatrix *= rotate(angleCube, 0.0f, 0.0f, 1.0f);
  scaleMatrix = scale(0.75f, 0.75f, 0.75f);
  modelViewMatrix = translateMatrix * scaleMatrix * rotateMatrix;

  // do necessary matrix multiplication
  modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

  // send necessary matrices to shader in respective uniforms
  glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

  // bind the texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, kundaliTexture);
  glUniform1i(textureSamplerUniform, 0);

  // bind with vaoPyramid (this will avoid many binding to vbo)
  glBindVertexArray(vaoCube);

  // draw necessary scene
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
  glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

  // unbind vaoPyramid
  glBindVertexArray(0);

  // stop using OpenGL program object
  glUseProgram(0);

  [self spin];

  CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
  CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)spin {
  // code
  anglePyramid += 1.0f;
  if (anglePyramid >= 360.0f) anglePyramid = 0.0f;

  angleCube += 1.0f;
  if (angleCube >= 360.0f) angleCube = 0.0f;
}

- (BOOL)acceptsFirstResponder {
  // code
  [[self window] makeFirstResponder:self];
  return (YES);
}

- (void)keyDown:(NSEvent *)theEvent {
  // code
  int key = (int)[[theEvent characters] characterAtIndex:0];

  switch (key) {
    case 27:  // esc key
      [self release];
      [NSApp terminate:self];
      break;

    case 'F':
    case 'f':
      [[self window] toggleFullScreen:self];
      break;
  }
}

- (void)mouseDown:(NSEvent *)theEvent {
  // code
}

- (void)rightMouseDown:(NSEvent *)theEvent {
  // code
}

- (void)otherMouseDown:(NSEvent *)theEvent {
  // code
}

- (GLuint)loadTextureFromBMPFile:(const char *)imageFileName {
  // code
  NSBundle *appBundle = [NSBundle mainBundle];
  NSString *appDirPath = [appBundle bundlePath];
  NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
  NSString *imageFileNameWithPath =
      [NSString stringWithFormat:@"%@/%s", parentDirPath, imageFileName];

  // get NSImage representation of image file
  NSImage *bmpImage = [[NSImage alloc] initWithContentsOfFile:imageFileNameWithPath];
  if (!bmpImage) {
    fprintf(gpFile, "%s texture not fount..\n", imageFileName);
    return (0);
  }

  // get CGImage representation of NSImage
  CGImageRef cgImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];

  // get width and height of CGImage representation of texture image
  int imgWidth = (int)CGImageGetWidth(cgImage);
  int imgHeight = (int)CGImageGetHeight(cgImage);

  // get CFData representation of image data (Core Foundation)
  CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));

  // convert CFDataRef into generic format data
  void *pixels = (void *)CFDataGetBytePtr(imageData);

  // generate OpenGL texture using above data
  GLuint bmpTexture;
  glGenTextures(1, &bmpTexture);
  glBindTexture(GL_TEXTURE_2D, bmpTexture);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // setting texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

  CFRelease(imageData);
  return (bmpTexture);
}

- (void)dealloc {
  // code
  glDeleteTextures(1, &stoneTexture);
  glDeleteTextures(1, &kundaliTexture);

  if (vaoPyramid) {
    glDeleteVertexArrays(1, &vaoPyramid);
    vaoPyramid = 0;
  }

  if (vboPositionPyramid) {
    glDeleteBuffers(1, &vboPositionPyramid);
    vboPositionPyramid = 0;
  }

  if (vboColorPyramid) {
    glDeleteBuffers(1, &vboColorPyramid);
    vboColorPyramid = 0;
  }

  if (vaoCube) {
    glDeleteVertexArrays(1, &vaoCube);
    vaoCube = 0;
  }

  if (vboPositionCube) {
    glDeleteBuffers(1, &vboPositionCube);
    vboPositionCube = 0;
  }

  if (vboColorCube) {
    glDeleteBuffers(1, &vboColorCube);
    vboColorCube = 0;
  }

  // destroy shader programs
  if (gShaderProgramObject) {
    GLsizei shaderCount;
    GLsizei i;

    glUseProgram(gShaderProgramObject);
    glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

    GLuint *pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));
    if (pShaders) {
      glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

      for (i = 0; i < shaderCount; i++) {
        // detach shader
        glDetachShader(gShaderProgramObject, pShaders[i]);

        // delete shader
        glDeleteShader(pShaders[i]);
        pShaders[i] = 0;
      }

      free(pShaders);
    }

    glDeleteProgram(gShaderProgramObject);
    gShaderProgramObject = 0;
    glUseProgram(0);
  }

  CVDisplayLinkStop(displayLink);
  CVDisplayLinkRelease(displayLink);

  [super dealloc];
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////

// callback "C" style function in global space
// displayLinkContext: MyOpenGLView pointer set by OS while calling this
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *now,
                               const CVTimeStamp *outputTime, CVOptionFlags flagsIn,
                               CVOptionFlags *flagsOut, void *displayLinkContext) {
  // code
  CVReturn result = [(MyOpenGLView *)displayLinkContext getFrameForTime:outputTime];
  return (result);
}