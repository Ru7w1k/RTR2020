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
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0,
    
};

// light settings
typedef struct _Light {
    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];
} Light;

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
    [window setTitle:@"OpenGL| Two Lights on Pyramid"];
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

    Light lights[2];

    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;

    GLuint vao_pyramid;                    // vertex array object
    GLuint vbo_position_pyramid;        // vertex buffer object
    GLuint vbo_normal_pyramid;            // vertex buffer object

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

    mat4   perspectiveProjectionMatrix;
    BOOL   bLight;
    GLfloat anglePyramid;
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
        
        bLight = FALSE;
        anglePyramid = 0.0f;
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
    "in vec3 vNormal;" \
    "uniform mat4 u_m_matrix;" \
    "uniform mat4 u_v_matrix;" \
    "uniform mat4 u_p_matrix;" \
    "uniform vec4 u_light_position_red;" \
    "uniform vec4 u_light_position_blue;" \
    "uniform int u_enable_light;" \
    "out vec3 tnorm;" \
    "out vec3 viewer_vector;" \
    "out vec3 light_direction_red;" \
    "out vec3 light_direction_blue;" \
    "void main(void)" \
    "{" \
    "   if (u_enable_light == 1) " \
    "   { " \
    "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
    "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" \
    "       viewer_vector = vec3(-eye_coordinates.xyz);" \
    "       light_direction_red   = vec3(u_light_position_red - eye_coordinates);" \
    "       light_direction_blue  = vec3(u_light_position_blue - eye_coordinates);" \
    "    }" \
    "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" \
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
   "in vec3 tnorm;" \
    "in vec3 light_direction_red;" \
    "in vec3 light_direction_blue;" \
    "in vec3 viewer_vector;" \
    "uniform vec3 u_la_red;" \
    "uniform vec3 u_ld_red;" \
    "uniform vec3 u_ls_red;" \
    "uniform vec3 u_la_blue;" \
    "uniform vec3 u_ld_blue;" \
    "uniform vec3 u_ls_blue;" \
    "uniform vec3 u_ka;" \
    "uniform vec3 u_kd;" \
    "uniform vec3 u_ks;" \
    "uniform float u_shininess;" \
    "uniform int u_enable_light;" \
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
    "   if (u_enable_light == 1) " \
    "   { " \
    "       vec3 ntnorm = normalize(tnorm);" \
    "       vec3 nviewer_vector = normalize(viewer_vector);" \
    "                                                                                                                            " \
    "       vec3 nlight_direction_red = normalize(light_direction_red);" \
    "       vec3 reflection_vector_red = reflect(-nlight_direction_red, ntnorm);" \
    "       float tn_dot_ldir_red = max(dot(ntnorm, nlight_direction_red), 0.0);" \
    "       vec3 ambient_red  = u_la_red * u_ka;" \
    "       vec3 diffuse_red  = u_ld_red * u_kd * tn_dot_ldir_red;" \
    "       vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red, nviewer_vector), 0.0), u_shininess);" \
    "                                                                                                                            " \
    "       vec3 nlight_direction_blue = normalize(light_direction_blue);" \
    "       vec3 reflection_vector_blue = reflect(-nlight_direction_blue, ntnorm);" \
    "       float tn_dot_ldir_blue = max(dot(ntnorm, nlight_direction_blue), 0.0);" \
    "       vec3 ambient_blue  = u_la_blue * u_ka;" \
    "       vec3 diffuse_blue  = u_ld_blue * u_kd * tn_dot_ldir_blue;" \
    "       vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue, nviewer_vector), 0.0), u_shininess);" \
    "                                                                                                                            " \
    "       vec3 phong_ads_light = ambient_red + diffuse_red + specular_red + ambient_blue + diffuse_blue + specular_blue;" \
    "       FragColor = vec4(phong_ads_light, 1.0);" \
    "    }" \
    "   else" \
    "   {" \
    "        FragColor = vec4(1.0);" \
    "   }" \
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
    glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
    
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
        -1.0f, -1.0f, -1.0f
    };

    const GLfloat pyramideNormals[] = {
        /* Front */
        0.0f, 0.447214f, 0.894427f,
        0.0f, 0.447214f, 0.894427f,
        0.0f, 0.447214f, 0.894427f,

        /* Right */
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,

        /* Left */
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,

        /* Back */
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f
    };

    // create vao
    glGenVertexArrays(1, &vao_pyramid);
    glBindVertexArray(vao_pyramid);

    // vertex positions
    glGenBuffers(1, &vbo_position_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramideVertices), pyramideVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

    // vertex normals
    glGenBuffers(1, &vbo_normal_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_pyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramideNormals), pyramideNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
        
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

    // clear the depth buffer
    glClearDepth(1.0f);

    // enable depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // clear the screen by OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
    translationMatrix *= translate(0.0f, 0.0f, -6.0f);
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
    
    //////////////////////////////////////////////////////////////////////////////////////////
    
    // unuse program
    glUseProgram(0);
    
    [self step];
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

- (void)step
{
      if (anglePyramid >= 360.0)
      {
          anglePyramid = 0.0;
      }
      else
      {
          anglePyramid += 1.0f;
      }
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
            
        case 'L':
        case 'l':
            if (bLight == YES) {
                bLight = NO;
            }
            else {
                bLight = YES;
            }
            break;
            
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self]; // repainting occurs automatically
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
    if (vbo_position_pyramid)
    {
        glDeleteBuffers(1, &vbo_position_pyramid);
        vbo_position_pyramid = 0;
    }

    if (vbo_normal_pyramid)
    {
        glDeleteBuffers(1, &vbo_normal_pyramid);
        vbo_normal_pyramid = 0;
    }

    if (vao_pyramid)
    {
        glDeleteVertexArrays(1, &vao_pyramid);
        vao_pyramid = 0;
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

