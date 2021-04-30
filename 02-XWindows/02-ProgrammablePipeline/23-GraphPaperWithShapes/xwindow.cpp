// headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/glew.h> // for programmable pipeline APIs
#include <GL/gl.h>   // for OpenGL APIs
#include <GL/glx.h>  // for GLX APIs (bridging API)

#include "vmath.h"

// namespace
using namespace std;
using namespace vmath;

// vertex shader attributes indices
enum {
	RMC_ATTRIBUTE_POSITION = 0,
	RMC_ATTRIBUTE_COLOR,
	RMC_ATTRIBUTE_NORMAL,
	RMC_ATTRIBUTE_TEXCOORD,
};

// global variable declarations
Display *gpDisplay = NULL;
Window gWindow;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
FILE* gpFile = NULL;

GLXContext gGLXContext;

typedef GLXContext (*glXCreateContextAttribsARBProc) (Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;

bool bFullscreen = false;
int giWindowWidth = 800;
int giWindowHeight = 600;

GLuint gShaderProgramObject;

GLuint vao;			// vertex array object
GLuint vbo_vertex;	// vertex buffer object
GLuint vbo_color;   // vertex buffer object

GLuint vaoAxes;			// vertex array object
GLuint vbo_vertexAxes;	// vertex buffer object
GLuint vbo_colorAxes;	// vertex buffer object

GLuint vaoShapes;
GLuint vbo_vertexShapes;

GLuint mvpUniform;
mat4   perspectiveProjectionMatrix;

// entry-point function
int main(void)
{
	// function prototypes
	void CreateWindow(void);
	void ToggleFullscreen(void);

	void initialize(void);
	void display(void);
	void update(void);
	void resize(int, int);
	void uninitialize(void);

	// variable declarations
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;

	bool bDone = false;

	// code
	// open file for logging
	gpFile = fopen("RMCLog.txt", "w");
	if (!gpFile)
	{
		printf("Cannot open RMCLog.txt file..\n");
		exit(0);
	}
	fprintf(gpFile, "==== Application Started ====\n");

	CreateWindow();
	initialize();

	// game loop
	XEvent event;
	KeySym keysym;

	while (bDone == false)
	{
		while (XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);
			switch (event.type)
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

						case XK_F:
						case XK_f:
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

						default:
							break;
					}
					break;

				case ButtonPress:
					switch (event.xbutton.button)
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
	int styleMask;

	GLXFBConfig *pGLXFBConfigs = NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo = NULL;
	int numFBConfigs = 0;

	// ~ pixel format descriptor
	static int frameBufferAttributes[] = {
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_DOUBLEBUFFER, True,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		None // here '0' is also possible
	};

	// code
	gpDisplay = XOpenDisplay(NULL);
	if (gpDisplay == NULL)
	{
		fprintf(gpFile, "ERROR: Unable to open XDisplay.\nExiting..\n");
		uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	// retrieve all FBConfigs
	pGLXFBConfigs = glXChooseFBConfig(gpDisplay, defaultScreen, frameBufferAttributes, &numFBConfigs);
	fprintf(gpFile, "There are %d matching FBConfigs..\n", numFBConfigs);

	int bestFramebufferConfig  = -1;
	int bestNoOfSamples        = -1;
	int worstFramebufferConfig = -1;
	int worstNoOfSamples       = 999;

	for (int i = 0; i < numFBConfigs; i++)
	{
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfigs[i]);

		// for each FBConfig, check for best config
		if (pTempXVisualInfo)
		{
			int sampleBuffers, samples;

			// get the number of sample buffers from FBConfig
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfigs[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);

			// get the number of samples from FBConfig
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfigs[i], GLX_SAMPLES, &samples);

			// check for best config
			if (bestFramebufferConfig < 0 || sampleBuffers && samples > bestNoOfSamples)
			{
				bestFramebufferConfig = i;
				bestNoOfSamples = samples;
			}

			// check for worst config
			if (worstFramebufferConfig < 0 || !sampleBuffers || samples < worstNoOfSamples)
			{
				worstFramebufferConfig = i;
				worstNoOfSamples = samples;
			}

			fprintf(gpFile, "FBConfig %d: SampleBuffers -> %d Samples -> %d XVisualInfoID -> %lu\n", i, sampleBuffers, samples, pTempXVisualInfo->visualid);
		}
		XFree(pTempXVisualInfo);
	}

	// assign best FBConfig
	bestGLXFBConfig = pGLXFBConfigs[bestFramebufferConfig];

	// assign the same to global
	gGLXFBConfig = bestGLXFBConfig;

	// free the obtained GLXFBConfig array
	XFree(pGLXFBConfigs);

	gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGLXFBConfig);
	if (gpXVisualInfo == NULL)
	{
		fprintf(gpFile, "ERROR: unable to get XVisualInfo..\nExiting..\n");
		uninitialize();
		exit(1);
	}

	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay,
		RootWindow(gpDisplay, gpXVisualInfo->screen),
		gpXVisualInfo->visual,
		AllocNone);
	gColormap = winAttribs.colormap;
	winAttribs.background_pixel = BlackPixel(gpDisplay, gpXVisualInfo->depth);

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
		fprintf(gpFile, "ERROR: Failed to create Main Window.\nExiting..\n");
		uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay, gWindow, "OpenGL | Graphpaper with Shapes");

	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);

	XMapWindow(gpDisplay, gWindow);
}

