// headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

#import "vmath.h"
#import "materials.h"

using namespace vmath;

#define CHECK_IMG_WIDTH     64
#define CHECK_IMG_HEIGHT    64

enum
{
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

// 'C' style global function declaration
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *);

// global variables
FILE *gpFile = NULL;
float gWidth, gHeight;

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
    [window setTitle:@"OpenGL | Materials"];
    [window center];
    
    fprintf(gpFile, "Scale Factor: %f\n", [window backingScaleFactor]);
    
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
    
    Light lights[3];
    
    GLuint vao_sphere;                    // vertex array object
    GLuint vbo_position_sphere;        // vertex buffer object
    GLuint vbo_normal_sphere;            // vertex buffer object
    
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
    
        bLight = NO;
        
        bXLight = true;
        bYLight = false;
        bZLight = false;
        
        InitMaterials();
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
    
    //////////// P E R   F R A G M E N T ////////////////////////////////////////
    
    // create vertex shader object
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    // vertex shader source code
    const GLchar *vertexShaderSourceCode = (GLchar *)
    "#version 410 core" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec3 vNormal;" \
    "uniform mat4 u_m_matrix;" \
    "uniform mat4 u_v_matrix;" \
    "uniform mat4 u_p_matrix;" \
    "uniform vec4 u_light_position;" \
    "uniform int u_enable_light;" \
    "out vec3 tnorm;" \
    "out vec3 viewer_vector;" \
    "out vec3 light_direction;" \
    "void main(void)" \
    "{" \
    "   if (u_enable_light == 1) " \
    "   { " \
    "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
    "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" \
    "       viewer_vector = vec3(-eye_coordinates.xyz);" \
    "       light_direction   = vec3(u_light_position - eye_coordinates);" \
    "    }" \
    "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" \
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
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
    "   if (u_enable_light == 1) " \
    "   { " \
    "       vec3 ntnorm = normalize(tnorm);" \
    "       vec3 nviewer_vector = normalize(viewer_vector);" \
    "                                                                                                                            " \
    "       vec3 nlight_direction = normalize(light_direction);" \
    "       vec3 reflection_vector = reflect(-nlight_direction, ntnorm);" \
    "       float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);" \
    "       vec3 ambient  = u_la * u_ka;" \
    "       vec3 diffuse  = u_ld * u_kd * tn_dot_ldir;" \
    "       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), u_shininess);" \
    "                                                                                                                            " \
    "       vec3 phong_ads_light = ambient + diffuse + specular;" \
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
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_NORMAL, "vNormal");
    
    // link the shader program
    glLinkProgram(gShaderProgramObject);
    
    // linking errors
    int iProgramLinkStatus = 0;
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
    coords = [self generateSphereCoords:0.5f  slices:100  pos:&sphereVertices norm:&sphereNormals tex:&sphereTexcoords];
    
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
    
    // clear the depth buffer
    glClearDepth(1.0f);
    
    // clear the screen by OpenGL
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    
    // enable depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
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
    rect = [self convertRectToBacking:rect];
    
    GLfloat width = rect.size.width;
    GLfloat height = rect.size.height;
    
    if (height == 0)
        height = 1;
    
    gWidth = width;
    gHeight = height;
    
    fprintf(gpFile, "\nResize with %f %f", gWidth, gHeight);
    
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

    ///// CUBE ///////////////////////////////////////////////////////////////////////////////

    // intialize above matrices to identity
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    ViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    // send necessary matrices to shader in respective uniforms
    glUniformMatrix4fv(vUniform, 1, false, ViewMatrix);


    if (bLight == YES)
    {
        glUniform3fv(laUniform, 1, lights[0].lightAmbient);
        glUniform3fv(ldUniform, 1, lights[0].lightDiffuse);
        glUniform3fv(lsUniform, 1, lights[0].lightSpecular);
        
        if(bXLight)
            glUniform4f(lightPositionUniform, 0.0f, 100.0f*cos(lights[0].angle), 100.0f*sin(lights[0].angle), 1.0f);

        if(bYLight)
            glUniform4f(lightPositionUniform, 100.0f*cos(lights[1].angle), 0.0f, 100.0f*sin(lights[1].angle), 1.0f);

        if(bZLight)
            glUniform4f(lightPositionUniform, 100.0f*cos(lights[2].angle), 100.0f*sin(lights[2].angle), 0.0f, 1.0f);

        glUniform1i(enableLightUniform, 1);
    }
    else
        glUniform1i(enableLightUniform, 0);

    // gWidth = 2880.0f;
    // gHeight = 1800.0f;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            glViewport(
                (float)i * (float)gWidth / 6.0, (float)j * (float)gHeight / 4.0,
                (GLsizei)(float)gWidth / 6.0, (GLsizei)(float)gHeight / 4.0);

            perspectiveProjectionMatrix = perspective(60.0, ((GLsizei)gWidth / 6.0) / ((GLsizei)gHeight / 4.0), 0.1f, 40.0f);

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
    
    [self step];
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    
}

- (void)step
{
   if (bXLight) {
        if (lights[0].angle < 360.0f)
        {
            lights[0].angle += 0.02f;
        }
        else
        {
            lights[0].angle = 0.0f;
        }
    }

    if (bYLight) {
        if (lights[1].angle < 360.0f)
        {
            lights[1].angle += 0.02f;
        }
        else
        {
            lights[1].angle = 0.0f;
        }
    }

    if (bZLight) {
        if (lights[2].angle < 360.0f)
        {
            lights[2].angle += 0.02f;
        }
        else
        {
            lights[2].angle = 0.0f;
        }
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
        case 'F':
        case 'f':
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
            
        case 27:
            [self release];
            [NSApp terminate:self];
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

