// headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

#import "vmath.h"

using namespace vmath;

#define CHECK_IMG_WIDTH     64
#define CHECK_IMG_HEIGHT    64

enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0,
    
};

// types
typedef struct _Light {
    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];
    GLfloat angle;
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
    [window setTitle:@"OpenGL | Three Lights on Sphere"];
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
    
    GLuint gShaderProgramObject_pv;
    GLuint gShaderProgramObject_pf;
    
    Light lights[3];
    
    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;
    
    GLuint vao_sphere;                    // vertex array object
    GLuint vbo_position_sphere;        // vertex buffer object
    GLuint vbo_normal_sphere;            // vertex buffer object
    
    ///// P E R   V E R T E X  //////////////////////////////////////
    GLuint mUniform_pv;
    GLuint vUniform_pv;
    GLuint pUniform_pv;
    
    GLuint laUniform_red_pv;
    GLuint ldUniform_red_pv;
    GLuint lsUniform_red_pv;
    GLuint lightPositionUniform_red_pv;
    
    GLuint laUniform_green_pv;
    GLuint ldUniform_green_pv;
    GLuint lsUniform_green_pv;
    GLuint lightPositionUniform_green_pv;
    
    GLuint laUniform_blue_pv;
    GLuint ldUniform_blue_pv;
    GLuint lsUniform_blue_pv;
    GLuint lightPositionUniform_blue_pv;
    
    GLuint kaUniform_pv;
    GLuint kdUniform_pv;
    GLuint ksUniform_pv;
    GLuint shininessUniform_pv;
    GLuint enableLightUniform_pv;
    /////////////////////////////////////////////////////////////////
    
    ///// P E R   F R A G M E N T ///////////////////////////////////
    GLuint mUniform_pf;
    GLuint vUniform_pf;
    GLuint pUniform_pf;
    
    GLuint laUniform_red_pf;
    GLuint ldUniform_red_pf;
    GLuint lsUniform_red_pf;
    GLuint lightPositionUniform_red_pf;
    
    GLuint laUniform_green_pf;
    GLuint ldUniform_green_pf;
    GLuint lsUniform_green_pf;
    GLuint lightPositionUniform_green_pf;
    
    GLuint laUniform_blue_pf;
    GLuint ldUniform_blue_pf;
    GLuint lsUniform_blue_pf;
    GLuint lightPositionUniform_blue_pf;
    
    GLuint kaUniform_pf;
    GLuint kdUniform_pf;
    GLuint ksUniform_pf;
    GLuint shininessUniform_pf;
    GLuint enableLightUniform_pf;
    /////////////////////////////////////////////////////////////////
    
    mat4   perspectiveProjectionMatrix;
    BOOL bLight;
    BOOL bFragment;
    
    bool bXLight;
    bool bYLight;
    bool bZLight;
    
    int coords;
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
    GLuint vertexShaderObject_pv;
    GLuint fragmentShaderObject_pv;
    
    GLuint vertexShaderObject_pf;
    GLuint fragmentShaderObject_pf;

    [super prepareOpenGL];
    
    // OpenGL Info
    fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version   : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    [[self openGLContext]makeCurrentContext];
    
    GLint swapInt = 1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    //////////// P E R   V E R T E X ////////////////////////////////////////
    
    // create vertex shader object
    vertexShaderObject_pv = glCreateShader(GL_VERTEX_SHADER);
    
    // vertex shader source code
    const GLchar *vertexShaderSourceCode_pv = (GLchar *)
    "#version 410 core                                                                                                                    "  \
    "\n                                                                                                                                    "  \
    "in vec4 vPosition;                                                                                                                    "  \
    "in vec3 vNormal;                                                                                                                    "  \
    "uniform mat4 u_m_matrix;                                                                                                            "  \
    "uniform mat4 u_v_matrix;                                                                                                            "  \
    "uniform mat4 u_p_matrix;                                                                                                            "  \
    "uniform vec3 u_la_red;                                                                                                                "  \
    "uniform vec3 u_ld_red;                                                                                                                "  \
    "uniform vec3 u_ls_red;                                                                                                                "  \
    "uniform vec4 u_light_position_red;                                                                                                    "  \
    "uniform vec3 u_la_green;                                                                                                            "  \
    "uniform vec3 u_ld_green;                                                                                                            "  \
    "uniform vec3 u_ls_green;                                                                                                            "  \
    "uniform vec4 u_light_position_green;                                                                                                "  \
    "uniform vec3 u_la_blue;                                                                                                            "  \
    "uniform vec3 u_ld_blue;                                                                                                            "  \
    "uniform vec3 u_ls_blue;                                                                                                            "  \
    "uniform vec4 u_light_position_blue;                                                                                                "  \
    "uniform vec3 u_ka;                                                                                                                    "  \
    "uniform vec3 u_kd;                                                                                                                    "  \
    "uniform vec3 u_ks;                                                                                                                    "  \
    "uniform float u_shininess;                                                                                                            "  \
    "uniform int u_enable_light;                                                                                                        "  \
    "out vec3 phong_ads_light;                                                                                                            "  \
    "void main(void)                                                                                                                    "  \
    "{                                                                                                                                    "  \
    "   if (u_enable_light == 1)                                                                                                         "  \
    "   {                                                                                                                                 "  \
    "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;                                                                    "  \
    "       vec3 tnorm = normalize(mat3(u_v_matrix * u_m_matrix) * vNormal);                                                            "  \
    "       vec3 viewer_vector = normalize(vec3(-eye_coordinates.xyz));                                                                    "  \
    "                                                                                                                                    "  \
    "       vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));                                            "  \
    "       float tn_dot_ldir_red = max(dot(light_direction_red, tnorm), 0.0);                                                            "  \
    "       vec3 reflection_vector_red = reflect(-light_direction_red, tnorm);                                                            "  \
    "       vec3 ambient_red  = u_la_red * u_ka;                                                                                        "  \
    "       vec3 diffuse_red  = u_ld_red * u_kd * tn_dot_ldir_red;                                                                        "  \
    "       vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red, viewer_vector), 0.0), u_shininess);                "  \
    "                                                                                                                                    "  \
    "       vec3 light_direction_green = normalize(vec3(u_light_position_green - eye_coordinates));                                        "  \
    "       float tn_dot_ldir_green = max(dot(light_direction_green, tnorm), 0.0);                                                        "  \
    "       vec3 reflection_vector_green = reflect(-light_direction_green, tnorm);                                                        "  \
    "       vec3 ambient_green  = u_la_green * u_ka;                                                                                    "  \
    "       vec3 diffuse_green  = u_ld_green * u_kd * tn_dot_ldir_green;                                                                "  \
    "       vec3 specular_green = u_ls_green * u_ks * pow(max(dot(reflection_vector_green, viewer_vector), 0.0), u_shininess);            "  \
    "                                                                                                                                    "  \
    "       vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));                                        "  \
    "       float tn_dot_ldir_blue = max(dot(light_direction_blue, tnorm), 0.0);                                                        "  \
    "       vec3 reflection_vector_blue = reflect(-light_direction_blue, tnorm);                                                        "  \
    "       vec3 ambient_blue  = u_la_blue * u_ka;                                                                                        "  \
    "       vec3 diffuse_blue  = u_ld_blue * u_kd * tn_dot_ldir_blue;                                                                    "  \
    "       vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue, viewer_vector), 0.0), u_shininess);                "  \
    "                                                                                                                                    "  \
    "       phong_ads_light = ambient_red + diffuse_red + specular_red;                                                                    "  \
    "       phong_ads_light = phong_ads_light + ambient_green + diffuse_green + specular_green;                                            "  \
    "       phong_ads_light = phong_ads_light + ambient_blue + diffuse_blue + specular_blue;                                            "  \
    "   }                                                                                                                                 "  \
    "   else                                                                                                                             "  \
    "   {                                                                                                                                "  \
    "        phong_ads_light = vec3(1.0, 1.0, 1.0);                                                                                        "  \
    "   }                                                                                                                                 "  \
    "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;                                                                    "  \
    "}";
    
    // attach source code to vertex shader
    glShaderSource(vertexShaderObject_pv, 1, (const GLchar **)&vertexShaderSourceCode_pv, NULL);
    
    // compile vertex shader source code
    glCompileShader(vertexShaderObject_pv);
    
    // compilation errors
    GLint iShaderCompileStatus = 0;
    GLint iInfoLogLength = 0;
    GLchar *szInfoLog = NULL;
    
    glGetShaderiv(vertexShaderObject_pv, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_pv, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject_pv, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, "Vertex Shader Compiler Info Log: %s", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // create fragment shader object
    fragmentShaderObject_pv = glCreateShader(GL_FRAGMENT_SHADER);
    
    // fragment shader source code
    const GLchar *fragmentShaderSourceCode_pv = (GLchar *)
    "#version 410 core                                    " \
    "\n                                                    " \
    "in vec3 phong_ads_light;                            " \
    "out vec4 FragColor;                                " \
    "void main(void)                                    " \
    "{                                                    " \
    "   FragColor = vec4(phong_ads_light, 1.0);            " \
    "}";
    
    // attach source code to fragment shader
    glShaderSource(fragmentShaderObject_pv, 1, (const GLchar **)&fragmentShaderSourceCode_pv, NULL);
    
    // compile fragment shader source code
    glCompileShader(fragmentShaderObject_pv);
    
    // compile errors
    iShaderCompileStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;
    
    glGetShaderiv(fragmentShaderObject_pv, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_pv, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject_pv, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, "Fragment Shader Compiler Info Log: %s", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // create shader program object
    gShaderProgramObject_pv = glCreateProgram();
    
    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject_pv, vertexShaderObject_pv);
    
    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject_pv, fragmentShaderObject_pv);
    
    // pre-linking binding to vertex attribute
    glBindAttribLocation(gShaderProgramObject_pv, AMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject_pv, AMC_ATTRIBUTE_NORMAL, "vNormal");
    
    // link the shader program
    glLinkProgram(gShaderProgramObject_pv);
    
    // linking errors
    GLint iProgramLinkStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;
    
    glGetProgramiv(gShaderProgramObject_pv, GL_LINK_STATUS, &iProgramLinkStatus);
    if (iProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject_pv, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject_pv, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, ("Shader Program Linking Info Log: %s"), szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // post-linking retrieving uniform locations
    mUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_m_matrix");
    vUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_v_matrix");
    pUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_p_matrix");
    
    laUniform_red_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_la_red");
    ldUniform_red_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ld_red");
    lsUniform_red_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ls_red");
    lightPositionUniform_red_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_light_position_red");
    
    laUniform_green_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_la_green");
    ldUniform_green_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ld_green");
    lsUniform_green_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ls_green");
    lightPositionUniform_green_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_light_position_green");
    
    laUniform_blue_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_la_blue");
    ldUniform_blue_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ld_blue");
    lsUniform_blue_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ls_blue");
    lightPositionUniform_blue_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_light_position_blue");
    
    kaUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ka");
    kdUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_kd");
    ksUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_ks");
    shininessUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_shininess");
    
    enableLightUniform_pv = glGetUniformLocation(gShaderProgramObject_pv, "u_enable_light");
    
    /////////////////////////////////////////////////////////////////////////////
    
    //////////// P E R   F R A G M E N T ////////////////////////////////////////
    
    // create vertex shader object
    vertexShaderObject_pf = glCreateShader(GL_VERTEX_SHADER);
    
    // vertex shader source code
    const GLchar *vertexShaderSourceCode_pf = (GLchar *)
    "#version 410 core" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec3 vNormal;" \
    "uniform mat4 u_m_matrix;" \
    "uniform mat4 u_v_matrix;" \
    "uniform mat4 u_p_matrix;" \
    "uniform vec4 u_light_position_red;" \
    "uniform vec4 u_light_position_green;" \
    "uniform vec4 u_light_position_blue;" \
    "uniform int u_enable_light;" \
    "out vec3 tnorm;" \
    "out vec3 viewer_vector;" \
    "out vec3 light_direction_red;" \
    "out vec3 light_direction_green;" \
    "out vec3 light_direction_blue;" \
    "void main(void)" \
    "{" \
    "   if (u_enable_light == 1) " \
    "   { " \
    "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
    "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" \
    "       viewer_vector = vec3(-eye_coordinates.xyz);" \
    "       light_direction_red   = vec3(u_light_position_red - eye_coordinates);" \
    "       light_direction_green = vec3(u_light_position_green - eye_coordinates);" \
    "       light_direction_blue  = vec3(u_light_position_blue - eye_coordinates);" \
    "    }" \
    "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" \
    "}";
    
    // attach source code to vertex shader
    glShaderSource(vertexShaderObject_pf, 1, (const GLchar **)&vertexShaderSourceCode_pf, NULL);
    
    // compile vertex shader source code
    glCompileShader(vertexShaderObject_pf);
    
    // compilation errors
    iShaderCompileStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;
    
    glGetShaderiv(vertexShaderObject_pf, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_pf, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject_pf, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, "Vertex Shader Compiler Info Log: %s", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // create fragment shader object
    fragmentShaderObject_pf = glCreateShader(GL_FRAGMENT_SHADER);
    
    // fragment shader source code
    const GLchar *fragmentShaderSourceCode_pf = (GLchar *)
    "#version 410 core" \
    "\n" \
    "in vec3 tnorm;" \
    "in vec3 light_direction_red;" \
    "in vec3 light_direction_green;" \
    "in vec3 light_direction_blue;" \
    "in vec3 viewer_vector;" \
    "uniform vec3 u_la_red;" \
    "uniform vec3 u_ld_red;" \
    "uniform vec3 u_ls_red;" \
    "uniform vec3 u_la_green ;" \
    "uniform vec3 u_ld_green ;" \
    "uniform vec3 u_ls_green ;" \
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
    "       vec3 nlight_direction_green = normalize(light_direction_green);" \
    "       vec3 reflection_vector_green = reflect(-nlight_direction_green, ntnorm);" \
    "       float tn_dot_ldir_green = max(dot(ntnorm, nlight_direction_green), 0.0);" \
    "       vec3 ambient_green  = u_la_green * u_ka;" \
    "       vec3 diffuse_green  = u_ld_green * u_kd * tn_dot_ldir_green;" \
    "       vec3 specular_green = u_ls_green * u_ks * pow(max(dot(reflection_vector_green, nviewer_vector), 0.0), u_shininess);" \
    "                                                                                                                            " \
    "       vec3 nlight_direction_blue = normalize(light_direction_blue);" \
    "       vec3 reflection_vector_blue = reflect(-nlight_direction_blue, ntnorm);" \
    "       float tn_dot_ldir_blue = max(dot(ntnorm, nlight_direction_blue), 0.0);" \
    "       vec3 ambient_blue  = u_la_blue * u_ka;" \
    "       vec3 diffuse_blue  = u_ld_blue * u_kd * tn_dot_ldir_blue;" \
    "       vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue, nviewer_vector), 0.0), u_shininess);" \
    "                                                                                                                            " \
    "       vec3 phong_ads_light = ambient_red + diffuse_red + specular_red + ambient_green + diffuse_green + specular_green + ambient_blue + diffuse_blue + specular_blue;" \
    "       FragColor = vec4(phong_ads_light, 1.0);" \
    "    }" \
    "   else" \
    "   {" \
    "        FragColor = vec4(1.0);" \
    "   }" \
    "}";
    
    // attach source code to fragment shader
    glShaderSource(fragmentShaderObject_pf, 1, (const GLchar **)&fragmentShaderSourceCode_pf, NULL);
    
    // compile fragment shader source code
    glCompileShader(fragmentShaderObject_pf);
    
    // compile errors
    iShaderCompileStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;
    
    glGetShaderiv(fragmentShaderObject_pf, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_pf, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject_pf, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, "Fragment Shader Compiler Info Log: %s", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // create shader program object
    gShaderProgramObject_pf = glCreateProgram();
    
    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject_pf, vertexShaderObject_pf);
    
    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject_pf, fragmentShaderObject_pf);
    
    // pre-linking binding to vertex attribute
    glBindAttribLocation(gShaderProgramObject_pf, AMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject_pf, AMC_ATTRIBUTE_NORMAL, "vNormal");
    
    // link the shader program
    glLinkProgram(gShaderProgramObject_pf);
    
    // linking errors
    iProgramLinkStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;
    
    glGetProgramiv(gShaderProgramObject_pf, GL_LINK_STATUS, &iProgramLinkStatus);
    if (iProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject_pf, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject_pf, GL_INFO_LOG_LENGTH, &written, szInfoLog);
                
                fprintf(gpFile, ("Shader Program Linking Info Log: %s"), szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // post-linking retrieving uniform locations
    mUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_m_matrix");
    vUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_v_matrix");
    pUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_p_matrix");
    
    laUniform_red_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_la_red");
    ldUniform_red_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ld_red");
    lsUniform_red_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ls_red");
    lightPositionUniform_red_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_light_position_red");
    
    laUniform_green_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_la_green");
    ldUniform_green_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ld_green");
    lsUniform_green_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ls_green");
    lightPositionUniform_green_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_light_position_green");
    
    laUniform_blue_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_la_blue");
    ldUniform_blue_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ld_blue");
    lsUniform_blue_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ls_blue");
    lightPositionUniform_blue_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_light_position_blue");
    
    kaUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ka");
    kdUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_kd");
    ksUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_ks");
    shininessUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_shininess");
    
    enableLightUniform_pf = glGetUniformLocation(gShaderProgramObject_pf, "u_enable_light");
    
    /////////////////////////////////////////////////////////////////////////
    
    // vertex array
    GLfloat *sphereVertices = NULL;
    GLfloat *sphereNormals = NULL;
    GLfloat *sphereTexcoords = NULL;
    coords = [self generateSphereCoords:0.5f  slices:100  pos:&sphereVertices norm:&sphereNormals tex:&sphereTexcoords];
    
    // create vao
    glGenVertexArrays(1, &vao_sphere);
    glBindVertexArray(vao_sphere);
    
    // vertex positions
    glGenBuffers(1, &vbo_position_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_sphere);
    glBufferData(GL_ARRAY_BUFFER, 3 * coords * sizeof(GLfloat), sphereVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    
    // vertex normals
    glGenBuffers(1, &vbo_normal_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_sphere);
    glBufferData(GL_ARRAY_BUFFER, 3 * coords * sizeof(GLfloat), sphereNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
    
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
    
    //////////////////////////////////////////////////////////////////////
    
    // light configurations
    // RED light
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
    
    lights[0].lightPosition[0] = 0.0f;
    lights[0].lightPosition[1] = 0.0f;
    lights[0].lightPosition[2] = 0.0f;
    lights[0].lightPosition[3] = 1.0f;
    
    lights[0].angle = 0.0f;
    
    // GREEN light
    lights[1].lightAmbient[0] = 0.0f;
    lights[1].lightAmbient[1] = 0.0f;
    lights[1].lightAmbient[2] = 0.0f;
    lights[1].lightAmbient[3] = 1.0f;
    
    lights[1].lightDiffuse[0] = 0.0f;
    lights[1].lightDiffuse[1] = 1.0f;
    lights[1].lightDiffuse[2] = 0.0f;
    lights[1].lightDiffuse[3] = 1.0f;
    
    lights[1].lightSpecular[0] = 0.0f;
    lights[1].lightSpecular[1] = 1.0f;
    lights[1].lightSpecular[2] = 0.0f;
    lights[1].lightSpecular[3] = 1.0f;
    
    lights[1].lightPosition[0] = 0.0f;
    lights[1].lightPosition[1] = 0.0f;
    lights[1].lightPosition[2] = 0.0f;
    lights[1].lightPosition[3] = 1.0f;
    
    lights[1].angle = 0.0f;
    
    // BLUE light
    lights[2].lightAmbient[0] = 0.0f;
    lights[2].lightAmbient[1] = 0.0f;
    lights[2].lightAmbient[2] = 0.0f;
    lights[2].lightAmbient[3] = 1.0f;
    
    lights[2].lightDiffuse[0] = 0.0f;
    lights[2].lightDiffuse[1] = 0.0f;
    lights[2].lightDiffuse[2] = 1.0f;
    lights[2].lightDiffuse[3] = 1.0f;
    
    lights[2].lightSpecular[0] = 0.0f;
    lights[2].lightSpecular[1] = 0.0f;
    lights[2].lightSpecular[2] = 1.0f;
    lights[2].lightSpecular[3] = 1.0f;
    
    lights[2].lightPosition[0] = 0.0f;
    lights[2].lightPosition[1] = 0.0f;
    lights[2].lightPosition[2] = 0.0f;
    lights[2].lightPosition[3] = 1.0f;
    
    lights[2].angle = 0.0f;
    
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
    
    materialShininess = 128.0f;
    
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
    if (bFragment == YES)
        glUseProgram(gShaderProgramObject_pf);
    else
        glUseProgram(gShaderProgramObject_pv);
    
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
    translationMatrix *= translate(0.0f, 0.0f, -3.0f);
    
    // do necessary matrix multiplication
    modelMatrix *= translationMatrix;
    
    if (bFragment == YES)
    {
        // send necessary matrices to shader in respective uniforms
        glUniformMatrix4fv(mUniform_pf, 1, false, modelMatrix);
        glUniformMatrix4fv(vUniform_pf, 1, false, ViewMatrix);
        glUniformMatrix4fv(pUniform_pf, 1, false, perspectiveProjectionMatrix);
        
        if (bLight == YES)
        {
            glUniform3fv(laUniform_red_pf, 1, lights[0].lightAmbient);
            glUniform3fv(ldUniform_red_pf, 1, lights[0].lightDiffuse);
            glUniform3fv(lsUniform_red_pf, 1, lights[0].lightSpecular);
            glUniform4f(lightPositionUniform_red_pf, 0.0f, 100.0f*cos(lights[0].angle), 100.0f*sin(lights[0].angle), 1.0f);
            
            glUniform3fv(laUniform_green_pf, 1, lights[1].lightAmbient);
            glUniform3fv(ldUniform_green_pf, 1, lights[1].lightDiffuse);
            glUniform3fv(lsUniform_green_pf, 1, lights[1].lightSpecular);
            glUniform4f(lightPositionUniform_green_pf, 100.0f*cos(lights[1].angle), 0.0f, 100.0f*sin(lights[1].angle), 1.0f);
            
            glUniform3fv(laUniform_blue_pf, 1, lights[2].lightAmbient);
            glUniform3fv(ldUniform_blue_pf, 1, lights[2].lightDiffuse);
            glUniform3fv(lsUniform_blue_pf, 1, lights[2].lightSpecular);
            glUniform4f(lightPositionUniform_blue_pf, 100.0f*cos(lights[2].angle), 100.0f*sin(lights[2].angle), 0.0f, 1.0f);
            
            glUniform3fv(kaUniform_pf, 1, materialAmbient);
            glUniform3fv(kdUniform_pf, 1, materialDiffuse);
            glUniform3fv(ksUniform_pf, 1, materialSpecular);
            glUniform1f(shininessUniform_pf, materialShininess);
            glUniform1i(enableLightUniform_pf, 1);
        }
        else
            glUniform1i(enableLightUniform_pf, 0);
    }
    else
    {
        // send necessary matrices to shader in respective uniforms
        glUniformMatrix4fv(mUniform_pv, 1, false, modelMatrix);
        glUniformMatrix4fv(vUniform_pv, 1, false, ViewMatrix);
        glUniformMatrix4fv(pUniform_pv, 1, false, perspectiveProjectionMatrix);
        
        if (bLight == YES)
        {
            glUniform3fv(laUniform_red_pv, 1, lights[0].lightAmbient);
            glUniform3fv(ldUniform_red_pv, 1, lights[0].lightDiffuse);
            glUniform3fv(lsUniform_red_pv, 1, lights[0].lightSpecular);
            glUniform4f(lightPositionUniform_red_pv, 0.0f, 100.0f*cos(lights[0].angle), 100.0f*sin(lights[0].angle), 1.0f);
            
            glUniform3fv(laUniform_green_pv, 1, lights[1].lightAmbient);
            glUniform3fv(ldUniform_green_pv, 1, lights[1].lightDiffuse);
            glUniform3fv(lsUniform_green_pv, 1, lights[1].lightSpecular);
            glUniform4f(lightPositionUniform_green_pv, 100.0f*cos(lights[1].angle), 0.0f, 100.0f*sin(lights[1].angle), 1.0f);
            
            glUniform3fv(laUniform_blue_pv, 1, lights[2].lightAmbient);
            glUniform3fv(ldUniform_blue_pv, 1, lights[2].lightDiffuse);
            glUniform3fv(lsUniform_blue_pv, 1, lights[2].lightSpecular);
            glUniform4f(lightPositionUniform_blue_pv, 100.0f*cos(lights[2].angle), 100.0f*sin(lights[2].angle), 0.0f, 1.0f);
            
            glUniform3fv(kaUniform_pv, 1, materialAmbient);
            glUniform3fv(kdUniform_pv, 1, materialDiffuse);
            glUniform3fv(ksUniform_pv, 1, materialSpecular);
            glUniform1f(shininessUniform_pv, materialShininess);
            glUniform1i(enableLightUniform_pv, 1);
        }
        else
            glUniform1i(enableLightUniform_pv, 0);
    }
    
    glBindVertexArray(vao_sphere);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3*coords);
    
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
    if (lights[0].angle < 360.0f)
    {
        lights[0].angle += 0.02f;
    }
    else
    {
        lights[0].angle = 0.0f;
    }
    
    if (lights[1].angle < 360.0f)
    {
        lights[1].angle += 0.02f;
    }
    else
    {
        lights[1].angle = 0.0f;
    }
    
    if (lights[2].angle < 360.0f)
    {
        lights[2].angle += 0.02f;
    }
    else
    {
        lights[2].angle = 0.0f;
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
            [[self window]toggleFullScreen:self]; // repainting occurs automatically
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
            
        case 'E':
        case 'e':
            [self release];
            [NSApp terminate:self];
            break;
            
        case 'F':
        case 'f':
            bFragment = YES;
            break;
            
        case 'V':
        case 'v':
            bFragment = NO;
            break;
            
        case 'X':
        case 'x':
            bXLight = !bXLight;
            break;
            
        case 'Y':
        case 'y':
            bYLight = !bYLight;
            break;
            
        case 'Z':
        case 'z':
            bZLight = !bZLight;
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

- (int)generateSphereCoords:(GLfloat)r slices:(int)n pos:(GLfloat **)ppos norm:(GLfloat **)pnorm tex:(GLfloat **)ptex
{
    int iNoOfCoords = 0;
    int i, j;
    GLdouble phi1, phi2, theta, s, t;
    GLfloat ex, ey, ez, px, py, pz;
    
    *ppos  = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
    *pnorm = (GLfloat *)malloc(3 * sizeof(GLfloat) * n * (n + 1) * 2);
    *ptex  = (GLfloat *)malloc(2 * sizeof(GLfloat) * n * (n + 1) * 2);
    
    GLfloat *pos  = *ppos;
    GLfloat *norm = *pnorm;
    GLfloat *tex  = *ptex;
    
    if (ppos && pnorm && ptex)
    {
        //iNoOfCoords = n * (n + 1);
        
        for (j = 0; j < n; j++) {
            phi1 = j * M_PI * 2 / n;
            phi2 = (j + 1) * M_PI * 2 / n;
            
            //fprintf(gpFile, "phi1 [%g]...\n", phi1);
            //fprintf(gpFile, "phi2 [%g]...\n", phi2);
            
            for (i = 0; i <= n; i++) {
                theta = i * M_PI / n;
                
                ex = sin(theta) * cos(phi2);
                ey = sin(theta) * sin(phi2);
                ez = cos(theta);
                px = r * ex;
                py = r * ey;
                pz = r * ez;
                
                //glNormal3f(ex, ey, ez);
                norm[(iNoOfCoords*3) + 0] = ex;
                norm[(iNoOfCoords*3) + 1] = ey;
                norm[(iNoOfCoords*3) + 2] = ez;
                
                s = phi2 / (M_PI * 2);   // column
                t = 1 - (theta / M_PI);  // row
                //glTexCoord2f(s, t);
                tex[(iNoOfCoords*2) + 0] = s;
                tex[(iNoOfCoords*2) + 1] = t;
                
                //glVertex3f(px, py, pz);
                pos[(iNoOfCoords*3) + 0] = px;
                pos[(iNoOfCoords*3) + 1] = py;
                pos[(iNoOfCoords*3) + 2] = pz;
                
                /*fprintf(gpFile, "pos[%d]...\n", (iNoOfCoords * 3) + 0);
                 fprintf(gpFile, "pos[%d]...\n", (iNoOfCoords * 3) + 1);
                 fprintf(gpFile, "pos[%d]...\n", (iNoOfCoords * 3) + 2);*/
                
                ex = sin(theta) * cos(phi1);
                ey = sin(theta) * sin(phi1);
                ez = cos(theta);
                px = r * ex;
                py = r * ey;
                pz = r * ez;
                
                //glNormal3f(ex, ey, ez);
                norm[(iNoOfCoords*3) + 3] = ex;
                norm[(iNoOfCoords*3) + 4] = ey;
                norm[(iNoOfCoords*3) + 5] = ez;
                
                s = phi1 / (M_PI * 2);   // column
                t = 1 - (theta / M_PI);  // row
                //glTexCoord2f(s, t);
                tex[(iNoOfCoords*2) + 2] = s;
                tex[(iNoOfCoords*2) + 3] = t;
                
                //glVertex3f(px, py, pz);
                pos[(iNoOfCoords*3) + 3] = px;
                pos[(iNoOfCoords*3) + 4] = py;
                pos[(iNoOfCoords*3) + 5] = pz;
                
                /*fprintf(gpFile, "pos[%d]...\n", (iNoOfCoords * 3) + 3);
                 fprintf(gpFile, "pos[%d]...\n", (iNoOfCoords * 3) + 4);
                 fprintf(gpFile, "pos[%d]...\n", (iNoOfCoords * 3) + 5);*/
                
                iNoOfCoords += 2;
            }
        }
    }
    
    return iNoOfCoords;
}

- (void) dealloc
{
    if (vbo_position_sphere)
    {
        glDeleteBuffers(1, &vbo_position_sphere);
        vbo_position_sphere = 0;
    }
    
    if (vbo_normal_sphere)
    {
        glDeleteBuffers(1, &vbo_normal_sphere);
        vbo_normal_sphere = 0;
    }
    
    if (vao_sphere)
    {
        glDeleteVertexArrays(1, &vao_sphere);
        vao_sphere = 0;
    }
    
    if (gShaderProgramObject_pf)
    {
        GLsizei shaderCount;
        GLsizei shaderNumber;
        
        glUseProgram(gShaderProgramObject_pf);
        glGetProgramiv(gShaderProgramObject_pf, GL_ATTACHED_SHADERS, &shaderCount);
        
        GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
        if (pShaders)
        {
            glGetAttachedShaders(gShaderProgramObject_pf, shaderCount, &shaderCount, pShaders);
            
            for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
            {
                // detach shader
                glDetachShader(gShaderProgramObject_pf, pShaders[shaderNumber]);
                
                // delete shader
                glDeleteShader(pShaders[shaderNumber]);
                pShaders[shaderNumber] = 0;
            }
            free(pShaders);
        }
        
        glDeleteProgram(gShaderProgramObject_pf);
        gShaderProgramObject_pf = 0;
        glUseProgram(0);
    }
    
    if (gShaderProgramObject_pv)
    {
        GLsizei shaderCount;
        GLsizei shaderNumber;
        
        glUseProgram(gShaderProgramObject_pv);
        glGetProgramiv(gShaderProgramObject_pv, GL_ATTACHED_SHADERS, &shaderCount);
        
        GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);
        if (pShaders)
        {
            glGetAttachedShaders(gShaderProgramObject_pv, shaderCount, &shaderCount, pShaders);
            
            for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
            {
                // detach shader
                glDetachShader(gShaderProgramObject_pv, pShaders[shaderNumber]);
                
                // delete shader
                glDeleteShader(pShaders[shaderNumber]);
                pShaders[shaderNumber] = 0;
            }
            free(pShaders);
        }
        
        glDeleteProgram(gShaderProgramObject_pv);
        gShaderProgramObject_pv = 0;
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

