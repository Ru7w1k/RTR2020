// headers 

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "vmath.h"

// namespaces
using namespace std;
using namespace vmath;

enum {
    RMC_ATTRIBUTE_POSITION = 0,
    RMC_ATTRIBUTE_COLOR,
    RMC_ATTRIBUTE_NORMAL,
    RMC_ATTRIBUTE_TEXCOORD
};

// global variable declarations
static GLXContext gGLXContext;  /* static is required for multiple display configuration */
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;

typedef GLXContext (*glxCreateContextAttribsARBProc) (Display*, GLXFBConfig, GLXContext, Bool, const int *);
glxCreateContextAttribsARBProc glxCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;

int giWindowWidth = 800;
int giWindowHeight = 600;

int winWidth;
int winHeight;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVertexShaderObject_FBO;
GLuint gFragmentShaderObject_FBO;
GLuint gShaderProgramObject_FBO;

GLuint vaoPyramid;				// vertex array object
GLuint vaoCube;					// vertex array object
GLuint vbo_position_pyramid;	// vertex buffer object
GLuint vbo_color_pyramid;		// vertex buffer object
GLuint vbo_position_cube;		// vertex buffer object
GLuint vbo_cube_texture;
GLuint mvpUniform;
mat4   perspectiveProjectionMatrix;

GLuint mvpUniform_FBO;
GLuint samplerUniform_FBO;
GLuint FBO;
GLuint texture_FBO;
GLuint rboDepth;

float anglePyramid = 0.0f;
float angleCube = 0.0f;

// entry-point function
int main(void)
{
    // function prototypes
    void CreateWindow(void);
    void ToggleFullscreen(void);
    void initialize(void);
    void resize(int, int);
    void display(void);
    void update(void);
    void uninitialize(void);

    // variable declarations
    bool bDone = false;
    winWidth = giWindowWidth;
    winHeight = giWindowHeight;
    char keys[26];

    // code
    CreateWindow();
    initialize();

    // Message loop
    XEvent event;
    KeySym keysym;

    while(!bDone)
    {
        while(XPending(gpDisplay))
        {
            XNextEvent(gpDisplay, &event);
            switch(event.type)
            {
                case MapNotify:
                    break;

                case KeyPress:
                    keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
                    switch (keysym)
                    {
                        case XK_Escape:
                            bDone = true;
                            break;

                        default:
                            break;
                    }

                    XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
                    switch (keys[0])
                    {
                        case 'F':
                        case 'f':
                            if (bFullscreen == false)
                            {
                                ToggleFullscreen();
                                bFullscreen = true;
                            }
                            else
                            {
                                ToggleFullscreen();
                                bFullscreen = false;
                            }
                            break;
                    }
                    break;

                case ButtonPress:
                    switch(event.xbutton.button)
                    {
                        case 1:
                            break;

                        case 2:
                            break;

                        case 3:
                            break;

                        default:
                            break;
                    }
                    break;

                case MotionNotify:
                    break;

                case ConfigureNotify:
                    winWidth = event.xconfigure.width;
                    winHeight = event.xconfigure.height;
                    resize(winWidth, winHeight);
                    break;

                case Expose:
                    break;

                case DestroyNotify:
                    break;

                case 33:
                    bDone = true;
                    break;

                default:
                    break;
            }
        }

        update();
        display();
    }

    uninitialize();
    return(0);
}