void ToggleFullscreen(void)
{
	// variable declarations
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {0};

	// code
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	memset(&xev, 0, sizeof(XEvent));

	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = bFullscreen ? 0:1;

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
	// function declarations
	void resize(int, int);
	void uninitialize(void);

	int GenerateGraphCoordinates(GLfloat **ppos);
	int generateTriangleAndIncircleCoords(float fY, float fX, GLfloat *coords, int idx);
	int generateSquareCoords(GLfloat fX, GLfloat fY, GLfloat *coords, int idx);
	int generateOuterCircleCoords(GLfloat *coords, int idx);

	// code
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddress((GLubyte *)"glXCreateContextAttribsARB");

    if (glXCreateContextAttribsARB == NULL)
    {
        fprintf(gpFile, "glXCreateContextAttribsARB: Procedure not found...\nExiting...\n");
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
    gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);

    if (!gGLXContext)
    {
		fprintf(gpFile, "OpenGL 4.5 context failed..\nCreating context with highest possible version..\n");
        GLint attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };

        gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
    }

    if (!glXIsDirect(gpDisplay, gGLXContext))
    {
        fprintf(gpFile, "Rendering context is NOT HW rendering context!\n");
    }
    else 
    {
        fprintf(gpFile, "Rendering context is HW rendering context!\n");
    }
    
    glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	GLenum result = glewInit();
	if (result != GLEW_OK) {
		fprintf(gpFile, "GLEW initialization failed..\n");
		uninitialize();
		XDestroyWindow(gpDisplay, gWindow);
    }
    fprintf(gpFile, "GLEW: init successful..\n");

	// fetch OpenGL related details
	fprintf(gpFile, "OpenGL Vendor:   %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version:  %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// fetch OpenGL enabled extensions
	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "==== OpenGL Extensions ====\n");
	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "  %s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	fprintf(gpFile, "===========================\n\n");

	//// vertex shader
	// create shader
	GLuint gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCode = 
		"#version 450 core \n" \

		"in vec4 vPosition; \n" \
		"in vec4 vColor; \n" \

		"uniform mat4 u_mvpMatrix; \n" \

		"out vec4 out_Color; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
		"	out_Color = vColor; \n" \
		"} \n";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject);

	// compilation errors 
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
				glGetShaderInfoLog(gVertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Vertex Shader Compiler Info Log: \n%s\n", szInfoLog);
				free(szInfoLog);
				exit(1);
			}
		}
	}

	//// fragment shader
	// create shader
	GLuint gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCode = 
		"#version 450 core \n" \

		"in vec4 out_Color; \n" \
		"out vec4 FragColor; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	FragColor = out_Color; \n" \
		"} \n";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	// compile shader
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

				fprintf(gpFile, "Fragment Shader Compiler Info Log: \n%s\n", szInfoLog);
				free(szInfoLog);
				exit(1);
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
	glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_COLOR, "vColor");

	// link shader
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

				fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
				free(szInfoLog);
				exit(1);
			}
		}
	}

	// post-linking retrieving uniform locations
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
	
	// vertex array
	GLfloat *graphCoords = NULL;
	int coords = GenerateGraphCoordinates(&graphCoords);

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

	shapesCoordsCount = generateOuterCircleCoords(shapesCoords, shapesCoordsCount);

	fX = fY = (GLfloat)cos(M_PI / 4.0);
	shapesCoordsCount = generateSquareCoords(fX, fY, shapesCoords, shapesCoordsCount);
	shapesCoordsCount = generateTriangleAndIncircleCoords(fX, fY, shapesCoords, shapesCoordsCount);

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

	// set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// set clear depth
	glClearDepth(1.0f);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	perspectiveProjectionMatrix = mat4::identity();

	// warmup resize
	resize(giWindowWidth, giWindowHeight);
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (float)width/(float)height, 0.1f, 100.0f);
}

