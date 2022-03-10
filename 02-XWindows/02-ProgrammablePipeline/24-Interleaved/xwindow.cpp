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

#include <SOIL/SOIL.h>

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

GLuint gShaderProgramObject;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;

GLuint vao; // vertex array object
GLuint vbo; // vertex buffer object

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

bool bLight = false;
GLuint enableLightUniform;

mat4   perspectiveProjectionMatrix;

GLfloat angleCube = 0.0f;

const char TEX_SMILEY[] = "marble.bmp";

// light settings
GLfloat lightAmbient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[4] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat materialAmbient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat materialDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 128.0f;

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

                        case 'L':
                        case 'l':
                            if (bLight == false) bLight = true;
                            else bLight = false;
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

    XStoreName(gpDisplay, gWindow, "OpenGL | Interleaved");

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
    GLuint loadTexture(const char *path);

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
        "   }" \
        "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" \
        "   out_Texcoord = vTexcoord;" \
        "   out_Color = vColor;" \
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
        "   vec3 phong_ads_light = vec3(1.0);" \
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
        "   }" \
        "   vec4 tex = texture(u_sampler, out_Texcoord);" \
        "   FragColor = vec4((vec3(tex) * vec3(out_Color) * phong_ads_light), 1.0);" \
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
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_NORMAL, "vNormal");
    glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
         1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

        /* Bottom */
         1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,

        /* Front */
         1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,

        /* Back */
         1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

        /* Right */
        1.0f,  1.0f, -1.0f,     1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,     1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        1.0f, -1.0f,  1.0f,     1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,     1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,

        /* Left */
        -1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 1.0f
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

    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    // clear the depth buffer
	glClearDepth(1.0f);

	// clear the screen by OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    // texture
    glEnable(GL_TEXTURE_2D);
    texture_marble = loadTexture(TEX_SMILEY);

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

    if (bLight == true)
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

	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
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

// load texture
GLuint loadTexture(const char *path)
{
    GLuint texture = 0;
    int imgWidth, imgHeight;
    bool bStatus = false;

    unsigned char *imageData = SOIL_load_image(path, &imgWidth, &imgHeight, 0, SOIL_LOAD_RGB);

    if (imageData)
    {
        bStatus = true;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        SOIL_free_image_data(imageData);
    }

    return texture;
}


