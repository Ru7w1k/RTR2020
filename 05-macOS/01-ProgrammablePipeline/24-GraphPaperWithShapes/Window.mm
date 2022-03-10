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
    RMC_ATTRIBUTE_TEXTURE0,

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
    [window setTitle:@"OpenGL | Graphpaper with Shapes"];
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

    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    GLuint vao;            // vertex array object
    GLuint vbo_vertex;    // vertex buffer object
    GLuint vbo_color;   // vertex buffer object

    GLuint vaoAxes;            // vertex array object
    GLuint vbo_vertexAxes;    // vertex buffer object
    GLuint vbo_colorAxes;    // vertex buffer object

    GLuint vaoShapes;
    GLuint vbo_vertexShapes;

    GLuint mvpUniform;
    mat4   perspectiveProjectionMatrix;

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

    // create vertex shader object
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    // vertex shader source code 
    const GLchar *vertexShaderSourceCode = (GLchar *)
        "#version 410 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec4 vColor;" \
        "out vec4 out_Color;" \
        "uniform mat4 u_mvp_matrix;" \
        "void main (void)" \
        "{" \
        "    gl_Position = u_mvp_matrix * vPosition;" \
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

            }
        }
    }

    // create fragment shader object
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // fragment shader source code
    const GLchar *fragmentShaderSourceCode = (GLchar *)
        "#version 410 core" \
        "\n" \
        "in vec4 out_Color;" \
        "out vec4 FragColor;" \
        "void main (void)" \
        "{" \
        "    FragColor = out_Color;" \
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

    // vertex array
    GLfloat *graphCoords = NULL;
    int coords = [self GenerateGraphCoordinates:&graphCoords];

    // color array
    const GLfloat axisCoords[] = {
        -1.0f,  0.0f, 0.0f,
         1.0f,  0.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
          0.0f,  1.0f, 0.0f
    };

    const GLfloat axisColors[] = {
         1.0f,  0.0f, 0.0f,
         1.0f,  0.0f, 0.0f,
         0.0f,  1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };

    GLfloat *smallAxisColors = (GLfloat *)malloc(coords * 3 * sizeof(GLfloat));
    for (int i = 0; i < (coords * 3); i += 3) {
        smallAxisColors[i + 0] = 0.0f;
        smallAxisColors[i + 1] = 0.0f;
        smallAxisColors[i + 2] = 1.0f;
    }

    GLfloat shapesCoords[1300 * 3];
    int shapesCoordsCount = 0;
    float fX, fY;

    shapesCoordsCount = [self generateOuterCircleCoords:shapesCoords :shapesCoordsCount];

    fX = fY = (GLfloat)cos(M_PI / 4.0);
    shapesCoordsCount = [self generateSquareCoords:fX :fY :shapesCoords :shapesCoordsCount];
    shapesCoordsCount = [self generateTriangleAndIncircleCoords:fX :fY :shapesCoords :shapesCoordsCount];

    // create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // vbo position axes
    glGenBuffers(1, &vbo_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
    glBufferData(GL_ARRAY_BUFFER, coords * 3 * sizeof(GLfloat), graphCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    // vbo color axes
    glGenBuffers(1, &vbo_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
    glBufferData(GL_ARRAY_BUFFER, coords * 3 * sizeof(GLfloat), smallAxisColors, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);

    // vao for Major axis
    glGenVertexArrays(1, &vaoAxes);
    glBindVertexArray(vaoAxes);

    // vbo position axes
    glGenBuffers(1, &vbo_vertexAxes);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexAxes);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisCoords), axisCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    // vbo color axes
    glGenBuffers(1, &vbo_colorAxes);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colorAxes);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisColors), axisColors, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);


    // vao for shapes
    glGenVertexArrays(1, &vaoShapes);
    glBindVertexArray(vaoShapes);

    // shapes vertices
    glGenBuffers(1, &vbo_vertexShapes);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexShapes);

    glBufferData(GL_ARRAY_BUFFER, sizeof(shapesCoords), shapesCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    glVertexAttrib3f(RMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
       
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
    
    perspectiveProjectionMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    
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
    mat4 modelViewMatrix;
    mat4 modelViewProjectionMatrix;

    // intialize above matrices to identity
    translationMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    // perform necessary transformations
    translationMatrix = translate(0.0f, 0.0f, -2.5f);

    // do necessary matrix multiplication
    modelViewMatrix *= translationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    // send necessary matrices to shader in respective uniforms
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    // bind with vao (this will avoid many binding to vbo_vertex)
    glBindVertexArray(vao);

    // bind with textures

    // draw necessary scene
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, 160);

    // bind with vao (this will avoid many binding to vbo_vertex)
    glBindVertexArray(vaoAxes);

    // draw necessary scene
    glLineWidth(3.0f);
    glDrawArrays(GL_LINES, 0, 4);

    // shapes
    glBindVertexArray(vaoShapes);

    // draw necessary scene
    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 629);
    glDrawArrays(GL_LINE_LOOP, 629, 4);
    glDrawArrays(GL_LINE_LOOP, 633, 3);
    glDrawArrays(GL_LINE_LOOP, 636, 629);

    // unbind vao
    glBindVertexArray(0);

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