void CreateWindow(void)
{
    // function prototypes
    void uninitialize(void);

    // variable declarations
    XSetWindowAttributes winAttribs;
    int defaultScreen;
    int defaultDepth;
    int styleMask;

    GLXFBConfig *pGLXFBConfigs = NULL;
    GLXFBConfig bestGLXFBConfig;
    XVisualInfo *pTempXVisualInfo = NULL;
    int iNoOfFBConfigs = 0;

    static int frameBufferAttributes[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };

    // code
    gpDisplay = XOpenDisplay(NULL);
    if (gpDisplay == NULL)
    {
        printf("ERROR: Unable to open X Display...\nTerminating...");
        uninitialize();
        exit(1);
    }

    defaultScreen = XDefaultScreen(gpDisplay);

    defaultDepth = DefaultDepth(gpDisplay, defaultScreen);

    // get the best VisualInfo for opengl 4.5 context
    // retrive all FBConfigs from driver
    pGLXFBConfigs = glXChooseFBConfig(gpDisplay, defaultScreen, frameBufferAttributes, &iNoOfFBConfigs);
    printf("There are %d matching configs\n", iNoOfFBConfigs);

    int bestFrameBufferConfig  = -1;
    int bestNoOfSamples        = -1;
    int worstFrameBufferConfig = -1;
    int worstNoOfSamples       = 999;

    for (int i = 0; i < iNoOfFBConfigs; i++)
    {
        pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfigs[i]);

        // for each FBConfig, check the compatibility
        if (pTempXVisualInfo)
        {
            int sampleBuffers, samples;

            // get number of sample buffers from respective FBConfig
            glXGetFBConfigAttrib(gpDisplay, 
                pGLXFBConfigs[i],
                GLX_SAMPLE_BUFFERS,
                &sampleBuffers);

            //get number of samples from respective FBConfig
            glXGetFBConfigAttrib(gpDisplay,
                pGLXFBConfigs[i],
                GLX_SAMPLES,
                &samples);

            // get the best of them all
            if (bestFrameBufferConfig < 0 || sampleBuffers && samples > bestNoOfSamples)
            {
                bestFrameBufferConfig = i;
                bestNoOfSamples = samples;
            }

            // get the worst of them all
            if (worstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNoOfSamples)
            {
                worstFrameBufferConfig = i;
                worstNoOfSamples = samples;
            }

        }
        XFree(pTempXVisualInfo);
    }

    // assign the found best one
    bestGLXFBConfig = pGLXFBConfigs[bestFrameBufferConfig];

    // assign the same to global one
    gGLXFBConfig = bestGLXFBConfig;

    // free the obtained GLXFBConfig array
    XFree(pGLXFBConfigs);

    gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGLXFBConfig);
    if (gpXVisualInfo == NULL)
    {
        printf("ERROR: Unable to get XVisualInfo...\nTerminating...");
        uninitialize();
        exit(1);
    }

    winAttribs.border_pixel = 0;
    winAttribs.border_pixmap = 0;
    winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);
    winAttribs.background_pixmap = 0;
    winAttribs.colormap = XCreateColormap(gpDisplay,
                            RootWindow(gpDisplay, gpXVisualInfo->screen),
                            gpXVisualInfo->visual,
                            AllocNone);
    
    gColormap = winAttribs.colormap;
    
    winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;

    styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

    gWindow = XCreateWindow(gpDisplay,
                RootWindow(gpDisplay, gpXVisualInfo->screen),
                0,
                0,
                giWindowWidth,
                giWindowHeight,
                0,
                gpXVisualInfo->depth,
                InputOutput,
                gpXVisualInfo->visual,
                styleMask,
                &winAttribs);

    if (!gWindow)
    {
        printf("ERROR: failed to create main window...\nTerminating...");
        uninitialize();
        exit(1);
    }

    XStoreName(gpDisplay, gWindow, "Framebuffer");

    Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
    
    XMapWindow(gpDisplay, gWindow);
}

void ToggleFullscreen(void)
{
    // variable declaration
    Atom wm_state;
    Atom fullscreen;
    XEvent xev = { 0 };

    // code
    wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
    memset(&xev, 0, sizeof(xev));

    xev.type = ClientMessage;
    xev.xclient.window = gWindow;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = bFullscreen ? 0 : 1;
    
    fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
    xev.xclient.data.l[1] = fullscreen;

    XSendEvent(gpDisplay,
        RootWindow(gpDisplay, gpXVisualInfo->screen),
        False,
        StructureNotifyMask,
        &xev);
}