void display(void)
{
	// code
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

	// stop using OpenGL program object
	glUseProgram(0);

	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
	// code
}

void uninitialize(void)
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo_vertex)
	{
		glDeleteBuffers(1, &vbo_vertex);
		vbo_vertex = 0;
	}

	if (vbo_color)
	{
		glDeleteBuffers(1, &vbo_color);
		vbo_color = 0;
	}

	if (vaoAxes)
	{
		glDeleteVertexArrays(1, &vaoAxes);
		vaoAxes = 0;
	}

	if (vbo_vertexAxes)
	{
		glDeleteBuffers(1, &vbo_vertexAxes);
		vbo_vertexAxes = 0;
	}

	if (vbo_colorAxes)
	{
		glDeleteBuffers(1, &vbo_colorAxes);
		vbo_colorAxes = 0;
	}

	if (vaoShapes)
	{
		glDeleteVertexArrays(1, &vaoShapes);
		vaoShapes = 0;
	}

	if (vbo_vertexShapes)
	{
		glDeleteBuffers(1, &vbo_vertexShapes);
		vbo_vertexShapes = 0;
	}

	// destroy shader programs
	if (gShaderProgramObject)
	{
		GLsizei shaderCount;
		GLsizei i;

		glUseProgram(gShaderProgramObject);
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		
		GLuint *pShaders = (GLuint*) malloc(shaderCount * sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

			for (i = 0; i < shaderCount; i++)
			{
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
	
	GLXContext currentGLXContext = glXGetCurrentContext();
	if (currentGLXContext != NULL && currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}

	if (gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}

	if (gWindow)
	{
		XDestroyWindow(gpDisplay, gWindow);
	}

	if (gColormap)
	{
		XFreeColormap(gpDisplay, gColormap);
	}

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

	if (gpFile)
	{
		fprintf(gpFile, "==== Application Terminated ====\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

int GenerateGraphCoordinates(GLfloat **ppos)
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

int generateTriangleAndIncircleCoords(float fY, float fX, GLfloat *coords, int idx) 
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

int generateSquareCoords(GLfloat fX, GLfloat fY, GLfloat *coords, int idx) {

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

int generateOuterCircleCoords(GLfloat *coords, int idx) {
	float fRadius = 1.0f;

	for (float fAngle = 0.0f; fAngle < 2 * M_PI; fAngle += 0.01f)
	{
		coords[idx++] = fRadius * (GLfloat)cos(fAngle);
		coords[idx++] = fRadius * (GLfloat)sin(fAngle);
		coords[idx++] = 0.0f;
	}

	return idx;
}
