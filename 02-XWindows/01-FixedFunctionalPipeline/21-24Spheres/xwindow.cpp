// headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/gl.h>   // for OpenGL APIs
#include <GL/glu.h>  // for OpenGL utility functions
#include <GL/glx.h>  // for GLX APIs (bridging API)

// namespace
using namespace std;

// global variable declarations
Display *gpDisplay = NULL;
Window gWindow;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;

GLXContext gGLXContext;

bool bFullscreen = false;
int giWindowWidth = 800;
int giWindowHeight = 600;

bool bLight = false;

GLfloat lightAmbient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat materialAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[]  = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess = 128.0f;

GLfloat light_model_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light_model_local_viewer[] = { 0.0f };

GLUquadric *quadric[24];
GLfloat angleOfXRotation = 0.0f;
GLfloat angleOfYRotation = 0.0f;
GLfloat angleOfZRotation = 0.0f;

GLuint keyPressed = 0;

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

						case XK_L:
						case XK_l:
							bLight = !bLight;
							if (bLight)
							{
								glEnable(GL_LIGHTING);
							}
							else
							{
								glDisable(GL_LIGHTING);
							}
							break;

						case XK_X:
						case XK_x:
							keyPressed = 1;
							angleOfXRotation = 0.0f;
							break;

						case XK_Y:
						case XK_y:
							keyPressed = 2;
							angleOfYRotation = 0.0f;
							break;

						case XK_Z:
						case XK_z:
							keyPressed = 3;
							angleOfZRotation = 0.0f;
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

	// ~ pixel format descriptor
	static int frameBufferAttributes[] = {
		GLX_DOUBLEBUFFER, True,
		GLX_RGBA,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		None // here '0' is also possible
	};

	// code
	gpDisplay = XOpenDisplay(NULL);
	if (gpDisplay == NULL)
	{
		printf("ERROR: Unable to open XDisplay.\nExiting..\n");
		uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	if (gpXVisualInfo == NULL)
	{
		printf("ERROR: Unable to get a Visual.\nExiting..\n");
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
		printf("ERROR: Failed to create Main Window.\nExiting..\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay, gWindow, "OpenGL | 24 Spheres");

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
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	// opengl
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	// light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

	for (int i = 0; i < 24; i++)
	{
		quadric[i] = gluNewQuadric();
	}

	// warmup resize
	resize(giWindowWidth, giWindowHeight);
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (width <= height)
	{
		glOrtho(0.0,
			15.5,
			(0.0 * ((GLdouble)height / (GLdouble)width)),
			(15.5 * ((GLdouble)height / (GLdouble)width)),
			-10.0,
			10.0);
	}
	else
	{
		glOrtho((0.0 * ((GLdouble)width / (GLdouble)height)),
			(15.5 * ((GLdouble)width / (GLdouble)height)),
			0.0,
			15.5,
			-10.0,
			10.0);
	}
}

void display(void)
{
	// function declarations
	void Draw24Spheres(void);

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (keyPressed == 1)
	{
		glRotatef(angleOfXRotation, 1.0f, 0.0f, 0.0f);
		lightPosition[1] = 100.0f;
	}
	else if (keyPressed == 2)
	{
		glRotatef(angleOfYRotation, 0.0f, 1.0f, 0.0f);
		lightPosition[2] = 100.0f;
	}
	else if (keyPressed == 3)
	{
		glRotatef(angleOfZRotation, 0.0f, 0.0f, 1.0f);
		lightPosition[0] = 100.0f;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	Draw24Spheres();

	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
	// code
	if (angleOfXRotation < 360)
	{
		angleOfXRotation += 0.75;
	}
	else
	{
		angleOfXRotation = 0.0;
	}

	if (angleOfYRotation < 360)
	{
		angleOfYRotation += 0.75;
	}
	else
	{
		angleOfYRotation = 0.0;
	}

	if (angleOfZRotation < 360)
	{
		angleOfZRotation += 0.75;
	}
	else
	{
		angleOfZRotation = 0.0;
	}
}

void uninitialize(void)
{
	if (quadric)
	{
		for (int i = 0; i < 24; i++)
		{
			gluDeleteQuadric(quadric[i]);
		}
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
}

void Draw24Spheres(void) 
{
	GLfloat dX = 2.5f;
	GLfloat dY = 2.0f;
	GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuse[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShininess[1];
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	#pragma region Gems

	////// Emerald /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0215f; // r
	MaterialAmbient[1] = 0.1745f; // g
	MaterialAmbient[2] = 0.0215f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.07568f; // r
	MaterialDiffuse[1] = 0.61424f; // g
	MaterialDiffuse[2] = 0.07568f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.633f;    // r
	MaterialSpecular[1] = 0.727811f; // g
	MaterialSpecular[2] = 0.633f;    // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.6 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[0], 1.0f, 30, 30);
	dX += 4.5f;

	////// Jade /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.135f;  // r
	MaterialAmbient[1] = 0.2225f; // g
	MaterialAmbient[2] = 0.1575f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.54f; // r
	MaterialDiffuse[1] = 0.89f; // g
	MaterialDiffuse[2] = 0.63f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.316228f; // r
	MaterialSpecular[1] = 0.316228f; // g
	MaterialSpecular[2] = 0.316228f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.1 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[1], 1.0f, 30, 30);
	dX += 4.5f;

	////// Obsidian /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.05375f; // r
	MaterialAmbient[1] = 0.05f;    // g
	MaterialAmbient[2] = 0.06625f; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.18275f; // r
	MaterialDiffuse[1] = 0.17f;    // g
	MaterialDiffuse[2] = 0.22525f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.332741f; // r
	MaterialSpecular[1] = 0.328634f; // g
	MaterialSpecular[2] = 0.346435f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.3 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[2], 1.0f, 30, 30);
	dX += 4.5f;

	////// Pearl /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.25f;    // r
	MaterialAmbient[1] = 0.20725f; // g
	MaterialAmbient[2] = 0.20725f; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 1.0f;   // r
	MaterialDiffuse[1] = 0.829f; // g
	MaterialDiffuse[2] = 0.829f; // b
	MaterialDiffuse[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.296648f; // r
	MaterialSpecular[1] = 0.296648f; // g
	MaterialSpecular[2] = 0.296648f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.088 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[3], 1.0f, 30, 30);
	dX += 4.5f;

	////// Ruby /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.1745f;  // r
	MaterialAmbient[1] = 0.01175f; // g
	MaterialAmbient[2] = 0.01175f; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.61424f; // r
	MaterialDiffuse[1] = 0.04136f; // g
	MaterialDiffuse[2] = 0.04136f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.727811f; // r
	MaterialSpecular[1] = 0.626959f; // g
	MaterialSpecular[2] = 0.626959f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.6 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[4], 1.0f, 30, 30);
	dX += 4.5f;

	////// Torquoise /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.1f;     // r
	MaterialAmbient[1] = 0.18725f; // g
	MaterialAmbient[2] = 0.1745f;  // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.396f;   // r
	MaterialDiffuse[1] = 0.74151f; // g
	MaterialDiffuse[2] = 0.69102f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.297254f;    // r
	MaterialSpecular[1] = 0.30829f; // g
	MaterialSpecular[2] = 0.306678f;    // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.1 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[5], 1.0f, 30, 30);
	dX += 4.5f;

	dX = 2.5f;
	dY += 4.0f;
	#pragma endregion

	#pragma region Metals

	////// Brass /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.329412f; // r
	MaterialAmbient[1] = 0.223529f; // g
	MaterialAmbient[2] = 0.027451f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.780392f; // r
	MaterialDiffuse[1] = 0.568627f; // g
	MaterialDiffuse[2] = 0.113725f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.992157f;    // r
	MaterialSpecular[1] = 0.941176f; // g
	MaterialSpecular[2] = 0.807843f;    // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.21794872 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[6], 1.0f, 30, 30);
	dX += 4.5f;

	////// Bronze /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.2125f;  // r
	MaterialAmbient[1] = 0.1275f; // g
	MaterialAmbient[2] = 0.054f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.714f; // r
	MaterialDiffuse[1] = 0.4284f; // g
	MaterialDiffuse[2] = 0.18144f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.393548f; // r
	MaterialSpecular[1] = 0.271906f; // g
	MaterialSpecular[2] = 0.166721f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.2 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[7], 1.0f, 30, 30);
	dX += 4.5f;

	////// Chrome /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.25; // r
	MaterialAmbient[1] = 0.25; // g
	MaterialAmbient[2] = 0.25; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.4;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.774597f; // r
	MaterialSpecular[1] = 0.774597f; // g
	MaterialSpecular[2] = 0.774597f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.6 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[8], 1.0f, 30, 30);
	dX += 4.5f;

	////// Copper /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.19125f;    // r
	MaterialAmbient[1] = 0.0735f; // g
	MaterialAmbient[2] = 0.0225f; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 1.7038f;   // r
	MaterialDiffuse[1] = 0.27048f; // g
	MaterialDiffuse[2] = 0.0828f; // b
	MaterialDiffuse[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.256777f; // r
	MaterialSpecular[1] = 0.137622f; // g
	MaterialSpecular[2] = 0.086014f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.1 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[9], 1.0f, 30, 30);
	dX += 4.5f;

	////// Gold /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.24725f; // r
	MaterialAmbient[1] = 0.1995f;  // g
	MaterialAmbient[2] = 0.0745f;  // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.75164f; // r
	MaterialDiffuse[1] = 0.60648f; // g
	MaterialDiffuse[2] = 0.22648f; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.628281f; // r
	MaterialSpecular[1] = 0.555802f; // g
	MaterialSpecular[2] = 0.366065f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.4 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[10], 1.0f, 30, 30);
	dX += 4.5f;

	////// Silver /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.19225f; // r
	MaterialAmbient[1] = 0.19225f; // g
	MaterialAmbient[2] = 0.19225f; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.50754f; // r
	MaterialDiffuse[1] = 0.50754f; // g
	MaterialDiffuse[2] = 0.50754f; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.508273f; // r
	MaterialSpecular[1] = 0.508273f; // g
	MaterialSpecular[2] = 0.508273f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.4 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[11], 1.0f, 30, 30);
	dX += 4.5f;

	dX = 2.5f;
	dY += 4.0f;
	#pragma endregion

	#pragma region Plastic

	////// Black /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.01f; // r
	MaterialDiffuse[1] = 0.01f; // g
	MaterialDiffuse[2] = 0.01f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.50f; // r
	MaterialSpecular[1] = 0.50f; // g
	MaterialSpecular[2] = 0.50f; // b
	MaterialSpecular[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[12], 1.0f, 30, 30);
	dX += 4.5f;

	////// Cyan /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f;  // r
	MaterialAmbient[1] = 0.1f;  // g
	MaterialAmbient[2] = 0.06f; // b
	MaterialAmbient[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.0f; // r
	MaterialDiffuse[1] = 0.50980392f; // g
	MaterialDiffuse[2] = 0.50980392f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.50196078f; // r
	MaterialSpecular[1] = 0.50196078f; // g
	MaterialSpecular[2] = 0.50196078f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[13], 1.0f, 30, 30);
	dX += 4.5f;

	////// Green /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.1;  // r
	MaterialDiffuse[1] = 0.35;  // g
	MaterialDiffuse[2] = 0.1;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.45f; // r
	MaterialSpecular[1] = 0.55f; // g
	MaterialSpecular[2] = 0.45f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[14], 1.0f, 30, 30);
	dX += 4.5f;

	////// Red /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.0f; // g
	MaterialDiffuse[2] = 0.0f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.7f; // r
	MaterialSpecular[1] = 0.6f; // g
	MaterialSpecular[2] = 0.6f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[15], 1.0f, 30, 30);
	dX += 4.5f;

	////// White /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.55f; // r
	MaterialDiffuse[1] = 0.55f; // g
	MaterialDiffuse[2] = 0.55f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.70f; // r
	MaterialSpecular[1] = 0.70f; // g
	MaterialSpecular[2] = 0.70f; // b
	MaterialSpecular[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[16], 1.0f, 30, 30);
	dX += 4.5f;

	////// Yellow /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.50f; // g
	MaterialDiffuse[2] = 0.0f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.60f; // r
	MaterialSpecular[1] = 0.60f; // g
	MaterialSpecular[2] = 0.50f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[16], 1.0f, 30, 30);
	dX += 4.5f;

	dX = 2.5f;
	dY += 4.0f;
	#pragma endregion

	#pragma region Rubber

	////// Black /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.02f; // r
	MaterialAmbient[1] = 0.02f; // g
	MaterialAmbient[2] = 0.02f; // b
	MaterialAmbient[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.01f; // r
	MaterialDiffuse[1] = 0.01f; // g
	MaterialDiffuse[2] = 0.01f; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.4f; // r
	MaterialSpecular[1] = 0.4f; // g
	MaterialSpecular[2] = 0.4f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.0878125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[17], 1.0f, 30, 30);
	dX += 4.5f;

	////// Cyan /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f ; // r
	MaterialAmbient[1] = 0.05f; // g
	MaterialAmbient[2] = 0.05f; // b
	MaterialAmbient[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.4f; // r
	MaterialDiffuse[1] = 0.5f; // g
	MaterialDiffuse[2] = 0.5f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.04f; // r
	MaterialSpecular[1] = 0.7f;  // g
	MaterialSpecular[2] = 0.7f;  // b
	MaterialSpecular[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.25 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[18], 1.0f, 30, 30);
	dX += 4.5f;

	////// Green /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.0f; // r
	MaterialAmbient[1] = 0.05f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.04f; // r
	MaterialSpecular[1] = 0.7f; // g
	MaterialSpecular[2] = 0.04f; // b
	MaterialSpecular[3] = 1.0f;      // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.078125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[19], 1.0f, 30, 30);
	dX += 4.5f;

	////// Red /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.05f; // r
	MaterialAmbient[1] = 0.0f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.4f; // g
	MaterialDiffuse[2] = 0.4f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.7f; // r
	MaterialSpecular[1] = 0.04f; // g
	MaterialSpecular[2] = 0.04f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.078125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[20], 1.0f, 30, 30);
	dX += 4.5f;

	////// White /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.05f; // r
	MaterialAmbient[1] = 0.05f; // g
	MaterialAmbient[2] = 0.05f; // b
	MaterialAmbient[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.5f; // g
	MaterialDiffuse[2] = 0.5f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.7f; // r
	MaterialSpecular[1] = 0.7f; // g
	MaterialSpecular[2] = 0.7f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.078125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[21], 1.0f, 30, 30);
	dX += 4.5f;

	////// Yellow /////////////////////////////////////
	// Ambient
	MaterialAmbient[0] = 0.05f; // r
	MaterialAmbient[1] = 0.05f; // g
	MaterialAmbient[2] = 0.0f; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// Diffuse
	MaterialDiffuse[0] = 0.5f; // r
	MaterialDiffuse[1] = 0.5f; // g
	MaterialDiffuse[2] = 0.4f; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// Specular
	MaterialSpecular[0] = 0.7f; // r
	MaterialSpecular[1] = 0.7f; // g
	MaterialSpecular[2] = 0.04f; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// Shininess
	MaterialShininess[0] = 0.078125 * 128.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(dX, dY, 0.0f);
	gluSphere(quadric[22], 1.0f, 30, 30);
	dX += 4.5f;

	dX = 2.5f;
	dY += 4.0f;
	#pragma endregion
	
}
