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

GLuint gVertexShaderObject;
GLuint gGeometryShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint vao;
GLuint vbo;
GLuint mvpUniform;
mat4   perspectiveProjectionMatrix;

// entry-point function
int main(void)
{
    // function prototypes
    void CreateWindow(void);
    void ToggleFullscreen(void);
    void initialize(void);
    void resize(int, int);
    void display(void);
    void uninitialize(void);

    // variable declarations
    bool bDone = false;
    int winWidth = giWindowWidth;
    int winHeight = giWindowHeight;
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

        // update();
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

    XStoreName(gpDisplay, gWindow, "OpenGL | Geometry Shader");

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
        "in vec3 vPosition;" \
        "void main(void)" \
        "{" \
        "   gl_Position = vec4(vPosition, 1.0);" \
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

    // create geometry shader object
	gGeometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);

	// tessellation control shader source code
	const GLchar *geometryShaderSourceCode = (GLchar *)
		"#version 450 core" \
		"\n" \
		"layout(triangles)in;" \
		"layout(triangle_strip, max_vertices=9)out;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main (void)" \
		"{" \
		"	for(int vertex = 0; vertex < 3; vertex++)" \
		"	{" \
		"		gl_Position = u_mvp_matrix * (gl_in[vertex].gl_Position + vec4(0.0, 0.5, 0.0, 1.0));" \
		"		EmitVertex();" \
		"		gl_Position = u_mvp_matrix * (gl_in[vertex].gl_Position + vec4(-0.5, -0.5, 0.0, 1.0));" \
		"		EmitVertex();" \
		"		gl_Position = u_mvp_matrix * (gl_in[vertex].gl_Position + vec4(0.5, -0.5, 0.0, 1.0));" \
		"		EmitVertex();" \
		"		EndPrimitive();" \
		"	}" \
		"}";

	// attach source code to fragment shader
	glShaderSource(gGeometryShaderObject, 1, (const GLchar **)&geometryShaderSourceCode, NULL);

	// compile fragment shader source code
	glCompileShader(gGeometryShaderObject);

	// compile errors
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gGeometryShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				printf("Geometry Shader Compiler Info Log: %s", szInfoLog);
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
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "   FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
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

    // attach geometry shader to shader program
	glAttachShader(gShaderProgramObject, gGeometryShaderObject);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    // pre-linking binding to vertex attribute
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_POSITION, "vPosition");

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


    // vertex array
	const float vertices[] = {
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

    // create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    // clear the depth buffer
	glClearDepth(1.0f);

	// clear the screen by OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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

	perspectiveProjectionMatrix =  perspective(45.0, (float)width / (float)height, 0.1f, 100.0f);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use shader program
	glUseProgram(gShaderProgramObject);

	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	///// RECTANGLE   ////////////////////////////////////////////////////////////////////////

	// intialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	// perform necessary transformations	
	modelViewMatrix = translate(0.0f, 0.0f, -6.0f);

	// do necessary matrix multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	// bind with vao (this will avoid many binding to vbo_vertex)
	glBindVertexArray(vao);

	// glPatchParameteri(GL_PATCH_VERTICES, 4);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	// unbind vao
	glBindVertexArray(0);

    // unuse program
    glUseProgram(0);

	glXSwapBuffers(gpDisplay, gWindow);
}

void uninitialize(void)
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



