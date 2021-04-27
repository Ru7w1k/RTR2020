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

#include "Sphere.h"

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

//// per vertex /////////////////////////////
GLuint gShaderProgramObjectPerVert;

GLuint mMatrixUniformPerVert;
GLuint vMatrixUniformPerVert;
GLuint pMatrixUniformPerVert;

GLuint laUniformPerVert[3];
GLuint ldUniformPerVert[3];
GLuint lsUniformPerVert[3];
GLuint lightPositionUniformPerVert[3];

GLuint kaUniformPerVert;
GLuint kdUniformPerVert;
GLuint ksUniformPerVert;
GLuint shininessUniformPerVert;

GLuint enableLightUniformPerVert;
//////////////////////////////////////////////

//// per fragment ////////////////////////////
GLuint gShaderProgramObjectPerFrag;

GLuint mMatrixUniformPerFrag;
GLuint vMatrixUniformPerFrag;
GLuint pMatrixUniformPerFrag;

GLuint laUniformPerFrag[3];
GLuint ldUniformPerFrag[3];
GLuint lsUniformPerFrag[3];
GLuint lightPositionUniformPerFrag[3];

GLuint kaUniformPerFrag;
GLuint kdUniformPerFrag;
GLuint ksUniformPerFrag;
GLuint shininessUniformPerFrag;

GLuint enableLightUniformPerFrag;
//////////////////////////////////////////////

GLuint vaoSphere;
GLuint vboPositionSphere;
GLuint vboNormalSphere;
GLuint vboElementSphere;

mat4 perspectiveProjectionMatrix;

float sphereVertices[1146];
float sphereNormals[1146];
float sphereTextures[1146];
unsigned short sphereElements[2280];
int gNumVertices = 0;
int gNumElements = 0;

bool bLight = false;
bool bFragment = false;

typedef struct light {
	vec3 la;
	vec3 ld;
	vec3 ls;
	vec4 position;
} Light;