void initialize(void)
{
    // variables
    GLenum result;

    // function prototypes
    void resize(int, int);
    void uninitialize(void);

    // code
    glxCreateContextAttribsARB = (glxCreateContextAttribsARBProc) glXGetProcAddress((GLubyte *)"glXCreateContextAttribsARB");

    if (glxCreateContextAttribsARB == NULL)
    {
        printf("glxCreateContextAttribsARB: Procedure not found...\nTerminating...\n");
        uninitialize();
        exit(1);
    }

    GLint attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 5,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    // now get the context
    gGLXContext = glxCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);

    if (!gGLXContext)
    {
        GLint attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };

        gGLXContext = glxCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
    }

    if (!glXIsDirect(gpDisplay, gGLXContext))
    {
        printf("The obtained context is NOT h/w rendering context\n");
    }
    else 
    {
        printf("The obtained context is h/w rendering context!\n");
    }
    
    glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
    
    ////// Programmable Pipeline /////////////////////////////////////////////////////////
    // GLEW
	result = glewInit();
	if (result != GLEW_OK) {
		printf("GLEW initialization failed..\n");
		uninitialize();
		XDestroyWindow(gpDisplay, gWindow);
    }
    printf("glew: init successful..\n");

    // create vertex shader object
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    // vertex shader source code
    const GLchar *vertexShaderSourceCode = (GLchar *)
        "#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"out vec4 out_Color;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main (void)" \
		"{" \
		"	gl_Position = u_mvp_matrix * vPosition;" \
		"	out_Color = vColor;" \
		"}";

    // attach source code to vertex shader
    glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

    // compile the vertex shader source code
    glCompileShader(gVertexShaderObject);

    // compile errors
    GLint iShaderCompileStatus = 0;
    GLint iInfoLogLength = 0;
    GLchar *szInfoLog = NULL;

    glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObject, GL_INFO_LOG_LENGTH,&written, szInfoLog);

   				printf("Vertex Shader Compiler Info Log: %s", szInfoLog);
                free(szInfoLog);
                uninitialize();
                exit(0);
            }
        }
    }

    // create fragment shader object
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // fragment shader source code
    const GLchar *fragmentShaderSourceCode = (GLchar *)
        "#version 450 core" \
		"\n" \
		"in vec4 out_Color;" \
		"out vec4 FragColor;" \
		"void main (void)" \
		"{" \
		"	FragColor = out_Color;" \
		"}";

    // attach source code to fragment shader
    glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

    // compile fragment shader source code
    glCompileShader(gFragmentShaderObject);

    // compile errors
    iShaderCompileStatus = 0;
    iInfoLogLength = 0;
    szInfoLog = NULL;

    glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				printf("Fragment Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

    // create shader program object
    gShaderProgramObject = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject, gVertexShaderObject);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    // pre-linking binding to vertex attribute
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_COLOR, "vColor");

    // link shader program
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

				printf("Shader Program Linking Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

    // post-linking retriving uniform locations
    mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

    ///////// draw to texture //////////////////////////////////////////////////////

	// create vertex shader object
	gVertexShaderObject_FBO = glCreateShader(GL_VERTEX_SHADER);

	// vertex shader source code 
	const GLchar *vertexShaderSourceCode_FBO = (GLchar *)
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexcoord;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec2 out_Texcoord;" \
		"void main (void)" \
		"{" \
		"	gl_Position = u_mvp_matrix * vPosition;" \
		"	out_Texcoord = vTexcoord;" \
		"}";

	// attach source code to vertex shader
	glShaderSource(gVertexShaderObject_FBO, 1, (const GLchar **)&vertexShaderSourceCode_FBO, NULL);

	// compile vertex shader source code
	glCompileShader(gVertexShaderObject_FBO);

	// compilation errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_FBO, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_FBO, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				printf("Vertex Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// create fragment shader object
	gFragmentShaderObject_FBO = glCreateShader(GL_FRAGMENT_SHADER);

	// fragment shader source code
	const GLchar *fragmentShaderSourceCode_FBO = (GLchar *)
		"#version 450 core" \
		"\n" \
		"in vec2 out_Texcoord;" \
		"uniform sampler2D u_sampler;" \
		"out vec4 FragColor;" \
		"void main (void)" \
		"{" \
		"	FragColor = texture(u_sampler, out_Texcoord);" \
		"}";

	// attach source code to fragment shader
	glShaderSource(gFragmentShaderObject_FBO, 1, (const GLchar **)&fragmentShaderSourceCode_FBO, NULL);

	// compile fragment shader source code
	glCompileShader(gFragmentShaderObject_FBO);

	// compile errors
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject_FBO, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_FBO, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				printf("Fragment Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// create shader program object 
	gShaderProgramObject_FBO = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject_FBO, gVertexShaderObject_FBO);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject_FBO, gFragmentShaderObject_FBO);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObject_FBO, RMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject_FBO, RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

	// link the shader program
	glLinkProgram(gShaderProgramObject_FBO);

	// linking errors
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObject_FBO, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_FBO, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				printf(("Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// post-linking retrieving uniform locations
	mvpUniform_FBO = glGetUniformLocation(gShaderProgramObject_FBO, "u_mvp_matrix");
	samplerUniform_FBO = glGetUniformLocation(gShaderProgramObject_FBO, "u_sampler");

	////////////////////////////////////////////////////////////////////////////////

    // vertex array
	const GLfloat pyramidVertices[] = {
		/* Front */
		 0.0f,  1.0f,  0.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,

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

		  ///* bottom surface */
		  //- 1.0f, -1.0, -1.0f,
		  //1.0f, -1.0, -1.0f,
		  //-1.0f, -1.0, -1.0f,

		  //-1.0f, -1.0, -1.0f,
		  //-1.0f, -1.0, -1.0f,
		  //-1.0f, -1.0, -1.0f,


	};

	const GLfloat cubeVertices[] = {
		/* Top */
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,

		 /* Bottom */
		  1.0f, -1.0f,  1.0f,
		 -1.0f, -1.0f,  1.0f,
		 -1.0f, -1.0f, -1.0f,
		  1.0f, -1.0f, -1.0f,

		  /* Front */
		   1.0f,  1.0f,  1.0f,
		  -1.0f,  1.0f,  1.0f,
		  -1.0f, -1.0f,  1.0f,
		   1.0f, -1.0f,  1.0f,

		   /* Back */
			1.0f, -1.0f, -1.0f,
		   -1.0f, -1.0f, -1.0f,
		   -1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,

			/* Right */
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,

			/* Left */
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f
	};

	const GLfloat pyramidColors[] = {
		/* Front */
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		/* Right */
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,

		/* Left */
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,

		/* Back */
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	//const GLfloat cubeColors[] = {
	//	/* Top */
	//	1.0f, 0.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,

	//	/* Bottom */
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,

	//	/* Front */
	//	0.0f, 0.0f, 1.0f,
	//	0.0f, 0.0f, 1.0f,
	//	0.0f, 0.0f, 1.0f,
	//	0.0f, 0.0f, 1.0f,

	//	/* Back */
	//	0.0f, 1.0f, 1.0f,
	//	0.0f, 1.0f, 1.0f,
	//	0.0f, 1.0f, 1.0f,
	//	0.0f, 1.0f, 1.0f,

	//	/* Right */
	//	1.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 1.0f,

	//	/* Left */
	//	1.0f, 1.0f, 0.0f,
	//	1.0f, 1.0f, 0.0f,
	//	1.0f, 1.0f, 0.0f,
	//	1.0f, 1.0f, 0.0f
	//};

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
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

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
		0.0f, 1.0f
	};

	// create vao
	glGenVertexArrays(1, &vaoPyramid);
	glBindVertexArray(vaoPyramid);

	// vertex positions
	glGenBuffers(1, &vbo_position_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

	// vertex colors
	glGenBuffers(1, &vbo_color_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColors), pyramidColors, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// create vao
	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	// vertex positions
	glGenBuffers(1, &vbo_position_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

	// vertex texture
	glGenBuffers(1, &vbo_cube_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_TEXCOORD);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	///////// FrameBuffer stuff //////////////////////////////////////////

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &texture_FBO);
	glBindTexture(GL_TEXTURE_2D, texture_FBO);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1024, 1024);

	// turn off mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_FBO, 0);

	static const GLenum draw_buffers[] = {
		GL_COLOR_ATTACHMENT0
	};

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	glDrawBuffers(1, draw_buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	printf("\nAdded framebuffer facility...");

    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    // clear the depth buffer
	glClearDepth(1.0f);

	// clear the screen by OpenGL
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	// enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);

	perspectiveProjectionMatrix = mat4::identity();

	// warm-up call to resize
	resize(giWindowWidth, giWindowHeight);
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0, (float)width / (float)height, 0.1f, 100.0f);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use shader program
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	///// PYRAMID ///////////////////////////////////////////////////////////////////////////

	// intialize above matrices to identity
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, 1024, 1024);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// perform necessary transformations
	translationMatrix = translate(0.0f, 0.15f, -4.0f);
	rotationMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
	scaleMatrix = scale(0.75f, 0.75f, 0.75f);

	// do necessary matrix multiplication
	modelViewMatrix *= translationMatrix;
	modelViewMatrix *= rotationMatrix;
	modelViewMatrix *= scaleMatrix;
	modelViewProjectionMatrix = perspective(45.0, 1.0f, 0.1f, 100.0f) * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform_FBO, 1, GL_FALSE, modelViewProjectionMatrix);

	// bind with vao (this will avoid many binding to vbo_vertex)
	glBindVertexArray(vaoPyramid);

	// draw necessary scene
	glDrawArrays(GL_TRIANGLES, 0, 12);

	// unbind vao
	glBindVertexArray(0);

	// unuse program
	glUseProgram(0);

	// unuse framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, winWidth, winHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////////////////////

	///// CUBE ///////////////////////////////////////////////////////////////////////////////

	// use shader program
	glUseProgram(gShaderProgramObject_FBO);

	// intialize above matrices to identity
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	// perform necessary transformations
	translationMatrix *= translate(0.0f, 0.0f, -5.0f);
	rotationMatrix *= rotate(angleCube, angleCube, angleCube);
	scaleMatrix = scale(0.75f, 0.75f, 0.75f);

	// do necessary matrix multiplication
	modelViewMatrix *= translationMatrix;
	modelViewMatrix *= rotationMatrix;
	modelViewMatrix *= scaleMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_FBO);
	glUniform1f(GL_TEXTURE0, 0);

	// bind with vao (this will avoid many binding to vbo_vertex)
	glBindVertexArray(vaoCube);

	// draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	// unbind vao
	glBindVertexArray(0);

	////////////////////////////////////////////////////////////////////////////////////////

    // unuse program
    glUseProgram(0);

	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
    if (anglePyramid >= 360.0)
	{
		anglePyramid = 0.0;
	}
	else
	{
		anglePyramid += 1.0f;
	}

	if (angleCube >= 360.0)
	{
		angleCube = 0.0;
	}
	else
	{
		angleCube += 0.5f;
	}
}

