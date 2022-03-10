// headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

#import "vmath.h"

using namespace vmath;

enum
{
    RMC_ATTRIBUTE_POSITION = 0,
    RMC_ATTRIBUTE_COLOR,
    RMC_ATTRIBUTE_NORMAL,
    RMC_ATTRIBUTE_TEXCOORD,
    
};

// 'C' style global function declaration
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *);

// global variables
FILE *gpFile = NULL;

// interface declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

// Entry-point function
int main(int args, const char *argv[])
{
    // code
    NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc]init];
    
    NSApp = [NSApplication sharedApplication];
    
    [NSApp setDelegate: [[AppDelegate alloc]init]];
    
    [NSApp run];
    
    [pPool release];
    
    return(0);
}

// interface implementation
@implementation AppDelegate
{
@private
    NSWindow *window;
    GLView *glView;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // code
    
    // Log file
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *appDirName = [mainBundle bundlePath];
    NSString *parentDirPath = [appDirName stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/Log.txt", parentDirPath];
    const char *pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    
    gpFile = fopen(pszLogFileNameWithPath, "w");
    if (gpFile == NULL)
    {
        printf("Cannot create Log.txt file.\nExiting..\n");
        [self release];
        [NSApp terminate:self];
    }
    
    fprintf(gpFile, "Program is started successfully...\n");
    
    // window
    NSRect win_rect;
    win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    
    // create simple window
    window = [[NSWindow alloc] initWithContentRect:win_rect
                                         styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [window setTitle:@"OpenGL | Interleaved"];
    [window center];
    
    glView = [[GLView alloc]initWithFrame:win_rect];
    
    [window setContentView:glView];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    // code
    fprintf(gpFile, "Program is terminated successfully...\n");
    
    if (gpFile)
    {
        fclose(gpFile);
        gpFile = NULL;
    }
    
}

- (void)windowWillClose:(NSNotification *)aNotification
{
    // code
    [NSApp terminate:self];
}

- (void)dealloc
{
    // code
    [glView release];
    
    [window release];
    
    [super dealloc];
}
@end

@implementation GLView
{
@private
    CVDisplayLinkRef displayLink; // these are by default private fields
    
    GLuint gShaderProgramObject;
    
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

    mat4   perspectiveProjectionMatrix;

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
}

- (id)initWithFrame:(NSRect)frame;
{
    // code
    self = [super initWithFrame:frame];
    
    if(self)
    {
        [[self window]setContentView:self];
        
        NSOpenGLPixelFormatAttribute attrs[] =
        {
            // must specify the 4.1 core profile, to use OpenGL 4.1
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
            //specify the display id to associate the GL context with (main display for now)
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,  // want h/w context, do not settle for s/w renderer
            NSOpenGLPFAAccelerated, // h/w accelerator
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer,
            0
        };
        
        NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs] autorelease];
        if (pixelFormat == nil)
        {
            fprintf(gpFile, "No valid OpenGL Pixel Format is available..");
            [self release];
            [NSApp terminate:self];
        }
        
        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        
        [self setPixelFormat:pixelFormat];
        [self setOpenGLContext:glContext]; // it automatically releases older context, if present, and sets the newer one
        
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
    }
    
    return(self);
}

