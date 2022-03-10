// headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

#import "vmath.h"
#import "stack.h"

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
    [window setTitle:@"OpenGL | Solar System"];
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
    mat4   perspectiveProjectionMatrix;
    
    GLuint mvpUniform;
    
    int day;
    int year;
    
    GLuint vao_sphere;                    // vertex array object
    GLuint vbo_position_sphere;        // vertex buffer object
    GLuint vbo_normal_sphere;            // vertex buffer object
    
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
    "uniform mat4 u_mvp_matrix;" \
    "out vec4 outColor;" \
    "void main(void)" \
    "{" \
    "   gl_Position = u_mvp_matrix * vPosition;" \
    "   outColor = vColor;" \
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
    "in vec4 outColor;" \
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
    "   FragColor = outColor;" \
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
    mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
    
    /////////////////////////////////////////////////////////////////////////////

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
    
    glVertexAttrib3f(RMC_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.05f);
    
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
    mat4 scaleMatrix;
    mat4 modelViewMatrix;
    mat4 modelViewProjectionMatrix;

    // intialize above matrices to identity
    translationMatrix         = mat4::identity();
    rotationMatrix            = mat4::identity();
    scaleMatrix               = mat4::identity();
    modelViewMatrix           = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();
    push(mat4::identity());

    // do necessary matrix multiplication
    vec3 pos = vec3(0.0f, 0.0f, 5.0f);
    vec3 center = vec3(0.0f, 0.0f, 0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    modelViewMatrix *= lookat(pos, center, up);
    push(modelViewMatrix);

    modelViewMatrix *= rotate(90.0f, 0.0f, 0.0f);
    push(modelViewMatrix);

    // SUN
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    modelViewProjectionMatrix = perspectiveProjectionMatrix * peek();
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(vao_sphere);
    glVertexAttrib3f(RMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3 * coords);
    glBindVertexArray(0);
    pop();

    modelViewMatrix = peek();
    modelViewMatrix *= rotate(0.0f, (float)year, 0.0f);
    modelViewMatrix *= translate(1.5f, 0.0f, 0.0f);
    modelViewMatrix *= rotate(90.0f, 0.0f, 0.0f);
    modelViewMatrix *= rotate(00.0f, 0.0f, (float)day);
    modelViewMatrix *= scale(0.3f, 0.3f, 0.3f);
    push(modelViewMatrix);

    // EARTH
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    modelViewProjectionMatrix = perspectiveProjectionMatrix * peek();
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(vao_sphere);
    glVertexAttrib3f(RMC_ATTRIBUTE_COLOR, 0.4f, 0.9f, 1.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3 * coords);
    glBindVertexArray(0);
    pop();

    modelViewMatrix = peek();
    modelViewMatrix *= rotate(0.0f, (float)year, 0.0f);
    modelViewMatrix *= translate(1.5f, 0.0f, 0.0f);
    modelViewMatrix *= rotate(90.0f, 0.0f, 0.0f);
    modelViewMatrix *= rotate(00.0f, 0.0f, (float)day);
    modelViewMatrix *= translate(0.6f, 0.0f, 0.0f);
    modelViewMatrix *= scale(0.25f, 0.25f, 0.25f);
    push(modelViewMatrix);

    // MOON
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    modelViewProjectionMatrix = perspectiveProjectionMatrix * peek();
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(vao_sphere);
    glVertexAttrib3f(RMC_ATTRIBUTE_COLOR, 0.9f, 0.9f, 0.9f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3 * coords);
    glBindVertexArray(0);
    pop();

    resetStack();

    // unuse program
    glUseProgram(0);
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
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
            
        case 'Y':
            year = (year + 3) % 360;
            break;

        case 'y':
            year = (year - 3) % 360;
            break;

        case 'D':
            day = (day + 6) % 360;
            break;

        case 'd':
            day = (day - 6) % 360;
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