void uninitialize(void)
{
if (texture_FBO)
	{
		glDeleteBuffers(1, &texture_FBO);
		texture_FBO = 0;
	}

	if (FBO)
	{
		glDeleteFramebuffers(1, &FBO);
		FBO = 0;
	}

	if (vbo_position_pyramid)
	{
		glDeleteBuffers(1, &vbo_position_pyramid);
		vbo_position_pyramid = 0;
	}

	if (vbo_color_pyramid)
	{
		glDeleteBuffers(1, &vbo_color_pyramid);
		vbo_color_pyramid = 0;
	}

	if (vbo_position_cube)
	{
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}

	if (vbo_cube_texture)
	{
		glDeleteBuffers(1, &vbo_cube_texture);
		vbo_cube_texture = 0;
	}

	if (vaoCube)
	{
		glDeleteVertexArrays(1, &vaoCube);
		vaoCube = 0;
	}

	if (vaoPyramid)
	{
		glDeleteVertexArrays(1, &vaoPyramid);
		vaoPyramid = 0;
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

    GLXContext currentContext = glXGetCurrentContext();
    if (currentContext != NULL && currentContext == gGLXContext)
    {
        glXMakeCurrent(gpDisplay, 0, 0);
    }

    if (gGLXContext)
    {
        glXDestroyContext(gpDisplay, gGLXContext);
    }

    if (gWindow)
        XDestroyWindow(gpDisplay, gWindow);

    if (gColormap)
        XFreeColormap(gpDisplay, gColormap);
    
    if (gpXVisualInfo)
    {
        free(gpXVisualInfo);
        gpXVisualInfo = NULL;
    }

    if (gpDisplay)
    {
        XCloseDisplay(gpDisplay);
        gpDisplay = NULL;
    }
        
}