Light lights[3];

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
						case XK_q:
							bDone = true;
							break;

						case XK_Escape:
							ToggleFullscreen();
							bFullscreen = !bFullscreen;
							break;

						case XK_f:
							bFragment = true;
							break;
						
						case XK_v:
							bFragment = false;
							break;

						case XK_l:
							bLight = !bLight;
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

	XStoreName(gpDisplay, gWindow, "OpenGL | Three Lights on Sphere");

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

	/// P E R   V E R T E X ////////////////////////////////////////////////////

	//// vertex shader
	// create shader
	GLuint gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCodePerVert = 
		"#version 450 core \n" \

		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \

		"uniform mat4 u_mMatrix; \n" \
		"uniform mat4 u_vMatrix; \n" \
		"uniform mat4 u_pMatrix; \n" \

		"uniform vec3 u_La[3]; \n" \
		"uniform vec3 u_Ld[3]; \n" \
		"uniform vec3 u_Ls[3]; \n" \
		"uniform vec3 u_Ka; \n" \
		"uniform vec3 u_Kd; \n" \
		"uniform vec3 u_Ks; \n" \

		"uniform float u_Shininess; \n" \
		"uniform vec4 u_LightPos[3]; \n" \
		"uniform int u_bLight; \n" \

		"out vec3 out_PhongLight; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	out_PhongLight = vec3(0.0); \n" \
		"	if (u_bLight == 1)" \
		"	{ \n" \
		"		vec4 eyeCoordinates = u_vMatrix * u_mMatrix * vPosition; \n" \
		"		vec3 tNorm = normalize(mat3(u_vMatrix * u_mMatrix) * vNormal); \n" \
		"		vec3 viewerVector = normalize(vec3(-eyeCoordinates.xyz)); \n" \

		"		for (int i = 0; i < 3; i++) \n" \
		"		{ \n" \
		"			vec3 lightDir = normalize(vec3(u_LightPos[i] - eyeCoordinates)); \n" \
		"			float tNormDotLightDir = max(dot(tNorm, lightDir), 0.0); \n" \
		"			vec3 reflectionVector = reflect(-lightDir, tNorm); \n" \

		"			vec3 ambient = u_La[i] * u_Ka; \n" \
		"			vec3 diffuse = u_Ld[i] * u_Kd * tNormDotLightDir; \n" \
		"			vec3 specular = u_Ls[i] * u_Ks * pow(max(dot(reflectionVector, viewerVector), 0.0), u_Shininess); \n" \

		"			out_PhongLight += ambient + diffuse + specular; \n" \
		"		} \n" \
		"	} \n" \
		"	else \n" \
		"	{ \n" \
		"		out_PhongLight = vec3(1.0, 1.0, 1.0); \n" \
		"	} \n" \
		"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" \
		"} \n";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCodePerVert, NULL);

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
	const GLchar *fragmentShaderSourceCodePerVert = 
		"#version 450 core \n" \

		"in vec3 out_PhongLight; \n" \
		"out vec4 FragColor; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	FragColor = vec4(out_PhongLight, 1.0); \n" \
		"} \n";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCodePerVert, NULL);

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
	gShaderProgramObjectPerVert = glCreateProgram();

	// attach shaders
	glAttachShader(gShaderProgramObjectPerVert, gVertexShaderObject);
	glAttachShader(gShaderProgramObjectPerVert, gFragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObjectPerVert, RMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerVert, RMC_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObjectPerVert);

	// linking errors
	GLint iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObjectPerVert, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerVert, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPerVert, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
				free(szInfoLog);
				exit(1);
			}
		}
	}

	// post-linking retrieving uniform locations
	mMatrixUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_mMatrix");
	vMatrixUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_vMatrix");
	pMatrixUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_pMatrix");

	for (int i = 0; i < 3; i++)
	{
		char str[64];

		sprintf(str, "u_La[%d]", i);
		laUniformPerVert[i] = glGetUniformLocation(gShaderProgramObjectPerVert, str);

		sprintf(str, "u_Ld[%d]", i);
		ldUniformPerVert[i] = glGetUniformLocation(gShaderProgramObjectPerVert, str);

		sprintf(str, "u_Ls[%d]", i);
		lsUniformPerVert[i] = glGetUniformLocation(gShaderProgramObjectPerVert, str);
	
		sprintf(str, "u_LightPos[%d]", i);
		lightPositionUniformPerVert[i] = glGetUniformLocation(gShaderProgramObjectPerVert, str);
	}

	kaUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_Ka");
	kdUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_Kd");
	ksUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_Ks");

	shininessUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_Shininess");
	enableLightUniformPerVert = glGetUniformLocation(gShaderProgramObjectPerVert, "u_bLight");

	////////////////////////////////////////////////////////////////////////////

	/// P E R   F R A G M E N T ////////////////////////////////////////////////

	//// vertex shader
	// create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCodePerFrag = 
		"#version 450 core \n" \

		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \

		"uniform mat4 u_mMatrix; \n" \
		"uniform mat4 u_vMatrix; \n" \
		"uniform mat4 u_pMatrix; \n" \

		"uniform vec4 u_LightPos[3]; \n" \
		"uniform int u_bLight; \n" \

		"out vec3 tNorm; \n" \
		"out vec3 lightDir[3]; \n" \
		"out vec3 viewerVector; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	if (u_bLight == 1)" \
		"	{ \n" \
		"		vec4 eyeCoordinates = u_vMatrix * u_mMatrix * vPosition; \n" \
		"		tNorm = mat3(u_vMatrix * u_mMatrix) * vNormal; \n" \
		"		viewerVector = normalize(vec3(-eyeCoordinates.xyz)); \n" \
		"		for (int i = 0; i < 3; i++) \n" \
		"		{ \n" \
		"			lightDir[i] = vec3(u_LightPos[i] - eyeCoordinates); \n" \
		"		} \n" \
		"	} \n" \
		"	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" \
		"} \n";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCodePerFrag, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject);

	// compilation errors 
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

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
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCodePerFrag = 
		"#version 450 core \n" \

		"in vec3 tNorm; \n" \
		"in vec3 lightDir[3]; \n" \
		"in vec3 viewerVector; \n" \

		"uniform vec3 u_La[3]; \n" \
		"uniform vec3 u_Ld[3]; \n" \
		"uniform vec3 u_Ls[3]; \n" \
		"uniform vec3 u_Ka; \n" \
		"uniform vec3 u_Kd; \n" \
		"uniform vec3 u_Ks; \n" \

		"uniform float u_Shininess; \n" \
		"uniform int u_bLight; \n" \

		"out vec4 FragColor; \n" \

		"void main (void) \n" \
		"{ \n" \
		"	if (u_bLight == 1)" \
		"	{ \n" \
		"		vec3 phongLight = vec3(0.0); \n" \
		"		vec3 normTNorm = normalize(tNorm); \n" \
		"		vec3 normViewerVector = normalize(viewerVector); \n" \

		"		for (int i = 0; i < 3; i++) \n" \
		"		{ \n" \
		"			vec3 normLightDir = normalize(lightDir[i]); \n" \
		"			vec3 reflectionVector = reflect(-normLightDir, normTNorm); \n" \
		"			float tNormDotLightDir = max(dot(normTNorm, normLightDir), 0.0); \n" \

		"			vec3 ambient = u_La[i] * u_Ka; \n" \
		"			vec3 diffuse = u_Ld[i] * u_Kd * tNormDotLightDir; \n" \
		"			vec3 specular = u_Ls[i] * u_Ks * pow(max(dot(reflectionVector, normViewerVector), 0.0), u_Shininess); \n" \
		"			phongLight += ambient + diffuse + specular; \n" \

		"		} \n" \
		"		FragColor = vec4(phongLight, 1.0); \n" \
		"	} \n" \
		"	else \n" \
		"	{ \n" \
		"		FragColor = vec4(1.0); \n" \
		"	} \n" \
		"} \n";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCodePerFrag, NULL);

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
	gShaderProgramObjectPerFrag = glCreateProgram();

	// attach shaders
	glAttachShader(gShaderProgramObjectPerFrag, gVertexShaderObject);
	glAttachShader(gShaderProgramObjectPerFrag, gFragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObjectPerFrag, RMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerFrag, RMC_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObjectPerFrag);

	// linking errors
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObjectPerFrag, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerFrag, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPerFrag, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
				free(szInfoLog);
				exit(1);
			}
		}
	}

	// post-linking retrieving uniform locations
	mMatrixUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_mMatrix");
	vMatrixUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_vMatrix");
	pMatrixUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_pMatrix");

	for (int i = 0; i < 3; i++)
	{
		char str[64];

		sprintf(str, "u_La[%d]", i);
		laUniformPerFrag[i] = glGetUniformLocation(gShaderProgramObjectPerFrag, str);

		sprintf(str, "u_Ld[%d]", i);
		ldUniformPerFrag[i] = glGetUniformLocation(gShaderProgramObjectPerFrag, str);

		sprintf(str, "u_Ls[%d]", i);
		lsUniformPerFrag[i] = glGetUniformLocation(gShaderProgramObjectPerFrag, str);
	
		sprintf(str, "u_LightPos[%d]", i);
		lightPositionUniformPerFrag[i] = glGetUniformLocation(gShaderProgramObjectPerFrag, str);
	}

	kaUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_Ka");
	kdUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_Kd");
	ksUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_Ks");

	shininessUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_Shininess");
	enableLightUniformPerFrag = glGetUniformLocation(gShaderProgramObjectPerFrag, "u_bLight");
	
	////////////////////////////////////////////////////////////////////////////

	// sphere data
	getSphereVertexData(sphereVertices, sphereNormals, sphereTextures, sphereElements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();	

	// create vao for cube
	glGenVertexArrays(1, &vaoSphere);
	glBindVertexArray(vaoSphere);

	// create vbo for position
	glGenBuffers(1, &vboPositionSphere);
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionSphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// create vbo for normal
	glGenBuffers(1, &vboNormalSphere);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormalSphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphereNormals), sphereNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(RMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(RMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// create vbo for elements
	glGenBuffers(1, &vboElementSphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElementSphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereElements), sphereElements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// set clear depth
	glClearDepth(1.0f);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// light settings
	lights[0].la = vec3(0.0f, 0.0f, 0.0f);
	lights[0].ld = vec3(1.0f, 0.0f, 0.0f);
	lights[0].ls = vec3(1.0f, 0.0f, 0.0f);
	lights[0].position = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	lights[1].la = vec3(0.0f, 0.0f, 0.0f);
	lights[1].ld = vec3(0.0f, 1.0f, 0.0f);
	lights[1].ls = vec3(0.0f, 1.0f, 0.0f);
	lights[1].position = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	lights[2].la = vec3(0.0f, 0.0f, 0.0f);
	lights[2].ld = vec3(0.0f, 0.0f, 1.0f);
	lights[2].ls = vec3(0.0f, 0.0f, 1.0f);
	lights[2].position = vec4(1.0f, 0.0f, 0.0f, 1.0f);

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

	//declaration of matrices
	mat4 translateMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;

	//// cube ////////////////////////

	// intialize above matrices to identity
	translateMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	// transformations
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translateMatrix;

	// send necessary matrices to shader in respective uniforms
	if (bFragment)
	{
		glUseProgram(gShaderProgramObjectPerFrag);
		glUniformMatrix4fv(mMatrixUniformPerFrag, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(vMatrixUniformPerFrag, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(pMatrixUniformPerFrag, 1, GL_FALSE, perspectiveProjectionMatrix);

		if (bLight)
		{
			for (int i = 0; i < 3; i++)
			{
				glUniform3fv(laUniformPerFrag[i], 1, lights[i].la);
				glUniform3fv(ldUniformPerFrag[i], 1, lights[i].ld);
				glUniform3fv(lsUniformPerFrag[i], 1, lights[i].ls);
				glUniform4fv(lightPositionUniformPerFrag[i], 1, lights[i].position);
			}
			
			glUniform3f(kaUniformPerFrag, 0.0f, 0.0f, 0.0f);
			glUniform3f(kdUniformPerFrag, 1.0f, 1.0f, 1.0f);
			glUniform3f(ksUniformPerFrag, 1.0f, 1.0f, 1.0f);
			glUniform1f(shininessUniformPerFrag, 128.0f);

			glUniform1i(enableLightUniformPerFrag, 1);
		}
		else
		{
			glUniform1i(enableLightUniformPerFrag, 0);
		}
	}
	else
	{
		glUseProgram(gShaderProgramObjectPerVert);
		glUniformMatrix4fv(mMatrixUniformPerVert, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(vMatrixUniformPerVert, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(pMatrixUniformPerVert, 1, GL_FALSE, perspectiveProjectionMatrix);

		if (bLight)
		{
			for (int i = 0; i < 3; i++)
			{
				glUniform3fv(laUniformPerVert[i], 1, lights[i].la);
				glUniform3fv(ldUniformPerVert[i], 1, lights[i].ld);
				glUniform3fv(lsUniformPerVert[i], 1, lights[i].ls);
				glUniform4fv(lightPositionUniformPerVert[i], 1, lights[i].position);
			}
			
			glUniform3f(kaUniformPerVert, 0.0f, 0.0f, 0.0f);
			glUniform3f(kdUniformPerVert, 1.0f, 1.0f, 1.0f);
			glUniform3f(ksUniformPerVert, 1.0f, 1.0f, 1.0f);
			glUniform1f(shininessUniformPerVert, 128.0f);

			glUniform1i(enableLightUniformPerVert, 1);
		}
		else
		{
			glUniform1i(enableLightUniformPerVert, 0);
		}
	}

	// bind with vaoPyramid (this will avoid many binding to vbo)
	glBindVertexArray(vaoSphere);

	// draw necessary scene
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElementSphere);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// unbind vaoPyramid
	glBindVertexArray(0);

	// stop using OpenGL program object
	glUseProgram(0);

	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
	// code
	static float angle = 0.0f;
	static float radius = 100.0f;

	float cosA = cosf(radians(angle));
	float sinA = sinf(radians(angle));

	lights[0].position = vec4(0.0f, radius * cosA, radius * sinA, 1.0f);
	lights[1].position = vec4(radius * cosA, 0.0f, radius * sinA, 1.0f);
	lights[2].position = vec4(radius * cosA, radius * sinA, 0.0f, 1.0f);

	angle += 1.0f;
	if (angle >= 360.0f) 
		angle = 0.0f;
}

void uninitialize(void)
{
	if (vaoSphere)
	{
		glDeleteVertexArrays(1, &vaoSphere);
		vaoSphere = 0;
	}

	if (vboPositionSphere)
	{
		glDeleteBuffers(1, &vboPositionSphere);
		vboPositionSphere = 0;
	}

	if (vboNormalSphere)
	{
		glDeleteBuffers(1, &vboNormalSphere);
		vboNormalSphere = 0;
	}

	if (vboElementSphere)
	{
		glDeleteBuffers(1, &vboElementSphere);
		vboElementSphere = 0;
	}

	// destroy shader programs
	if (gShaderProgramObjectPerFrag)
	{
		GLsizei shaderCount;
		GLsizei i;

		glUseProgram(gShaderProgramObjectPerFrag);
		glGetProgramiv(gShaderProgramObjectPerFrag, GL_ATTACHED_SHADERS, &shaderCount);
		
		GLuint *pShaders = (GLuint*) malloc(shaderCount * sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectPerFrag, shaderCount, &shaderCount, pShaders);

			for (i = 0; i < shaderCount; i++)
			{
				// detach shader
				glDetachShader(gShaderProgramObjectPerFrag, pShaders[i]);

				// delete shader
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObjectPerFrag);
		gShaderProgramObjectPerFrag = 0;
		glUseProgram(0);
	}

	if (gShaderProgramObjectPerVert)
	{
		GLsizei shaderCount;
		GLsizei i;

		glUseProgram(gShaderProgramObjectPerVert);
		glGetProgramiv(gShaderProgramObjectPerVert, GL_ATTACHED_SHADERS, &shaderCount);
		
		GLuint *pShaders = (GLuint*) malloc(shaderCount * sizeof(GLuint));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectPerVert, shaderCount, &shaderCount, pShaders);

			for (i = 0; i < shaderCount; i++)
			{
				// detach shader
				glDetachShader(gShaderProgramObjectPerVert, pShaders[i]);

				// delete shader
				glDeleteShader(pShaders[i]);
				pShaders[i] = 0;
			}

			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObjectPerVert);
		gShaderProgramObjectPerVert = 0;
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