- (CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime
{
    // code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
    
    [self drawView];
    
    [pool release];
    return(kCVReturnSuccess);
}

- (void)prepareOpenGL // NSOpenGL
{
    // code
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;

    [super prepareOpenGL];
    
    // OpenGL Info
    fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version   : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    [[self openGLContext]makeCurrentContext];
    
    GLint swapInt = 1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    //////////// P E R   V E R T E X ////////////////////////////////////////
    
    // create vertex shader object
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    // vertex shader source code
    const GLchar *vertexShaderSourceCode = (GLchar *)
    "#version 410 core "  \
    "\n" \
    "in vec4 vPosition;" \
    "in vec4 vColor;" \
    "in vec3 vNormal;" \
    "in vec2 vTexcoord;" \
    "uniform mat4 u_m_matrix;" \
    "uniform mat4 u_v_matrix;" \
    "uniform mat4 u_p_matrix;" \
    "uniform vec4 u_light_position;" \
    "uniform int u_enable_light;" \
    "out vec3 tnorm;" \
    "out vec3 light_direction;" \
    "out vec3 viewer_vector;" \
    "out vec2 out_Texcoord;" \
    "out vec4 out_Color;" \
    "void main (void)" \
    "{" \
    "   if (u_enable_light == 1) " \
    "   { " \
    "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
    "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" \
    "       light_direction = vec3(u_light_position - eye_coordinates);" \
    "       float tn_dot_ldir = max(dot(tnorm, light_direction), 0.0);" \
    "       viewer_vector = vec3(-eye_coordinates.xyz);" \
    "    }" \
    "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" \
    "    out_Texcoord = vTexcoord;" \
    "    out_Color = vColor;" \
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
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, "Vertex Shader Compiler Info Log: %s", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // create fragment shader object
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    
    // fragment shader source code
    const GLchar *fragmentShaderSourceCode = (GLchar *)
    "#version 410 core" \
    "\n" \
    "in vec2 out_Texcoord;" \
    "in vec4 out_Color;" \
    "in vec3 tnorm;" \
    "in vec3 light_direction;" \
    "in vec3 viewer_vector;" \
    "uniform vec3 u_la;" \
    "uniform vec3 u_ld;" \
    "uniform vec3 u_ls;" \
    "uniform vec3 u_ka;" \
    "uniform vec3 u_kd;" \
    "uniform vec3 u_ks;" \
    "uniform float u_shininess;" \
    "uniform int u_enable_light;" \
    "uniform sampler2D u_sampler;" \
    "out vec4 FragColor;" \
    "void main (void)" \
    "{" \
    "    vec3 phong_ads_light = vec3(1.0);" \
    "   if (u_enable_light == 1) " \
    "   { " \
    "       vec3 ntnorm = normalize(tnorm);" \
    "       vec3 nlight_direction = normalize(light_direction);" \
    "       vec3 nviewer_vector = normalize(viewer_vector);" \
    "       vec3 reflection_vector = reflect(-nlight_direction, ntnorm);" \
    "       float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);" \
    "       vec3 ambient  = u_la * u_ka;" \
    "       vec3 diffuse  = u_ld * u_kd * tn_dot_ldir;" \
    "       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), u_shininess);" \
    "       phong_ads_light = ambient + diffuse + specular;" \
    "    }" \
    "    vec4 tex = texture(u_sampler, out_Texcoord);" \
    "    FragColor = vec4((vec3(tex) * vec3(out_Color) * phong_ads_light), 1.0);" \
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
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, "Fragment Shader Compiler Info Log: %s", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
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
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_NORMAL, "vNormal");
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");
    
    // link the shader program
    glLinkProgram(gShaderProgramObject);
    
    // linking errors
    GLint iProgramLinkStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;
    
    glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
    if (iProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, ("Shader Program Linking Info Log: %s"), szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // post-linking retrieving uniform locations
    mUniform = glGetUniformLocation(gShaderProgramObject, "u_m_matrix");
    vUniform = glGetUniformLocation(gShaderProgramObject, "u_v_matrix");
    pUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");

    samplerUniform = glGetUniformLocation(gShaderProgramObject, "u_sampler");

    laUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
    kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    lsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
    ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_shininess");

    enableLightUniform = glGetUniformLocation(gShaderProgramObject, "u_enable_light");
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

    // vertex array
    const GLfloat cubeData[] = {
        /* Top */
         1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,

        /* Bottom */
         1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, -1.0f, 0.0f,    1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, -1.0f, 0.0f,    0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, -1.0f, 0.0f,    0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, -1.0f, 0.0f,    1.0f, 0.0f,

        /* Front */
         1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,

        /* Back */
         1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, -1.0f,    1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, -1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, -1.0f,    0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, -1.0f,    0.0f, 0.0f,

        /* Right */
        1.0f,  1.0f, -1.0f,        1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,        1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        1.0f, -1.0f,  1.0f,        1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,        1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

        /* Left */
        -1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 0.0f,    -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,    -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,    -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 0.0f,    -1.0f, 0.0f, 0.0f,    0.0f, 1.0f
    };

    // create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // vertex position
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(0 * sizeof(float)));
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    // vertex colors
    glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);

    // vertex normals
    glVertexAttribPointer(RMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(RMC_ATTRIBUTE_NORMAL);

    // vertex texcoords
    glVertexAttribPointer(RMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(9 * sizeof(float)));
    glEnableVertexAttribArray(RMC_ATTRIBUTE_TEXCOORD);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    //////////////////////////////////////////////////////////////////////
    
    // clear the depth buffer
    glClearDepth(1.0f);
    
    // clear the screen by OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // enable depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // texture
    glEnable(GL_TEXTURE_2D);
    texture_marble = [self loadTextureFromBMPFile:"marble.bmp"];
    
    //////////////////////////////////////////////////////////////////////

    perspectiveProjectionMatrix = mat4::identity();
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

- (void)reshape // NSOpenGL
{
    // code
    [super reshape];

    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    NSRect rect = [self bounds];
    
    GLfloat width = rect.size.width;
    GLfloat height = rect.size.height;
    
    if (height == 0)
        height = 1;
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0, (float)width / (float)height, 0.1f, 100.0f);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect
{
    // code
    [self drawView];
}

- (void)drawView
{
    // code
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // use shader program
    glUseProgram(gShaderProgramObject);

    //declaration of matrices
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
    
    [self step];
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

- (void)step
{
    if (angleCube >= 360.0)
    {
        angleCube = 0.0;
    }
    else
    {
        angleCube += 1.0f;
    }
}

- (GLuint)loadTextureFromBMPFile:(const char *)texFileName
{
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *appDirName = [mainBundle bundlePath];
    NSString *parentDirPath = [appDirName stringByDeletingLastPathComponent];
    NSString *textureFileNameWithPath = [NSString stringWithFormat:@"%@/%s", parentDirPath, texFileName];

    NSImage *bmpImage = [[NSImage alloc]initWithContentsOfFile:textureFileNameWithPath];
    if (!bmpImage)
    {
        NSLog(@"can't find %@", textureFileNameWithPath);
        return(0);
    }

    CGImageRef cgImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];

    int w = (int)CGImageGetWidth(cgImage);
    int h = (int)CGImageGetHeight(cgImage);
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    void *pixels = (void *)CFDataGetBytePtr(imageData);

    GLuint bmpTexture;
    glGenTextures(1, &bmpTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // set 1 rather that default 4, for better performance
    glBindTexture(GL_TEXTURE_2D, bmpTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // create mipmaps for this texture for better image quality
    glGenerateMipmap(GL_TEXTURE_2D);

    CFRelease(imageData);
    return(bmpTexture);
}

- (BOOL)acceptsFirstResponder
{
    // code
    [[self window]makeFirstResponder:self];
    return(YES);
}

- (void)keyDown:(NSEvent *)theEvent
{
    // code
    int key = (int)[[theEvent characters]characterAtIndex:0];
    switch(key)
    {
        case 27: // ESC key
            [self release];
            [NSApp terminate:self];
            break;
            
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self]; // repainting occurs automatically
            break;
            
        case 'L':
        case 'l':
            bLight = !bLight;
            break;
            
        default:
            break;
    }
}

- (void)mouseDown:(NSEvent *)theEvent
{
    // code
    [self setNeedsDisplay:YES]; // repainting
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    // code
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    // code
    [self setNeedsDisplay:YES]; // repainting
}

- (void) dealloc
{
    if (vbo)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    
    if (gShaderProgramObject)
    {
        GLsizei shaderCount;
        GLsizei shaderNumber;
        
        glUseProgram(gShaderProgramObject);
        glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
        
        GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
        if (pShaders)
        {
            glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);
            
            for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
            {
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
    
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    
    [super dealloc];
}

@end


CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, 
                               const CVTimeStamp *pNow, const CVTimeStamp *pOutputTime,
                               CVOptionFlags flagsIn, CVOptionFlags *pFlagsOut,
                               void *pDisplayLinkContext)
{
    CVReturn result = [(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}