- (int)GenerateGraphCoordinates:(GLfloat **)ppos
{
    int iNoOfCoords = 0;

    *ppos = (GLfloat *)malloc(3 * sizeof(GLfloat) * 160);

    GLfloat *pos = *ppos;

    for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f))
    {
        pos[(iNoOfCoords * 3) + 0] = -1.0f;
        pos[(iNoOfCoords * 3) + 1] = fOffset;
        pos[(iNoOfCoords * 3) + 2] = 0.0f;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = 1.0f;
        pos[(iNoOfCoords * 3) + 1] = fOffset;
        pos[(iNoOfCoords * 3) + 2] = 0.0f;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = -1.0f;
        pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0f + (1.0f / 20.0f);
        pos[(iNoOfCoords * 3) + 2] = 0.0f;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = 1.0f;
        pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0f + (1.0f / 20.0f);
        pos[(iNoOfCoords * 3) + 2] = 0.0f;
        iNoOfCoords++;
    }

    for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f))
    {
        pos[(iNoOfCoords * 3) + 0] = fOffset;
        pos[(iNoOfCoords * 3) + 1] = -1.0f;
        pos[(iNoOfCoords * 3) + 2] = 0.0f;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = fOffset;
        pos[(iNoOfCoords * 3) + 1] = 1.0f;
        pos[(iNoOfCoords * 3) + 2] = 0.0f;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0f + (1.0f / 20.0f);
        pos[(iNoOfCoords * 3) + 1] = -1.0f;
        pos[(iNoOfCoords * 3) + 2] = 0.0f;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0f + (1.0f / 20.0f);
        pos[(iNoOfCoords * 3) + 1] = 1.0f;
        pos[(iNoOfCoords * 3) + 2] = 0.0f;
        iNoOfCoords++;
    }

    return iNoOfCoords;
}

- (int)generateTriangleAndIncircleCoords:(float)fY :(float)fX :(GLfloat *)coords :(int)idx
{
    // variables
    GLfloat s, a, b, c;
    GLfloat fRadius = 1.0f;
    GLfloat fAngle = 0.0f;

    /* Triangle */
    coords[idx++] = 0.0f;
    coords[idx++] = fY;
    coords[idx++] = 0.0f;

    coords[idx++] = -fX;
    coords[idx++] = -fY;
    coords[idx++] = 0.0f;

    coords[idx++] = fX;
    coords[idx++] = -fY;
    coords[idx++] = 0.0f;

    /* Radius Of Incircle */
    a = (GLfloat)sqrt(pow((-fX - 0.0f), 2.0f) + pow(-fY - fY, 2.0f));
    b = (GLfloat)sqrt(pow((fX - (-fX)), 2.0f) + pow(-fY - (-fY), 2.0f));
    c = (GLfloat)sqrt(pow((fX - 0.0f), 2.0f) + pow(-fY - fY, 2.0f));
    s = (a + b + c) / 2.0f;
    fRadius = (GLfloat)sqrt(s * (s - a) * (s - b) * (s - c)) / s;

    /* Incircle */
    for (fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f)
    {
        coords[idx++] = fRadius * (GLfloat)cos(fAngle);
        coords[idx++] = (fRadius * (GLfloat)sin(fAngle)) - fX + fRadius;
        coords[idx++] = 0.0f;
    }

    return idx;
}

- (int)generateSquareCoords:(GLfloat)fX :(GLfloat)fY :(GLfloat *)coords :(int)idx {

    coords[idx++] = fX;
    coords[idx++] = fY;
    coords[idx++] = 0.0f;

    coords[idx++] = -fX;
    coords[idx++] = fY;
    coords[idx++] = 0.0f;

    coords[idx++] = -fX;
    coords[idx++] = -fY;
    coords[idx++] = 0.0f;

    coords[idx++] = fX;
    coords[idx++] = -fY;
    coords[idx++] = 0.0f;

    return idx;
}

- (int)generateOuterCircleCoords: (GLfloat *)coords :(int)idx {
    float fRadius = 1.0f;

    for (float fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f)
    {
        coords[idx++] = fRadius * (GLfloat)cos(fAngle);
        coords[idx++] = fRadius * (GLfloat)sin(fAngle);
        coords[idx++] = 0.0f;
    }

    return idx;
}

- (void) dealloc
{
    // code
    if (vbo_vertex)
    {
        glDeleteBuffers(1, &vbo_vertex);
        vbo_vertex = 0;
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
